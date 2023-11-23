u8R"(
$.Osiris = (function () {
  var activeTab;

  return {
    rootPanel: (function () {
      const rootPanel = $.CreatePanel('Panel', $.GetContextPanel(), 'OsirisMenuTab', {
        class: "mainmenu-content__container",
        useglobalcontext: "true"
      });

      rootPanel.visible = false;
      rootPanel.SetReadyForDisplay(false);
      rootPanel.RegisterForReadyEvents(true);
      $.RegisterEventHandler('PropertyTransitionEnd', rootPanel, function (panelName, propertyName) {
        if (rootPanel.id === panelName && propertyName === 'opacity') {
          if (rootPanel.visible === true && rootPanel.BIsTransparent()) {
            rootPanel.visible = false;
            rootPanel.SetReadyForDisplay(false);
            return true;
          } else if (newPanel.visible === true) {
            $.DispatchEvent('MainMenuTabShown', 'OsirisMenuTab');
          }
        }
        return false;
      });

      return rootPanel;
    })(),
    goHome: function () {
      $.DispatchEvent('Activated', this.rootPanel.GetParent().GetParent().GetParent().FindChildInLayoutFile("MainMenuNavBarHome"), 'mouse');
    },
    addCommand: function (command, value = '') {
      var existingCommands = this.rootPanel.GetAttributeString('cmd', '');
      this.rootPanel.SetAttributeString('cmd', existingCommands + command + ' ' + value);
    },
    navigateToTab: function (tabID) {
      if (activeTab === tabID)
        return;

      if (activeTab) {
        var panelToHide = this.rootPanel.FindChildInLayoutFile(activeTab);
        panelToHide.RemoveClass('Active');
      }

      this.rootPanel.FindChildInLayoutFile(tabID + '_button').checked = true;

      activeTab = tabID;
      var activePanel = this.rootPanel.FindChildInLayoutFile(tabID);
      activePanel.AddClass('Active');
      activePanel.visible = true;
      activePanel.SetReadyForDisplay(true);
    },
    dropDownUpdated: function (tabID, dropDownID) {
      this.addCommand('set', tabID + '/' + dropDownID + '/' + this.rootPanel.FindChildInLayoutFile(dropDownID).GetSelected().GetAttributeString('value', ''));
    }
  };
})();

