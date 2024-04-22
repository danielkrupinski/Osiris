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

  var createDropDown = function (parent, labelText, section, feature, options, defaultIndex = 1) {
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

    for (let i = 0; i < options.length; ++i) {
      dropdown.AddOption($.CreatePanel('Label', dropdown, i, {
      value: i,
      text: options[i]
      }));
    }

    dropdown.SetSelectedIndex(defaultIndex);
    dropdown.RefreshDisplay();
  };

  var createOnOffDropDown = function (parent, labelText, section, feature) {
    createDropDown(parent, labelText, section, feature, ["On", "Off"]);
  };

  var createYesNoDropDown = function (parent, labelText, section, feature) {
    createDropDown(parent, labelText, section, feature, ["Yes", "No"]);
  };

  var hud = createTab('hud');
  var bomb = createSection(hud, 'Bomb');
  createYesNoDropDown(bomb, "Show Bomb Explosion Countdown And Site", 'hud', 'bomb_timer');
  $.CreatePanel('Panel', bomb, '', { class: "horizontal-separator" });
  createYesNoDropDown(bomb, "Show Bomb Defuse Countdown", 'hud', 'defusing_alert');
  var killfeed = createSection(hud, 'Killfeed');
  createYesNoDropDown(killfeed, "Preserve My Killfeed During The Round", 'hud', 'preserve_killfeed');

  var visuals = createTab('visuals');

  var playerInfo = createSection(visuals, 'Player Information Through Walls');
  createDropDown(playerInfo, "Enabled", 'visuals', 'player_information_through_walls', ['Enemies', 'All Players', 'Off'], 2);
  $.CreatePanel('Panel', playerInfo, '', { class: "horizontal-separator" });
  createYesNoDropDown(playerInfo, "Show Player Position", 'visuals', 'player_info_position');
  $.CreatePanel('Panel', playerInfo, '', { class: "horizontal-separator" });
  createYesNoDropDown(playerInfo, "Show Player Health", 'visuals', 'player_info_health');
  $.CreatePanel('Panel', playerInfo, '', { class: "horizontal-separator" });
  createDropDown(playerInfo, "Player Health Text Color", 'visuals', 'player_info_health_color', ['Health-based', 'White'], 0);
  $.CreatePanel('Panel', playerInfo, '', { class: "horizontal-separator" });
  createYesNoDropDown(playerInfo, "Show Player Active Weapon", 'visuals', 'player_info_weapon');

  var sound = createTab('sound');
  
  var playerSoundVisualization = createSection(sound, 'Player Sound Visualization');
  $.CreatePanel('Panel', playerSoundVisualization, '', { class: "horizontal-separator" });
  createYesNoDropDown(playerSoundVisualization, "Visualize Player Footstep Sound", 'sound', 'visualize_player_footsteps');

  var bombSoundVisualization = createSection(sound, 'Bomb Sound Visualization');
  createYesNoDropDown(bombSoundVisualization, "Visualize Bomb Plant Sound", 'sound', 'visualize_bomb_plant');
  $.CreatePanel('Panel', bombSoundVisualization, '', { class: "horizontal-separator" });
  createYesNoDropDown(bombSoundVisualization, "Visualize Bomb Beep Sound", 'sound', 'visualize_bomb_beep');
  $.CreatePanel('Panel', bombSoundVisualization, '', { class: "horizontal-separator" });
  createYesNoDropDown(bombSoundVisualization, "Visualize Bomb Defuse Sound", 'sound', 'visualize_bomb_defuse');

  var weaponSoundVisualization = createSection(sound, 'Weapon Sound Visualization');
  createYesNoDropDown(weaponSoundVisualization, "Visualize Weapon Scope Sound", 'sound', 'visualize_scope_sound');
  $.CreatePanel('Panel', weaponSoundVisualization, '', { class: "horizontal-separator" });
  createYesNoDropDown(weaponSoundVisualization, "Visualize Weapon Reload Sound", 'sound', 'visualize_reload_sound');

  $.Osiris.navigateToTab('hud');
})();
)"
