/*----------------------------------------------*/
/* Конфигурации системы TJpgDec R0 .03          */
/*----------------------------------------------*/

#define JD_SZBUF        512
/* Определяет размер входного буфера потока */

#define JD_FORMAT       0
/* Определяет выходной формат пикселей.
/  0: RGB888 (24-bit/pix)
/  1: RGB565 (16-bit/pix)
/  2: Grayscale (8-bit/pix)
*/

#define JD_USE_SCALE    0
/* Переключает функцию удаления накипи на выходе.
/  0: Disable
/  1: Enable
*/

#define JD_TBLCLIP      1
/* Используйте преобразование таблицы для арифметики насыщения. Немного быстрее, но увеличивает размер кода на 1 KB.
/  0: Disable
/  1: Enable
*/

#define JD_FASTDECODE   1
/* Уровень оптимизации
/  0: Basic optimization. Suitable for 8/16-bit MCUs.
/  1: + 32-bit barrel shifter. Suitable for 32-bit MCUs.
/  2: + Table conversion for huffman decoding (wants 6 << HUFF_BIT bytes of RAM)
*/

