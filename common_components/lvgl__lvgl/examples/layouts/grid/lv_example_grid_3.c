#include "../../lv_examples.h"
#if LV_USE_GRID && LV_BUILD_EXAMPLES

/**
 * Демонстрация «свободной единицы» сетки
 */
void lv_example_grid_3(void)
{
    /*Столбец 1: исправить ширину 60 пикселей.
     *Столбец 2: 1 единица из оставшегося свободного места
     *Столбец 3: 2 единицы из оставшегося свободного места.*/
    static int32_t col_dsc[] = {60, LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};

    /*Строка 1: исправить ширину 50 пикселей.
     *2 ряд: 1 единица из оставшегося свободного места.
     *Строка 3: исправить ширину 50 пикселей.*/
    static int32_t row_dsc[] = {50, LV_GRID_FR(1), 50, LV_GRID_TEMPLATE_LAST};

    /*Создать контейнер с сеткой*/
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, 300, 220);
    lv_obj_center(cont);
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);

    lv_obj_t * label;
    lv_obj_t * obj;
    uint8_t i;
    for(i = 0; i < 9; i++) {
        uint8_t col = i % 3;
        uint8_t row = i / 3;

        obj = lv_obj_create(cont);
        /*Растянуть ячейку по горизонтали и по вертикали тоже
         *Установите диапазон равным 1, чтобы размер ячейки был равен 1 столбцу/строке.*/
        lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, col, 1,
                             LV_GRID_ALIGN_STRETCH, row, 1);

        label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "%d,%d", col, row);
        lv_obj_center(label);
    }
}

#endif
