#pragma once

#include <GameClasses/PanoramaUiEngine.h>
#include <Utils/StringParser.h>
#include <Helpers/UnloadFlag.h>
#include <FeatureHelpers/FeatureToggle.h>

#include "PanoramaCommandDispatcher.h"

class PanoramaGUI {
public:
    void init(auto&& mainMenu) noexcept
    {
        if (!mainMenu)
            return;

        // ensure settings tab is loaded because we use CSS classes from settings
        // TODO: replace use of settings CSS classes with raw style properties
        PanoramaUiEngine::runScript(mainMenu, "if (!$('#JsSettings')) MainMenu.NavigateToTab('JsSettings', 'settings/settings');", "", 0);

        const auto settings = mainMenu.findChildInLayoutFile("JsSettings");
        if (settings)
            settingsPanelPtr.handle = settings.getHandle();

        PanoramaUiEngine::runScript(settings, reinterpret_cast<const char*>(
#include "CreateGUI.js"
), "", 0);

        PanoramaUiEngine::runScript(mainMenu, R"(
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
)", "", 0);

        if (const auto guiButtonPanel = mainMenu.findChildInLayoutFile("OsirisOpenMenuButton"))
            guiButtonPointer.handle = guiButtonPanel.getHandle();

        if (const auto guiPanel = mainMenu.findChildInLayoutFile("OsirisMenuTab"))
            guiPanelPointer.handle = guiPanel.getHandle();
    }

    ~PanoramaGUI() noexcept
    {
        if (guiButtonPointer.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(guiButtonPointer.getHandle());

        if (guiPanelPointer.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(guiPanelPointer.getHandle());

        if (const auto settingsPanel = settingsPanelPtr.get())
            PanoramaUiEngine::runScript(settingsPanel, "delete $.Osiris", "", 0);
    }

    void run(HookDependencies& hookDependencies, Features features, UnloadFlag& unloadFlag) const noexcept
    {
        const auto guiPanel = PanoramaUiPanel{PanoramaUiPanelContext{hookDependencies, guiPanelPointer.get()}};
        if (!guiPanel)
            return;

        const auto cmdSymbol = PanoramaUiEngine::makeSymbol(0, "cmd");
        const auto cmd = guiPanel.getAttributeString(cmdSymbol, "");
        PanoramaCommandDispatcher{cmd, features, unloadFlag}();
        guiPanel.setAttributeString(cmdSymbol, "");
    }

private:
    PanoramaPanelPointer guiPanelPointer;
    PanoramaPanelPointer guiButtonPointer;
    PanoramaPanelPointer settingsPanelPtr;
};
