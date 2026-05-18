/**
 * @file lv_draw_sw_grad.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_grad.h"
#if LV_USE_DRAW_SW

#include "../../misc/lv_types.h"
#include "../../osal/lv_os_private.h"
#include "../../misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define GRAD_CM(r,g,b) lv_color_make(r,g,b)
#define GRAD_CONV(t, x) t = x

#undef ALIGN
#if defined(LV_ARCH_64)
    #define ALIGN(X)    (((X) + 7) & ~7)
#else
    #define ALIGN(X)    (((X) + 3) & ~3)
#endif

/**********************
 *      TYPEDEFS
 **********************/

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS

typedef struct {
    /* w = (-b(xp, yp) + sqrt(sqr(b(xp, yp)) - 4 * a * c(xp, yp))) / (2 * a) */
    int32_t x0;         /* центр стартового круга */
    int32_t y0;         /* центр стартового круга */
    int32_t r0;         /* радиус стартового круга */
    int32_t inv_dr;     /* 1 / (r1 - r0) */
    int32_t a4;         /* 4 * a */
    int32_t inv_a4;     /* 1 / (4 * a) */
    int32_t dx;
    /* b(xp, yp) = xp * bpx + yp * bpy + bc */
    int32_t bpx;
    int32_t bpy;
    int32_t bc;
    lv_area_t clip_area;
    lv_draw_sw_grad_calc_t * cgrad;  /*Буфер кэша на 256 элементов, содержащий карту цветов градиента.*/
} lv_grad_radial_state_t;

typedef struct {
    /* w = a * xp + b * yp + c */
    int32_t a;
    int32_t b;
    int32_t c;
    lv_draw_sw_grad_calc_t * cgrad; /*Буфер кэша на 256 элементов, содержащий карту цветов градиента.*/
} lv_grad_linear_state_t;

typedef struct {
    /* w = a * xp + b * yp + c */
    int32_t x0;
    int32_t y0;
    int32_t a;
    int32_t da;
    int32_t inv_da;
    lv_draw_sw_grad_calc_t * cgrad; /*Буфер кэша на 256 элементов, содержащий карту цветов градиента.*/
} lv_grad_conical_state_t;

#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/
typedef lv_result_t (*op_cache_t)(lv_draw_sw_grad_calc_t * c, void * ctx);
static lv_draw_sw_grad_calc_t * allocate_item(const lv_grad_dsc_t * g, int32_t w, int32_t h);

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS

    static inline int32_t extend_w(int32_t w, lv_grad_extend_t extend);

#endif

/**********************
 *   STATIC VARIABLE
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_draw_sw_grad_calc_t * allocate_item(const lv_grad_dsc_t * g, int32_t w, int32_t h)
{
    int32_t size;
    switch(g->dir) {
        case LV_GRAD_DIR_HOR:
        case LV_GRAD_DIR_LINEAR:
        case LV_GRAD_DIR_RADIAL:
        case LV_GRAD_DIR_CONICAL:
            size = w;
            break;
        case LV_GRAD_DIR_VER:
            size = h;
            break;
        default:
            size = 64;
    }

    size_t req_size = ALIGN(sizeof(lv_draw_sw_grad_calc_t)) + ALIGN(size * sizeof(lv_color_t)) + ALIGN(size * sizeof(
                                                                                                           lv_opa_t));
    lv_draw_sw_grad_calc_t * item  = lv_malloc(req_size);
    LV_ASSERT_MALLOC(item);
    if(item == NULL) return NULL;

    uint8_t * p = (uint8_t *)item;
    item->color_map = (lv_color_t *)(p + ALIGN(sizeof(*item)));
    item->opa_map = (lv_opa_t *)(p + ALIGN(sizeof(*item)) + ALIGN(size * sizeof(lv_color_t)));
    item->size = size;
    return item;
}

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS

static inline int32_t extend_w(int32_t w, lv_grad_extend_t extend)
{
    if(extend == LV_GRAD_EXTEND_PAD) {                  /**< Repeat the same color*/
        return w < 0 ? 0 : LV_MIN(w, 255);
    }
    if(extend == LV_GRAD_EXTEND_REPEAT) {       /**< Repeat the pattern*/
        return w & 255;
    }
    /*LV_GRAD_EXTEND_REFLECT*/
    w &= 511;
    if(w > 255)
        w ^= 511;   /* 511 - w */
    return w;
}

