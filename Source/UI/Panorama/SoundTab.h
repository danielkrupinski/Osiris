#pragma once

#include <GameClient/Panorama/PanoramaDropDown.h>
#include <Platform/Macros/FunctionAttributes.h>

template <typename HookContext>
class SoundTab {
public:
    explicit SoundTab(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        const auto& soundFeaturesState = hookContext.featuresStates().soundFeaturesStates;
        setDropDownSelectedIndex(mainMenu, "visualize_player_footsteps", !soundFeaturesState.footstepVisualizerState.enabled);
        setDropDownSelectedIndex(mainMenu, "visualize_bomb_plant", !soundFeaturesState.bombPlantVisualizerState.enabled);
        setDropDownSelectedIndex(mainMenu, "visualize_bomb_beep", !soundFeaturesState.bombBeepVisualizerState.enabled);
        setDropDownSelectedIndex(mainMenu, "visualize_bomb_defuse", !soundFeaturesState.bombDefuseVisualizerState.enabled);
        setDropDownSelectedIndex(mainMenu, "visualize_scope_sound", !soundFeaturesState.weaponScopeVisualizerState.enabled);
        setDropDownSelectedIndex(mainMenu, "visualize_reload_sound", !soundFeaturesState.weaponReloadVisualizerState.enabled);
    }

private:
    [[NOINLINE]] void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        mainMenu.findChildInLayoutFile(dropDownId).clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    HookContext& hookContext;
};
