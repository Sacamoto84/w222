// stb_rect_pack.h - v1.01 - общедоступное достояние - прямоугольная упаковка
// Шон Барретт, 2014 г.
//
// Полезно, например. упаковка прямоугольных текстур в атлас.
// Не выполняет вращение.
//
// До #including,
//
//    #define STB_RECT_PACK_IMPLEMENTATION
//
// в файле, который вы хотите иметь реализацию.
//
// Не обязательно самый потрясающий метод упаковки, но лучше, чем
// Совершенно наивный вstb_truetype(что в первую очередь и есть
// это замена).
//
// Было проведено всего несколько тестов, могут возникнуть проблемы.
//
// Еще больше документов впереди.
//
// Никаких выделений памяти; используетqsort()иassert()из включенных библиотек.
// Можно переопределить их, определив STBRP_SORT и STBRP_ASSERT .
//
// В настоящее время эта библиотека использует алгоритм Skyline Bottom-Left.
//
// Обратите внимание: лучшие прямоугольные упаковщики приветствуются! Пожалуйста
// реализовать их в том же API, но с другим инициализатором
// функция.
//
// Кредиты
//
//  Библиотека
//    Шон Барретт
//  Незначительные особенности
//    Мартиньш Можейко
//    github:IntellectualKitty
//
//  Исправления ошибок/предупреждений
//    Джереми Жоссо
//    Фабиан Гизен
//
// История версий:
//
//     1.01 (11 июля 2021 г.) всегда используйте режим больших прямоугольников, выставляйте STBRP__MAXVAL в общедоступном разделе.
//     1.00 (25.02.2019) избегайте небольших космических отходов; изящно провалить слишком широкие прямоугольники
//     0.99 (07.02.2019) исправления предупреждений
//     0.11 (03.03.2017) результат успешной/неуспешной упаковки
//     0.10 (25 октября 2016 г.) удалена cast-away-const, чтобы избежать предупреждений
//     0.09 (27 августа 2016 г.) исправлены предупреждения компилятора.
//     0.08 (13 сентября 2015 г.) действительно исправлена ошибка с пустыми прямоугольниками (w=0 или h=0)
//     0.07 (13 сентября 2015 г.) исправлена ошибка с пустыми прямоугольниками (w=0 или h=0)
//     В версии 0.06 (15 апреля 2015 г.) добавлен STBRP_SORT, позволяющий заменить qsort.
//     0.05: добавлен STBRP_ASSERT, позволяющий заменять утверждение.
//     0.04: исправлена небольшая ошибка в поддержке STBRP_LARGE_RECTS.
//     0.01: первоначальный выпуск
//
// LICENSE
//
//   Информацию о лицензии смотрите в конце файла.

//////////////////////////////////////////////////////////////////////////////
//
//       INCLUDE SECTION
//

#ifndef STB_INCLUDE_STB_RECT_PACK_H
#define STB_INCLUDE_STB_RECT_PACK_H

#define STB_RECT_PACK_VERSION  1

#ifdef STBRP_STATIC
    #define STBRP_DEF static
#else
    #define STBRP_DEF extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// @cond
/**
 *  Сообщает Doxygen сторона дублирующего объявления.
 */
typedef struct stbrp_context stbrp_context;
typedef struct stbrp_node    stbrp_node;
typedef struct stbrp_rect    stbrp_rect;
/// @endcond

typedef int            stbrp_coord;

#define STBRP__MAXVAL  0x7fffffff
// В основном для внутреннего использования, но это максимальное поддерживаемое значение координат.

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

STBRP_DEF int stbrp_pack_rects(stbrp_context * context, stbrp_rect * rects, int num_rects);
// Назначьте упакованные места прямоугольникам. Прямоугольники имеют тип
// 'stbrp_rect', сейчас ниже, сохраните в массиве 'rects' и там
// многие из них - 'num_rects'.
//
// Успешно прикрепленные цифры имеют флаг was_packed.
// установлено ненулевое значение, а «x» и «y» сохраняют минимальное местоположение.
// на каждой оси (т.е. внизу слева в декартовых координатах, вверху слева
// если представить, что y увеличивается вниз). Прямоугольники, которые не подходят
// установите флаг «was_packed» в 0.
//
// Вам не следует пытаться получить доступ к массиву «прямоугольники» из другого потока.
// пока эта функция работает, поскольку функция временно меняет порядок
// массив во время его выполнения.
//
// Чтобы упаковать в другой валюту, вам нужно вызвать stbrp_init_target.
// снова. Чтобы продолжить упаковку в тот же прямоугольник, вы можете вызвать
// эту функцию еще раз. Вызов этого несколько раз с несколькими прямоугольниками
// массивы, вероятно, дадут худшие результаты упаковки, чем их вызов
// один раз с полным массивом прямоугольников, но опция
// доступен.
//
// Функция возвращает 1, если все прямоугольники были успешно обработаны.
// упаковано и 0 в противном случае.

