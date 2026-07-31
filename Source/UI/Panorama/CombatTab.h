#pragma once

#include <cstdint>

#include <GameClient/Panorama/PanoramaDropDown.h>
#include <EntryPoints/GuiEntryPoints.h>
#include <Features/Combat/Aimbot/AimbotAimPointSelectionChangeHandler.h>
#include <Features/Combat/Aimbot/AimbotHotKeySelectionChangeHandler.h>
#include <Features/Combat/Aimbot/AimbotLockModeSelectionChangeHandler.h>
#include <Features/Combat/Aimbot/AimbotMovementPenaltySelectionChangeHandler.h>
#include <Features/Combat/BHop/BHopConfigVariables.h>
#include <Features/Combat/ThirdPerson/ThirdPersonConfigVariables.h>
#include <Features/Combat/BoneDumper/BoneDumperConfigVariables.h>
#include <Platform/Macros/FunctionAttributes.h>
#include "OnOffDropdownSelectionChangeHandler.h"
#include "Tabs/VisualsTab/IntSlider.h"

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
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, triggerbot_vars::Enabled>>(guiPanel, "triggerbot_switch");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, triggerbot_vars::VisibilityCheck>>(guiPanel, "triggerbot_visibility");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, triggerbot_vars::FlashImmunity>>(guiPanel, "triggerbot_flash");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, triggerbot_vars::FirearmsOnly>>(guiPanel, "triggerbot_weapon_only");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::Enabled>>(guiPanel, "aimbot_switch");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::Smooth>>(guiPanel, "aimbot_smooth");
        initDropDown<AimbotAimPointSelectionChangeHandler<HookContext>>(guiPanel, "aimbot_aim_point");
        initDropDown<AimbotHotKeySelectionChangeHandler<HookContext>>(guiPanel, "aimbot_hotkey");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::PredictVelocity>>(guiPanel, "aimbot_predict_velocity");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::Humanization>>(guiPanel, "aimbot_humanization");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::SilentAim>>(guiPanel, "aimbot_silent");
        initDropDown<AimbotMovementPenaltySelectionChangeHandler<HookContext>>(guiPanel, "aimbot_move");
        initDropDown<AimbotLockModeSelectionChangeHandler<HookContext>>(guiPanel, "aimbot_lock_mode");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::LockIndicator>>(guiPanel, "aimbot_indicator");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, thirdperson_vars::Enabled>>(guiPanel, "thirdperson_switch");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, bhop_vars::Enabled>>(guiPanel, "bhop_switch");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, bhop_vars::AutoStrafe>>(guiPanel, "bhop_strafe");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, bhop_vars::Humanization>>(guiPanel, "bhop_humanize");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, bone_dumper_vars::Enabled>>(guiPanel, "bone_dumper_switch");
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "no_scope_inacc_vis", !GET_CONFIG_VAR(no_scope_inaccuracy_vis_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "triggerbot_switch", !GET_CONFIG_VAR(triggerbot_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "triggerbot_visibility", !GET_CONFIG_VAR(triggerbot_vars::VisibilityCheck));
        setDropDownSelectedIndex(mainMenu, "triggerbot_flash", !GET_CONFIG_VAR(triggerbot_vars::FlashImmunity));
        updateSlider(mainMenu, "triggerbot_cooldown", static_cast<std::uint8_t>(GET_CONFIG_VAR(triggerbot_vars::FireCooldownMs)));
        updateSlider(mainMenu, "triggerbot_radius", static_cast<std::uint8_t>(GET_CONFIG_VAR(triggerbot_vars::PixelRadius)));
        setDropDownSelectedIndex(mainMenu, "triggerbot_weapon_only", !GET_CONFIG_VAR(triggerbot_vars::FirearmsOnly));
        setDropDownSelectedIndex(mainMenu, "aimbot_switch", !GET_CONFIG_VAR(aimbot_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "aimbot_smooth", !GET_CONFIG_VAR(aimbot_vars::Smooth));
        setDropDownSelectedIndex(mainMenu, "aimbot_aim_point", static_cast<int>(GET_CONFIG_VAR(aimbot_vars::AimPoint)));
        setDropDownSelectedIndex(mainMenu, "aimbot_hotkey", static_cast<int>(GET_CONFIG_VAR(aimbot_vars::HotKey)));
        setDropDownSelectedIndex(mainMenu, "aimbot_predict_velocity", !GET_CONFIG_VAR(aimbot_vars::PredictVelocity));
        setDropDownSelectedIndex(mainMenu, "aimbot_humanization", !GET_CONFIG_VAR(aimbot_vars::Humanization));
        setDropDownSelectedIndex(mainMenu, "aimbot_silent", !GET_CONFIG_VAR(aimbot_vars::SilentAim));
        setDropDownSelectedIndex(mainMenu, "thirdperson_switch", !GET_CONFIG_VAR(thirdperson_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "bhop_switch", !GET_CONFIG_VAR(bhop_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "bhop_strafe", !GET_CONFIG_VAR(bhop_vars::AutoStrafe));
        setDropDownSelectedIndex(mainMenu, "bhop_humanize", !GET_CONFIG_VAR(bhop_vars::Humanization));
        updateSlider(mainMenu, "aimbot_cooldown", static_cast<std::uint8_t>(GET_CONFIG_VAR(aimbot_vars::KillCooldown)));
        updateSlider(mainMenu, "aimbot_miss", static_cast<std::uint8_t>(GET_CONFIG_VAR(aimbot_vars::MissChance)));
        setDropDownSelectedIndex(mainMenu, "aimbot_move", static_cast<int>(GET_CONFIG_VAR(aimbot_vars::MovementPenalty)));
        updateSlider(mainMenu, "aimbot_lock_break", static_cast<std::uint8_t>(GET_CONFIG_VAR(aimbot_vars::LockBreakInterval)));
        setDropDownSelectedIndex(mainMenu, "aimbot_lock_mode", static_cast<int>(GET_CONFIG_VAR(aimbot_vars::LockMode)));
        setDropDownSelectedIndex(mainMenu, "aimbot_indicator", !GET_CONFIG_VAR(aimbot_vars::LockIndicator));
        updateSlider(mainMenu, "aimbot_radius", static_cast<std::uint8_t>(GET_CONFIG_VAR(aimbot_vars::FovRadius)));
        setDropDownSelectedIndex(mainMenu, "bone_dumper_switch", !GET_CONFIG_VAR(bone_dumper_vars::Enabled));
    }

private:
    template <typename Handler>
    void initDropDown(auto&& guiPanel, const char* panelId) const
    {
        if (auto&& panel = guiPanel.findChildInLayoutFile(panelId)) {
            auto&& dropDown = panel.clientPanel().template as<PanoramaDropDown>();
            dropDown.registerSelectionChangedHandler(&GuiEntryPoints<HookContext>::template dropDownSelectionChanged<Handler>);
        }
    }

    [[NOINLINE]] void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        if (auto&& panel = mainMenu.findChildInLayoutFile(dropDownId))
            panel.clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    void updateSlider(auto&& mainMenu, const char* sliderId, std::uint8_t value) const noexcept
    {
        auto&& slider = hookContext.template make<IntSlider>(mainMenu.findChildInLayoutFile(sliderId));
        slider.updateSlider(value);
        slider.updateTextEntry(value);
    }

    HookContext& hookContext;
};
