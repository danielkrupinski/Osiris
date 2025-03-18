#pragma once

#include <GameClient/Panorama/PanoramaDropDown.h>
#include <Platform/Macros/FunctionAttributes.h>

template <typename HookContext>
class HudTab {
public:
    explicit HudTab(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "bomb_timer", !hookContext.config().template getVariable<BombTimerEnabled>());
        setDropDownSelectedIndex(mainMenu, "defusing_alert", !hookContext.config().template getVariable<DefusingAlertEnabled>());
        setDropDownSelectedIndex(mainMenu, "preserve_killfeed", !hookContext.config().template getVariable<KillfeedPreserverEnabled>());
        setDropDownSelectedIndex(mainMenu, "postround_timer", !hookContext.config().template getVariable<PostRoundTimerEnabled>());
    }

private:
    [[NOINLINE]] void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        mainMenu.findChildInLayoutFile(dropDownId).clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    HookContext& hookContext;
};