struct stbrp_rect {
    // зарезервировано для вашего использования:
    int            id;

    // ввод:
    stbrp_coord    w, h;

    // вывод:
    stbrp_coord    x, y;
    int            was_packed;  // ненулевое, если действительная упаковка

}; // 16 байт, номинально

STBRP_DEF void stbrp_init_target(stbrp_context * context, int width, int height, stbrp_node * nodes, int num_nodes);
// Инициализируйте упаковщик прямоугольников, чтобы:
//    упакуйте прямоугольник размером «ширина» на «высоту»
//    используя временное хранилище, предоставляемое массивом «узлы», длиной «num_nodes»
//
// Вы должны вызывать эту функцию каждый раз, когда начинаете упаковывать новую цель.
//
// Функции «выключения» нет. Память «узлов» должна оставаться действующей в течение
// следующий вызовstbrp_pack_rects() (или вызовы), но может быть освобожден после
// звонок (или звонки) завершаются.
//
// Note: чтобы гарантировать наилучшие результаты, либо:
//       1. make sure 'num_nodes' >= 'width'
//   или 2. вызвать stbrp_allow_out_of_mem(), определенный ниже, с 'allow_out_of_mem = 1'
//
// Если вы не выполните ни одно из вышеперечисленных действий, ширина будет кратна кратному значению.
// небольших целых чисел, чтобы гарантировать, что алгоритму не хватит места во временной памяти.
//
// Если вы сделаете №2, то будет использоваться неквантованный алгоритм, но алгоритм
// может не хватить места для временного хранения, и он не сможет упаковать некоторые прямоугольники.

STBRP_DEF void stbrp_setup_allow_out_of_mem(stbrp_context * context, int allow_out_of_mem);
// При желании вызовите эту функцию после инициализации, но перед выполнением какой-либо упаковки в
// измените обработку сценария нехватки памяти, описанного выше.
// Если вы снова вызовете init, оно будет сброшено до значений по умолчанию (false).

STBRP_DEF void stbrp_setup_heuristic(stbrp_context * context, int heuristic);
// При необходимости выберите, какую эвристику упаковки должна использовать библиотека. Разные
// эвристика даст лучшие/худшие результаты для разных наборов данных.
// Если вы снова вызовете init, оно будет сброшено до значений по умолчанию.

enum {
    STBRP_HEURISTIC_Skyline_default = 0,
    STBRP_HEURISTIC_Skyline_BL_sortHeight = STBRP_HEURISTIC_Skyline_default,
    STBRP_HEURISTIC_Skyline_BF_sortHeight
};

//////////////////////////////////////////////////////////////////////////////
//
// детали следующих структур не имеют для вас значения, но они должны
// быть видимыми, чтобы вы могли управлять выделением памяти для них

struct stbrp_node {
    stbrp_coord  x, y;
    stbrp_node * next;
};

struct stbrp_context {
    int width;
    int height;
    int align;
    int init_mode;
    int heuristic;
    int num_nodes;
    stbrp_node * active_head;
    stbrp_node * free_head;
    stbrp_node extra[2]; // мы выделяем два дополнительных узла, поэтому оптимальное количество пользовательских узлов равно «ширине», а не «ширине +2».
};

#ifdef __cplusplus
}
#endif

#endif

//////////////////////////////////////////////////////////////////////////////
//
//     IMPLEMENTATION SECTION
//

#ifdef STB_RECT_PACK_IMPLEMENTATION
#ifndef STBRP_SORT
    #include <stdlib.h>
    #define STBRP_SORT qsort
#endif

#ifndef STBRP_ASSERT
    #include <assert.h>
    #define STBRP_ASSERT assert
#endif

#ifdef _MSC_VER
    #define STBRP__NOTUSED(v)  (void)(v)
    #define STBRP__CDECL       __cdecl
#else
    #define STBRP__NOTUSED(v)  (void)sizeof(v)
    #define STBRP__CDECL
