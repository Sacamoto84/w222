#ifndef _THORVG_LOTTIE_H_
#define _THORVG_LOTTIE_H_

#include "thorvg.h"

namespace tvg
{

/**
 * @class ЛоттиАнимация
 *
 * @brief The LottieAnimation class enables control of advanced Lottie features.
 *
 * Этот класс расширяет Animation и имеет дополнительные интерфейсы.
 *
 * @see Animation
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
     * @brief Override Lottie properties using slot data.
     *
     * @param [in] slot Данные слота Lottie в формате JSON для переопределения или @c nullptr для сброса.
     *
     * @retval Result::Success When succeed.
     * @retval Result::InsufficientCondition In case the animation is not loaded.
     * @retval Result::InvalidArguments When the given parameter is invalid.
     *
     * @note Experimental API
     */
    Result override(const char* slot) noexcept;

    /**
    * @brief Specifies a segment by marker. 
    * 
    * Маркеры используются для управления воспроизведением анимации путем указания начальной и конечной точек. 
    * устраняя необходимость знать точные номера кадров.
    * Обычно маркеры обозначаются на уровне проекта. 
    * это означает, что вызывающие абоненты должны заранее знать имя маркера, чтобы использовать его.
    *
    * @param Маркер [in] Имя маркера сегмента.
    *
    * @retval Result::Success When successful.
    * @retval Result::InsufficientCondition If the animation is not loaded.
    * @retval Result::InvalidArguments When the given parameter is invalid.
    * @retval Result::NonSupport When it's not animatable.
    *
    * @note If a @c marker is specified, the previously set segment will be disregarded.
    * @note Set @c nullptr to reset the specified segment.
    * @see Animation::segment(float begin, float end)
    * @note Experimental API
    */
    Result segment(const char* marker) noexcept;

    /**
     * @brief Gets the marker count of the animation.
     *
     * @retval The count of the markers, zero if there is no marker.
     * 
     * @see LottieAnimation::marker()
     * @note Experimental API
     */
    uint32_t markersCnt() noexcept;
    
    /**
     * @brief Gets the marker name by a given index.
     *
     * @param [in] idx Индекс маркера анимации начинается с 0.
     *
     * @retval The name of marker when succeed, @c nullptr otherwise.
     * 
     * @see LottieAnimation::markersCnt()
     * @note Experimental API
     */
    const char* marker(uint32_t idx) noexcept;

    /**
     * @brief Creates a new LottieAnimation object.
     *
     * @return A new LottieAnimation object.
     *
     * @since 0,15
     */
    static std::unique_ptr<LottieAnimation> gen() noexcept;
};

} //пространство имен

#endif //_THORVG_LOTTIE_H_

#endif /* LV_USE_THORVG_INTERNAL */

