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
        setDropDownSelectedIndex(mainMenu, "visualize_player_footsteps", !hookContext.config().template getVariable<FootstepSoundVisualizationEnabled>());
        setDropDownSelectedIndex(mainMenu, "visualize_bomb_plant", !hookContext.config().template getVariable<BombPlantSoundVisualizationEnabled>());
        setDropDownSelectedIndex(mainMenu, "visualize_bomb_beep", !hookContext.config().template getVariable<BombBeepSoundVisualizationEnabled>());
        setDropDownSelectedIndex(mainMenu, "visualize_bomb_defuse", !hookContext.config().template getVariable<BombDefuseSoundVisualizationEnabled>());
        setDropDownSelectedIndex(mainMenu, "visualize_scope_sound", !hookContext.config().template getVariable<WeaponScopeSoundVisualizationEnabled>());
        setDropDownSelectedIndex(mainMenu, "visualize_reload_sound", !hookContext.config().template getVariable<WeaponReloadSoundVisualizationEnabled>());
    }

private:
    [[NOINLINE]] void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        mainMenu.findChildInLayoutFile(dropDownId).clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    HookContext& hookContext;
};
