/*ИспользованиеLVGLс Arduino требует некоторых усовершенствований:
 *Обязательно прочитайте документацию здесь: https://docs.lvgl.io/master/integration/framework/arduino.html..  */

#include <lvgl.h>

#if LV_USE_TFT_ESPI
#include <TFT_eSPI.h>
#endif

/*Чтобы использовать встроенные примеры и демо-версии LVGL, раскомментируйте включенные ниже файлы соответственно.
 *Вам также необходимо скопировать`lvgl/examples`в `lvgl/src/examples`. Аналогично для демо-версий от`lvgl/demos`до `lvgl/src/demos`.
 *Обратите внимание, что библиотека`lv_examples`создана дляLVGLv7, и вам не следует сохранять ее для этой версии (начиная сLVGLv8).
 *поскольку примеры и демо теперь являются частью основной библиотеки LVGL. */

//#include <примеры/lv_examples.h>
//#include <демо/lv_demos.h>

/*Установите разрешение экрана и поворот.*/
#define TFT_HOR_RES   320
#define TFT_VER_RES   240
#define TFT_ROTATION  LV_DISPLAY_ROTATION_0

/*LVGL рисует в этот буфер, размер экрана 1/10 обычно работает хорошо. Размер указан в байтах*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

#if LV_USE_LOG != 0
void my_print( lv_log_level_t level, const char * buf )
{
    LV_UNUSED(level);
    Serial.println(buf);
    Serial.flush();
}
#endif

/* LVGL вызывает его, когда визуализированное изображение необходимо скопировать на дисплей.*/
void my_disp_flush( lv_display_t *disp, const lv_area_t *area, uint8_t * px_map)
{
    /*Скопируйте`px map`в `area`.*/

    /*Например (функции my_... должны быть реализованы вами)
    uint32_t w =lv_area_get_width(площадь);
    uint32_t h =lv_area_get_height(площадь);

    my_set_window (площадь->x1, область->y1, ш, ч);
    my_draw_bitmaps (px_map, ш*ч);
     */

    /*Позвоните и сообщите LVGL, что вы готовы.*/
    lv_display_flush_ready(disp);
}

/*Чтение тачпада*/
void my_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data )
{
    /*Например (функции my_... должны быть реализованы вами)
    int32_t х, у;
    bool touched =my_get_touch( &x, &y );

    если(!тронул) {
        данные->состояние = LV_INDEV_STATE_RELEASED ;
    } еще {
        данные->состояние = LV_INDEV_STATE_PRESSED ;

        данные->point.x = x;
        данные->point.y = y;
    }
     */
}

/*Arduinomillis()в качестве источника тиков*/
static uint32_t my_tick(void)
{
    return millis();
}

void setup()
{
    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.begin( 115200 );
    Serial.println( LVGL_Arduino );

    lv_init();

    /*Установите источник тиков, чтобы LVGL знал, сколько времени прошло. */
    lv_tick_set_cb(my_tick);

    /* зарегистрировать функцию печати для отладки */
#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print );
#endif

    lv_display_t * disp;
#if LV_USE_TFT_ESPI
    /*TFT_eSPI можно включитьlv_conf.hдля простого подключения дисплея.*/
    disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
    lv_display_set_rotation(disp, TFT_ROTATION);

#else
    /*В противном случае создайте отображение самостоятельно*/
    disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

    /*Инициализируйте (фиктивный) драйвер устройства ввода.*/
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Тачпад должен иметь тип POINTER.*/
    lv_indev_set_read_cb(indev, my_touchpad_read);

    /* Создайте простую этикетку
     * ---------------------
     lv_obj_t *label =lv_label_create(lv_screen_active());
     lv_label_set_text (метка «Привет, Arduino, я LVGL!»);
     lv_obj_align (метка,LV_ALIGN_CENTER, 0, 0);

     * Попробуйте пример. Посмотреть все примеры
     *  - Онлайн: https://docs.lvgl.io/master/examples.html
     *  - Исходные коды: https://github.com/lvgl/lvgl/tree/master/examples
     * ----------------------------------------------------------------

     lv_example_btn_1 ();

     * Или оформите демо-версию. Не забудьте включить демо-версию в lv_conf.h. Например.   LV_USE_DEMO_WIDGETS
     * -------------------------------------------------------------------------------------------

     lv_demo_widgets ();
     */

    lv_obj_t *label = lv_label_create( lv_screen_active() );
    lv_label_set_text( label, "Hello Arduino, I'm LVGL!" );
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );

    Serial.println( "Setup done" );
}

void loop()
{
    lv_timer_handler(); /* пусть GUI делает свою работу */
    delay(5); /* пусть это время пройдет */
}