#endif

enum {
    STBRP__INIT_skyline = 1
};

STBRP_DEF void stbrp_setup_heuristic(stbrp_context * context, int heuristic)
{
    switch(context->init_mode) {
        case STBRP__INIT_skyline:
            STBRP_ASSERT(heuristic == STBRP_HEURISTIC_Skyline_BL_sortHeight || heuristic == STBRP_HEURISTIC_Skyline_BF_sortHeight);
            context->heuristic = heuristic;
            break;
        default:
            STBRP_ASSERT(0);
    }
}

STBRP_DEF void stbrp_setup_allow_out_of_mem(stbrp_context * context, int allow_out_of_mem)
{
    if(allow_out_of_mem)
        // если не хватает памяти, то не беспокойтесь о их выравнивании;
        // это дает лучшую упаковку, но может потерпеть неудачу из-за OOM (хотя
        // прямоугольники легко помещаются).  @TODO более разумным подходом было бы только
        // квантовать, как только мы нажмем OOM, тогда мы сможем избавиться от этого параметра.
        context->align = 1;
    else {
        // если нехватка памяти не является нормой, тогда квантовайте ширину
        // так что узловnum_nodesвсегда достаточно.
        //
        // Т.е.  num_nodes* спортивные >= ширина
        //                  выровнять >= ширина / num_nodes
        //                  align = ceil(width/num_nodes)

        context->align = (context->width + context->num_nodes - 1) / context->num_nodes;
    }
}

STBRP_DEF void stbrp_init_target(stbrp_context * context, int width, int height, stbrp_node * nodes, int num_nodes)
{
    int i;

    for(i = 0; i < num_nodes - 1; ++i)
        nodes[i].next = &nodes[i + 1];
    nodes[i].next = NULL;
    context->init_mode = STBRP__INIT_skyline;
    context->heuristic = STBRP_HEURISTIC_Skyline_default;
    context->free_head = &nodes[0];
    context->active_head = &context->extra[0];
    context->width = width;
    context->height = height;
    context->num_nodes = num_nodes;
    stbrp_setup_allow_out_of_mem(context, 0);

    // узел 0 — полная ширина, узел 1 — контрольный (позволяет не сохранять ширину явно)
    context->extra[0].x = 0;
    context->extra[0].y = 0;
    context->extra[0].next = &context->extra[1];
    context->extra[1].x = (stbrp_coord) width;
    context->extra[1].y = (1 << 30);
    context->extra[1].next = NULL;
}

// найти минимальную позицию y, если она начинается с x1
static int stbrp__skyline_find_min_y(stbrp_context * c, stbrp_node * first, int x0, int width, int * pwaste)
{
    stbrp_node * node = first;
    int x1 = x0 + width;
    int min_y, visited_width, waste_area;

    STBRP__NOTUSED(c);

    STBRP_ASSERT(first->x <= x0);

#if 0
    // пропустить, если мы прошли узел
    while(node->next->x <= x0)
        ++node;
#else
    STBRP_ASSERT(node->next->x > x0); // в конечном итоге мы обработали это в вызывающей программе для повышения эффективности
#endif

    STBRP_ASSERT(node->x <= x0);

    min_y = 0;
    waste_area = 0;
    visited_width = 0;
    while(node->x < x1) {
        if(node->y > min_y) {
            // поднимитеmin_yвыше.
            // мы учли все отходы доmin_y,
            // но теперь мы добавим больше отходов ко всему, что мы посетили
            waste_area += visited_width * (node->y - min_y);
            min_y = node->y;
            // при первом прохожденииvisited_widthможет быть уменьшено
            if(node->x < x0)
                visited_width += node->next->x - x0;
            else
                visited_width += node->next->x - node->x;
        }
        else {
            // добавить место для мусора
            int under_width = node->next->x - node->x;
            if(under_width + visited_width > width)
                under_width = width - visited_width;
            waste_area += under_width * (min_y - node->y);
            visited_width += under_width;
        }
        node = node->next;
    }

    *pwaste = waste_area;
    return min_y;
}

typedef struct {
    int x, y;
    stbrp_node ** prev_link;
} stbrp__findresult;

