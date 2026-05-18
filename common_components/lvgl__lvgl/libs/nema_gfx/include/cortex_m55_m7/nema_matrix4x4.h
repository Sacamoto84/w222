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


#ifndef NEMA_MATRIX4X4_H__
#define NEMA_MATRIX4X4_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef float nema_matrix4x4_t[4][4];


/** \brief Загрузите матрицу идентичности 4x4
 *
 * \param m Матрица для загрузки
 *
 */
void nema_mat4x4_load_identity(nema_matrix4x4_t m);

/** \brief Умножение двух матриц 4x4
 *
 * \param m Матрица результатов
 * \paramm_lЛевый операнд
 * \paramm_rПравый операнд
 *
 */
void nema_mat4x4_mul(nema_matrix4x4_t  m,
                     nema_matrix4x4_t  m_l,
                     nema_matrix4x4_t  m_r);


void nema_mat4x4_copy(nema_matrix4x4_t m_l,
                 nema_matrix4x4_t m_r);

/** \brief Умножение вектора 4x1 на матрицу 4x4
 *
 * \param m Матрица, которую нужно умножить
 * \param x Первый элемент вектора
 * \param y Второй элемент вектора
 * \param z Третий элемент вектора
 * \param w Четвертый элемент вектора
 *
 */
void nema_mat4x4_mul_vec(nema_matrix4x4_t m, float *x, float *y, float *z, float *w);

// ------------------------------------------------------------------------------------
// Преобразование объекта — матрица ModelView
// Координаты объекта в координатах глаза
// ------------------------------------------------------------------------------------

/** \brief Изменение преобразования перевода
 *
 * \param m Матрица для применения конвертировать
 * \param tx X коэффициент перевода
 * \param ty Y коэффициент перевода
 * \param tz Коэффициент перевода Z
 *
 */
void nema_mat4x4_translate(nema_matrix4x4_t m, float tx, float ty, float tz);

/** \brief Применить масштабное преобразование
 *
 * \param m Матрица для применения конвертировать
 * \param sx X коэффициент масштабирования
 * \param sy Коэффициент масштабирования Y
 * \param sz Z коэффициент масштабирования
 *
 */
void nema_mat4x4_scale(nema_matrix4x4_t m, float sx, float sy, float sz);

/** \brief Применить преобразование поворота вокруг оси X
 *
 * \param m Матрица для применения конвертировать
 * \paramangle_degreesУгол поворота в градусах
 *
 */
void nema_mat4x4_rotate_X    (nema_matrix4x4_t m, float angle_degrees);

/** \brief Применить преобразование поворота вокруг оси Y
 *
 * \param m Матрица для применения конвертировать
 * \paramangle_degreesУгол поворота в градусах
 *
 */
void nema_mat4x4_rotate_Y    (nema_matrix4x4_t m, float angle_degrees);

/** \brief Применить преобразование поворота вокруг оси Z
 *
 * \param m Матрица для применения конвертировать
 * \paramangle_degreesУгол поворота в градусах
 *
 */
void nema_mat4x4_rotate_Z    (nema_matrix4x4_t m, float angle_degrees);

// ------------------------------------------------------------------------------------
// Трансформация сцен/Frustum — матрица проекции
// Координаты глаза для координат обрезки
// ------------------------------------------------------------------------------------

/** \brief Настройка матрицы перспективной проекции
 *
 * \param m Матрица 4x4
 * \paramfovy_degreesПоле взгляда в градусах
 * \param аспект Соотношение сторон, определяющее поле зрения по оси x.
 * \param closeVal Расстояние от наблюдателя до ближней плоскости отсечения (всегда положительное)
 * \param FarVal Расстояние от наблюдателя до дальней плоскости отсечения (всегда положительное)
 *
 */
void nema_mat4x4_load_perspective(nema_matrix4x4_t m, float fovy_degrees, float aspect,
                                  float nearVal, float farVal);


