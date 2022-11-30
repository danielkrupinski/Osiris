#include <algorithm>
#include <array>
#include <fstream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

#include "Platform/IsPlatform.h"

#if IS_WIN32()
#include <ShlObj.h>
#include <Windows.h>
#endif

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include "imguiCustom.h"

#include "GUI.h"
#include "Config.h"
#include "ConfigStructs.h"
#include "Hacks/Misc.h"
#include "InventoryChanger/InventoryChanger.h"
#include "Helpers.h"
#include "SDK/InputSystem.h"
#include "Hacks/Visuals.h"
#include "Hacks/Glow.h"
#include "Hacks/Backtrack.h"
#include "Hacks/Sound.h"
#include "Hacks/StreamProofESP.h"
#include <Config/ResetConfigurator.h>

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

static ImFont* addFontFromVFONT(const std::string& path, float size, const ImWchar* glyphRanges, bool merge) noexcept
{
    auto file = Helpers::loadBinaryFile(path);
    if (!Helpers::decodeVFONT(file))
        return nullptr;

    ImFontConfig cfg;
    cfg.FontData = file.data();
    cfg.FontDataSize = file.size();
    cfg.FontDataOwnedByAtlas = false;
    cfg.MergeMode = merge;
    cfg.GlyphRanges = glyphRanges;
    cfg.SizePixels = size;

    return ImGui::GetIO().Fonts->AddFont(&cfg);
}

GUI::GUI() noexcept
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    style.ScrollbarSize = 9.0f;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    ImFontConfig cfg;
    cfg.SizePixels = 15.0f;

#if IS_WIN32()
    if (PWSTR pathToFonts; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &pathToFonts))) {
        const std::filesystem::path path{ pathToFonts };
        CoTaskMemFree(pathToFonts);

        fonts.normal15px = io.Fonts->AddFontFromFileTTF((path / "tahoma.ttf").string().c_str(), 15.0f, &cfg, Helpers::getFontGlyphRanges());
        if (!fonts.normal15px)
            io.Fonts->AddFontDefault(&cfg);

        cfg.MergeMode = true;
        static constexpr ImWchar symbol[]{
            0x2605, 0x2605, // ★
            0
        };
        io.Fonts->AddFontFromFileTTF((path / "seguisym.ttf").string().c_str(), 15.0f, &cfg, symbol);
        cfg.MergeMode = false;
    }
#else
    fonts.normal15px = addFontFromVFONT("csgo/panorama/fonts/notosans-regular.vfont", 15.0f, Helpers::getFontGlyphRanges(), false);
#endif
    if (!fonts.normal15px)
        io.Fonts->AddFontDefault(&cfg);
    addFontFromVFONT("csgo/panorama/fonts/notosanskr-regular.vfont", 15.0f, io.Fonts->GetGlyphRangesKorean(), true);
    addFontFromVFONT("csgo/panorama/fonts/notosanssc-regular.vfont", 17.0f, io.Fonts->GetGlyphRangesChineseFull(), true);
}

void GUI::render(Glow& glow, Backtrack& backtrack, Visuals& visuals, const Engine& engine, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, Config& config) noexcept
{
    if (!config.style.menuStyle) {
        renderMenuBar(glow, backtrack, visuals);
        renderAimbotWindow(config);
        renderTriggerbotWindow(config);
        backtrack.drawGUI(false);
        glow.drawGUI(false);
        renderChamsWindow(config);
        StreamProofESP::drawGUI(config, false);
        visuals.drawGUI(false);
        inventory_changer::InventoryChanger::instance(interfaces, memory).drawGUI(interfaces, memory, false);
        Sound::drawGUI(false);
        renderStyleWindow(config);
        Misc::drawGUI(glow, engine, clientInterfaces, interfaces, memory, false);
        renderConfigWindow(glow, backtrack, visuals, interfaces, memory, config);
    } else {
        renderGuiStyle2(glow, backtrack, visuals, engine, clientInterfaces, interfaces, memory, config);
    }
}