(function () {
  var createNavbar = function () {
    var navbar = $.CreatePanel('Panel', $.Osiris.rootPanel, '', {
      class: "content-navbar__tabs content-navbar__tabs--noflow"
    });

    var leftContainer = $.CreatePanel('Panel', navbar, '', {
      style: "horizontal-align: left; flow-children: right; height: 100%; padding-left: 5px;"
    });

    var unloadButton = $.CreatePanel('Button', leftContainer, 'UnloadButton', {
      class: "content-navbar__tabs__btn",
      onactivate: "UiToolkitAPI.ShowGenericPopupOneOptionCustomCancelBgStyle('Unload Osiris', 'Are you sure you want to unload Osiris?', '', 'UNLOAD', function() { $.Osiris.goHome(); $.Osiris.addCommand('unload'); }, 'RETURN', function() {}, 'dim');"
    });

    unloadButton.SetPanelEvent('onmouseover', function () { UiToolkitAPI.ShowTextTooltip('UnloadButton', 'Unload'); });
    unloadButton.SetPanelEvent('onmouseout', function () { UiToolkitAPI.HideTextTooltip(); });

    $.CreatePanel('Image', unloadButton, '', {
      src: "s2r://panorama/images/icons/ui/cancel.vsvg",
      texturewidth: "24",
      class: "negativeColor"
    });

    var centerContainer = $.CreatePanel('Panel', navbar, '', {
      class: "content-navbar__tabs__center-container",
    });

    var hudTabButton = $.CreatePanel('RadioButton', centerContainer, 'hud_button', {
      group: "SettingsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.Osiris.navigateToTab('hud');"
    });

    $.CreatePanel('Label', hudTabButton, '', { text: "HUD" });

    var visualsTabButton = $.CreatePanel('RadioButton', centerContainer, 'visuals_button', {
      group: "SettingsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.Osiris.navigateToTab('visuals');"
    });

    $.CreatePanel('Label', visualsTabButton, '', { text: "Visuals" });
    
    var soundTabButton = $.CreatePanel('RadioButton', centerContainer, 'sound_button', {
      group: "SettingsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.Osiris.navigateToTab('sound');"
    });

    $.CreatePanel('Label', soundTabButton, '', { text: "Sound" });
  };

  createNavbar();

  var settingContent = $.CreatePanel('Panel', $.Osiris.rootPanel, 'SettingsMenuContent', {
    class: "full-width full-height"
  });

  var createTab = function(tabName) {
    var tab = $.CreatePanel('Panel', settingContent, tabName, {
      useglobalcontext: "true",
      class: "SettingsMenuTab"
    });

    var content = $.CreatePanel('Panel', tab, '', {
      class: "SettingsMenuTabContent vscroll"
    });
  
    return content;
  };

  var createSection = function(tab, sectionName) {
    var background = $.CreatePanel('Panel', tab, '', {
      class: "SettingsBackground"
    });

    var titleContainer = $.CreatePanel('Panel', background, '', {
      class: "SettingsSectionTitleContianer"
    });

    $.CreatePanel('Label', titleContainer, '', {
      class: "SettingsSectionTitleLabel",
      text: sectionName
    });

    var content = $.CreatePanel('Panel', background, '', {
      class: "top-bottom-flow full-width"
    });

    return content;
  };

  var createEnableDisableDropDown = function (parent, labelText, section, feature, enableString, disableString) {
    var container = $.CreatePanel('Panel', parent, '', {
      class: "SettingsMenuDropdownContainer"
    });

    $.CreatePanel('Label', container, '', {
      class: "half-width",
      text: labelText
    });

    var dropdown = $.CreatePanel('CSGOSettingsEnumDropDown', container, feature, {
      class: "PopupButton White",
      oninputsubmit: `$.Osiris.dropDownUpdated('${section}', '${feature}');`
    });

    dropdown.AddOption($.CreatePanel('Label', dropdown, '1', {
      value: "1",
      text: enableString
    }));

    dropdown.AddOption($.CreatePanel('Label', dropdown, '0', {
      value: "0",
      text: disableString
    }));

    dropdown.SetSelectedIndex(1);
    dropdown.RefreshDisplay();
  };

  var createOnOffDropDown = function (parent, labelText, section, feature) {
    createEnableDisableDropDown(parent, labelText, section, feature, "On", "Off");
  };

  var createYesNoDropDown = function (parent, labelText, section, feature) {
    createEnableDisableDropDown(parent, labelText, section, feature, "Yes", "No");
  };

  var hud = createTab('hud');
  var bomb = createSection(hud, 'Bomb');
  createOnOffDropDown(bomb, "Bomb Timer", 'hud', 'bomb_timer');
  $.CreatePanel('Panel', bomb, '', { class: "horizontal-separator" });
  createOnOffDropDown(bomb, "Defusing Alert", 'hud', 'defusing_alert');
  var killfeed = createSection(hud, 'Killfeed');
  createYesNoDropDown(killfeed, "Preserve My Killfeed", 'hud', 'preserve_killfeed');

  var visuals = createTab('visuals');
  var weapons = createSection(visuals, 'Weapons');
  createYesNoDropDown(weapons, "Remove Scope Overlay", 'visuals', 'remove_scope_overlay');

  var sound = createTab('sound');
  var visualization = createSection(sound, 'Visualization');
  createYesNoDropDown(visualization, "Visualize Player Footsteps", 'sound', 'visualize_player_footsteps');
  $.CreatePanel('Panel', visualization, '', { class: "horizontal-separator" });
  createYesNoDropDown(visualization, "Visualize Bomb Plant", 'sound', 'visualize_bomb_plant');
  $.CreatePanel('Panel', visualization, '', { class: "horizontal-separator" });
  createYesNoDropDown(visualization, "Visualize Bomb Beep", 'sound', 'visualize_bomb_beep');
  $.CreatePanel('Panel', visualization, '', { class: "horizontal-separator" });
  createYesNoDropDown(visualization, "Visualize Bomb Defuse", 'sound', 'visualize_bomb_defuse');

  $.Osiris.navigateToTab('hud');
})();
)"
