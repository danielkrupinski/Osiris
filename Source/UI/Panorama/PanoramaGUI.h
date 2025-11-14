#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <GameClient/Entities/PreviewPlayer.h>
#include <Features/Visuals/ModelGlow/Preview/PlayerModelGlowPreview.h>
#include <Features/Visuals/ModelGlow/Preview/PlayerModelGlowPreviewColorMode.h>
#include <Features/Visuals/ModelGlow/Preview/WeaponModelGlowPreview.h>
#include <GameClient/Entities/TeamNumber.h>
#include <GlobalContext/UnloadFlag.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <GameClient/Panorama/PanoramaUiEngine.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>
#include <GameClient/Panorama/Ui3dPanel.h>
#include <Utils/StringBuilder.h>

#include "PanoramaCommandDispatcher.h"
#include "CombatTab.h"
#include "HudTab.h"
#include "SoundTab.h"
#include "VisualsTab.h"
#include "Tabs/VisualsTab/ViewmodelModPreviewPanel.h"

template <typename HookContext>
class PlayerModelGlowPreviewPanel {
public:
    PlayerModelGlowPreviewPanel(HookContext& hookContext, cs2::CUIPanel* panel, TeamNumber teamNumber) noexcept
        : hookContext{hookContext}
        , panel{panel}
        , teamNumber{teamNumber}
    {
    }

    void update() const noexcept
    {
        if (!state().shouldUpdatePanel)
            return;

        StringBuilderStorage<100> storage;
        auto builder = storage.builder();
        builder.put(playerName(), ' ', teamName());

        if (state().colorMode == PlayerModelGlowPreviewColorMode::PlayerOrTeamColor) {
            if (const auto colorString = colorIndexToString()) {
                builder.put(" - ");
                builder.put(colorString);
            }
        } else if (state().colorMode == PlayerModelGlowPreviewColorMode::HealthBased) {
            builder.put(" - ", state().previewPlayerHealth, " HP");
        }

        labelPanel().setText(builder.cstring());
    }

private:
    [[nodiscard]] const char* playerName() const noexcept
    {
        switch (state().enemyTeam) {
        case EnemyTeam::Both: return "Enemy";
        case EnemyTeam::CT: return teamNumber == TeamNumber::CT ? "Enemy" : "Ally";
        case EnemyTeam::T: return teamNumber == TeamNumber::TT ? "Enemy" : "Ally";
        default: return "Player";
        }
    }

    [[nodiscard]] const char* teamName() const noexcept
    {
        return teamNumber == TeamNumber::TT ? "T" : "CT";
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.playerModelGlowPreviewState();
    }

    [[nodiscard]] decltype(auto) labelPanel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(panel).clientPanel().template as<PanoramaLabel>();
    }

    [[nodiscard]] const char* colorIndexToString() const noexcept
    {
        switch (state().previewPlayerColorIndex) {
        using enum cs2::PlayerColorIndex;
        case Blue: return "Blue";
        case Green: return "Green";
        case Yellow: return "Yellow";
        case Orange: return "Orange";
        case Purple: return "Purple";
        default: return nullptr;
        }
    }

    HookContext& hookContext;
    cs2::CUIPanel* panel;
    TeamNumber teamNumber;
};

