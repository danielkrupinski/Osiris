#pragma once

#include <Features/Combat/AimBot/AimBotConfigVariables.h>
#include <GameClient/Panorama/PanoramaDropDown.h>
#include <EntryPoints/GuiEntryPoints.h>
#include <Platform/Macros/FunctionAttributes.h>
#include "Tabs/VisualsTab/IntSlider.h"
#include "OnOffDropdownSelectionChangeHandler.h"

template <typename HookContext>
struct AimBotActivationKeyDropdownSelectionChangeHandler {
    explicit AimBotActivationKeyDropdownSelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        switch (selectedIndex) {
        case 0: SET_CONFIG_VAR(aim_bot_vars::ActivationKey, aim_bot_vars::kAltKey); break;
        case 1: SET_CONFIG_VAR(aim_bot_vars::ActivationKey, aim_bot_vars::kMouse4Key); break;
        case 2: SET_CONFIG_VAR(aim_bot_vars::ActivationKey, aim_bot_vars::kShiftKey); break;
        default: break;
        }
    }

private:
    HookContext& hookContext;
};

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
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aim_bot_vars::Enabled>>(guiPanel, "aim_bot");
        initDropDown<AimBotActivationKeyDropdownSelectionChangeHandler<HookContext>>(guiPanel, "aim_bot_activation_key");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aim_bot_vars::TeamCheck>>(guiPanel, "aim_bot_team_check");
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "no_scope_inacc_vis", !GET_CONFIG_VAR(no_scope_inaccuracy_vis_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "aim_bot", !GET_CONFIG_VAR(aim_bot_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "aim_bot_activation_key", aimBotActivationKeyDropdownIndex());
        setDropDownSelectedIndex(mainMenu, "aim_bot_team_check", !GET_CONFIG_VAR(aim_bot_vars::TeamCheck));
        updateSlider<aim_bot_vars::FovPixels>(mainMenu, "aim_bot_fov");
        updateSlider<aim_bot_vars::MaxDistance>(mainMenu, "aim_bot_max_distance");
        updateSlider<aim_bot_vars::Smooth>(mainMenu, "aim_bot_smooth");
        updateSlider<aim_bot_vars::VerticalAdjustment>(mainMenu, "aim_bot_vertical_adj");
        updateSlider<aim_bot_vars::DeadzonePixels>(mainMenu, "aim_bot_deadzone");
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

    template <typename ConfigVariable>
    void updateSlider(auto&& mainMenu, const char* sliderId) const noexcept
    {
        auto&& slider = hookContext.template make<IntSlider>(mainMenu.findChildInLayoutFile(sliderId));
        slider.updateSlider(static_cast<typename ConfigVariable::ValueType::ValueType>(GET_CONFIG_VAR(ConfigVariable)));
        slider.updateTextEntry(static_cast<typename ConfigVariable::ValueType::ValueType>(GET_CONFIG_VAR(ConfigVariable)));
    }

    [[nodiscard]] int aimBotActivationKeyDropdownIndex() const noexcept
    {
        switch (GET_CONFIG_VAR(aim_bot_vars::ActivationKey)) {
        case aim_bot_vars::kAltKey: return 0;
        case aim_bot_vars::kMouse4Key: return 1;
        case aim_bot_vars::kShiftKey: return 2;
        default: return 0;
        }
    }

    HookContext& hookContext;
};
