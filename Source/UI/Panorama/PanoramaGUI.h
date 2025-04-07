#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <GameClient/Entities/PreviewPlayer.h>
#include <Features/Visuals/ModelGlow/Preview/PlayerModelGlowPreview.h>
#include <Features/Visuals/ModelGlow/Preview/PlayerModelGlowPreviewColorMode.h>
#include <Features/Visuals/ModelGlow/Preview/WeaponModelGlowPreview.h>
#include <Features/Features.h>
#include <GameClient/Entities/TeamNumber.h>
#include <GlobalContext/UnloadFlag.h>
#include <GameClient/Panorama/MapPreviewPanel.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <GameClient/Panorama/PanoramaUiEngine.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>
#include <Utils/StringBuilder.h>

#include "PanoramaCommandDispatcher.h"
#include "HudTab.h"
#include "SoundTab.h"
#include "VisualsTab.h"

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
        builder.put("Player ");
        if (teamNumber == TeamNumber::TT)
            builder.put("TT");
        else
            builder.put("CT");
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
        // todo: add color index constants to cs2 namespace
        switch (state().previewPlayerColorIndex) {
        case 0: return "Blue";
        case 1: return "Green";
        case 2: return "Yellow";
        case 3: return "Orange";
        case 4: return "Purple";
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
        }

        updateFromConfig();
    }

    void run(Features<HookContext> features, UnloadFlag& unloadFlag) const noexcept
    {
        auto&& guiPanel = uiEngine().getPanelFromHandle(state().guiPanelHandle);
        if (!guiPanel)
            return;

        auto&& playerModelGlowPreview = hookContext.template make<PlayerModelGlowPreview>();
        if (!playerModelGlowPreview.isPreviewPlayerSetTT())
            playerModelGlowPreview.setPreviewPlayerTT(guiPanel.findChildInLayoutFile("ModelGlowPreviewPlayerTT").clientPanel().template as<MapPreviewPanel>().findPreviewPlayer());
        if (!playerModelGlowPreview.isPreviewPlayerSetCT())
            playerModelGlowPreview.setPreviewPlayerCT(guiPanel.findChildInLayoutFile("ModelGlowPreviewPlayerCT").clientPanel().template as<MapPreviewPanel>().findPreviewPlayer());

        auto&& weaponModelGlowPreview = hookContext.template make<WeaponModelGlowPreview>();
        if (!weaponModelGlowPreview.isPreviewWeaponSet())
            weaponModelGlowPreview.setPreviewWeapon(guiPanel.findChildInLayoutFile("ModelGlowPreviewWeapon").clientPanel().template as<MapPreviewPanel>().findPreviewWeapon());
        if (!weaponModelGlowPreview.isPreviewFlashbangSet())
            weaponModelGlowPreview.setPreviewFlashbang(guiPanel.findChildInLayoutFile("ModelGlowPreviewFlashbang").clientPanel().template as<MapPreviewPanel>().findPreviewWeapon());
        if (!weaponModelGlowPreview.isPreviewHEGrenadeSet())
            weaponModelGlowPreview.setPreviewHEGrenade(guiPanel.findChildInLayoutFile("ModelGlowPreviewHEGrenade").clientPanel().template as<MapPreviewPanel>().findPreviewWeapon());
        if (!weaponModelGlowPreview.isPreviewSmokeGrenadeSet())
            weaponModelGlowPreview.setPreviewSmokeGrenade(guiPanel.findChildInLayoutFile("ModelGlowPreviewSmoke").clientPanel().template as<MapPreviewPanel>().findPreviewWeapon());
        if (!weaponModelGlowPreview.isPreviewIncendiarySet())
            weaponModelGlowPreview.setPreviewIncendiary(guiPanel.findChildInLayoutFile("ModelGlowPreviewIncendiary").clientPanel().template as<MapPreviewPanel>().findPreviewWeapon());

        const auto cmdSymbol = uiEngine().makeSymbol(0, "cmd");
        const auto cmd = guiPanel.getAttributeString(cmdSymbol, "");
        PanoramaCommandDispatcher{cmd, features, unloadFlag, hookContext}();
        guiPanel.setAttributeString(cmdSymbol, "");

        hookContext.template make<PlayerModelGlowPreview>().hookPreviewPlayersSceneObjectUpdaters();
        hookContext.template make<PlayerModelGlowPreview>().update();

        hookContext.template make<WeaponModelGlowPreview>().updateSceneObjectUpdaterHooks();

        hookContext.template make<PlayerModelGlowPreviewPanel>(uiEngine().getPanelFromHandle(state().modelGlowPreviewPlayerLabelHandleTT), TeamNumber::TT).update();
        hookContext.template make<PlayerModelGlowPreviewPanel>(uiEngine().getPanelFromHandle(state().modelGlowPreviewPlayerLabelHandleCT), TeamNumber::CT).update();
    }

    void updateFromConfig() noexcept
    {
        const auto mainMenuPointer = hookContext.clientPatternSearchResults().template get<MainMenuPanelPointer>();
        auto&& mainMenu = hookContext.template make<ClientPanel>(mainMenuPointer ? *mainMenuPointer : nullptr).uiPanel();
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
