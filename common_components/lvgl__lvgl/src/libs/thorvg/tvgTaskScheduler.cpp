/*
 * Copyright (c) 2020 - 2024 the ThorVG project. All rights reserved.

 * Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию.
 * данного программного обеспечения и связанных с ним файлов документации («Программное обеспечение») для решения
 * в Программном обеспечении без ограничений, включая, помимо прочего, права
 * использовать, копировать, изменять, объединять, публиковать, распространять, сублицензировать и/или продавать
 * копий Программного обеспечения и разрешать лицам, которым Программное обеспечение
 * предоставлено для этого при соблюдении следующих условий:

 * Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены во все
 * копии или существенные части Программного обеспечения.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../../lv_conf_internal.h"
#if LV_USE_THORVG_INTERNAL

#include "tvgArray.h"
#include "tvgInlist.h"
#include "tvgTaskScheduler.h"

#ifdef THORVG_THREAD_SUPPORT
    #include <thread>
    #include <atomic>
#endif

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

namespace tvg {

struct TaskSchedulerImpl;
static TaskSchedulerImpl* inst = nullptr;

#ifdef THORVG_THREAD_SUPPORT

static thread_local bool _async = true;

struct TaskQueue {
    Inlist<Task>             taskDeque;
    mutex                    mtx;
    condition_variable       ready;
    bool                     done = false;

    bool tryPop(Task** task)
    {
        unique_lock<mutex> lock{mtx, try_to_lock};
        if (!lock || taskDeque.empty()) return false;
        *task = taskDeque.front();
        return true;
    }

    bool tryPush(Task* task)
    {
        {
            unique_lock<mutex> lock{mtx, try_to_lock};
            if (!lock) return false;
            taskDeque.back(task);
        }
        ready.notify_one();
        return true;
    }

    void complete()
    {
        {
            lock_guard<mutex> lock{mtx};
            done = true;
        }
        ready.notify_all();
    }

    bool pop(Task** task)
    {
        unique_lock<mutex> lock{mtx};

        while (taskDeque.empty() && !done) {
            ready.wait(lock);
        }

        if (taskDeque.empty()) return false;

        *task = taskDeque.front();
        return true;
    }

    void push(Task* task)
    {
        {
            lock_guard<mutex> lock{mtx};
            taskDeque.back(task);
        }
        ready.notify_one();
    }
};


struct TaskSchedulerImpl
{
    Array<thread*>                 threads;
    Array<TaskQueue*>              taskQueues;
    atomic<uint32_t>               idx{0};

    TaskSchedulerImpl(uint32_t threadCnt)
    {
        threads.reserve(threadCnt);
        taskQueues.reserve(threadCnt);

        for (uint32_t i = 0; i < threadCnt; ++i) {
            taskQueues.push(new TaskQueue);
            threads.push(new thread);
        }
        for (uint32_t i = 0; i < threadCnt; ++i) {
            *threads.data[i] = thread([&, i] { run(i); });
        }
    }

    ~TaskSchedulerImpl()
    {
        for (auto tq = taskQueues.begin(); tq < taskQueues.end(); ++tq) {
            (*tq)->complete();
        }
        for (auto thread = threads.begin(); thread < threads.end(); ++thread) {
            (*thread)->join();
            delete(*thread);
        }
        for (auto tq = taskQueues.begin(); tq < taskQueues.end(); ++tq) {
            delete(*tq);
        }
    }

    void run(unsigned i)
    {
        Task* task;

        //Петля ниток
        while (true) {
            auto success = false;
            for (uint32_t x = 0; x < threads.count * 2; ++x) {
                if (taskQueues[(i + x) % threads.count]->tryPop(&task)) {
                    success = true;
                    break;
                }
            }

            if (!success && !taskQueues[i]->pop(&task)) break;
            (*task)(i + 1);
        }
    }

    void request(Task* task)
    {
        //Асинхронный
        if (threads.count > 0 && _async) {
            task->prepare();
            auto i = idx++;
            for (uint32_t n = 0; n < threads.count; ++n) {
                if (taskQueues[(i + n) % threads.count]->tryPush(task)) return;
            }
            taskQueues[i % threads.count]->push(task);
        //Синхронизировать
        } else {
            task->run(0);
        }
    }

    uint32_t threadCnt()
    {
        return threads.count;
    }
};

#else //THORVG_THREAD_SUPPORT

static bool _async = true;

struct TaskSchedulerImpl
{
    TaskSchedulerImpl(TVG_UNUSED uint32_t threadCnt) {}
    void request(Task* task) { task->run(0); }
    uint32_t threadCnt() { return 0; }
};

#endif //THORVG_THREAD_SUPPORT

} //пространство имен

/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

void TaskScheduler::init(uint32_t threads)
{
    if (inst) return;
    inst = new TaskSchedulerImpl(threads);
}


void TaskScheduler::term()
{
    delete(inst);
    inst = nullptr;
}


void TaskScheduler::request(Task* task)
{
    if (inst) inst->request(task);
}


uint32_t TaskScheduler::threads()
{
    if (inst) return inst->threadCnt();
    return 0;
}


void TaskScheduler::async(bool on)
{
    //включить/выключить асинхронное выполнение задач для каждого потока
    _async = on;
}

#endif /* LV_USE_THORVG_INTERNAL */

