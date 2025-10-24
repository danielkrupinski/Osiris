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
        setDropDownSelectedIndex(mainMenu, "visualize_player_footsteps", !GET_CONFIG_VAR(FootstepSoundVisualizationEnabled));
        setDropDownSelectedIndex(mainMenu, "visualize_bomb_plant", !GET_CONFIG_VAR(BombPlantSoundVisualizationEnabled));
        setDropDownSelectedIndex(mainMenu, "visualize_bomb_beep", !GET_CONFIG_VAR(BombBeepSoundVisualizationEnabled));
        setDropDownSelectedIndex(mainMenu, "visualize_bomb_defuse", !GET_CONFIG_VAR(BombDefuseSoundVisualizationEnabled));
        setDropDownSelectedIndex(mainMenu, "visualize_scope_sound", !GET_CONFIG_VAR(WeaponScopeSoundVisualizationEnabled));
        setDropDownSelectedIndex(mainMenu, "visualize_reload_sound", !GET_CONFIG_VAR(WeaponReloadSoundVisualizationEnabled));
    }

private:
    [[NOINLINE]] void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        mainMenu.findChildInLayoutFile(dropDownId).clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    HookContext& hookContext;
};
