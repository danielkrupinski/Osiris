#pragma once

#include <cstdint>

#include <GameClient/Panorama/PanoramaDropDown.h>
#include <EntryPoints/GuiEntryPoints.h>
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
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::Enabled>>(guiPanel, "aimbot_switch");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, aimbot_vars::Smooth>>(guiPanel, "aimbot_smooth");
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "no_scope_inacc_vis", !GET_CONFIG_VAR(no_scope_inaccuracy_vis_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "triggerbot_switch", !GET_CONFIG_VAR(triggerbot_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "triggerbot_visibility", !GET_CONFIG_VAR(triggerbot_vars::VisibilityCheck));
        setDropDownSelectedIndex(mainMenu, "triggerbot_flash", !GET_CONFIG_VAR(triggerbot_vars::FlashImmunity));
        setDropDownSelectedIndex(mainMenu, "aimbot_switch", !GET_CONFIG_VAR(aimbot_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "aimbot_smooth", !GET_CONFIG_VAR(aimbot_vars::Smooth));
        updateSlider(mainMenu, "aimbot_radius", static_cast<std::uint8_t>(GET_CONFIG_VAR(aimbot_vars::FovRadius)));
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