#endif

/**********************
 *     FUNCTIONS
 **********************/

lv_draw_sw_grad_calc_t * lv_draw_sw_grad_get(const lv_grad_dsc_t * g, int32_t w, int32_t h)
{
    /* Нет градиента, нет кэша */
    if(g->dir == LV_GRAD_DIR_NONE) return NULL;

    /* Шаг 1. Найдите в кеше данный ключ. */
    lv_draw_sw_grad_calc_t * item = allocate_item(g, w, h);
    if(item == NULL) {
        LV_LOG_WARN("Failed to allocate item for the gradient");
        return item;
    }

    /* Шаг 3: Заполните его градиентом, как и ожидалось. */
    uint32_t i;
    for(i = 0; i < item->size; i++) {
        lv_draw_sw_grad_color_calculate(g, item->size, i, &item->color_map[i], &item->opa_map[i]);
    }
    return item;
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_grad_color_calculate(const lv_grad_dsc_t * dsc, int32_t range,
                                                           int32_t frac, lv_color_t * color_out, lv_opa_t * opa_out)
{
    lv_color_t tmp;
    /*Сначала клип выходит за пределы поля*/
    int32_t min = (dsc->stops[0].frac * range) >> 8;
    if(frac <= min) {
        GRAD_CONV(tmp, dsc->stops[0].color);
        *color_out = tmp;
        *opa_out = dsc->stops[0].opa;
        return;
    }

    int32_t max = (dsc->stops[dsc->stops_count - 1].frac * range) >> 8;
    if(frac >= max) {
        GRAD_CONV(tmp, dsc->stops[dsc->stops_count - 1].color);
        *color_out = tmp;
        *opa_out = dsc->stops[dsc->stops_count - 1].opa;
        return;
    }

    /*Найдите 2 ближайшие остановки прямо сейчас*/
    int32_t d = 0;
    int32_t found_i = 0;
    for(uint8_t i = 1; i < dsc->stops_count; i++) {
        int32_t cur = (dsc->stops[i].frac * range) >> 8;
        if(frac <= cur) {
            found_i = i;
            break;
        }
    }

    LV_ASSERT(found_i != 0);

    lv_color_t one, two;
    one = dsc->stops[found_i - 1].color;
    two = dsc->stops[found_i].color;
    min = (dsc->stops[found_i - 1].frac * range) >> 8;
    max = (dsc->stops[found_i].frac * range) >> 8;
    d = max - min;

    /*Затем интерполируйте*/
    frac -= min;
    lv_opa_t mix = (frac * 255) / d;
    lv_opa_t imix = 255 - mix;

    *color_out = GRAD_CM(LV_UDIV255(two.red * mix   + one.red * imix),
                         LV_UDIV255(two.green * mix + one.green * imix),
                         LV_UDIV255(two.blue * mix  + one.blue * imix));

    *opa_out = LV_UDIV255(dsc->stops[found_i].opa * mix   + dsc->stops[found_i - 1].opa * imix);
}

void lv_draw_sw_grad_cleanup(lv_draw_sw_grad_calc_t * grad)
{
    lv_free(grad);
}


#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS

/*
    Рассчитайте радиальный градиент на основе следующего уравнения:

    | P - ( C1 - C0 )w - C0 | = (r1 - r0)w + r0, где

        P: {xp, yp} — точка интереса
        C0: {x0, y0} — центр начального круга.
        C1: {x1, y1} — центр конечного круга
        r0 — радиус стартового круга
        r1 — радиус конечной окружности
        w — неизвестная переменная
        || длина вектора

    Приведенное выше уравнение можно переписать как:

    ((r1-r0)^2 - (x1-x0)^2 - (y1-y0)^2) * w^2 + 2*((xp-x0)*(x1-x0) + (yp-y0)*(y1-y0)) * w + (-(xp-x0)^2 - (yp-y0)^) = 0

    Корни квадратного уравнения можно получить по формуле (-b +- sqrt(b^2 - 4ac))/2a
    Нам нужен только более положительный корень.

    Обозначим
        dx = x1 - x0
        dy = y1 - y0
        dr = r1 - r0

    Таким образом:

    w = (-b(xp, yp) + sqrt(sqr(b(xp, yp)) - 4 * a * c(xp, yp))) / (2 * a), где

        b(xp, yp) = 2dx * xp + 2dy * yp + 2(r0 * dr - x0 * dx - y0 * dy)
        c(xp, yp) = r0^2 - (xp - x0)^2 - (yp - y0)^2

    Перепишите b(xp, yp) как:

    b(xp, yp) = xp * bpx + yp * bpy + bc, где

        бпх = 2dx
        бpy = 2dy
        bc = 2(r0 * dr - x0 * dx - y0 * dy)

    Мы можем заранее рассчитать константы, поскольку они не зависят от координат пикселей.

*/

void lv_draw_sw_grad_radial_setup(lv_grad_dsc_t * dsc, const lv_area_t * coords)
{
    lv_point_t start = dsc->params.radial.focal;
    lv_point_t end = dsc->params.radial.end;
    lv_point_t start_extent = dsc->params.radial.focal_extent;
    lv_point_t end_extent = dsc->params.radial.end_extent;
    lv_grad_radial_state_t * state = lv_malloc(sizeof(lv_grad_radial_state_t));
    dsc->state = state;

    /* Преобразование из процентных координат */
    int32_t wdt = lv_area_get_width(coords);
    int32_t hgt = lv_area_get_height(coords);

    start.x = lv_pct_to_px(start.x, wdt);
    end.x = lv_pct_to_px(end.x, wdt);
    start_extent.x = lv_pct_to_px(start_extent.x, wdt);
    end_extent.x = lv_pct_to_px(end_extent.x, wdt);
    start.y = lv_pct_to_px(start.y, hgt);
    end.y = lv_pct_to_px(end.y, hgt);
    start_extent.y = lv_pct_to_px(start_extent.y, hgt);
    end_extent.y = lv_pct_to_px(end_extent.y, hgt);

    /* Вычислить радиусы */
    int16_t r_start = lv_sqrt32(lv_sqr(start_extent.x - start.x) + lv_sqr(start_extent.y - start.y));
    int16_t r_end = lv_sqrt32(lv_sqr(end_extent.x - end.x) + lv_sqr(end_extent.y - end.y));
    LV_ASSERT(r_end != 0);

    /* Создать карту цветов градиента */
    state->cgrad = lv_draw_sw_grad_get(dsc, 256, 0);

    state->x0 = start.x;
    state->y0 = start.y;
    state->r0 = r_start;
    int32_t dr = r_end - r_start;
    if(end.x == start.x && end.y == start.y) {
        LV_ASSERT(dr != 0);
        state->a4 = lv_sqr(dr) << 2;
        state->bpx = 0;
        state->bpy = 0;
        state->bc = (state->r0 * dr) << 1;
        state->dx = 0;
        state->inv_dr = (1 << (8 + 16)) / dr;
    }
    else {
        int32_t dx = end.x - start.x;
        int32_t dy = end.y - start.y;
        state->dx = dx;    /* необходимо для пошагового расчета */
        state->a4 = (lv_sqr(dr) - lv_sqr(dx) - lv_sqr(dy)) << 2;
        /* b(xp, yp) = xp * bpx + yp * bpy + bc */
        state->bpx = dx << 1;
        state->bpy = dy << 1;
        state->bc = (state->r0 * dr - state->x0 * dx - state->y0 * dy) << 1;
    }
    state->inv_a4 = state->a4 != 0 ? (1 << (13 + 16)) / state->a4 : 0;
    /* проверьте возможное отсечение */
    if(dsc->extend == LV_GRAD_EXTEND_PAD &&
       /* если режим расширения — «площадка», то мы можем обрезать ограничивающую рамку конечного круга, если начальный круг полностью находится внутри конечного круга */
       (lv_sqr(start.x - end.x) + lv_sqr(start.y - end.y) < lv_sqr(r_end - r_start))) {
        if(r_end > r_start) {
            lv_area_set(&state->clip_area, end.x - r_end, end.y - r_end, end.x  + r_end, end.y + r_end);
        }
        else {
            lv_area_set(&state->clip_area, start.x - r_start, start.y - r_start, start.x  + r_start, start.y + r_start);
        }
    }
    else {
        state->clip_area.x1 = -0x7fffffff;
    }
}

void lv_draw_sw_grad_radial_cleanup(lv_grad_dsc_t * dsc)
{
    lv_grad_radial_state_t * state = dsc->state;
    if(state == NULL)
        return;
    if(state->cgrad)
        lv_draw_sw_grad_cleanup(state->cgrad);
    lv_free(state);
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_grad_radial_get_line(lv_grad_dsc_t * dsc, int32_t xp, int32_t yp,
                                                           int32_t width, lv_draw_sw_grad_calc_t * result)
{
    lv_grad_radial_state_t * state = (lv_grad_radial_state_t *)dsc->state;
    lv_color_t * buf = result->color_map;
    lv_opa_t * opa = result->opa_map;
    lv_draw_sw_grad_calc_t * grad = state->cgrad;

    int32_t w;  /* результат: это смещение в таблице цветов градиента из 256 элементов. */
    int32_t b, db, c, dc;

    /* проверьте возможное отсечение */
    if(state->clip_area.x1 != -0x7fffffff) {
        /* заполнить линию конечным цветом для пикселей за пределами обрезанной области */
        lv_color_t * _buf = buf;
        lv_opa_t * _opa = opa;
        lv_color_t _c = grad->color_map[255];
        lv_opa_t _o = grad->opa_map[255];
        int32_t _w = width;
        for(; _w > 0; _w--) {
            *_buf++ = _c;
            *_opa++ = _o;
        }
        /* is this line fully outside the clip area? */
        if(yp < state->clip_area.y1 ||
           yp >= state->clip_area.y2 ||
           xp >= state->clip_area.x2 ||
           xp + width < state->clip_area.x1) {
            return;
        }
        else {      /* не полностью снаружи: обрезать линию до ограничивающей рамки */
            int32_t _x1 = LV_MAX(xp, state->clip_area.x1);
            int32_t _x2 = LV_MIN(xp + width, state->clip_area.x2);
            buf += _x1 - xp;
            opa += _x1 - xp;
            xp = _x1;
            width = _x2 - _x1;
        }
    }

    b = xp * state->bpx + yp * state->bpy + state->bc;
    c = lv_sqr(state->r0) - lv_sqr(xp - state->x0) - lv_sqr(yp - state->y0);
    /* Мы можем сэкономить некоторые вычисления, используя предыдущие значения b и c. */
    db = state->dx << 1;
    dc = ((xp - state->x0) << 1) + 1;

    if(state->a4 == 0) {   /* не квадратное уравнение: решите линейное уравнение: w = -c/b */
        for(; width > 0; width--) {
            w = extend_w(b == 0 ? 0 : -(c << 8) / b, dsc->extend);
            *buf++ = grad->color_map[w];
            *opa++ = grad->opa_map[w];
            b += db;
            c -= dc;
            dc += 2;
        }
    }
    else {                  /* решить квадратное уравнение */
        if(state->bpx ||
           state->bpy) {    /* общий случай (круги не концентричны): w = (-b + sqrt(b^2 - 4ac))/2a (нам нужны только более положительные корень)*/
            int32_t a4 = state->a4 >> 4;
            for(; width > 0; width--) {
                int32_t det = lv_sqr(b >> 4) - (a4 * (c >> 4));     /* b^2 смещен вниз на 2*4=8, 4ac смещен вниз на 8 */
                /* проверьте определитель: если отрицательно, то решения нет: используйте начальный цвет */
                w = det < 0 ? 0 : extend_w(((lv_sqrt32(det) - (b >> 4)) * state->inv_a4) >>  16,
                                           dsc->extend);        /* квадратный корень сдвинут вниз на 4 (включая *256 для установки диапазона вывода) */
                *buf++ = grad->color_map[w];
                *opa++ = grad->opa_map[w];
                b += db;
                c -= dc;
                dc += 2;
            }
        }
        else {              /* Особый случай: концентрические круги: w = (sqrt((xp-x0)^2 + (yx-y0)^2)-r0)/(r1-r0) */
            c = lv_sqr(xp - state->x0) + lv_sqr(yp - state->y0);
            for(; width > 0; width--) {
                w = extend_w((((lv_sqrt32(c) - state->r0)) * state->inv_dr) >> 16, dsc->extend);
                *buf++ = grad->color_map[w];
                *opa++ = grad->opa_map[w];
                c += dc;
                dc += 2;
            }
        }
    }
}

/*
    Рассчитайте линейный градиент на основе следующего уравнения:

    w = ((P -C0) x (C1-C0)) / | C1-C0|^2, где

        P: {xp, yp} — точка интереса
        C0: {x0, y0} — начальная точка вектора градиента.
        C1: {x1, y1} — конечная точка вектора градиента.
        w — неизвестная переменная

        || длина вектора
        x — скалярное произведение

    Приведенное выше уравнение можно переписать как:

    w = xp * (dx / (dx^2 + dy^2)) + yp * (dy / (dx^2 + dy^2)) - (x0 * dx + y0 * dy) / (dx^2 + dy^2), где

        dx = x1 - x0
        dy = y1 - y0

    Мы можем заранее рассчитать константы, поскольку они не зависят от координат пикселей.

*/

void lv_draw_sw_grad_linear_setup(lv_grad_dsc_t * dsc, const lv_area_t * coords)
{
    lv_point_t start = dsc->params.linear.start;
    lv_point_t end = dsc->params.linear.end;
    lv_grad_linear_state_t * state = lv_malloc(sizeof(lv_grad_linear_state_t));
    dsc->state = state;

    /* Создать карту цветов градиента */
    state->cgrad = lv_draw_sw_grad_get(dsc, 256, 0);

    /* Преобразование из процентных координат */
    int32_t wdt = lv_area_get_width(coords);
    int32_t hgt = lv_area_get_height(coords);

    start.x = lv_pct_to_px(start.x, wdt);
    end.x = lv_pct_to_px(end.x, wdt);
    start.y = lv_pct_to_px(start.y, hgt);
    end.y = lv_pct_to_px(end.y, hgt);

    /* Предварительный расчет констант */
    int32_t dx = end.x - start.x;
    int32_t dy = end.y - start.y;

    int32_t l2 = lv_sqr(dx) + lv_sqr(dy);
    if(l2 == 0) l2 = 1;
    state->a = (dx << 16) / l2;
    state->b = (dy << 16) / l2;
    state->c = ((start.x * dx + start.y * dy) << 16) / l2;
}

void lv_draw_sw_grad_linear_cleanup(lv_grad_dsc_t * dsc)
{
    lv_grad_linear_state_t * state = dsc->state;
    if(state == NULL)
        return;
    if(state->cgrad)
        lv_free(state->cgrad);
    lv_free(state);
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_grad_linear_get_line(lv_grad_dsc_t * dsc, int32_t xp, int32_t yp,
                                                           int32_t width, lv_draw_sw_grad_calc_t * result)
{
    lv_grad_linear_state_t * state = (lv_grad_linear_state_t *)dsc->state;
    lv_color_t * buf = result->color_map;
    lv_opa_t * opa = result->opa_map;
    lv_draw_sw_grad_calc_t * grad = state->cgrad;

    int32_t w;  /* результат: это смещение в таблице цветов градиента из 256 элементов. */
    int32_t x, d;

    x = xp * state->a + yp * state->b - state->c;
    d = state->a;

    for(; width > 0; width--) {
        w = extend_w(x >> 8, dsc->extend);
        *buf++ = grad->color_map[w];
        *opa++ = grad->opa_map[w];
        x += d;
    }
}

/*
    Рассчитайте конический градиент на основе следующего уравнения:

    w = (atan((yp - y0)/(xp - x0)) - альфа) / (бета - альфа), где

        P: {xp, yp} — точка интереса
        C0: {x0, y0} — центр градиента
        альфа — начальный угол
        бета — это конечный угол
        w — неизвестная переменная
*/

void lv_draw_sw_grad_conical_setup(lv_grad_dsc_t * dsc, const lv_area_t * coords)
{
    lv_point_t c0 = dsc->params.conical.center;
    int32_t alpha = dsc->params.conical.start_angle % 360;
    int32_t beta = dsc->params.conical.end_angle % 360;
    lv_grad_conical_state_t * state = lv_malloc(sizeof(lv_grad_conical_state_t));
    dsc->state = state;

    /* Создать карту цветов градиента */
    state->cgrad = lv_draw_sw_grad_get(dsc, 256, 0);

    /* Преобразование из процентных координат */
    int32_t wdt = lv_area_get_width(coords);
    int32_t hgt = lv_area_get_height(coords);

    c0.x = lv_pct_to_px(c0.x, wdt);
    c0.y = lv_pct_to_px(c0.y, hgt);

    /* Предварительный расчет констант */
    if(beta <= alpha)
        beta += 360;
    state->x0 = c0.x;
    state->y0 = c0.y;
    state->a = alpha;
    state->da = beta - alpha;
    state->inv_da = (1 << 16) / (beta - alpha);
}

void lv_draw_sw_grad_conical_cleanup(lv_grad_dsc_t * dsc)
{
    lv_grad_conical_state_t * state = dsc->state;
    if(state == NULL)
        return;
    if(state->cgrad)
        lv_free(state->cgrad);
    lv_free(state);
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_grad_conical_get_line(lv_grad_dsc_t * dsc, int32_t xp, int32_t yp,
                                                            int32_t width, lv_draw_sw_grad_calc_t * result)
{
    lv_grad_conical_state_t * state = (lv_grad_conical_state_t *)dsc->state;
    lv_color_t * buf = result->color_map;
    lv_opa_t * opa = result->opa_map;
    lv_draw_sw_grad_calc_t * grad = state->cgrad;

    int32_t w;  /* результат: это смещение в таблице цветов градиента из 256 элементов. */
    int32_t dx = xp - state->x0;
    int32_t dy = yp - state->y0;

    if(dy == 0) {   /* в конце концов мы проходим через центр конуса: нужна дополнительная проверка в цикле, чтобы избежать того, что dx и dy были равными условиями в atan2 */
        for(; width > 0; width--) {
            if(dx == 0) {
                w = 0;
            }
            else {
                int32_t d = lv_atan2(dy, dx) - state->a;
                if(d < 0)
                    d += 360;
                w = extend_w((d * state->inv_da) >> 8, dsc->extend);
            }
            *buf++ = grad->color_map[w];
            *opa++ = grad->opa_map[w];
            dx++;
        }
    }
    else {
        for(; width > 0; width--) {
            int32_t d = lv_atan2(dy, dx) - state->a;
            if(d < 0)
                d += 360;
            w = extend_w((d * state->inv_da) >> 8, dsc->extend);
            *buf++ = grad->color_map[w];
            *opa++ = grad->opa_map[w];
            dx++;
        }
    }
}

#endif /* LV_USE_DRAW_SW_COMPLEX_GRADIENTS */

#endif /*LV_USE_DRAW_SW*/
