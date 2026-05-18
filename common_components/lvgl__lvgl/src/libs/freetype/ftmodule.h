/*
 * Этот файл регистрирует модули FreeType, скомпилированные в библиотеку.
 *
 * Если вы используете GNU make, этот файл IS NOT USED !  Вместо этого он создается в
 * каталог объектов (обычно `<topdir>/objs/`) на основе информации
 * из `<topdir>/modules.cfg`.
 *
 * Пожалуйста, прочитайте `docs/INSTALL.ANY` и `docs/CUSTOMIZE`, как скомпилировать.
 * FreeType без марки GNU.
 *
 */

/* FT_USE_MODULE ( FT_Module_Class , autofit_module_class ) */
FT_USE_MODULE(FT_Driver_ClassRec, tt_driver_class)
/* FT_USE_MODULE ( FT_Driver_ClassRec , t1_driver_class ) */
/* FT_USE_MODULE ( FT_Driver_ClassRec , cff_driver_class ) */
/* FT_USE_MODULE ( FT_Driver_ClassRec , t1cid_driver_class ) */
/* FT_USE_MODULE ( FT_Driver_ClassRec , pfr_driver_class ) */
/* FT_USE_MODULE ( FT_Driver_ClassRec , t42_driver_class ) */
/* FT_USE_MODULE ( FT_Driver_ClassRec , winfnt_driver_class ) */
/* FT_USE_MODULE ( FT_Driver_ClassRec , pcf_driver_class ) */
/* FT_USE_MODULE ( FT_Driver_ClassRec , bdf_driver_class ) */
/* FT_USE_MODULE ( FT_Module_Class , psaux_module_class ) */
/* FT_USE_MODULE ( FT_Module_Class , psnames_module_class ) */
/* FT_USE_MODULE ( FT_Module_Class , pshinter_module_class ) */
FT_USE_MODULE(FT_Module_Class, sfnt_module_class)
FT_USE_MODULE(FT_Renderer_Class, ft_smooth_renderer_class)
/* FT_USE_MODULE ( FT_Renderer_Class , ft_raster1_renderer_class ) */
/* FT_USE_MODULE ( FT_Renderer_Class , ft_sdf_renderer_class ) */
/* FT_USE_MODULE ( FT_Renderer_Class , ft_bitmap_sdf_renderer_class ) */
/* FT_USE_MODULE ( FT_Renderer_Class , ft_svg_renderer_class ) */

/* EOF */
