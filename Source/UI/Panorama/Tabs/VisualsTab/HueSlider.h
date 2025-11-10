#pragma once

#include <GameClient/Panorama/PanoramaUiPanel.h>
#include <GameClient/Panorama/Slider.h>
#include <Utils/ColorUtils.h>
#include <Utils/StringBuilder.h>

template <typename HookContext>
class HueSlider {
public:
    HueSlider(HookContext& hookContext, cs2::CUIPanel* _panel) noexcept
        : hookContext{hookContext}
        , _panel{_panel}
    {
    }

    void updateSlider(color::HueInteger hue) const noexcept
    {
        panel().children()[0].clientPanel().template as<Slider>().setValue(hue);
    }

    void updateTextEntry(color::HueInteger hue) const noexcept
    {
        panel().children()[1].clientPanel().template as<TextEntry>()
            .setText(StringBuilderStorage<100>{}.builder().put(static_cast<color::HueInteger::UnderlyingType>(hue)).cstring());
    }

    void updateColorPreview(color::HueInteger hue) const noexcept
    {
        panel().children()[2].setBackgroundColor(color::HSBtoRGB(hue, color::Saturation{0.7f}, color::Brightness{0.9f}));
    }

    void registerSliderValueChangedHandler(cs2::CSlider::SliderValueChangedEventHandler* func)
    {
        auto&& uiEngine = hookContext.template make<PanoramaUiEngine>();
        uiEngine.registerEventHandler(
            uiEngine.makeSymbol(0, "SliderValueChanged"),
            panel().children()[0],
            cs2::CUtlAbstractDelegate{func}
        );
    }

    void registerTextEntrySubmitHandler(cs2::CTextEntry::TextEntrySubmitEventHandler* func)
    {
        auto&& uiEngine = hookContext.template make<PanoramaUiEngine>();
        uiEngine.registerEventHandler(
            uiEngine.makeSymbol(0, "TextEntrySubmit"),
            panel().children()[1],
            cs2::CUtlAbstractDelegate{func}
        );
    }

private:
    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(_panel);
    }

    HookContext& hookContext;
    cs2::CUIPanel* _panel;
};