/** \brief Настройка матрицы проекции правого взгляда
 *
 * \param m Матрица 4x4
 * \paramfovy_degreesПоле взгляда в градусах
 * \param аспект Соотношение сторон, определяющее поле зрения по оси x.
 * \param closeVal Расстояние от наблюдателя до ближней плоскости отсечения (всегда положительное)
 * \param FarVal Расстояние от наблюдателя до дальней плоскости отсечения (всегда положительное)
 *
 */
void nema_mat4x4_load_perspective_rh(nema_matrix4x4_t m, float fovy_degrees, float aspect,
                                  float nearVal, float farVal);

/** \brief Настройка матрицы ортогональной проекции
 *
 * \param m Матрица 4x4
 * \param left Левая вертикальная плоскость отсечения
 * \param right Правая вертикальная плоскость отсечения
 * \param нижняя нижняя горизонтальная плоскость отсечения
 * \param top Верхняя горизонтальная плоскость отсечения
 * \param closeVal Расстояние от наблюдателя до ближней плоскости отсечения (всегда положительное)
 * \param FarVal Расстояние от наблюдателя до дальней плоскости отсечения (всегда положительное)
 *
 */
void nema_mat4x4_load_ortho(nema_matrix4x4_t m,
                            float left,    float right,
                            float bottom,  float top,
                            float nearVal, float farVal);

/** \brief Настройка матрицы двумерной ортогональной проекции
 *
 * \param m Матрица 4x4
 * \param left Левая вертикальная плоскость отсечения
 * \param right Правая вертикальная плоскость отсечения
 * \param нижняя нижняя горизонтальная плоскость отсечения
 * \param top Верхняя горизонтальная плоскость отсечения
 *
 */
void nema_mat4x4_load_ortho_2d(nema_matrix4x4_t m,
                               float left,   float right,
                               float bottom, float top);

/** \brief Настроить матрицу правостороннего представления.
 *
 * \param m Матрица 4x4
 * \parameye_xПоложение глаз x.
 * \parameye_yПоложение глаз y.
 * \parameye_zПоложение глаз z.
 * \paramcenter_xЦентр x для просмотра
 * \paramcenter_yЦентр y для просмотра
 * \paramcenter_zЦентр z для просмотра
 * \paramup_xВосходящий вектор x. (Обычно 0)
 * \paramup_yВектор вверх y. (Обычно 1)
 * \paramup_zВосходящий вектор z. (Обычно 0)
 *
 */
void nema_mat4x4_look_at_rh(nema_matrix4x4_t m,
                        float eye_x, float eye_y, float eye_z,
                        float center_x, float center_y, float center_z,
                        float up_x, float up_y, float up_z);

// ------------------------------------------------------------------------------------
// Прикрепить координаты к координатам окна
// ------------------------------------------------------------------------------------

/** \brief Удобная функция для расчета координат окна по координатам объекта
 *
 * \param Модель mvp, матрица вида и проекции
 * \paramx_origВерхняя левая координата X окна
 * \paramy_origВерхняя левая координата Y окна
 * \param width Ширина окна
 * \param height Высота окна
 * \param closeVal Расстояние от наблюдателя до ближней плоскости отсечения (всегда положительное)
 * \param FarVal Расстояние от наблюдателя до дальней плоскости отсечения (всегда положительное)
 * \param x X координата объекта
 * \param y координата объекта Y
 * \param z Координата объекта Z
 * \param w W координата объекта
 * \return 1, если вершина находится вне усеченной пирамиды (должна быть обрезана)
 *
 */
int nema_mat4x4_obj_to_win_coords(nema_matrix4x4_t mvp,
                                   float x_orig,  float y_orig,
                                   int width, int height,
                                   float nearVal, float farVal,
                                   float *x,
                                   float *y,
                                   float *z,
                                   float *w);

#ifdef __cplusplus
}
#endif

#endif
