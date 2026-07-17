#pragma once

#include <GameClient/Panorama/PanoramaUiPanel.h>
#include <GameClient/Panorama/Slider.h>
#include <GameClient/Panorama/TextEntry.h>
#include <Utils/StringBuilder.h>

template <typename HookContext>
class FloatSlider {
public:
    FloatSlider(HookContext& hookContext, cs2::CUIPanel* panel) noexcept
        : hookContext{hookContext}
        , panel_{panel}
    {
    }

    void updateSlider(float value) const noexcept
    {
        this->panel().children()[0].clientPanel().template as<Slider>().setValue(value);
    }

    void updateTextEntry(float value) const noexcept
    {
        const auto tenths = static_cast<std::uint32_t>(value * 10.0f + 0.5f);
        this->panel().children()[1].clientPanel().template as<TextEntry>()
            .setText(StringBuilderStorage<100>{}.builder().put(tenths / 10, '.', tenths % 10).cstring());
    }

private:
    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(panel_);
    }

    HookContext& hookContext;
    cs2::CUIPanel* panel_;
};