template <typename HookContext>
class PanoramaGUI {
public:
    explicit PanoramaGUI(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void init(auto&& mainMenu) noexcept
    {
        if (!mainMenu)
            return;

        // ensure settings tab is loaded because we use CSS classes from settings
        // TODO: replace use of settings CSS classes with raw style properties
        uiEngine().runScript(mainMenu, "if (!$('#JsSettings')) MainMenu.NavigateToTab('JsSettings', 'settings/settings');");

        const auto settings = mainMenu.findChildInLayoutFile("JsSettings");
        if (settings)
            state().settingsPanelHandle = settings.getHandle();

        uiEngine().runScript(settings, reinterpret_cast<const char*>(
#include "CreateGUI.js"
));

        uiEngine().runScript(mainMenu, R"(
(function () {
  $('#JsSettings').FindChildInLayoutFile('OsirisMenuTab').SetParent($('#JsMainMenuContent'));

  var openMenuButton = $.CreatePanel('RadioButton', $.GetContextPanel().FindChildTraverse('MainMenuNavBarSettings').GetParent(), 'OsirisOpenMenuButton', {
    class: "mainmenu-top-navbar__radio-iconbtn",
    group: "NavBar",
    onactivate: "MainMenu.NavigateToTab('OsirisMenuTab', '');"
  });

  $.CreatePanel('Image', openMenuButton, '', {
    class: "mainmenu-top-navbar__radio-btn__icon",
    src: "s2r://panorama/images/icons/ui/bug.vsvg"
  });

  $.DispatchEvent('Activated', $.GetContextPanel().FindChildTraverse("MainMenuNavBarHome"), 'mouse');
})();
)");

        if (const auto guiButtonPanel = mainMenu.findChildInLayoutFile("OsirisOpenMenuButton"))
            state().guiButtonHandle = guiButtonPanel.getHandle();

        if (const auto guiPanel = mainMenu.findChildInLayoutFile("OsirisMenuTab")) {
            state().guiPanelHandle = guiPanel.getHandle();
            state().modelGlowPreviewPlayerLabelHandleTT = guiPanel.findChildInLayoutFile("ModelGlowPreviewPlayerTTLabel").getHandle();
            state().modelGlowPreviewPlayerLabelHandleCT = guiPanel.findChildInLayoutFile("ModelGlowPreviewPlayerCTLabel").getHandle();
            state().viewmodelPreviewPanelHandle = guiPanel.findChildInLayoutFile("ViewmodelPreview").getHandle();

            hookContext.template make<CombatTab>().init(guiPanel);
            hookContext.template make<HudTab>().init(guiPanel);
            hookContext.template make<VisualsTab>().init(guiPanel);
            hookContext.template make<SoundTab>().init(guiPanel);
        }

        updateFromConfig();
    }

    template <typename ConfigVariable>
    void onHueSliderValueChanged(const char* panelId, float value) const
    {
        const auto newVariableValue = handleHueSlider(panelId, value, ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax, GET_CONFIG_VAR(ConfigVariable));
        hookContext.config().template setVariable<ConfigVariable>(typename ConfigVariable::ValueType{newVariableValue});
    }

    template <typename ConfigVariable>
    void onHueSliderTextEntrySubmit(const char* panelId, const char* value) const noexcept
    {
        const auto newVariableValue = handleHueTextEntry(panelId, value, ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax, GET_CONFIG_VAR(ConfigVariable));
        hookContext.config().template setVariable<ConfigVariable>(typename ConfigVariable::ValueType{newVariableValue});
    }

    [[nodiscard]] decltype(auto) modelGlowPreviewPanel(const char* panelId) const noexcept
    {
        auto&& guiPanel = uiEngine().getPanelFromHandle(state().guiPanelHandle);
        return guiPanel.findChildInLayoutFile(panelId).clientPanel().template as<Ui3dPanel>();
    }

    void run(UnloadFlag& unloadFlag) const noexcept
    {
        auto&& guiPanel = uiEngine().getPanelFromHandle(state().guiPanelHandle);
        if (!guiPanel)
            return;

        auto&& playerModelGlowPreview = hookContext.template make<PlayerModelGlowPreview>();
        if (!playerModelGlowPreview.isPreviewPlayerSetTT())
            playerModelGlowPreview.setPreviewPlayerTT(guiPanel.findChildInLayoutFile("ModelGlowPreviewPlayerTT").clientPanel().template as<Ui3dPanel>().portraitWorld().findPreviewPlayer());
        if (!playerModelGlowPreview.isPreviewPlayerSetCT())
            playerModelGlowPreview.setPreviewPlayerCT(guiPanel.findChildInLayoutFile("ModelGlowPreviewPlayerCT").clientPanel().template as<Ui3dPanel>().portraitWorld().findPreviewPlayer());

        const auto cmdSymbol = uiEngine().makeSymbol(0, "cmd");
        const auto cmd = guiPanel.getAttributeString(cmdSymbol, "");
        PanoramaCommandDispatcher{cmd, unloadFlag, hookContext}();
        guiPanel.setAttributeString(cmdSymbol, "");

        hookContext.template make<PlayerModelGlowPreview>().update();
        hookContext.template make<PlayerModelGlowPreview>().hookPreviewPlayersSceneObjectUpdaters();

        hookContext.template make<WeaponModelGlowPreview>().updateSceneObjectUpdaterHooks();

        auto&& viewmodelModPreviewPanel = uiEngine().getPanelFromHandle(state().viewmodelPreviewPanelHandle).clientPanel().template as<ViewmodelModPreviewPanel>();
        viewmodelModPreviewPanel.setupPreviewModel();
        viewmodelModPreviewPanel.setFov();

        hookContext.template make<PlayerModelGlowPreviewPanel>(uiEngine().getPanelFromHandle(state().modelGlowPreviewPlayerLabelHandleTT), TeamNumber::TT).update();
        hookContext.template make<PlayerModelGlowPreviewPanel>(uiEngine().getPanelFromHandle(state().modelGlowPreviewPlayerLabelHandleCT), TeamNumber::CT).update();
    }

    void updateFromConfig() noexcept
    {
        const auto mainMenuPointer = hookContext.patternSearchResults().template get<MainMenuPanelPointer>();
        auto&& mainMenu = hookContext.template make<ClientPanel>(mainMenuPointer ? *mainMenuPointer : nullptr).uiPanel();
        hookContext.template make<CombatTab>().updateFromConfig(mainMenu);
        hookContext.template make<HudTab>().updateFromConfig(mainMenu);
        hookContext.template make<VisualsTab>().updateFromConfig(mainMenu);
        hookContext.template make<SoundTab>().updateFromConfig(mainMenu);
    }

    void onUnload() const noexcept
    {
        uiEngine().deletePanelByHandle(state().guiButtonHandle);
        uiEngine().deletePanelByHandle(state().guiPanelHandle);

        if (auto&& settingsPanel = uiEngine().getPanelFromHandle(state().settingsPanelHandle))
            uiEngine().runScript(settingsPanel, "delete $.Osiris");
    }

private:
    [[nodiscard]] decltype(auto) getHueSlider(const char* sliderId) const noexcept
    {
        auto&& guiPanel = uiEngine().getPanelFromHandle(state().guiPanelHandle);
        return hookContext.template make<HueSlider>(guiPanel.findChildInLayoutFile(sliderId));
    }

    [[nodiscard]] color::HueInteger handleHueTextEntry(const char* sliderId, const char* value, color::HueInteger min, color::HueInteger max, color::HueInteger current) const noexcept
    {
        auto&& hueSlider = getHueSlider(sliderId);
        color::HueInteger::UnderlyingType hueIntegral;
        if (!StringParser{value}.parseInt(hueIntegral) || hueIntegral < min || hueIntegral > max) {
            hueSlider.updateTextEntry(current);
            return current;
        }

        if (hueIntegral == current)
            return current;

        const color::HueInteger hue{hueIntegral};
        hueSlider.updateSlider(hue);
        hueSlider.updateColorPreview(hue);
        return hue;
    }

    [[nodiscard]] color::HueInteger handleHueSlider(const char* sliderId, float value, color::HueInteger min, color::HueInteger max, color::HueInteger current) const noexcept
    {
        const auto hueIntegral = static_cast<color::HueInteger::UnderlyingType>(value);
        if (hueIntegral < min || hueIntegral > max || hueIntegral == current)
            return current;

        const auto hue = color::HueInteger{hueIntegral};
        auto&& hueSlider = getHueSlider(sliderId);
        hueSlider.updateTextEntry(hue);
        hueSlider.updateColorPreview(hue);
        return hue;
    }

    [[nodiscard]] decltype(auto) uiEngine() const noexcept
    {
        return hookContext.template make<PanoramaUiEngine>();
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.panoramaGuiState();
    }

    HookContext& hookContext;
};
