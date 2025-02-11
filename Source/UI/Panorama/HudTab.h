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
        const auto& hudFeaturesState = hookContext.featuresStates().hudFeaturesStates;
        setDropDownSelectedIndex(mainMenu, "bomb_timer", !hudFeaturesState.bombTimerState.enabled);
        setDropDownSelectedIndex(mainMenu, "defusing_alert", !hudFeaturesState.defusingAlertState.enabled);
        setDropDownSelectedIndex(mainMenu, "preserve_killfeed", !hudFeaturesState.killfeedPreserverState.enabled);
        setDropDownSelectedIndex(mainMenu, "postround_timer", !hudFeaturesState.postRoundTimerState.enabled);
    }

private:
    [[NOINLINE]] void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        mainMenu.findChildInLayoutFile(dropDownId).clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    HookContext& hookContext;
};