static stbrp__findresult stbrp__skyline_find_best_pos(stbrp_context * c, int width, int height)
{
    int best_waste = (1 << 30), best_x, best_y = (1 << 30);
    stbrp__findresult fr;
    stbrp_node ** prev, * node, * tail, ** best = NULL;

    // выровнять по кратному c->align
    width = (width + c->align - 1);
    width -= width % c->align;
    STBRP_ASSERT(width % c->align == 0);

    // если оно не помещается, немедленно внесите залог
    if(width > c->width || height > c->height) {
        fr.prev_link = NULL;
        fr.x = fr.y = 0;
        return fr;
    }

    node = c->active_head;
    prev = &c->active_head;
    while(node->x + width <= c->width) {
        int y, waste;
        y = stbrp__skyline_find_min_y(c, node, node->x, width, &waste);
        if(c->heuristic == STBRP_HEURISTIC_Skyline_BL_sortHeight) {  // на самом деле просто хочу протестировать BL
            // внизу слева
            if(y < best_y) {
                best_y = y;
                best = prev;
            }
        }
        else {
            // наиболее подходящий
            if(y + height <= c->height) {
                // можно использовать только если он сначала вертикально
                if(y < best_y || (y == best_y && waste < best_waste)) {
                    best_y = y;
                    best_waste = waste;
                    best = prev;
                }
            }
        }
        prev = &node->next;
        node = node->next;
    }

    best_x = (best == NULL) ? 0 : (*best)->x;

    // если мы делаем наилучшее соответствие ( BF ), нам также нужно попытаться выровнять правый край по положению каждого узла.
    //
    // например, если подходит
    //
    //     ____________________
    //    |____________________|
    //
    //            в
    //
    //   |                         |
    //   |             ____________|
    //   |____________|
    //
    // тогда выравнивание по правому краю уменьшает количество отходов, но нижний левый BL всегда выбирает выравнивание по левому краю
    //
    // Из-за этого BF занимает примерно в 2 раза больше времени.

    if(c->heuristic == STBRP_HEURISTIC_Skyline_BF_sortHeight) {
        tail = c->active_head;
        node = c->active_head;
        prev = &c->active_head;
        // найти первый допустимый узел
        while(tail->x < width)
            tail = tail->next;
        while(tail) {
            int xpos = tail->x - width;
            int y, waste;
            STBRP_ASSERT(xpos >= 0);
            // найдите левую позицию, соответствующую этому
            while(node->next->x <= xpos) {
                prev = &node->next;
                node = node->next;
            }
            STBRP_ASSERT(node->next->x > xpos && node->x <= xpos);
            y = stbrp__skyline_find_min_y(c, node, xpos, width, &waste);
            if(y + height <= c->height) {
                if(y <= best_y) {
                    if(y < best_y || waste < best_waste || (waste == best_waste && xpos < best_x)) {
                        best_x = xpos;
                        STBRP_ASSERT(y <= best_y);
                        best_y = y;
                        best_waste = waste;
                        best = prev;
                    }
                }
            }
            tail = tail->next;
        }
    }

    fr.prev_link = best;
    fr.x = best_x;
    fr.y = best_y;
    return fr;
}

static stbrp__findresult stbrp__skyline_pack_rectangle(stbrp_context * context, int width, int height)
{
    // найти лучшую позицию согласно эвристике
    stbrp__findresult res = stbrp__skyline_find_best_pos(context, width, height);
    stbrp_node * node, * cur;

    // залог, если:
    //    1. it failed
    //    2. the best node doesn't fit (we don't always check this)
    //    3. we're out of memory
    if(res.prev_link == NULL || res.y + height > context->height || context->free_head == NULL) {
        res.prev_link = NULL;
        return res;
    }

    // в случае успеха создайте новый узел
    node = context->free_head;
    node->x = (stbrp_coord) res.x;
    node->y = (stbrp_coord)(res.y + height);

    context->free_head = node->next;

    // вставьте новый узел в правильную начальную точку и
    // пусть 'cur' указывает на важные узлы, которые необходимы
    // вшит обратно

    cur = *res.prev_link;
    if(cur->x < res.x) {
        // сохраните существующий, поэтому начните тестирование со следующего
        stbrp_node * next = cur->next;
        cur->next = node;
        cur = next;
    }
    else {
        *res.prev_link = node;
    }

    // Отсюда проходим Cur и освобождаем узлы, пока не доберемся до одного
    // это не должно быть освобождено
    while(cur->next && cur->next->x <= res.x + width) {
        stbrp_node * next = cur->next;
        // переместить текущий узел в свободный список
        cur->next = context->free_head;
        context->free_head = cur;
        cur = next;
    }

    // вшить список обратно
    node->next = cur;

    if(cur->x < res.x + width)
        cur->x = (stbrp_coord)(res.x + width);

#ifdef _DEBUG
    cur = context->active_head;
    while(cur->x < context->width) {
        STBRP_ASSERT(cur->x < cur->next->x);
        cur = cur->next;
    }
    STBRP_ASSERT(cur->next == NULL);

    {
        int count = 0;
        cur = context->active_head;
        while(cur) {
            cur = cur->next;
            ++count;
        }
        cur = context->free_head;
        while(cur) {
            cur = cur->next;
            ++count;
        }
        STBRP_ASSERT(count == context->num_nodes + 2);
    }
#endif

    return res;
}

