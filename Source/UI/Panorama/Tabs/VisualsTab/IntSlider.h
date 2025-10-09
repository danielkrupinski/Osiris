#pragma once

#include <GameClient/Panorama/PanoramaUiPanel.h>
#include <GameClient/Panorama/Slider.h>
#include <Utils/StringBuilder.h>

template <typename HookContext>
class IntSlider {
public:
    IntSlider(HookContext& hookContext, cs2::CUIPanel* _panel) noexcept
        : hookContext{hookContext}
        , _panel{_panel}
    {
    }

    void updateSlider(std::uint8_t value) const noexcept
    {
        panel().children()[0].clientPanel().template as<Slider>().setValue(value);
    }

    void updateTextEntry(std::uint8_t value) const noexcept
    {
        panel().children()[1].clientPanel().template as<TextEntry>()
            .setText(StringBuilderStorage<100>{}.builder().put(value).cstring());
    }

private:
    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(_panel);
    }

    HookContext& hookContext;
    cs2::CUIPanel* _panel;
};
