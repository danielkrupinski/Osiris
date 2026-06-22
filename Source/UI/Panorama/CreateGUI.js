u8R"(
$.Osiris = (function () {
  var activeTab;
  var activeSubTab = {};

  // ===== Translation System =====
  // Capture language set by C++ injection BEFORE $.Osiris is overwritten
  var _language = ($.Osiris && $.Osiris.language === 1) ? 1 : 0;

  var _strings = {
    // Tabs
    combat:             { en: "Combat",             zh: "战斗" },
    hud:                { en: "HUD",                zh: "HUD" },
    visuals:            { en: "Visuals",            zh: "视觉" },
    sound:              { en: "Sound",              zh: "声音" },
    // Sub-tabs
    sniper_rifles:      { en: "Sniper rifles",      zh: "狙击步枪" },
    player_info_in_world:{ en: "Player Info In World", zh: "世界玩家信息" },
    outline_glow:       { en: "Outline Glow",       zh: "轮廓发光" },
    model_glow:         { en: "Model Glow",         zh: "模型发光" },
    viewmodel:          { en: "Viewmodel",          zh: "视角模型" },
    // Section titles
    sec_no_scope:       { en: "No scope",           zh: "无准镜" },
    sec_bomb:           { en: "Bomb",               zh: "炸弹" },
    sec_killfeed:       { en: "Killfeed",           zh: "击杀信息" },
    sec_time:           { en: "Time",               zh: "时间" },
    sec_player_info:    { en: "Player Info In World", zh: "世界玩家信息" },
    sec_player_position:{ en: "Player Position",    zh: "玩家位置" },
    sec_player_name:    { en: "Player Name",        zh: "玩家名称" },
    sec_player_health:  { en: "Player Health",      zh: "玩家血量" },
    sec_player_weapon:  { en: "Player Weapon",      zh: "玩家武器" },
    sec_icons:          { en: "Icons",              zh: "图标" },
    sec_outline_glow:   { en: "Outline Glow",       zh: "轮廓发光" },
    sec_players:        { en: "Players",            zh: "玩家" },
    sec_weapons:        { en: "Weapons",            zh: "武器" },
    sec_bomb_defuse:    { en: "Bomb & Defuse Kit",  zh: "炸弹和拆弹包" },
    sec_hostages:       { en: "Hostages",           zh: "人质" },
    sec_model_glow:     { en: "Model Glow",         zh: "模型发光" },
    sec_viewmodel_mod:  { en: "Viewmodel Modification", zh: "视角模型修改" },
    sec_viewmodel_fov:  { en: "Viewmodel Fov",      zh: "视角模型视野" },
    sec_player_sound:   { en: "Player Sound Visualization", zh: "玩家声音可视化" },
    sec_bomb_sound:     { en: "Bomb Sound Visualization", zh: "炸弹声音可视化" },
    sec_weapon_sound:   { en: "Weapon Sound Visualization", zh: "武器声音可视化" },
    // Labels
    lbl_master_switch:  { en: "Master Switch",      zh: "总开关" },
    lbl_bomb_timer:     { en: "Show Bomb Explosion Countdown And Site", zh: "显示炸弹爆炸倒计时和位置" },
    lbl_defuse_alert:   { en: "Show Bomb Defuse Countdown", zh: "显示拆弹倒计时" },
    lbl_bomb_plant:     { en: "Show Bomb Plant Alert", zh: "显示安放炸弹警报" },
    lbl_preserve_kf:    { en: "Preserve My Killfeed During The Round", zh: "回合内保留我的击杀信息" },
    lbl_postround_timer:{ en: "Show Post-round Timer", zh: "显示回合结束计时器" },
    lbl_pos_arrow:      { en: "Show Player Position Arrow", zh: "显示玩家位置箭头" },
    lbl_pos_arrow_color:{ en: "Player Position Arrow Color", zh: "玩家位置箭头颜色" },
    lbl_show_name:      { en: "Show Player Name",   zh: "显示玩家名称" },
    lbl_name_color:     { en: "Player Name Color",  zh: "玩家名称颜色" },
    lbl_show_health:    { en: "Show Player Health", zh: "显示玩家血量" },
    lbl_health_color:   { en: "Player Health Text Color", zh: "血量文字颜色" },
    lbl_weapon_icon:    { en: "Show Player Active Weapon Icon", zh: "显示玩家当前武器图标" },
    lbl_weapon_ammo:    { en: "Show Player Active Weapon Ammo", zh: "显示玩家当前武器弹药" },
    lbl_bomb_carrier:   { en: "Show Bomb Carrier Icon", zh: "显示炸弹携带者图标" },
    lbl_bomb_planting:  { en: "Show Bomb Planting Icon", zh: "显示安放炸弹图标" },
    lbl_defuse_icon:    { en: "Show Defuse Icon",   zh: "显示拆弹图标" },
    lbl_hostage_pickup: { en: "Show Picking Up Hostage Icon", zh: "显示营救人质图标" },
    lbl_hostage_rescue: { en: "Show Rescuing Hostage Icon", zh: "显示拯救人质图标" },
    lbl_blinded:        { en: "Show Blinded By Flashbang Icon", zh: "显示被闪光弹致盲图标" },
    lbl_glow_players:   { en: "Glow Players",       zh: "玩家发光" },
    lbl_glow_color:     { en: "Player Glow Color",  zh: "玩家发光颜色" },
    lbl_glow_weapons:   { en: "Glow Weapons on Ground Nearby", zh: "附近地面武器发光" },
    lbl_glow_grenade:   { en: "Glow Grenade Projectiles", zh: "投掷物发光" },
    lbl_glow_bomb_drop: { en: "Glow Dropped Bomb",  zh: "掉落炸弹发光" },
    lbl_glow_bomb_tick: { en: "Glow Ticking Bomb",  zh: "计时炸弹发光" },
    lbl_glow_defuse:    { en: "Glow Defuse Kits on Ground Nearby", zh: "附近地面拆弹包发光" },
    lbl_glow_hostages:  { en: "Glow Hostages",      zh: "人质发光" },
    lbl_mglow_players:  { en: "Glow Player Models", zh: "玩家模型发光" },
    lbl_mglow_color:    { en: "Player Model Glow Color Mode", zh: "玩家模型发光颜色模式" },
    lbl_mglow_weapons:  { en: "Glow Weapon Models on Ground", zh: "地面武器模型发光" },
    lbl_mglow_grenade:  { en: "Glow Grenade Projectile Models", zh: "投掷物模型发光" },
    lbl_mglow_bomb_drop:{ en: "Glow Dropped Bomb Model", zh: "掉落炸弹模型发光" },
    lbl_mglow_bomb_tick:{ en: "Glow Ticking Bomb Model", zh: "计时炸弹模型发光" },
    lbl_mglow_defuse:   { en: "Glow Defuse Kit Models on Ground", zh: "地面拆弹包模型发光" },
    lbl_fov_mod:        { en: "Modify Viewmodel Fov", zh: "修改视角模型视野" },
    lbl_fov:            { en: "Fov",                 zh: "视野" },
    lbl_footstep_vis:   { en: "Visualize Player Footstep Sound", zh: "可视化玩家脚步声" },
    lbl_bomb_plant_vis: { en: "Visualize Bomb Plant Sound", zh: "可视化安放炸弹声音" },
    lbl_bomb_beep_vis:  { en: "Visualize Bomb Beep Sound", zh: "可视化炸弹蜂鸣声" },
    lbl_bomb_defuse_vis:{ en: "Visualize Bomb Defuse Sound", zh: "可视化拆弹声音" },
    lbl_scope_vis:      { en: "Visualize Weapon Scope Sound", zh: "可视化武器开镜声音" },
    lbl_reload_vis:     { en: "Visualize Weapon Reload Sound", zh: "可视化武器换弹声音" },
    lbl_inacc_vis:      { en: "Visualize Inaccuracy When Not Using a Scope", zh: "未开镜时显示不准确度" },
    // Hue labels
    hue_player_blue:    { en: "Player Blue Hue",    zh: "玩家蓝色色相" },
    hue_player_green:   { en: "Player Green Hue",   zh: "玩家绿色色相" },
    hue_player_yellow:  { en: "Player Yellow Hue",  zh: "玩家黄色色相" },
    hue_player_orange:  { en: "Player Orange Hue",  zh: "玩家橙色色相" },
    hue_player_purple:  { en: "Player Purple Hue",  zh: "玩家紫色色相" },
    hue_t_hue:          { en: "Team T Hue",         zh: "T阵营色相" },
    hue_ct_hue:         { en: "Team CT Hue",        zh: "CT阵营色相" },
    hue_high_hp:        { en: "High Health Hue",    zh: "高血量色相" },
    hue_low_hp:         { en: "Low Health Hue",     zh: "低血量色相" },
    hue_enemy:          { en: "Enemy Hue",          zh: "敌人色相" },
    hue_ally:           { en: "Ally Hue",           zh: "队友色相" },
    hue_flashbang:      { en: "Flashbang Hue",      zh: "闪光弹色相" },
    hue_hegrenade:      { en: "HE Grenade Hue",     zh: "高爆手雷色相" },
    hue_smoke:          { en: "Smoke Grenade Hue",  zh: "烟雾弹色相" },
    hue_molotov:        { en: "Molotov / Incendiary Grenade Hue", zh: "燃烧弹色相" },
    hue_dropped_bomb:   { en: "Dropped Bomb Hue",   zh: "掉落炸弹色相" },
    hue_ticking_bomb:   { en: "Ticking Bomb Hue",   zh: "计时炸弹色相" },
    hue_defuse_kit:     { en: "Defuse Kit Hue",     zh: "拆弹包色相" },
    hue_hostage:        { en: "Hostage Hue",        zh: "人质色相" },
    // Dropdown options
    opt_on:             { en: "On",                 zh: "开启" },
    opt_off:            { en: "Off",                zh: "关闭" },
    opt_yes:            { en: "Yes",                zh: "是" },
    opt_no:             { en: "No",                 zh: "否" },
    opt_enemies:        { en: "Enemies",            zh: "敌人" },
    opt_all_players:    { en: "All Players",        zh: "所有玩家" },
    opt_player_team:    { en: "Player / Team Color", zh: "玩家/队伍颜色" },
    opt_team_color:     { en: "Team Color",         zh: "队伍颜色" },
    opt_white:          { en: "White",              zh: "白色" },
    opt_health_based:   { en: "Health-based",       zh: "基于血量" },
    opt_enemy_ally:     { en: "Enemy / Ally",       zh: "敌人/队友" },
    // Preview
    preview_title:      { en: "Preview",            zh: "预览" },
    weapons_ground:     { en: "Weapons on the Ground", zh: "地面武器" },
    // Navbar
    active_cfg_tooltip: { en: "Active config file. Changes are saved automatically.", zh: "当前配置文件。更改会自动保存。" },
    restore_defaults:   { en: "Restore Defaults",   zh: "恢复默认" },
    restore_confirm:    { en: "Are you sure you want to restore default settings in the active config file (default.cfg)?", zh: "确定要将当前配置文件(default.cfg)恢复为默认设置吗？" },
    restore_btn:        { en: "RESTORE DEFAULTS",   zh: "恢复默认" },
    restore_return:     { en: "RETURN",             zh: "返回" },
    restore_tooltip:    { en: "Restore defaults",   zh: "恢复默认设置" },
    unload_title:       { en: "Unload Osiris",      zh: "卸载 Osiris" },
    unload_confirm:     { en: "Are you sure you want to unload Osiris?", zh: "确定要卸载 Osiris 吗？" },
    unload_btn:         { en: "UNLOAD",             zh: "卸载" },
    unload_return:      { en: "RETURN",             zh: "返回" },
    unload_tooltip:     { en: "Unload",             zh: "卸载" },
    // Language
    lang_reload_hint:   { en: "Language will change next time you load Osiris", zh: "语言将在下次加载 Osiris 时生效" }
  };

  var _translatables = [];

  function _tr(key) {
    var s = _strings[key];
    if (!s) return key;
    return _language === 1 ? s.zh : s.en;
  }

  function _registerTranslatable(panel, key) {
    _translatables.push({ panel: panel, key: key });
  }

  function _refreshLanguage() {
    var i;
    for (i = 0; i < _translatables.length; ++i) {
      var item = _translatables[i];
      item.panel.text = _tr(item.key);
    }
    // update language indicator label
    var rootPanel = _self.rootPanel;
    if (rootPanel) {
      var langLabel = rootPanel.FindChildInLayoutFile('LanguageLabel');
      if (langLabel)
        langLabel.text = (_language === 1) ? "中文" : "EN";
    }
  }

  var _self = {
    get language() { return _language; },
    tr: _tr,
    registerTranslatable: _registerTranslatable,
    refreshLanguage: _refreshLanguage,

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
      $.DispatchEvent('Activated', _self.rootPanel.GetParent().GetParent().GetParent().FindChildInLayoutFile("MainMenuNavBarHome"), 'mouse');
    },
    addCommand: function (command, value = '') {
      var existingCommands = _self.rootPanel.GetAttributeString('cmd', '');
      _self.rootPanel.SetAttributeString('cmd', existingCommands + command + ' ' + value);
    },
    navigateToTab: function (tabID) {
      if (activeTab === tabID)
        return;

      if (activeTab) {
        var panelToHide = _self.rootPanel.FindChildInLayoutFile(activeTab);
        panelToHide.RemoveClass('Active');
      }

      _self.rootPanel.FindChildInLayoutFile(tabID + '_button').checked = true;

      activeTab = tabID;
      var activePanel = _self.rootPanel.FindChildInLayoutFile(tabID);
      activePanel.AddClass('Active');
      activePanel.visible = true;
      activePanel.SetReadyForDisplay(true);
    },
    navigateToSubTab: function (tabID, subTabID) {
      if (activeSubTab[tabID] === subTabID)
        return;

      if (activeSubTab[tabID]) {
        var panelToHide = _self.rootPanel.FindChildInLayoutFile(activeSubTab[tabID]);
        panelToHide.RemoveClass('Active');
      }

      _self.rootPanel.FindChildInLayoutFile(subTabID + '_button').checked = true;

      activeSubTab[tabID] = subTabID;
      var activePanel = _self.rootPanel.FindChildInLayoutFile(subTabID);
      activePanel.AddClass('Active');
      activePanel.visible = true;
      activePanel.SetReadyForDisplay(true);
    },
    sliderUpdated: function (tabID, sliderID, slider) {
      _self.addCommand('set', tabID + '/' + sliderID + '/' + Math.floor(slider.value));
    },
    sliderTextEntryUpdated: function (tabID, sliderID, panel) {
      _self.addCommand('set', tabID + '/' + sliderID + '/' + panel.text);
    },
    setLanguage: function (langValue) {
      _language = langValue;
      _self.addCommand('language', String(langValue));
      _refreshLanguage();
      UiToolkitAPI.ShowTextTooltip('LanguageButton', _tr('lang_reload_hint'));
    },
    toggleLanguage: function () {
      _self.setLanguage(_language === 0 ? 1 : 0);
    }
  };

  return _self;
})();
)"
// split the string literal because MSVC does not support string literals longer than 16k chars - error C2026
u8R"(
(function () {
  var tr = $.Osiris.tr;
  var register = $.Osiris.registerTranslatable;

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

    activeCfgNameLabel.SetPanelEvent('onmouseover', function () { UiToolkitAPI.ShowTextTooltip('ActiveConfigName', tr('active_cfg_tooltip')); });
    activeCfgNameLabel.SetPanelEvent('onmouseout', function () { UiToolkitAPI.HideTextTooltip(); });

    var restoreDefaultsButton = $.CreatePanel('Button', leftContainer, 'RestoreDefaultsButton', {
      class: "content-navbar__tabs__btn",
      style: "margin-left: 5px;",
      onactivate: "UiToolkitAPI.ShowGenericPopupOneOptionCustomCancelBgStyle('" + tr('restore_defaults') + "', '" + tr('restore_confirm') + "', '', '" + tr('restore_btn') + "', function() { $.Osiris.addCommand('restore_defaults'); }, '" + tr('restore_return') + "', function() {}, 'dim');"
    });

    restoreDefaultsButton.SetPanelEvent('onmouseover', function () { UiToolkitAPI.ShowTextTooltip('RestoreDefaultsButton', tr('restore_tooltip')); });
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

    $.CreatePanel('Label', combatTabButton, '', { text: tr('combat') });

    var hudTabButton = $.CreatePanel('RadioButton', centerContainer, 'hud_button', {
      group: "SettingsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.Osiris.navigateToTab('hud');"
    });

    $.CreatePanel('Label', hudTabButton, '', { text: tr('hud') });

    var visualsTabButton = $.CreatePanel('RadioButton', centerContainer, 'visuals_button', {
      group: "SettingsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.Osiris.navigateToTab('visuals');"
    });

    $.CreatePanel('Label', visualsTabButton, '', { text: tr('visuals') });

    var soundTabButton = $.CreatePanel('RadioButton', centerContainer, 'sound_button', {
      group: "SettingsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.Osiris.navigateToTab('sound');"
    });

    $.CreatePanel('Label', soundTabButton, '', { text: tr('sound') });

    var rightContainer = $.CreatePanel('Panel', navbar, '', {
        style: "horizontal-align: right; flow-children: right; height: 100%; margin-right: 70px;"
    });

    // Language toggle button - shows current language, click to switch
    var langButton = $.CreatePanel('Button', rightContainer, 'LanguageButton', {
        class: "content-navbar__tabs__btn",
        style: "margin-right: 5px; min-width: 50px;",
        onactivate: "$.Osiris.toggleLanguage();"
    });

    var langLabel = $.CreatePanel('Label', langButton, 'LanguageLabel', {
        text: ($.Osiris.language === 1) ? "中文" : "EN",
        style: "font-size: 16px; color: #ccccccff; horizontal-align: center; vertical-align: center;"
    });

    langButton.SetPanelEvent('onmouseover', function () { UiToolkitAPI.ShowTextTooltip('LanguageButton', tr('lang_reload_hint')); });
    langButton.SetPanelEvent('onmouseout', function () { UiToolkitAPI.HideTextTooltip(); });

    var unloadButton = $.CreatePanel('Button', rightContainer, 'UnloadButton', {
        class: "content-navbar__tabs__btn",
        onactivate: "UiToolkitAPI.ShowGenericPopupOneOptionCustomCancelBgStyle('" + tr('unload_title') + "', '" + tr('unload_confirm') + "', '', '" + tr('unload_btn') + "', function() { $.Osiris.goHome(); $.Osiris.addCommand('unload'); }, '" + tr('unload_return') + "', function() {}, 'dim');"
    });

    unloadButton.SetPanelEvent('onmouseover', function () { UiToolkitAPI.ShowTextTooltip('UnloadButton', tr('unload_tooltip')); });
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

    $.CreatePanel('Label', playerInfoTabButton, '', { text: tr('player_info_in_world') });

    var outlineGlowTabButton = $.CreatePanel('RadioButton', centerContainer, 'outline_glow_button', {
      group: "VisualsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.Osiris.navigateToSubTab('visuals', 'outline_glow');"
    });

    $.CreatePanel('Label', outlineGlowTabButton, '', { text: tr('outline_glow') });

    var modelGlowTabButton = $.CreatePanel('RadioButton', centerContainer, 'model_glow_button', {
        group: "VisualsNavBar",
        class: "content-navbar__tabs__btn",
        onactivate: "$.Osiris.navigateToSubTab('visuals', 'model_glow');"
    });

    $.CreatePanel('Label', modelGlowTabButton, '', { text: tr('model_glow') });

    var viewmodelTabButton = $.CreatePanel('RadioButton', centerContainer, 'viewmodel_button', {
        group: "VisualsNavBar",
        class: "content-navbar__tabs__btn",
        onactivate: "$.Osiris.navigateToSubTab('visuals', 'viewmodel');"
    });

    $.CreatePanel('Label', viewmodelTabButton, '', { text: tr('viewmodel') });
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

    $.CreatePanel('Label', sniperRiflesTabButton, '', { text: tr('sniper_rifles') });
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

  var createSection = function(tab, sectionKey) {
    var background = $.CreatePanel('Panel', tab, '', {
      class: "SettingsBackground"
    });

    var titleContainer = $.CreatePanel('Panel', background, '', {
      class: "SettingsSectionTitleContianer"
    });

    var label = $.CreatePanel('Label', titleContainer, '', {
      class: "SettingsSectionTitleLabel",
      text: tr(sectionKey)
    });
    register(label, sectionKey);

    var content = $.CreatePanel('Panel', background, '', {
      class: "top-bottom-flow full-width"
    });

    return content;
  };

  var createDropDown = function (parent, labelKey, section, feature, optionKeys) {
    var container = $.CreatePanel('Panel', parent, '', {
      class: "SettingsMenuDropdownContainer"
    });

    var label = $.CreatePanel('Label', container, '', {
      class: "half-width",
      text: tr(labelKey)
    });
    register(label, labelKey);

    var dropdown = $.CreatePanel('CSGOSettingsEnumDropDown', container, feature, { class: "PopupButton White" });

    for (let i = 0; i < optionKeys.length; ++i) {
      dropdown.AddOption($.CreatePanel('Label', dropdown, i, {
      value: i,
      text: tr(optionKeys[i])
      }));
    }
  };

  var createOnOffDropDown = function (parent, labelKey, section, feature) {
    createDropDown(parent, labelKey, section, feature, ["opt_on", "opt_off"]);
  };

  var createYesNoDropDown = function (parent, labelKey, section, feature) {
    createDropDown(parent, labelKey, section, feature, ["opt_yes", "opt_no"]);
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
  var createSlider = function (parent, nameKey, id, min, max) {
    var container = $.CreatePanel('Panel', parent, '', {
      class: "SettingsMenuDropdownContainer"
    });

    var label = $.CreatePanel('Label', container, '', {
      class: "half-width",
      text: tr(nameKey)
    });
    register(label, nameKey);

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

  var createHueSlider = function (parent, nameKey, id, min, max) {
    var container = $.CreatePanel('Panel', parent, '', {
      class: "SettingsMenuDropdownContainer"
    });

    var label = $.CreatePanel('Label', container, '', {
      class: "half-width",
      text: tr(nameKey)
    });
    register(label, nameKey);

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
  var noScope = createSection(sniperRiflesTab, 'sec_no_scope');
  separator(noScope);
  createYesNoDropDown(noScope, "lbl_inacc_vis", 'combat', 'no_scope_inacc_vis');

  $.Osiris.navigateToSubTab('combat', 'sniper_rifles');

  var hud = createTab('hud');

  var bomb = createSection(hud, 'sec_bomb');
  createYesNoDropDown(bomb, "lbl_bomb_timer", 'hud', 'bomb_timer');
  separator(bomb);
  createYesNoDropDown(bomb, "lbl_defuse_alert", 'hud', 'defusing_alert');
  separator(bomb);
  createYesNoDropDown(bomb, "lbl_bomb_plant", 'hud', 'bomb_plant_alert');

  var killfeed = createSection(hud, 'sec_killfeed');
  separator(killfeed);
  createYesNoDropDown(killfeed, "lbl_preserve_kf", 'hud', 'preserve_killfeed');

  var time = createSection(hud, 'sec_time');
  separator(time);
  createYesNoDropDown(time, "lbl_postround_timer", 'hud', 'postround_timer');

  var visuals = createVisualsTab();

  var playerInfoTab = createSubTab(visuals, 'player_info');

  var playerInfo = createSection(playerInfoTab, 'sec_player_info');
  createDropDown(playerInfo, "lbl_master_switch", 'visuals', 'player_information_through_walls', ['opt_enemies', 'opt_all_players', 'opt_off']);

  var playerPosition = createSection(playerInfoTab, 'sec_player_position');
  createYesNoDropDown(playerPosition, "lbl_pos_arrow", 'visuals', 'player_info_position');
  separator(playerPosition);
  createDropDown(playerPosition, "lbl_pos_arrow_color", 'visuals', 'player_info_position_color', ['opt_player_team', 'opt_team_color']);

  var playerName = createSection(playerInfoTab, 'sec_player_name');
  createYesNoDropDown(playerName, "lbl_show_name", 'visuals', 'player_info_name');
  separator(playerName);
  createDropDown(playerName, "lbl_name_color", 'visuals', 'player_info_name_color', ['opt_white', 'opt_team_color', 'opt_health_based']);

  var playerHealth = createSection(playerInfoTab, 'sec_player_health');
  createYesNoDropDown(playerHealth, "lbl_show_health", 'visuals', 'player_info_health');
  separator(playerHealth);
  createDropDown(playerHealth, "lbl_health_color", 'visuals', 'player_info_health_color', ['opt_health_based', 'opt_white']);

  var playerWeapon = createSection(playerInfoTab, 'sec_player_weapon');
  createYesNoDropDown(playerWeapon, "lbl_weapon_icon", 'visuals', 'player_info_weapon');
  separator(playerWeapon);
  createYesNoDropDown(playerWeapon, "lbl_weapon_ammo", 'visuals', 'player_info_weapon_clip');
  separator(playerWeapon);
  createYesNoDropDown(playerWeapon, "lbl_bomb_carrier", 'visuals', 'player_info_bomb_carrier');
  separator(playerWeapon);
  createYesNoDropDown(playerWeapon, "lbl_bomb_planting", 'visuals', 'player_info_bomb_planting');

  var playerIcons = createSection(playerInfoTab, 'sec_icons');
  createYesNoDropDown(playerIcons, "lbl_defuse_icon", 'visuals', 'player_info_defuse');
  separator(playerIcons);
  createYesNoDropDown(playerIcons, "lbl_hostage_pickup", 'visuals', 'player_info_hostage_pickup');
  separator(playerIcons);
  createYesNoDropDown(playerIcons, "lbl_hostage_rescue", 'visuals', 'player_info_hostage_rescue');
  separator(playerIcons);
  createYesNoDropDown(playerIcons, "lbl_blinded", 'visuals', 'player_info_blinded');

  var outlineGlowTab = createSubTab(visuals, 'outline_glow');

  var outlineGlow = createSection(outlineGlowTab, 'sec_outline_glow');
  createOnOffDropDown(outlineGlow, "lbl_master_switch", 'visuals', 'outline_glow_enable');

  var playerOutlineGlow = createSection(outlineGlowTab, 'sec_players');
  createDropDown(playerOutlineGlow, "lbl_glow_players", 'visuals', 'player_outline_glow', ['opt_enemies', 'opt_all_players', 'opt_off']);
  separator(playerOutlineGlow);
  createDropDown(playerOutlineGlow, "lbl_glow_color", 'visuals', 'player_outline_glow_color', ['opt_player_team', 'opt_team_color', 'opt_health_based', 'opt_enemy_ally']);
  separator(playerOutlineGlow);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "hue_player_blue", 'player_outline_glow_blue_hue', 191, 240);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "hue_player_green", 'player_outline_glow_green_hue', 110, 140);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "hue_player_yellow", 'player_outline_glow_yellow_hue', 47, 60);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "hue_player_orange", 'player_outline_glow_orange_hue', 11, 20);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "hue_player_purple", 'player_outline_glow_purple_hue', 250, 280);
  separator(playerOutlineGlow);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "hue_t_hue", 'player_outline_glow_t_hue', 30, 40);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "hue_ct_hue", 'player_outline_glow_ct_hue', 210, 230);
  separator(playerOutlineGlow);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "hue_high_hp", 'player_outline_glow_high_hp_hue', 0, 359);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "hue_low_hp", 'player_outline_glow_low_hp_hue', 0, 359);
  separator(playerOutlineGlow);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "hue_enemy", 'player_outline_glow_enemy_hue', 0, 359);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "hue_ally", 'player_outline_glow_ally_hue', 0, 359);

  var weaponOutlineGlow = createSection(outlineGlowTab, 'sec_weapons');
  createYesNoDropDown(weaponOutlineGlow, "lbl_glow_weapons", 'visuals', 'weapon_outline_glow');
  separator(weaponOutlineGlow);
  createYesNoDropDown(weaponOutlineGlow, "lbl_glow_grenade", 'visuals', 'grenade_proj_outline_glow');
  separator(weaponOutlineGlow);
  separator(weaponOutlineGlow);
  createHueSlider(weaponOutlineGlow, "hue_flashbang", 'outline_glow_flashbang_hue', 191, 250);
  separator(weaponOutlineGlow);
  createHueSlider(weaponOutlineGlow, "hue_hegrenade", 'outline_glow_hegrenade_hue', 300, 359);
  separator(weaponOutlineGlow);
  createHueSlider(weaponOutlineGlow, "hue_smoke", 'outline_glow_smoke_hue', 110, 140);
  separator(weaponOutlineGlow);
  createHueSlider(weaponOutlineGlow, "hue_molotov", 'outline_glow_molotov_hue', 20, 60);

  var bombAndDefuseKitOutlineGlow = createSection(outlineGlowTab, 'sec_bomb_defuse');
  createYesNoDropDown(bombAndDefuseKitOutlineGlow, "lbl_glow_bomb_drop", 'visuals', 'dropped_bomb_outline_glow');
  separator(bombAndDefuseKitOutlineGlow);
  createYesNoDropDown(bombAndDefuseKitOutlineGlow, "lbl_glow_bomb_tick", 'visuals', 'ticking_bomb_outline_glow');
  separator(bombAndDefuseKitOutlineGlow);
  createYesNoDropDown(bombAndDefuseKitOutlineGlow, "lbl_glow_defuse", 'visuals', 'defuse_kit_outline_glow');
  separator(bombAndDefuseKitOutlineGlow);
  separator(bombAndDefuseKitOutlineGlow);
  createHueSlider(bombAndDefuseKitOutlineGlow, "hue_dropped_bomb", 'outline_glow_dropped_bomb_hue', 0, 359);
  separator(bombAndDefuseKitOutlineGlow);
  createHueSlider(bombAndDefuseKitOutlineGlow, "hue_ticking_bomb", 'outline_glow_ticking_bomb_hue', 0, 359);
  separator(bombAndDefuseKitOutlineGlow);
  createHueSlider(bombAndDefuseKitOutlineGlow, "hue_defuse_kit", 'outline_glow_defuse_kit_hue', 0, 359);

  var hostageOutlineGlow = createSection(outlineGlowTab, 'sec_hostages');
  createYesNoDropDown(hostageOutlineGlow, "lbl_glow_hostages", 'visuals', 'hostage_outline_glow');
  separator(hostageOutlineGlow);
  createHueSlider(hostageOutlineGlow, "hue_hostage", 'outline_glow_hostage_hue', 0, 359);

  var _modelGlowTab = createSubTab(visuals, 'model_glow');
  _modelGlowTab.style.overflow = 'squish squish';
  _modelGlowTab.style.flowChildren = 'right';

  var modelGlowPreview = $.CreatePanel('Panel', _modelGlowTab, '', { style: 'flow-children: down;' });
  var previewLabel = $.CreatePanel('Label', modelGlowPreview, '', { style: 'vertical-align: top; horizontal-align: center; font-size: 40;', text: tr('preview_title') });
  register(previewLabel, 'preview_title');
  var playerModelGlowPreview = $.CreatePanel('Panel', modelGlowPreview, '', { style: 'flow-children: right; margin-top: 20px;' });
  createPlayerModelGlowPreview(playerModelGlowPreview, 'ModelGlowPreviewPlayerTT', 'ModelGlowPreviewPlayerTTLabel', 'agents/models/tm_professional/tm_professional_varf.vmdl', makeFauxItemId(7, 921));
  createPlayerModelGlowPreview(playerModelGlowPreview, 'ModelGlowPreviewPlayerCT', 'ModelGlowPreviewPlayerCTLabel', 'agents/models/ctm_st6/ctm_st6_variante.vmdl', makeFauxItemId(9, 819));

  var weaponsGroundLabel = $.CreatePanel('Label', modelGlowPreview, '', { style: 'horizontal-align: center; margin-top: 20px;', text: tr('weapons_ground') });
  register(weaponsGroundLabel, 'weapons_ground');

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

  var modelGlow = createSection(modelGlowTab, 'sec_model_glow');
  createOnOffDropDown(modelGlow, "lbl_master_switch", 'visuals', 'model_glow_enable');

  var playerModelGlow = createSection(modelGlowTab, 'sec_players');
  createDropDown(playerModelGlow, "lbl_mglow_players", 'visuals', 'player_model_glow', ['opt_enemies', 'opt_all_players', 'opt_off']);
  separator(playerModelGlow);
  createDropDown(playerModelGlow, "lbl_mglow_color", 'visuals', 'player_model_glow_color', ['opt_player_team', 'opt_team_color', 'opt_health_based', 'opt_enemy_ally']);
  separator(playerModelGlow);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "hue_player_blue", 'player_model_glow_blue_hue', 191, 240);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "hue_player_green", 'player_model_glow_green_hue', 110, 140);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "hue_player_yellow", 'player_model_glow_yellow_hue', 47, 60);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "hue_player_orange", 'player_model_glow_orange_hue', 11, 20);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "hue_player_purple", 'player_model_glow_purple_hue', 250, 280);
  separator(playerModelGlow);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "hue_t_hue", 'player_model_glow_t_hue', 30, 40);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "hue_ct_hue", 'player_model_glow_ct_hue', 210, 230);
  separator(playerModelGlow);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "hue_high_hp", 'player_model_glow_high_hp_hue', 0, 359);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "hue_low_hp", 'player_model_glow_low_hp_hue', 0, 359);
  separator(playerModelGlow);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "hue_enemy", 'player_model_glow_enemy_hue', 0, 359);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "hue_ally", 'player_model_glow_ally_hue', 0, 359);
)"
// split the string literal because MSVC does not support string literals longer than 16k chars - error C2026
u8R"(
  var weaponModelGlow = createSection(modelGlowTab, 'sec_weapons');
  createYesNoDropDown(weaponModelGlow, "lbl_mglow_weapons", 'visuals', 'weapon_model_glow');
  separator(weaponModelGlow);
  createYesNoDropDown(weaponModelGlow, "lbl_mglow_grenade", 'visuals', 'grenade_proj_model_glow');
  separator(weaponModelGlow);
  separator(weaponModelGlow);
  createHueSlider(weaponModelGlow, "hue_flashbang", 'model_glow_flashbang_hue', 191, 250);
  separator(weaponModelGlow);
  createHueSlider(weaponModelGlow, "hue_hegrenade", 'model_glow_hegrenade_hue', 300, 359);
  separator(weaponModelGlow);
  createHueSlider(weaponModelGlow, "hue_smoke", 'model_glow_smoke_hue', 110, 140);
  separator(weaponModelGlow);
  createHueSlider(weaponModelGlow, "hue_molotov", 'model_glow_molotov_hue', 20, 60);

  var bombModelGlow = createSection(modelGlowTab, 'sec_bomb_defuse');
  createYesNoDropDown(bombModelGlow, "lbl_mglow_bomb_drop", 'visuals', 'dropped_bomb_model_glow');
  separator(bombModelGlow);
  createYesNoDropDown(bombModelGlow, "lbl_mglow_bomb_tick", 'visuals', 'ticking_bomb_model_glow');
  separator(bombModelGlow);
  createYesNoDropDown(bombModelGlow, "lbl_mglow_defuse", 'visuals', 'defuse_kit_model_glow');
  separator(bombModelGlow);
  separator(bombModelGlow);
  createHueSlider(bombModelGlow, "hue_dropped_bomb", 'model_glow_dropped_bomb_hue', 0, 359);
  separator(bombModelGlow);
  createHueSlider(bombModelGlow, "hue_ticking_bomb", 'model_glow_ticking_bomb_hue', 0, 359);
  separator(bombModelGlow);
  createHueSlider(bombModelGlow, "hue_defuse_kit", 'model_glow_defuse_kit_hue', 0, 359);

  var _viewmodelTab = createSubTab(visuals, 'viewmodel');
  _viewmodelTab.style.overflow = 'squish squish';
  _viewmodelTab.style.flowChildren = 'right';

  var viewmodelPreviewContainer = $.CreatePanel('Panel', _viewmodelTab, '', { style: 'flow-children: down;' });
  var viewmodelPreviewLabel = $.CreatePanel('Label', viewmodelPreviewContainer, '', { style: 'vertical-align: top; horizontal-align: center; font-size: 40;', text: tr('preview_title') });
  register(viewmodelPreviewLabel, 'preview_title');

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

  var viewmodelModification = createSection(viewmodelTab, 'sec_viewmodel_mod');
  createOnOffDropDown(viewmodelModification, "lbl_master_switch", 'visuals', 'viewmodel_mod');

  var viewmodelFov = createSection(viewmodelTab, 'sec_viewmodel_fov');
  createYesNoDropDown(viewmodelFov, "lbl_fov_mod", 'visuals', 'viewmodel_fov_mod');
  separator(viewmodelFov);
  createSlider(viewmodelFov, "lbl_fov", 'viewmodel_fov', 40, 90);

  $.Osiris.navigateToSubTab('visuals', 'player_info');

  var sound = createTab('sound');

  var playerSoundVisualization = createSection(sound, 'sec_player_sound');
  separator(playerSoundVisualization);
  createYesNoDropDown(playerSoundVisualization, "lbl_footstep_vis", 'sound', 'visualize_player_footsteps');

  var bombSoundVisualization = createSection(sound, 'sec_bomb_sound');
  createYesNoDropDown(bombSoundVisualization, "lbl_bomb_plant_vis", 'sound', 'visualize_bomb_plant');
  separator(bombSoundVisualization);
  createYesNoDropDown(bombSoundVisualization, "lbl_bomb_beep_vis", 'sound', 'visualize_bomb_beep');
  separator(bombSoundVisualization);
  createYesNoDropDown(bombSoundVisualization, "lbl_bomb_defuse_vis", 'sound', 'visualize_bomb_defuse');

  var weaponSoundVisualization = createSection(sound, 'sec_weapon_sound');
  createYesNoDropDown(weaponSoundVisualization, "lbl_scope_vis", 'sound', 'visualize_scope_sound');
  separator(weaponSoundVisualization);
  createYesNoDropDown(weaponSoundVisualization, "lbl_reload_vis", 'sound', 'visualize_reload_sound');

  $.Osiris.navigateToTab('hud');
})();
)"
