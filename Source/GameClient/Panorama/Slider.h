#pragma once

#include <CS2/Panorama/CSlider.h>
#include <MemoryPatterns/PatternTypes/SliderPatternTypes.h>

template <typename HookContext>
class Slider {
public:
    using RawType = cs2::CSlider;

    Slider(HookContext& hookContext, cs2::CSlider* slider) noexcept
        : hookContext{hookContext}
        , slider{slider}
    {
    }

    void setValue(float value) noexcept
    {
        if (slider && setValueFunction())
            setValueFunction()(slider, value);
    }

private:
    [[nodiscard]] decltype(auto) setValueFunction() const noexcept
    {
        return hookContext.patternSearchResults().template get<SliderSetValueFunction>();
    }

    HookContext& hookContext;
    cs2::CSlider* slider;
};
