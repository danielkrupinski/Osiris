#pragma once

#include <CS2/Panorama/CPanel2D.h>
#include <Utils/TemplateParameterCstring.h>

template <typename HookContext, typename ConfigVariable, TemplateParameterCstring kPanelName>
class GuiEntryPoints {
public:
    LINUX_ONLY([[gnu::aligned(8)]]) static bool hueSliderValueChanged(void* /* thisptr */, cs2::CPanel2D* /* panel */, float value)
    {
        HookContext hookContext;
        hookContext.gui().template onHueSliderValueChanged<ConfigVariable>(kPanelName, value);
        return true;
    }

    LINUX_ONLY([[gnu::aligned(8)]]) static bool hueSliderTextEntrySubmit(void* /* thisptr */, cs2::CPanel2D* /* panel */, const char* value)
    {
        HookContext hookContext;
        hookContext.gui().template onHueSliderTextEntrySubmit<ConfigVariable>(kPanelName, value);
        return true;
    }
};