void GUI::updateColors(Config& config) const noexcept
{
    switch (config.style.menuColors) {
    case 0: ImGui::StyleColorsDark(); break;
    case 1: ImGui::StyleColorsLight(); break;
    case 2: ImGui::StyleColorsClassic(); break;
    }
}

void GUI::handleToggle(const OtherInterfaces& interfaces) noexcept
{
    if (Misc::isMenuKeyPressed()) {
        open = !open;
        if (!open)
            interfaces.getInputSystem().resetInputState();
#if !IS_WIN32()
        ImGui::GetIO().MouseDrawCursor = gui->open;
#endif
    }
}

static void menuBarItem(const char* name, bool& enabled) noexcept
{
    if (ImGui::MenuItem(name)) {
        enabled = true;
        ImGui::SetWindowFocus(name);
        ImGui::SetWindowPos(name, { 100.0f, 100.0f });
    }
}

void GUI::renderMenuBar(Glow& glow, Backtrack& backtrack, Visuals& visuals) noexcept
{
    if (ImGui::BeginMainMenuBar()) {
        menuBarItem("自瞄", window.aimbot);
        menuBarItem("开火", window.triggerbot);
        backtrack.menuBarItem();
        glow.menuBarItem();
        menuBarItem("上色", window.chams);
        StreamProofESP::menuBarItem();
        visuals.menuBarItem();
        InventoryChanger::menuBarItem();
        Sound::menuBarItem();
        menuBarItem("主题", window.style);
        Misc::menuBarItem();
        menuBarItem("配置", window.config);
        ImGui::EndMainMenuBar();   
    }
}

