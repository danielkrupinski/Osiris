#pragma once

#include <Features/Combat/TriggerBot/TriggerBotConfigVariables.h>
#include <GameClient/Panorama/PanoramaDropDown.h>
#include <EntryPoints/GuiEntryPoints.h>
#include <Platform/Macros/FunctionAttributes.h>
#include "OnOffDropdownSelectionChangeHandler.h"

template <typename HookContext>
class CombatTab {
public:
    explicit CombatTab(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void init(auto&& guiPanel) const
    {
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, no_scope_inaccuracy_vis_vars::Enabled>>(guiPanel, "no_scope_inacc_vis");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, trigger_bot_vars::Enabled>>(guiPanel, "trigger_bot");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, trigger_bot_vars::TeamCheck>>(guiPanel, "trigger_bot_team_check");
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "no_scope_inacc_vis", !GET_CONFIG_VAR(no_scope_inaccuracy_vis_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "trigger_bot", !GET_CONFIG_VAR(trigger_bot_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "trigger_bot_team_check", !GET_CONFIG_VAR(trigger_bot_vars::TeamCheck));
    }

private:
    template <typename Handler>
    void initDropDown(auto&& guiPanel, const char* panelId) const
    {
        auto&& dropDown = guiPanel.findChildInLayoutFile(panelId).clientPanel().template as<PanoramaDropDown>();
        dropDown.registerSelectionChangedHandler(&GuiEntryPoints<HookContext>::template dropDownSelectionChanged<Handler>);
    }

    [[NOINLINE]] void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        mainMenu.findChildInLayoutFile(dropDownId).clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    HookContext& hookContext;
};
