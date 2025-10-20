#pragma once

#include <GameClient/Panorama/PanoramaDropDown.h>
#include <Platform/Macros/FunctionAttributes.h>

template <typename HookContext>
class CombatTab {
public:
    explicit CombatTab(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "no_scope_inacc_vis", !GET_CONFIG_VAR(no_scope_inaccuracy_vis_vars::Enabled));
    }

private:
    [[NOINLINE]] void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        mainMenu.findChildInLayoutFile(dropDownId).clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    HookContext& hookContext;
};
