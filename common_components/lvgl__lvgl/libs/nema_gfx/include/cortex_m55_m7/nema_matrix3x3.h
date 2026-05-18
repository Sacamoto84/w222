/* TSI 2023.xmo */
/*******************************************************************************
 * Copyright (c) 2023 Think Silicon Single Member PC
 *
 * Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию.
 * этого файла заголовка и/или связанных с ним файлов документации для использования, копирования,
 * изменять, объединять, публиковать, распространять, сублицензировать и/или продавать копии
 * Материалы и разрешать лицам, которым предоставлены Материалы, делать
 * Итак, при соблюдении следующих условий:
 *
 * Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены в
 * все копии или существенные части Материалов.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * NEMAGFX API. THE UNMODIFIED, NORMATIVE VERSIONS OF THINK-SILICON NEMAGFX
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT:
 *   https://think-silicon.com/products/software/nemagfx-api
 *
 *  Программное обеспечение предоставляется «как есть», без каких-либо явных или явных гарантий.
 *  подразумеваемые, включая, помимо прочего, гарантии товарной пригодности,
 *  пригодность для конкретной цели и отсутствие нарушений. Ни в коем случае нельзя
 *  Считайте, что Silicon Single MemberPCнесет ответственность за любые претензии, ущерб или другие
 *  ответственность, будь то по договору, правонарушению или иным образом, возникающая
 *  из, вне или в связи с программным обеспечением, использованием или другими сделками
 *  в программном обеспечении.
 ******************************************************************************/


#ifndef NEMA_MATRIX3X3_H__
#define NEMA_MATRIX3X3_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef float nema_matrix3x3_t[3][3];


/** \brief Матрица идентичности нагрузки
 *
 * \param m Матрица для загрузки
 *
 */
void nema_mat3x3_load_identity(nema_matrix3x3_t m);


/** \brief Копирование матрицы _m в матрицу m
 *
 * \param m Назначение матрицы
 * \param m Исходная матрица
 *
 */
void
nema_mat3x3_copy(nema_matrix3x3_t m, nema_matrix3x3_t _m);

/** \brief Изменение преобразования перевода
 *
 * \param m Матрица для применения конвертировать
 * \param tx X коэффициент перевода
 * \param ty Y коэффициент перевода
 *
 */
void nema_mat3x3_translate   (nema_matrix3x3_t m, float tx, float ty);

/** \brief Применить масштабное преобразование
 *
 * \param m Матрица для применения конвертировать
 * \param sx X коэффициент масштабирования
 * \param sy Коэффициент масштабирования Y
 *
 */
void nema_mat3x3_scale       (nema_matrix3x3_t m, float sx, float sy);

/** \brief Применение переменного преобразования
 *
 * \param m Матрица для применения конвертировать
 * \param shx X коэффициент сдвига
 * \param shy Коэффициент сдвига Y
 *
 */
void nema_mat3x3_shear       (nema_matrix3x3_t m, float shx, float shy);

/** \brief Применение зеркального преобразования
 *
 * \param m Матрица для применения конвертировать
 * \param mx, если не ноль, зеркально отразить по горизонтали
 * \param my, если ненулевое, отразить вертикально
 *
 */
void nema_mat3x3_mirror      (nema_matrix3x3_t m, int mx, int my);

/** \brief Применение преобразования компьютера
 *
 * \param m Матрица для применения конвертировать
 * \paramangle_degreesУгол поворота в градусах
 *
 */
void nema_mat3x3_rotate      (nema_matrix3x3_t m, float angle_degrees);

/** \brief Применение преобразования компьютера
 *
 * \param m Матрица для применения конвертировать
 * \param cosa Cos угол поворота
 * \param sina Sin угол поворота
 *
 */
void
nema_mat3x3_rotate2(nema_matrix3x3_t m, float cosa, float sina);


/** \brief Умножение двух матриц 3х3 ( m = m*_m)
 *
 * \param m левая матрица, будет перезаписана результат
 * \param _m правая матрица
 *
 */
void nema_mat3x3_mul(nema_matrix3x3_t m, nema_matrix3x3_t _m);

/** \brief Умножение вектора на матрицу
 *
 * \param m Матрица для умножения
 * \param x Коэффициент вектора x
 * \param y Коэффициент вектора y
 *
 */
void nema_mat3x3_mul_vec(nema_matrix3x3_t m, float *x, float *y);

/** \brief Умножение вектора на аффинную матрицу
 *
 * \param m Матрица для умножения
 * \param x Коэффициент вектора x
 * \param y Коэффициент вектора y
 *
 */
void nema_mat3x3_mul_vec_affine(nema_matrix3x3_t m, float *x, float *y);

/** \brief Вычислить связанное
 *
 * \param m Матрица
 *
 */
void nema_mat3x3_adj(nema_matrix3x3_t m);


/** \brief Разделение матрицы на скалярное значение
 *
 * \param m Матрица для деления
 * Скалярное значение \param s
 *
 */
void nema_mat3x3_div_scalar(nema_matrix3x3_t m, float s);

/** \brief Инвертировать матрицу
 *
 * \param m Матрица для инвертирования
 *
 */
int nema_mat3x3_invert(nema_matrix3x3_t m);

/** \private */
int nema_mat3x3_square_to_quad(float dx0, float dy0,
                             float dx1, float dy1,
                             float dx2, float dy2,
                             float dx3, float dy3,
                             nema_matrix3x3_t m);

/** \brief Сопоставьте контур с четырехугольником
 *
 * \param width Ширина ширины
 * \param height Высота высоты
 * \param sx0 координата x первой в вершине четырёхугольника
 * \param sy0 координата y первой вершины четырёхугольника
 * \param sx1 координата x на вершине второго четырёхугольника
 * \param sy1 Координата y на второй вершине четырёхугольника
 * \param sx2 координата x в вершине четырехугольника
 * \param sy2 Координата и вершина четырехугольника
 * \param sx3 Координата x на четвертой вершине четырехугольника
 * \param sy3 Координата и четвертая вершина четырёхугольника
 * \param m Матрица отображения
 *
 */
int nema_mat3x3_quad_to_rect(int width, int height,
                           float sx0, float sy0,
                           float sx1, float sy1,
                           float sx2, float sy2,
                           float sx3, float sy3,
                           nema_matrix3x3_t m);

/** \brief Применение микрофона вокруг точек поворота
 *
 * \param m Матрица для применения конвертировать
 * \paramangle_degreesУгол поворота в градусах
 * \param x X координаты точки поворота
 * \param y Y координата точки поворота
 *
 */
void nema_mat3x3_rotate_pivot(nema_matrix3x3_t m, float angle_degrees,
                              float x, float y);

/** \brief Примените масштаб, а затем поверните вокруг точки поворота.
 *
 * \param m Матрица для применения конвертировать
* \param sx X коэффициент масштабирования
 * \param sy Коэффициент масштабирования Y
 * \paramangle_degreesУгол поворота в градусах
 * \param x X координаты точки поворота
 * \param y Y координата точки поворота
 *
 */
void nema_mat3x3_scale_rotate_pivot(nema_matrix3x3_t m,
                                    float sx, float sy,
                                    float angle_degrees, float x, float y);


/** \brief Копирование матрицы _m в матрицу m
 *
 * \param m Назначение матрицы
 * \param m Исходная матрица
 *
 */

void
nema_mat3x3_copy(nema_matrix3x3_t m, nema_matrix3x3_t _m);


#ifdef __cplusplus
}
#endif

#endif