static int STBRP__CDECL rect_height_compare(const void * a, const void * b)
{
    const stbrp_rect * p = (const stbrp_rect *) a;
    const stbrp_rect * q = (const stbrp_rect *) b;
    if(p->h > q->h)
        return -1;
    if(p->h < q->h)
        return  1;
    return (p->w > q->w) ? -1 : (p->w < q->w);
}

static int STBRP__CDECL rect_original_order(const void * a, const void * b)
{
    const stbrp_rect * p = (const stbrp_rect *) a;
    const stbrp_rect * q = (const stbrp_rect *) b;
    return (p->was_packed < q->was_packed) ? -1 : (p->was_packed > q->was_packed);
}

STBRP_DEF int stbrp_pack_rects(stbrp_context * context, stbrp_rect * rects, int num_rects)
{
    int i, all_rects_packed = 1;

    // мы используем поле «was_packed» внутри, чтобы разрешить сортировку/отсортировку.
    for(i = 0; i < num_rects; ++i) {
        rects[i].was_packed = i;
    }

    // сортировать по эвристике
    STBRP_SORT(rects, num_rects, sizeof(rects[0]), rect_height_compare);

    for(i = 0; i < num_rects; ++i) {
        if(rects[i].w == 0 || rects[i].h == 0) {
            rects[i].x = rects[i].y = 0;  // пустой прямоугольник не требует места
        }
        else {
            stbrp__findresult fr = stbrp__skyline_pack_rectangle(context, rects[i].w, rects[i].h);
            if(fr.prev_link) {
                rects[i].x = (stbrp_coord) fr.x;
                rects[i].y = (stbrp_coord) fr.y;
            }
            else {
                rects[i].x = rects[i].y = STBRP__MAXVAL;
            }
        }
    }

    // не сортировать
    STBRP_SORT(rects, num_rects, sizeof(rects[0]), rect_original_order);

    // установить флагиwas_packedи статус all_rects_packed
    for(i = 0; i < num_rects; ++i) {
        rects[i].was_packed = !(rects[i].x == STBRP__MAXVAL && rects[i].y == STBRP__MAXVAL);
        if(!rects[i].was_packed)
            all_rects_packed = 0;
    }

    // вернуть статус all_rects_packed
    return all_rects_packed;
}
#endif

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif

/*
------------------------------------------------------------------------------
Это программное обеспечение доступно по двум лицензиям — выбирайте ту, которую предпочитаете.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT Лицензия
Copyright (c) 2017 Sean Barrett
Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию
данное программное обеспечение и связанные с ним файлы документации («Программное обеспечение») для решения
Программное обеспечение без ограничений, включая, помимо прочего, права на
использовать, копировать, изменять, объединять, публиковать, распространять, сублицензировать и/или продавать копии
Программного обеспечения и разрешать лицам, которым предоставлено Программное обеспечение, делать
Итак, при соблюдении следующих условий:
Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены во все
копии или существенные части Программного обеспечения.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - общественное состояние ( www.unlicense.org)
Это бесплатное и ничем не обремененное программное обеспечение, общедоступное.
Любой может свободно копировать, изменять, публиковать, использовать, компилировать, продавать или распространять это
программное обеспечение в виде исходного кода или в виде скомпилированного двоичного файла для любых целей,
коммерческие или некоммерческие, и любыми способами.
В юрисдикциях, признающих законы об авторском праве, автор или авторы настоящего
программное обеспечение передать все права, связанные с авторскими правами на программное обеспечение, для общественности
домен. Мы делаем это на благо общества в целом и
в ущерб нашим наследникам и преемникам. Мы намерены, чтобы это посвящение стало
явный акт отказа навечно от всех нынешних и будущих прав на
это программное обеспечение согласно закону об авторском праве.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