void GUI::renderAimbotWindow(Config& config, bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.aimbot)
            return;
        ImGui::SetNextWindowSize({ 600.0f, 0.0f });
        ImGui::Begin("自瞄", &window.aimbot, windowFlags);
    }
    ImGui::Checkbox("快捷键", &config.aimbotOnKey);
    ImGui::SameLine();
    ImGui::PushID("Aimbot Key");
    ImGui::hotkey("", config.aimbotKey);
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(2);
    ImGui::PushItemWidth(70.0f);
    ImGui::Combo("", &config.aimbotKeyMode, "按住\0切换\0");
    ImGui::PopItemWidth();
    ImGui::PopID();
    ImGui::Separator();
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "全部\0手枪\0重型武器\0冲锋枪\0步枪\0");
    ImGui::PopID();
    ImGui::SameLine();
    static int currentWeapon{ 0 };
    ImGui::PushID(1);

    switch (currentCategory) {
    case 0:
        currentWeapon = 0;
        ImGui::NewLine();
        break;
    case 1: {
        static int currentPistol{ 0 };
        static constexpr const char* pistols[]{ "全部", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

        ImGui::Combo("", &currentPistol, [](void* config, int idx, const char** out_text) {
            if (static_cast<Config*>(config)->aimbot[idx ? idx : 35].enabled) {
                static std::string name;
                name = pistols[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = pistols[idx];
            }
            return true;
            }, &config, IM_ARRAYSIZE(pistols));

        currentWeapon = currentPistol ? currentPistol : 35;
        break;
    }
    case 2: {
        static int currentHeavy{ 0 };
        static constexpr const char* heavies[]{ "全部", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

        ImGui::Combo("", &currentHeavy, [](void* config, int idx, const char** out_text) {
            if (static_cast<Config*>(config)->aimbot[idx ? idx + 10 : 36].enabled) {
                static std::string name;
                name = heavies[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = heavies[idx];
            }
            return true;
            }, &config, IM_ARRAYSIZE(heavies));

        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
        break;
    }
    case 3: {
        static int currentSmg{ 0 };
        static constexpr const char* smgs[]{ "全部", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

        ImGui::Combo("", &currentSmg, [](void* config, int idx, const char** out_text) {
            if (static_cast<Config*>(config)->aimbot[idx ? idx + 16 : 37].enabled) {
                static std::string name;
                name = smgs[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = smgs[idx];
            }
            return true;
            }, &config, IM_ARRAYSIZE(smgs));

        currentWeapon = currentSmg ? currentSmg + 16 : 37;
        break;
    }
    case 4: {
        static int currentRifle{ 0 };
        static constexpr const char* rifles[]{ "全部", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

        ImGui::Combo("", &currentRifle, [](void* config, int idx, const char** out_text) {
            if (static_cast<Config*>(config)->aimbot[idx ? idx + 23 : 38].enabled) {
                static std::string name;
                name = rifles[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = rifles[idx];
            }
            return true;
            }, &config, IM_ARRAYSIZE(rifles));

        currentWeapon = currentRifle ? currentRifle + 23 : 38;
        break;
    }
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::Checkbox("启用", &config.aimbot[currentWeapon].enabled);
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 220.0f);
    ImGui::Checkbox("锁定准心", &config.aimbot[currentWeapon].aimlock);
    ImGui::Checkbox("静默模式", &config.aimbot[currentWeapon].silent);
    ImGui::Checkbox("不分敌我", &config.aimbot[currentWeapon].friendlyFire);
    ImGui::Checkbox("仅可见", &config.aimbot[currentWeapon].visibleOnly);
    ImGui::Checkbox("仅开镜", &config.aimbot[currentWeapon].scopedOnly);
    ImGui::Checkbox("忽略闪", &config.aimbot[currentWeapon].ignoreFlash);
    ImGui::Checkbox("忽略烟", &config.aimbot[currentWeapon].ignoreSmoke);
    ImGui::Checkbox("自动开火", &config.aimbot[currentWeapon].autoShot);
    ImGui::Checkbox("自动开镜", &config.aimbot[currentWeapon].autoScope);
    ImGui::Combo("部位", &config.aimbot[currentWeapon].bone, "最近的\0最佳伤害\0头\0颈\0上胸部\0中胸部\0胃\0屁股\0");
    ImGui::NextColumn();
    ImGui::PushItemWidth(240.0f);
    ImGui::SliderFloat("角度", &config.aimbot[currentWeapon].fov, 0.0f, 255.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
    ImGui::SliderFloat("平滑", &config.aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
    ImGui::SliderFloat("最大瞄准误差", &config.aimbot[currentWeapon].maxAimInaccuracy, 0.0f, 1.0f, "%.5f", ImGuiSliderFlags_Logarithmic);
    ImGui::SliderFloat("最大射击误差", &config.aimbot[currentWeapon].maxShotInaccuracy, 0.0f, 1.0f, "%.5f", ImGuiSliderFlags_Logarithmic);
    ImGui::InputInt("最小伤害", &config.aimbot[currentWeapon].minDamage);
    config.aimbot[currentWeapon].minDamage = std::clamp(config.aimbot[currentWeapon].minDamage, 0, 250);
    ImGui::Checkbox("致命一击", &config.aimbot[currentWeapon].killshot);
    ImGui::Checkbox("Between shots", &config.aimbot[currentWeapon].betweenShots);
    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderTriggerbotWindow(Config& config, bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.triggerbot)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("开火", &window.triggerbot, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "全部\0手枪\0重型武器\0冲锋枪\0步枪\0Zeus x27\0");
    ImGui::PopID();
    ImGui::SameLine();
    static int currentWeapon{ 0 };
    ImGui::PushID(1);
    switch (currentCategory) {
    case 0:
        currentWeapon = 0;
        ImGui::NewLine();
        break;
    case 5:
        currentWeapon = 39;
        ImGui::NewLine();
        break;

    case 1: {
        static int currentPistol{ 0 };
        static constexpr const char* pistols[]{ "全部", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

        ImGui::Combo("", &currentPistol, [](void* config, int idx, const char** out_text) {
            if (static_cast<Config*>(config)->triggerbot[idx ? idx : 35].enabled) {
                static std::string name;
                name = pistols[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = pistols[idx];
            }
            return true;
            }, &config, IM_ARRAYSIZE(pistols));

        currentWeapon = currentPistol ? currentPistol : 35;
        break;
    }
    case 2: {
        static int currentHeavy{ 0 };
        static constexpr const char* heavies[]{ "全部", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

        ImGui::Combo("", &currentHeavy, [](void* config, int idx, const char** out_text) {
            if (static_cast<Config*>(config)->triggerbot[idx ? idx + 10 : 36].enabled) {
                static std::string name;
                name = heavies[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = heavies[idx];
            }
            return true;
            }, &config, IM_ARRAYSIZE(heavies));

        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
        break;
    }
    case 3: {
        static int currentSmg{ 0 };
        static constexpr const char* smgs[]{ "全部", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

        ImGui::Combo("", &currentSmg, [](void* config, int idx, const char** out_text) {
            if (static_cast<Config*>(config)->triggerbot[idx ? idx + 16 : 37].enabled) {
                static std::string name;
                name = smgs[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = smgs[idx];
            }
            return true;
            }, &config, IM_ARRAYSIZE(smgs));

        currentWeapon = currentSmg ? currentSmg + 16 : 37;
        break;
    }
    case 4: {
        static int currentRifle{ 0 };
        static constexpr const char* rifles[]{ "全部", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

        ImGui::Combo("", &currentRifle, [](void* config, int idx, const char** out_text) {
            if (static_cast<Config*>(config)->triggerbot[idx ? idx + 23 : 38].enabled) {
                static std::string name;
                name = rifles[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = rifles[idx];
            }
            return true;
            }, &config, IM_ARRAYSIZE(rifles));

        currentWeapon = currentRifle ? currentRifle + 23 : 38;
        break;
    }
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::Checkbox("启用", &config.triggerbot[currentWeapon].enabled);
    ImGui::Separator();
    ImGui::hotkey("按住键", config.triggerbotHoldKey);
    ImGui::Checkbox("不分敌我", &config.triggerbot[currentWeapon].friendlyFire);
    ImGui::Checkbox("仅开镜", &config.triggerbot[currentWeapon].scopedOnly);
    ImGui::Checkbox("忽略闪", &config.triggerbot[currentWeapon].ignoreFlash);
    ImGui::Checkbox("忽略烟", &config.triggerbot[currentWeapon].ignoreSmoke);
    ImGui::SetNextItemWidth(85.0f);
    ImGui::Combo("命中点", &config.triggerbot[currentWeapon].hitgroup, "全部\0头\0胸\0胃\0左手\0右手\0左腿\0右腿\0");
    ImGui::PushItemWidth(220.0f);
    ImGui::SliderInt("开火延迟", &config.triggerbot[currentWeapon].shotDelay, 0, 250, "%d ms");
    ImGui::InputInt("最小伤害", &config.triggerbot[currentWeapon].minDamage);
    config.triggerbot[currentWeapon].minDamage = std::clamp(config.triggerbot[currentWeapon].minDamage, 0, 250);
    ImGui::Checkbox("致命一击", &config.triggerbot[currentWeapon].killshot);
    ImGui::SliderFloat("过度时间", &config.triggerbot[currentWeapon].burstTime, 0.0f, 0.5f, "%.3f s");

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderChamsWindow(Config& config, bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.chams)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("上色", &window.chams, windowFlags);
    }

    ImGui::hotkey("切换键", config.chamsToggleKey, 80.0f);
    ImGui::hotkey("按住键", config.chamsHoldKey, 80.0f);
    ImGui::Separator();

    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);

    static int material = 1;

    if (ImGui::Combo("", &currentCategory, "友军\0敌人\0安放中\0拆除中\0本地玩家\0武器\0手\0回溯\0袖子\0"))
        material = 1;

    ImGui::PopID();

    ImGui::SameLine();

    if (material <= 1)
        ImGuiCustom::arrowButtonDisabled("##left", ImGuiDir_Left);
    else if (ImGui::ArrowButton("##left", ImGuiDir_Left))
        --material;

    ImGui::SameLine();
    ImGui::Text("%d", material);

    constexpr std::array categories{ "友军", "敌人", "安放中", "拆除中", "本地玩家", "武器", "手", "回溯", "袖子" };

    ImGui::SameLine();

    if (material >= int(config.chams[categories[currentCategory]].materials.size()))
        ImGuiCustom::arrowButtonDisabled("##right", ImGuiDir_Right);
    else if (ImGui::ArrowButton("##right", ImGuiDir_Right))
        ++material;

    ImGui::SameLine();

    auto& chams{ config.chams[categories[currentCategory]].materials[material - 1] };

    ImGui::Checkbox("启用", &chams.enabled);
    ImGui::Separator();
    ImGui::Checkbox("基于血量", &chams.healthBased);
    ImGui::Checkbox("闪烁", &chams.blinking);
    ImGui::Combo("材质", &chams.material, "Normal\0Flat\0Animated\0Platinum\0Glass\0Chrome\0Crystal\0Silver\0Gold\0Plastic\0Glow\0Pearlescent\0Metallic\0");
    ImGui::Checkbox("线框", &chams.wireframe);
    ImGui::Checkbox("Cover", &chams.cover);
    ImGui::Checkbox("Ignore-Z", &chams.ignorez);
    ImGuiCustom::colorPicker("颜色", chams);

    if (!contentOnly) {
        ImGui::End();
    }
}

void GUI::renderStyleWindow(Config& config, bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.style)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("主题", &window.style, windowFlags);
    }

    ImGui::PushItemWidth(150.0f);
    if (ImGui::Combo("菜单样式", &config.style.menuStyle, "经典\0单窗口\0"))
        window = { };
    if (ImGui::Combo("菜单颜色", &config.style.menuColors, "暗色\0亮色\0经典\0自定义\0"))
        updateColors(config);
    ImGui::PopItemWidth();

    if (config.style.menuColors == 3) {
        ImGuiStyle& style = ImGui::GetStyle();
        for (int i = 0; i < ImGuiCol_COUNT; i++) {
            if (i && i & 3) ImGui::SameLine(220.0f * (i & 3));

            ImGuiCustom::colorPicker(ImGui::GetStyleColorName(i), (float*)&style.Colors[i], &style.Colors[i].w);
        }
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderConfigWindow(Glow& glow, Backtrack& backtrack, Visuals& visuals, const OtherInterfaces& interfaces, const Memory& memory, Config& config, bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.config)
            return;
        ImGui::SetNextWindowSize({ 320.0f, 0.0f });
        if (!ImGui::Begin("配置", &window.config, windowFlags)) {
            ImGui::End();
            return;
        }
    }

    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 170.0f);

    static bool incrementalLoad = false;
    ImGui::Checkbox("Incremental Load", &incrementalLoad);

    ImGui::PushItemWidth(160.0f);

    auto& configItems = config.getConfigs();
    static int currentConfig = -1;

    static std::u8string buffer;

    timeToNextConfigRefresh -= ImGui::GetIO().DeltaTime;
    if (timeToNextConfigRefresh <= 0.0f) {
        config.listConfigs();
        if (const auto it = std::find(configItems.begin(), configItems.end(), buffer); it != configItems.end())
            currentConfig = std::distance(configItems.begin(), it);
        timeToNextConfigRefresh = 0.1f;
    }

    if (static_cast<std::size_t>(currentConfig) >= configItems.size())
        currentConfig = -1;

    if (ImGui::ListBox("", &currentConfig, [](void* data, int idx, const char** out_text) {
        auto& vector = *static_cast<std::vector<std::u8string>*>(data);
        *out_text = (const char*)vector[idx].c_str();
        return true;
        }, &configItems, configItems.size(), 5) && currentConfig != -1)
            buffer = configItems[currentConfig];

        ImGui::PushID(0);
        if (ImGui::InputTextWithHint("", "配置名称", &buffer, ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (currentConfig != -1)
                config.rename(currentConfig, buffer);
        }
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::PushItemWidth(100.0f);

        if (ImGui::Button("打开配置目录"))
            config.openConfigDir();

        if (ImGui::Button("创建配置", { 100.0f, 25.0f }))
            config.add(glow, backtrack, visuals, interfaces, memory, buffer.c_str());

        if (ImGui::Button("重置配置", { 100.0f, 25.0f }))
            ImGui::OpenPopup("要重置的配置");

        if (ImGui::BeginPopup("要重置的配置")) {
            static constexpr const char* names[]{ "全部", "自瞄", "开火", "回溯", "描边", "上色", "ESP", "视觉", "物品替换", "声音", "主题", "其他" };
            for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                if (i == 1) ImGui::Separator();

                ResetConfigurator configurator;

                if (ImGui::Selectable(names[i])) {
                    switch (i) {
                    case 0: config.reset(glow, backtrack, visuals, interfaces, memory); updateColors(config); Misc::updateClanTag(memory, true); inventory_changer::InventoryChanger::instance(interfaces, memory).scheduleHudUpdate(interfaces); break;
                    case 1: config.aimbot = { }; break;
                    case 2: config.triggerbot = { }; break;
                    case 3: backtrack.configure(configurator); break;
                    case 4: glow.resetConfig(); break;
                    case 5: config.chams = { }; break;
                    case 6: config.streamProofESP = { }; break;
                    case 7: visuals.resetConfig(); break;
                    case 8: inventory_changer::InventoryChanger::instance(interfaces, memory).reset(interfaces, memory); inventory_changer::InventoryChanger::instance(interfaces, memory).scheduleHudUpdate(interfaces); break;
                    case 9: Sound::resetConfig(); break;
                    case 10: config.style = { }; updateColors(config); break;
                    case 11: Misc::resetConfig(); Misc::updateClanTag(memory, true); break;
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (currentConfig != -1) {
            if (ImGui::Button("加载选中", { 100.0f, 25.0f })) {
                config.load(glow, backtrack, visuals, interfaces, memory, currentConfig, incrementalLoad);
                updateColors(config);
                inventory_changer::InventoryChanger::instance(interfaces, memory).scheduleHudUpdate(interfaces);
                Misc::updateClanTag(memory, true);
            }
            if (ImGui::Button("保存选中", { 100.0f, 25.0f }))
                config.save(glow, backtrack, visuals, interfaces, memory, currentConfig);
            if (ImGui::Button("删除选中", { 100.0f, 25.0f })) {
                config.remove(currentConfig);

                if (static_cast<std::size_t>(currentConfig) < configItems.size())
                    buffer = configItems[currentConfig];
                else
                    buffer.clear();
            }
        }
        ImGui::Columns(1);
        if (!contentOnly)
            ImGui::End();
}

void GUI::renderGuiStyle2(Glow& glow, Backtrack& backtrack, Visuals& visuals, const Engine& engine, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, Config& config) noexcept
{
    ImGui::Begin("Osiris", nullptr, windowFlags | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip)) {
        if (ImGui::BeginTabItem("自瞄")) {
            renderAimbotWindow(config, true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("开火")) {
            renderTriggerbotWindow(config, true);
            ImGui::EndTabItem();
        }
        backtrack.tabItem();
        glow.tabItem();
        if (ImGui::BeginTabItem("上色")) {
            renderChamsWindow(config, true);
            ImGui::EndTabItem();
        }
        StreamProofESP::tabItem(config);
        visuals.tabItem();
        InventoryChanger::tabItem(interfaces, memory);
        Sound::tabItem();
        if (ImGui::BeginTabItem("主题")) {
            renderStyleWindow(config, true);
            ImGui::EndTabItem();
        }
        Misc::tabItem(glow, engine, clientInterfaces, interfaces, memory);
        if (ImGui::BeginTabItem("配置")) {
            renderConfigWindow(glow, backtrack, visuals, interfaces, memory, config, true);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}
