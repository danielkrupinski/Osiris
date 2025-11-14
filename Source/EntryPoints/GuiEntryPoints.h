#pragma once

#include <CS2/Panorama/CPanel2D.h>
#include <Utils/TemplateParameterCstring.h>

template <typename HookContext>
class GuiEntryPoints {
public:
    template <typename ConfigVariable, TemplateParameterCstring kPanelName>
    LINUX_ONLY([[gnu::aligned(8)]]) static bool hueSliderValueChanged(void* /* thisptr */, cs2::CPanel2D* /* panel */, float value)
    {
        HookContext hookContext;
        hookContext.gui().template onHueSliderValueChanged<ConfigVariable>(kPanelName, value);
        return true;
    }

    template <typename ConfigVariable, TemplateParameterCstring kPanelName>
    LINUX_ONLY([[gnu::aligned(8)]]) static bool hueSliderTextEntrySubmit(void* /* thisptr */, cs2::CPanel2D* /* panel */, const char* value)
    {
        HookContext hookContext;
        hookContext.gui().template onHueSliderTextEntrySubmit<ConfigVariable>(kPanelName, value);
        return true;
    }

    template <typename DropdownSelectionChangedHandler>
    LINUX_ONLY([[gnu::aligned(8)]]) static bool dropDownSelectionChanged(void* /* thisptr */, cs2::CPanel2D* panel)
    {
        HookContext hookContext;
        auto&& dropdown = hookContext.template make<ClientPanel>(panel).template as<PanoramaDropDown>();
        hookContext.template make<DropdownSelectionChangedHandler>().onSelectionChanged(dropdown.getSelectedIndex());
        return true;
    }
};
