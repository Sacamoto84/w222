/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: MIT
 *
 * Журналы изменений:
 * Дата Примечания автора
 * 13 мая 2022 г. Meco Man Первая версия
 */

#ifdef __RTTHREAD__

void lv_user_gui_init(void)
{
    extern void ui_init(void);
    ui_init();
}

#endif /* __RTTHREAD__ */
