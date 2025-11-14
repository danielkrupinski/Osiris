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

    void init(auto&& guiPanel) const
    {
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, BombTimerEnabled>>(guiPanel, "bomb_timer");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, DefusingAlertEnabled>>(guiPanel, "defusing_alert");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, KillfeedPreserverEnabled>>(guiPanel, "preserve_killfeed");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, PostRoundTimerEnabled>>(guiPanel, "postround_timer");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, BombPlantAlertEnabled>>(guiPanel, "bomb_plant_alert");
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "bomb_timer", !GET_CONFIG_VAR(BombTimerEnabled));
        setDropDownSelectedIndex(mainMenu, "defusing_alert", !GET_CONFIG_VAR(DefusingAlertEnabled));
        setDropDownSelectedIndex(mainMenu, "preserve_killfeed", !GET_CONFIG_VAR(KillfeedPreserverEnabled));
        setDropDownSelectedIndex(mainMenu, "postround_timer", !GET_CONFIG_VAR(PostRoundTimerEnabled));
        setDropDownSelectedIndex(mainMenu, "bomb_plant_alert", !GET_CONFIG_VAR(BombPlantAlertEnabled));
    }

private:
    [[NOINLINE]] void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        mainMenu.findChildInLayoutFile(dropDownId).clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    template <typename Handler>
    void initDropDown(auto&& guiPanel, const char* panelId) const
    {
        auto&& dropDown = guiPanel.findChildInLayoutFile(panelId).clientPanel().template as<PanoramaDropDown>();
        dropDown.registerSelectionChangedHandler(&GuiEntryPoints<HookContext>::template dropDownSelectionChanged<Handler>);
    }

    HookContext& hookContext;
};
