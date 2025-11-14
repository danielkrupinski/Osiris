u8R"(
$.Osiris = (function () {
  var activeTab;
  var activeSubTab = {};

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
    navigateToSubTab: function (tabID, subTabID) {
      if (activeSubTab[tabID] === subTabID)
        return;

      if (activeSubTab[tabID]) {
        var panelToHide = this.rootPanel.FindChildInLayoutFile(activeSubTab[tabID]);
        panelToHide.RemoveClass('Active');
      }

      this.rootPanel.FindChildInLayoutFile(subTabID + '_button').checked = true;

      activeSubTab[tabID] = subTabID;
      var activePanel = this.rootPanel.FindChildInLayoutFile(subTabID);
      activePanel.AddClass('Active');
      activePanel.visible = true;
      activePanel.SetReadyForDisplay(true);
    },
    sliderUpdated: function (tabID, sliderID, slider) {
      this.addCommand('set', tabID + '/' + sliderID + '/' + Math.floor(slider.value));
    },
    sliderTextEntryUpdated: function (tabID, sliderID, panel) {
      this.addCommand('set', tabID + '/' + sliderID + '/' + panel.text);
    }
  };
})();

(function () {
  var createNavbar = function () {
    var navbar = $.CreatePanel('Panel', $.Osiris.rootPanel, '', {
      class: "content-navbar__tabs content-navbar__tabs--noflow"
    });

    var leftContainer = $.CreatePanel('Panel', navbar, '', {
      style: "horizontal-align: left; flow-children: right; height: 100%; margin-left: 15px;"
    });

    var activeCfgNameLabel = $.CreatePanel('Label', leftContainer, 'ActiveConfigName', {
      text: "default.cfg"
    });

    activeCfgNameLabel.SetPanelEvent('onmouseover', function () { UiToolkitAPI.ShowTextTooltip('ActiveConfigName', 'Active config file. Changes are saved automatically.'); });
    activeCfgNameLabel.SetPanelEvent('onmouseout', function () { UiToolkitAPI.HideTextTooltip(); });

    var restoreDefaultsButton = $.CreatePanel('Button', leftContainer, 'RestoreDefaultsButton', {
      class: "content-navbar__tabs__btn",
      style: "margin-left: 5px;",
      onactivate: "UiToolkitAPI.ShowGenericPopupOneOptionCustomCancelBgStyle('Restore Defaults', 'Are you sure you want to restore default settings in the active config file (default.cfg)?', '', 'RESTORE DEFAULTS', function() { $.Osiris.addCommand('restore_defaults'); }, 'RETURN', function() {}, 'dim');"
    });

    restoreDefaultsButton.SetPanelEvent('onmouseover', function () { UiToolkitAPI.ShowTextTooltip('RestoreDefaultsButton', 'Restore defaults'); });
    restoreDefaultsButton.SetPanelEvent('onmouseout', function () { UiToolkitAPI.HideTextTooltip(); });

    $.CreatePanel('Image', restoreDefaultsButton, '', {
      src: "s2r://panorama/images/icons/ui/recent.vsvg",
      texturewidth: "24"
    });
  
    var centerContainer = $.CreatePanel('Panel', navbar, '', {
      class: "content-navbar__tabs__center-container",
    });

    var combatTabButton = $.CreatePanel('RadioButton', centerContainer, 'combat_button', {
      group: "SettingsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.Osiris.navigateToTab('combat');"
    });

    $.CreatePanel('Label', combatTabButton, '', { text: "Combat" });

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

    var rightContainer = $.CreatePanel('Panel', navbar, '', {
        style: "horizontal-align: right; flow-children: right; height: 100%; margin-right: 70px;"
    });

    var unloadButton = $.CreatePanel('Button', rightContainer, 'UnloadButton', {
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
  };

  var createVisualsNavbar = function () {
    var navbar = $.CreatePanel('Panel', $.Osiris.rootPanel.FindChildInLayoutFile('visuals'), '', {
      class: "content-navbar__tabs content-navbar__tabs--dark content-navbar__tabs--noflow"
    });

    var centerContainer = $.CreatePanel('Panel', navbar, '', {
      class: "content-navbar__tabs__center-container",
    });

    var playerInfoTabButton = $.CreatePanel('RadioButton', centerContainer, 'player_info_button', {
      group: "VisualsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.Osiris.navigateToSubTab('visuals', 'player_info');"
    });

    $.CreatePanel('Label', playerInfoTabButton, '', { text: "Player Info In World" });

    var outlineGlowTabButton = $.CreatePanel('RadioButton', centerContainer, 'outline_glow_button', {
      group: "VisualsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.Osiris.navigateToSubTab('visuals', 'outline_glow');"
    });

    $.CreatePanel('Label', outlineGlowTabButton, '', { text: "Outline Glow" });

    var modelGlowTabButton = $.CreatePanel('RadioButton', centerContainer, 'model_glow_button', {
        group: "VisualsNavBar",
        class: "content-navbar__tabs__btn",
        onactivate: "$.Osiris.navigateToSubTab('visuals', 'model_glow');"
    });

    $.CreatePanel('Label', modelGlowTabButton, '', { text: "Model Glow" });
  
    var viewmodelTabButton = $.CreatePanel('RadioButton', centerContainer, 'viewmodel_button', {
        group: "VisualsNavBar",
        class: "content-navbar__tabs__btn",
        onactivate: "$.Osiris.navigateToSubTab('visuals', 'viewmodel');"
    });

    $.CreatePanel('Label', viewmodelTabButton, '', { text: "Viewmodel" });
  };

  var createCombatNavbar = function () {
    var navbar = $.CreatePanel('Panel', $.Osiris.rootPanel.FindChildInLayoutFile('combat'), '', {
      class: "content-navbar__tabs content-navbar__tabs--dark content-navbar__tabs--noflow"
    });

    var centerContainer = $.CreatePanel('Panel', navbar, '', {
      class: "content-navbar__tabs__center-container",
    });

    var sniperRiflesTabButton = $.CreatePanel('RadioButton', centerContainer, 'sniper_rifles_button', {
      group: "CombatNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.Osiris.navigateToSubTab('combat', 'sniper_rifles');"
    });

    $.CreatePanel('Label', sniperRiflesTabButton, '', { text: "Sniper rifles" });
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

  var createVisualsTab = function() {
    var tab = $.CreatePanel('Panel', settingContent, 'visuals', {
      useglobalcontext: "true",
      class: "SettingsMenuTab"
    });

    createVisualsNavbar();

    var content = $.CreatePanel('Panel', tab, '', {
      class: "full-width full-height"
    });
  
    return content;
  };

  var createCombatTab = function() {
    var tab = $.CreatePanel('Panel', settingContent, 'combat', {
      useglobalcontext: "true",
      class: "SettingsMenuTab"
    });

    createCombatNavbar();

    var content = $.CreatePanel('Panel', tab, '', {
      class: "full-width full-height"
    });
  
    return content;
  };

  var createSubTab = function(tab, subTabName) {
    var subTab = $.CreatePanel('Panel', tab, subTabName, {
      useglobalcontext: "true",
      class: "SettingsMenuTab"
    });

    var content = $.CreatePanel('Panel', subTab, '', {
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

  var createDropDown = function (parent, labelText, section, feature, options) {
    var container = $.CreatePanel('Panel', parent, '', {
      class: "SettingsMenuDropdownContainer"
    });

    $.CreatePanel('Label', container, '', {
      class: "half-width",
      text: labelText
    });

    var dropdown = $.CreatePanel('CSGOSettingsEnumDropDown', container, feature, { class: "PopupButton White" });

    for (let i = 0; i < options.length; ++i) {
      dropdown.AddOption($.CreatePanel('Label', dropdown, i, {
      value: i,
      text: options[i]
      }));
    }
  };

  var createOnOffDropDown = function (parent, labelText, section, feature) {
    createDropDown(parent, labelText, section, feature, ["On", "Off"]);
  };

  var createYesNoDropDown = function (parent, labelText, section, feature) {
    createDropDown(parent, labelText, section, feature, ["Yes", "No"]);
  };

  var separator = function (parent) {
    $.CreatePanel('Panel', parent, '', { class: "horizontal-separator" });
  };

  var makeFauxItemId = function (defIndex, paintKitId) {
    return (BigInt(0xF000000000000000) | BigInt(paintKitId << 16) | BigInt(defIndex))
  };

  var createPlayerModelGlowPreview = function (parent, id, labelId, playerModel, itemId) {
    var container = $.CreatePanel('Panel', parent, '', { style: 'flow-children: none;' });
    var previewPanel = $.CreatePanel('MapPlayerPreviewPanel', container, id, {
      map: "ui/buy_menu",
      camera: "cam_loadoutmenu_ct",
      "require-composition-layer": true,
      playermodel: playerModel,
      playername: "vanity_character",
      animgraphcharactermode: "buy-menu",
      player: true,
      mouse_rotate: false,
      sync_spawn_addons: true,
      "transparent-background": true,
      "pin-fov": "vertical",
      csm_split_plane0_distance_override: "250.0",
      style: "y: 5px; vertical-align: top; width: 300px; height: 300px; horizontal-align: center;"
    });
    previewPanel.EquipPlayerWithItem(itemId);
    $.CreatePanel('Label', container, labelId, { style: 'vertical-align: top; horizontal-align: center;' });
  };

  var createGrenadeModelGlowPreview = function (parent, id, defIndex) {
    var container = $.CreatePanel('Panel', parent, '', { style: 'width: 80px; overflow: clip;' });
    var panel = $.CreatePanel('MapItemPreviewPanel', container, id, {
      map: "ui/xpshop_item",
      camera: "camera_weapon_4",
      "require-composition-layer": true,
      player: false,
      initial_entity: "item",
      mouse_rotate: false,
      sync_spawn_addons: true,
      "transparent-background": true,
      "pin-fov": "vertical",
      style: "x: -10px; horizontal-align: center; width: 200px; height: 80px;"
    });
    panel.SetItemItemId(makeFauxItemId(defIndex, 0), {});
  };
)"
// split the string literal because MSVC does not support string literals longer than 16k chars - error C2026
u8R"(
  var createSlider = function (parent, name, id, min, max) {
    var container = $.CreatePanel('Panel', parent, '', {
      class: "SettingsMenuDropdownContainer"
    });

    $.CreatePanel('Label', container, '', {
      class: "half-width",
      text: name
    });

    var sliderContainer = $.CreatePanel('Panel', container, id, {
      style: "vertical-align: center; horizontal-align: right; flow-children: right; margin-right: 8px;"
    });

    var slider = $.CreatePanel('Slider', sliderContainer, '', {
      class: "HorizontalSlider",
      style: "width: 200px; vertical-align: center;",
      direction: "horizontal"
    });

    slider.SetPanelEvent('onvaluechanged', function () { $.Osiris.sliderUpdated('visuals', id, slider); });
    slider.min = min;
    slider.max = max;
    slider.increment = 1.0;

    var textEntry = $.CreatePanel('TextEntry', sliderContainer, id + '_text', {
      maxchars: "3",
      textmode: "numeric",
      style: "width: 75px; margin-left: 10px; padding-left: 10px; text-align: center; font-size: 20px; color: #ccccccff; font-weight: bold; font-family: Stratum2, notosans, 'Arial Unicode MS'; border: 2px solid #cccccc15;"
    });

    textEntry.SetPanelEvent('ontextentrysubmit', function () { $.Osiris.sliderTextEntryUpdated('visuals', `${id}_text`, textEntry); });
    textEntry.SetPanelEvent('onfocus', function () { textEntry.style.backgroundColor = 'gradient(linear, 100% 0%, 0% 0%, from(#00000080), color-stop(0, #00000060), to(#00000080))'; });
    textEntry.SetPanelEvent('onblur', function () { textEntry.style.backgroundColor = 'none'; });
    textEntry.SetPanelEvent('onmouseover', function () { if (!textEntry.BHasKeyFocus()) textEntry.style.backgroundColor = 'gradient(linear, 100% 0%, 0% 0%, from(#000000ff), color-stop(0, #00000000), to(#00000050));'; });
    textEntry.SetPanelEvent('onmouseout', function () { if (!textEntry.BHasKeyFocus()) textEntry.style.backgroundColor = 'none'; });
  }

  var createHueSlider = function (parent, name, id, min, max) {
    var container = $.CreatePanel('Panel', parent, '', {
      class: "SettingsMenuDropdownContainer"
    });

    $.CreatePanel('Label', container, '', {
      class: "half-width",
      text: name
    });

    var sliderContainer = $.CreatePanel('Panel', container, id, {
      style: "vertical-align: center; horizontal-align: right; flow-children: right; margin-right: 8px;"
    });

    var slider = $.CreatePanel('Slider', sliderContainer, '', {
      class: "HorizontalSlider",
      style: "width: 200px; vertical-align: center;",
      direction: "horizontal"
    });

    slider.min = min;
    slider.max = max;
    slider.increment = 1.0;

    var textEntry = $.CreatePanel('TextEntry', sliderContainer, id + '_text', {
      maxchars: "3",
      textmode: "numeric",
      style: "width: 75px; margin-left: 10px; padding-left: 10px; text-align: center; font-size: 20px; color: #ccccccff; font-weight: bold; font-family: Stratum2, notosans, 'Arial Unicode MS'; border: 2px solid #cccccc15;"
    });

    textEntry.SetPanelEvent('onfocus', function () { textEntry.style.backgroundColor = 'gradient(linear, 100% 0%, 0% 0%, from(#00000080), color-stop(0, #00000060), to(#00000080))'; });
    textEntry.SetPanelEvent('onblur', function () { textEntry.style.backgroundColor = 'none'; });
    textEntry.SetPanelEvent('onmouseover', function () { if (!textEntry.BHasKeyFocus()) textEntry.style.backgroundColor = 'gradient(linear, 100% 0%, 0% 0%, from(#000000ff), color-stop(0, #00000000), to(#00000050));'; });
    textEntry.SetPanelEvent('onmouseout', function () { if (!textEntry.BHasKeyFocus()) textEntry.style.backgroundColor = 'none'; });

    $.CreatePanel('Panel', sliderContainer, id + '_color', {
        style: "border: 2px solid #000000a0; border-radius: 5px; margin-left: 10px; width: 25px; vertical-align: center; height: 25px;"
    });
  }
)"
// split the string literal because MSVC does not support string literals longer than 16k chars - error C2026
u8R"(
  var combat = createCombatTab();
  var sniperRiflesTab = createSubTab(combat, 'sniper_rifles');
  var noScope = createSection(sniperRiflesTab, 'No scope');
  separator(noScope);
  createYesNoDropDown(noScope, "Visualize Inaccuracy When Not Using a Scope", 'combat', 'no_scope_inacc_vis');

  $.Osiris.navigateToSubTab('combat', 'sniper_rifles');

  var hud = createTab('hud');
  
  var bomb = createSection(hud, 'Bomb');
  createYesNoDropDown(bomb, "Show Bomb Explosion Countdown And Site", 'hud', 'bomb_timer');
  separator(bomb);
  createYesNoDropDown(bomb, "Show Bomb Defuse Countdown", 'hud', 'defusing_alert');
  separator(bomb);
  createYesNoDropDown(bomb, "Show Bomb Plant Alert", 'hud', 'bomb_plant_alert');

  var killfeed = createSection(hud, 'Killfeed');
  separator(killfeed);
  createYesNoDropDown(killfeed, "Preserve My Killfeed During The Round", 'hud', 'preserve_killfeed');

  var time = createSection(hud, 'Time');
  separator(time);
  createYesNoDropDown(time, "Show Post-round Timer", 'hud', 'postround_timer');

  var visuals = createVisualsTab();

  var playerInfoTab = createSubTab(visuals, 'player_info');

  var playerInfo = createSection(playerInfoTab, 'Player Info In World');
  createDropDown(playerInfo, "Master Switch", 'visuals', 'player_information_through_walls', ['Enemies', 'All Players', 'Off']);

  var playerPosition = createSection(playerInfoTab, 'Player Position');
  createYesNoDropDown(playerPosition, "Show Player Position Arrow", 'visuals', 'player_info_position');
  separator(playerPosition);
  createDropDown(playerPosition, "Player Position Arrow Color", 'visuals', 'player_info_position_color', ['Player / Team Color', 'Team Color']);

  var playerHealth = createSection(playerInfoTab, 'Player Health');
  createYesNoDropDown(playerHealth, "Show Player Health", 'visuals', 'player_info_health');
  separator(playerHealth);
  createDropDown(playerHealth, "Player Health Text Color", 'visuals', 'player_info_health_color', ['Health-based', 'White']);

  var playerWeapon = createSection(playerInfoTab, 'Player Weapon');
  createYesNoDropDown(playerWeapon, "Show Player Active Weapon Icon", 'visuals', 'player_info_weapon');
  separator(playerWeapon);
  createYesNoDropDown(playerWeapon, "Show Player Active Weapon Ammo", 'visuals', 'player_info_weapon_clip');
  separator(playerWeapon);
  createYesNoDropDown(playerWeapon, 'Show Bomb Carrier Icon', 'visuals', 'player_info_bomb_carrier');
  separator(playerWeapon);
  createYesNoDropDown(playerWeapon, 'Show Bomb Planting Icon', 'visuals', 'player_info_bomb_planting');

  var playerIcons = createSection(playerInfoTab, 'Icons');
  createYesNoDropDown(playerIcons, "Show Defuse Icon", 'visuals', 'player_info_defuse');
  separator(playerIcons);
  createYesNoDropDown(playerIcons, 'Show Picking Up Hostage Icon', 'visuals', 'player_info_hostage_pickup');
  separator(playerIcons);
  createYesNoDropDown(playerIcons, 'Show Rescuing Hostage Icon', 'visuals', 'player_info_hostage_rescue');
  separator(playerIcons);
  createYesNoDropDown(playerIcons, 'Show Blinded By Flashbang Icon', 'visuals', 'player_info_blinded');

  var outlineGlowTab = createSubTab(visuals, 'outline_glow');

  var outlineGlow = createSection(outlineGlowTab, 'Outline Glow');
  createOnOffDropDown(outlineGlow, "Master Switch", 'visuals', 'outline_glow_enable');

  var playerOutlineGlow = createSection(outlineGlowTab, 'Players');
  createDropDown(playerOutlineGlow, "Glow Players", 'visuals', 'player_outline_glow', ['Enemies', 'All Players', 'Off']);
  separator(playerOutlineGlow);
  createDropDown(playerOutlineGlow, "Player Glow Color", 'visuals', 'player_outline_glow_color', ['Player / Team Color', 'Team Color', 'Health-based', 'Enemy / Ally']);
  separator(playerOutlineGlow);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Player Blue Hue", 'player_outline_glow_blue_hue', 191, 240);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Player Green Hue", 'player_outline_glow_green_hue', 110, 140);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Player Yellow Hue", 'player_outline_glow_yellow_hue', 47, 60);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Player Orange Hue", 'player_outline_glow_orange_hue', 11, 20);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Player Purple Hue", 'player_outline_glow_purple_hue', 250, 280);
  separator(playerOutlineGlow);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Team T Hue", 'player_outline_glow_t_hue', 30, 40);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Team CT Hue", 'player_outline_glow_ct_hue', 210, 230);
  separator(playerOutlineGlow);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "High Health Hue", 'player_outline_glow_high_hp_hue', 0, 359);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Low Health Hue", 'player_outline_glow_low_hp_hue', 0, 359);
  separator(playerOutlineGlow);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Enemy Hue", 'player_outline_glow_enemy_hue', 0, 359);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Ally Hue", 'player_outline_glow_ally_hue', 0, 359);

  var weaponOutlineGlow = createSection(outlineGlowTab, 'Weapons');
  createYesNoDropDown(weaponOutlineGlow, "Glow Weapons on Ground Nearby", 'visuals', 'weapon_outline_glow');
  separator(weaponOutlineGlow);
  createYesNoDropDown(weaponOutlineGlow, "Glow Grenade Projectiles", 'visuals', 'grenade_proj_outline_glow');
  separator(weaponOutlineGlow);
  separator(weaponOutlineGlow);
  createHueSlider(weaponOutlineGlow, "Flashbang Hue", 'outline_glow_flashbang_hue', 191, 250);
  separator(weaponOutlineGlow);
  createHueSlider(weaponOutlineGlow, "HE Grenade Hue", 'outline_glow_hegrenade_hue', 300, 359);
  separator(weaponOutlineGlow);
  createHueSlider(weaponOutlineGlow, "Smoke Grenade Hue", 'outline_glow_smoke_hue', 110, 140);
  separator(weaponOutlineGlow);
  createHueSlider(weaponOutlineGlow, "Molotov / Incendiary Grenade Hue", 'outline_glow_molotov_hue', 20, 60);

  var bombAndDefuseKitOutlineGlow = createSection(outlineGlowTab, 'Bomb & Defuse Kit');
  createYesNoDropDown(bombAndDefuseKitOutlineGlow, "Glow Dropped Bomb", 'visuals', 'dropped_bomb_outline_glow');
  separator(bombAndDefuseKitOutlineGlow);
  createYesNoDropDown(bombAndDefuseKitOutlineGlow, "Glow Ticking Bomb", 'visuals', 'ticking_bomb_outline_glow');
  separator(bombAndDefuseKitOutlineGlow);
  createYesNoDropDown(bombAndDefuseKitOutlineGlow, "Glow Defuse Kits on Ground Nearby", 'visuals', 'defuse_kit_outline_glow');
  separator(bombAndDefuseKitOutlineGlow);
  separator(bombAndDefuseKitOutlineGlow);
  createHueSlider(bombAndDefuseKitOutlineGlow, "Dropped Bomb Hue", 'outline_glow_dropped_bomb_hue', 0, 359);
  separator(bombAndDefuseKitOutlineGlow);
  createHueSlider(bombAndDefuseKitOutlineGlow, "Ticking Bomb Hue", 'outline_glow_ticking_bomb_hue', 0, 359);
  separator(bombAndDefuseKitOutlineGlow);
  createHueSlider(bombAndDefuseKitOutlineGlow, "Defuse Kit Hue", 'outline_glow_defuse_kit_hue', 0, 359);

  var hostageOutlineGlow = createSection(outlineGlowTab, 'Hostages');
  createYesNoDropDown(hostageOutlineGlow, "Glow Hostages", 'visuals', 'hostage_outline_glow');
  separator(hostageOutlineGlow);
  createHueSlider(hostageOutlineGlow, "Hostage Hue", 'outline_glow_hostage_hue', 0, 359);

  var _modelGlowTab = createSubTab(visuals, 'model_glow');
  _modelGlowTab.style.overflow = 'squish squish';
  _modelGlowTab.style.flowChildren = 'right';

  var modelGlowPreview = $.CreatePanel('Panel', _modelGlowTab, '', { style: 'flow-children: down;' });
  $.CreatePanel('Label', modelGlowPreview, '', { style: 'vertical-align: top; horizontal-align: center; font-size: 40;', text: 'Preview' });
  var playerModelGlowPreview = $.CreatePanel('Panel', modelGlowPreview, '', { style: 'flow-children: right; margin-top: 20px;' });
  createPlayerModelGlowPreview(playerModelGlowPreview, 'ModelGlowPreviewPlayerTT', 'ModelGlowPreviewPlayerTTLabel', 'characters/models/tm_professional/tm_professional_varf.vmdl', makeFauxItemId(7, 921));
  createPlayerModelGlowPreview(playerModelGlowPreview, 'ModelGlowPreviewPlayerCT', 'ModelGlowPreviewPlayerCTLabel', 'characters/models/ctm_st6/ctm_st6_variante.vmdl', makeFauxItemId(9, 819));

  $.CreatePanel('Label', modelGlowPreview, '', { style: 'horizontal-align: center; margin-top: 20px;', text: 'Weapons on the Ground' });

  var weaponModelGlowPreview = $.CreatePanel('Panel', modelGlowPreview, '', { style: 'flow-children: right;' });

  var modelGlowPreviewWeapon = $.CreatePanel('MapItemPreviewPanel', weaponModelGlowPreview, 'ModelGlowPreviewWeapon', {
    map: "ui/xpshop_item",
    camera: "camera_weapon_0",
    "require-composition-layer": true,
    player: false,
    initial_entity: "item",
    mouse_rotate: false,
    sync_spawn_addons: true,
    "transparent-background": true,
    "pin-fov": "vertical",
    style: "width: 400px; height: 160px;"
  });
  modelGlowPreviewWeapon.SetItemItemId(makeFauxItemId(16, 255), {});

  var grenadeModelGlowPreview = $.CreatePanel('Panel', weaponModelGlowPreview, '', { style: 'flow-children: down;' });

  var grenadeModelGlowPreviewRow1 = $.CreatePanel('Panel', grenadeModelGlowPreview, '', { style: 'flow-children: right;' });
  createGrenadeModelGlowPreview(grenadeModelGlowPreviewRow1, 'ModelGlowPreviewFlashbang', 43);
  createGrenadeModelGlowPreview(grenadeModelGlowPreviewRow1, 'ModelGlowPreviewHEGrenade', 44);

  var grenadeModelGlowPreviewRow2 = $.CreatePanel('Panel', grenadeModelGlowPreview, '', { style: 'flow-children: right;' });
  createGrenadeModelGlowPreview(grenadeModelGlowPreviewRow2, 'ModelGlowPreviewSmoke', 45);
  createGrenadeModelGlowPreview(grenadeModelGlowPreviewRow2, 'ModelGlowPreviewIncendiary', 48);

  var modelGlowTab = $.CreatePanel('Panel', _modelGlowTab, '', { style: 'flow-children: down; margin-right: 40px; overflow: squish scroll;' });

  var modelGlow = createSection(modelGlowTab, 'Model Glow');
  createOnOffDropDown(modelGlow, "Master Switch", 'visuals', 'model_glow_enable');

  var playerModelGlow = createSection(modelGlowTab, 'Players');
  createDropDown(playerModelGlow, "Glow Player Models", 'visuals', 'player_model_glow', ['Enemies', 'All Players', 'Off']);
  separator(playerModelGlow);
  createDropDown(playerModelGlow, "Player Model Glow Color Mode", 'visuals', 'player_model_glow_color', ['Player / Team Color', 'Team Color', 'Health-based', 'Enemy / Ally']);
  separator(playerModelGlow);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Player Blue Hue", 'player_model_glow_blue_hue', 191, 240);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Player Green Hue", 'player_model_glow_green_hue', 110, 140);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Player Yellow Hue", 'player_model_glow_yellow_hue', 47, 60);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Player Orange Hue", 'player_model_glow_orange_hue', 11, 20);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Player Purple Hue", 'player_model_glow_purple_hue', 250, 280);
  separator(playerModelGlow);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Team T Hue", 'player_model_glow_t_hue', 30, 40);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Team CT Hue", 'player_model_glow_ct_hue', 210, 230);
  separator(playerModelGlow);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "High Health Hue", 'player_model_glow_high_hp_hue', 0, 359);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Low Health Hue", 'player_model_glow_low_hp_hue', 0, 359);
  separator(playerModelGlow);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Enemy Hue", 'player_model_glow_enemy_hue', 0, 359);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Ally Hue", 'player_model_glow_ally_hue', 0, 359);
)"
// split the string literal because MSVC does not support string literals longer than 16k chars - error C2026
u8R"(
  var weaponModelGlow = createSection(modelGlowTab, 'Weapons');
  createYesNoDropDown(weaponModelGlow, "Glow Weapon Models on Ground", 'visuals', 'weapon_model_glow');
  separator(weaponModelGlow);
  createYesNoDropDown(weaponModelGlow, "Glow Grenade Projectile Models", 'visuals', 'grenade_proj_model_glow');
  separator(weaponModelGlow);
  separator(weaponModelGlow);
  createHueSlider(weaponModelGlow, "Flashbang Hue", 'model_glow_flashbang_hue', 191, 250);
  separator(weaponModelGlow);
  createHueSlider(weaponModelGlow, "HE Grenade Hue", 'model_glow_hegrenade_hue', 300, 359);
  separator(weaponModelGlow);
  createHueSlider(weaponModelGlow, "Smoke Grenade Hue", 'model_glow_smoke_hue', 110, 140);
  separator(weaponModelGlow);
  createHueSlider(weaponModelGlow, "Molotov / Incendiary Grenade Hue", 'model_glow_molotov_hue', 20, 60);

  var bombModelGlow = createSection(modelGlowTab, 'Bomb & Defuse Kit');
  createYesNoDropDown(bombModelGlow, "Glow Dropped Bomb Model", 'visuals', 'dropped_bomb_model_glow');
  separator(bombModelGlow);
  createYesNoDropDown(bombModelGlow, "Glow Ticking Bomb Model", 'visuals', 'ticking_bomb_model_glow');
  separator(bombModelGlow);
  createYesNoDropDown(bombModelGlow, "Glow Defuse Kit Models on Ground", 'visuals', 'defuse_kit_model_glow');
  separator(bombModelGlow);
  separator(bombModelGlow);
  createHueSlider(bombModelGlow, "Dropped Bomb Hue", 'model_glow_dropped_bomb_hue', 0, 359);
  separator(bombModelGlow);
  createHueSlider(bombModelGlow, "Ticking Bomb Hue", 'model_glow_ticking_bomb_hue', 0, 359);
  separator(bombModelGlow);
  createHueSlider(bombModelGlow, "Defuse Kit Hue", 'model_glow_defuse_kit_hue', 0, 359);

  var _viewmodelTab = createSubTab(visuals, 'viewmodel');
  _viewmodelTab.style.overflow = 'squish squish';
  _viewmodelTab.style.flowChildren = 'right';

  var viewmodelPreviewContainer = $.CreatePanel('Panel', _viewmodelTab, '', { style: 'flow-children: down;' });
  $.CreatePanel('Label', viewmodelPreviewContainer, '', { style: 'vertical-align: top; horizontal-align: center; font-size: 40;', text: 'Preview' });

  var viewmodelPreview = $.CreatePanel('MapItemPreviewPanel', viewmodelPreviewContainer, 'ViewmodelPreview', {
    map: "ui/xpshop_item",
    camera: "camera_weapon_0",
    "require-composition-layer": true,
    player: false,
    initial_entity: "item",
    mouse_rotate: false,
    sync_spawn_addons: true,
    "transparent-background": true,
    "pin-fov": "vertical",
    style: "width: 700px; height: 400px;"
  });
  viewmodelPreview.SetHideStaticGeometry(true);

  var viewmodelTab = $.CreatePanel('Panel', _viewmodelTab, '', { style: 'flow-children: down; margin-right: 40px; overflow: squish scroll;' });

  var viewmodelModification = createSection(viewmodelTab, 'Viewmodel Modification');
  createOnOffDropDown(viewmodelModification, "Master Switch", 'visuals', 'viewmodel_mod');

  var viewmodelFov = createSection(viewmodelTab, 'Viewmodel Fov');
  createYesNoDropDown(viewmodelFov, "Modify Viewmodel Fov", 'visuals', 'viewmodel_fov_mod');
  separator(viewmodelFov);
  createSlider(viewmodelFov, "Fov", 'viewmodel_fov', 40, 90);

  $.Osiris.navigateToSubTab('visuals', 'player_info');

  var sound = createTab('sound');
  
  var playerSoundVisualization = createSection(sound, 'Player Sound Visualization');
  separator(playerSoundVisualization);
  createYesNoDropDown(playerSoundVisualization, "Visualize Player Footstep Sound", 'sound', 'visualize_player_footsteps');

  var bombSoundVisualization = createSection(sound, 'Bomb Sound Visualization');
  createYesNoDropDown(bombSoundVisualization, "Visualize Bomb Plant Sound", 'sound', 'visualize_bomb_plant');
  separator(bombSoundVisualization);
  createYesNoDropDown(bombSoundVisualization, "Visualize Bomb Beep Sound", 'sound', 'visualize_bomb_beep');
  separator(bombSoundVisualization);
  createYesNoDropDown(bombSoundVisualization, "Visualize Bomb Defuse Sound", 'sound', 'visualize_bomb_defuse');

  var weaponSoundVisualization = createSection(sound, 'Weapon Sound Visualization');
  createYesNoDropDown(weaponSoundVisualization, "Visualize Weapon Scope Sound", 'sound', 'visualize_scope_sound');
  separator(weaponSoundVisualization);
  createYesNoDropDown(weaponSoundVisualization, "Visualize Weapon Reload Sound", 'sound', 'visualize_reload_sound');

  $.Osiris.navigateToTab('hud');
})();
)"
