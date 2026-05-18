#ifndef _THORVG_LOTTIE_H_
#define _THORVG_LOTTIE_H_

#include "thorvg.h"

namespace tvg
{

/**
 * @class ЛоттиАнимация
 *
 * @brief Класс LottieAnimation позволяет управлять расширенными функциями Lottie.
 *
 * Этот класс расширяет анимацию и имеет дополнительные интерфейсы.
 *
 * @see Анимация
 * 
 * @since 0,15
 */

#include "../../lv_conf_internal.h"
#if LV_USE_THORVG_INTERNAL
class TVG_API LottieAnimation final : public Animation
{
public:
    ~LottieAnimation();

    /**
     * @brief Переопределить свойства Lottie, используя данные слота.
     *
     * @param [in] slot Данные слота Lottie в форматеJSONдля переопределения или@cnullptr для сброса.
     *
     * @retval Результат::Успех При успехе.
     * @retval Result::InsufficientCondition Если анимация не загружена.
     * @retval Result::InvalidArguments Когда данный параметр недействителен.
     *
     * @note Экспериментальный API
     */
    Result override(const char* slot) noexcept;

    /**
    * @brief Указывает сегмент по маркеру. 
    * 
    * Маркеры используются для управления воспроизведением анимации путем указания начальной и конечной точек. 
    * устраняя необходимость знать точные номера кадров.
    * Обычно маркеры обозначаются на уровне проекта. 
    * это означает, что вызывающие абоненты должны заранее знать имя маркера, чтобы использовать его.
    *
    * @param marker [in] Имя маркера сегмента.
    *
    * @retval Result::Success При успехе.
    * @retval Result::InsufficientCondition Если анимация не загружена.
    * @retval Result::InvalidArguments Когда данный параметр недействителен.
    * @retval Result::NonSupport Когда его нельзя анимировать.
    *
    * @note Если указан маркер @c, ранее установленный сегмент будет игнорироваться.
    * @note Установите@cnullptr для сброса указанного сегмента.
    * @see Анимация::сегмент(начало с плавающей запятой, конец с плавающей запятой)
    * @note Экспериментальный API
    */
    Result segment(const char* marker) noexcept;

    /**
     * @brief Получает количество маркеров анимации.
     *
     * @retval Количество маркеров, ноль, если маркера нет.
     * 
     * @see ЛоттиАнимация:: marker()
     * @note Экспериментальный API
     */
    uint32_t markersCnt() noexcept;
    
    /**
     * @brief Получает имя маркера по заданному индексу.
     *
     * @param [in] idx Индекс маркера анимации начинается с 0.
     *
     * @retval Имя маркера в случае успеха, в противном случае@cnullptr.
     * 
     * @see ЛоттиАнимация:: markersCnt()
     * @note Экспериментальный API
     */
    const char* marker(uint32_t idx) noexcept;

    /**
     * @brief Создает новый объект LottieAnimation.
     *
     * @return Новый объект LottieAnimation.
     *
     * @since 0,15
     */
    static std::unique_ptr<LottieAnimation> gen() noexcept;
};

} //пространство имен

#endif //_THORVG_LOTTIE_H_

#endif /* LV_USE_THORVG_INTERNAL */

