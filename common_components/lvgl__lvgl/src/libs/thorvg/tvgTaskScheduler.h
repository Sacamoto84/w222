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

#ifndef _TVG_TASK_SCHEDULER_H_
#define _TVG_TASK_SCHEDULER_H_

#ifdef THORVG_THREAD_SUPPORT
#include <mutex>
#include <condition_variable>
#include <thread>
#endif

#include "tvgCommon.h"
#include "tvgInlist.h"

#ifdef THORVG_THREAD_SUPPORT
using std::mutex;
using std::condition_variable;
using std::unique_lock;
using std::thread;
using std::atomic;
using std::try_to_lock;
#endif

namespace tvg {

#ifdef THORVG_THREAD_SUPPORT

struct Task
{
private:
    mutex                   mtx;
    condition_variable      cv;
    bool                    ready = true;
    bool                    pending = false;

public:
    INLIST_ITEM(Task);

    virtual ~Task() = default;

    void done()
    {
        if (!pending) return;

        unique_lock<mutex> lock(mtx);
        while (!ready) cv.wait(lock);
        pending = false;
    }

protected:
    virtual void run(unsigned tid) = 0;

private:
    void operator()(unsigned tid)
    {
        run(tid);

        lock_guard<mutex> lock(mtx);
        ready = true;
        cv.notify_one();
    }

    void prepare()
    {
        ready = false;
        pending = true;
    }

    friend struct TaskSchedulerImpl;
};

#else  //THORVG_THREAD_SUPPORT

struct Task
{
public:
    INLIST_ITEM(Task);

    virtual ~Task() = default;
    void done() {}

protected:
    virtual void run(unsigned tid) = 0;

private:
    friend struct TaskSchedulerImpl;
};

#endif  //THORVG_THREAD_SUPPORT


struct TaskScheduler
{
    static uint32_t threads();
    static void init(uint32_t threads);
    static void term();
    static void request(Task* task);
    static void async(bool on);
};

}  //пространство имен

#endif //_TVG_TASK_SCHEDULER_H_
 

#endif /* LV_USE_THORVG_INTERNAL */

