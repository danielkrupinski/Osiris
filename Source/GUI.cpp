#include <algorithm>
#include <array>
#include <fstream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

#include "Platform/Macros/IsPlatform.h"

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
#include "CSGO/InputSystem.h"
#include "Hacks/Visuals.h"
#include "Hacks/Glow.h"
#include "Hacks/Backtrack.h"
#include "Hacks/Sound.h"
#include "Hacks/StreamProofESP.h"
#include <Config/ResetConfigurator.h>
#include "Hacks/Features.h"

#include "Hooks.h"

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

void GUI::render(Hooks& hooks, const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, Config& config) noexcept
{
    auto& features = config.getFeatures();
    if (!config.style.menuStyle) {
        renderMenuBar(features);
        renderAimbotWindow(config);
        renderTriggerbotWindow(config);
        features.backtrack.drawGUI(false);
        features.glow.drawGUI(false);
        renderChamsWindow(config);
        StreamProofESP::drawGUI(config, false);
        features.visuals.drawGUI(false);
        features.inventoryChanger.drawGUI(memory, false);
        features.sound.drawGUI(false);
        renderStyleWindow(config);
        features.misc.drawGUI(features.visuals, features.inventoryChanger, features.glow, false);
        renderConfigWindow(interfaces, memory, config);
    } else {
        renderGuiStyle2(engineInterfaces, clientInterfaces, interfaces, memory, config);
    }
    if (features.misc.unhook) {
        hooks.uninstall(features.misc, features.glow, features.visuals, features.inventoryChanger);
        features.misc.unhook = false;
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

void GUI::handleToggle(Misc& misc, const OtherInterfaces& interfaces) noexcept
{
    if (misc.isMenuKeyPressed()) {
        open = !open;
        if (!open)
            interfaces.getInputSystem().resetInputState();
#if !IS_WIN32()
        ImGui::GetIO().MouseDrawCursor = open;
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

void GUI::renderMenuBar(Features& features) noexcept
{
    if (ImGui::BeginMainMenuBar()) {
        menuBarItem("Aimbot", window.aimbot);
        menuBarItem("Triggerbot", window.triggerbot);
        features.backtrack.menuBarItem();
        features.glow.menuBarItem();
        menuBarItem("Chams", window.chams);
        StreamProofESP::menuBarItem();
        features.visuals.menuBarItem();
        features.inventoryChanger.menuBarItem();
        features.sound.menuBarItem();
        menuBarItem("Style", window.style);
        features.misc.menuBarItem();
        menuBarItem("Config", window.config);
        ImGui::EndMainMenuBar();   
    }
}

void GUI::renderAimbotWindow(Config& config, bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.aimbot)
            return;
        ImGui::SetNextWindowSize({ 600.0f, 0.0f });
        ImGui::Begin("Aimbot", &window.aimbot, windowFlags);
    }
    ImGui::Checkbox("On key", &config.aimbotOnKey);
    ImGui::SameLine();
    ImGui::PushID("Aimbot Key");
    ImGui::hotkey("", config.aimbotKey);
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(2);
    ImGui::PushItemWidth(70.0f);
    ImGui::Combo("", &config.aimbotKeyMode, "Hold\0Toggle\0");
    ImGui::PopItemWidth();
    ImGui::PopID();
    ImGui::Separator();
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "All\0Pistols\0Heavy\0SMG\0Rifles\0");
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
        static constexpr const char* pistols[]{ "All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

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
        static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

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
        static constexpr const char* smgs[]{ "All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

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
        static constexpr const char* rifles[]{ "All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

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
    ImGui::Checkbox("Enabled", &config.aimbot[currentWeapon].enabled);
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 220.0f);
    ImGui::Checkbox("Aimlock", &config.aimbot[currentWeapon].aimlock);
    ImGui::Checkbox("Silent", &config.aimbot[currentWeapon].silent);
    ImGui::Checkbox("Friendly fire", &config.aimbot[currentWeapon].friendlyFire);
    ImGui::Checkbox("Visible only", &config.aimbot[currentWeapon].visibleOnly);
    ImGui::Checkbox("Scoped only", &config.aimbot[currentWeapon].scopedOnly);
    ImGui::Checkbox("Ignore flash", &config.aimbot[currentWeapon].ignoreFlash);
    ImGui::Checkbox("Ignore smoke", &config.aimbot[currentWeapon].ignoreSmoke);
    ImGui::Checkbox("Auto shot", &config.aimbot[currentWeapon].autoShot);
    ImGui::Checkbox("Auto scope", &config.aimbot[currentWeapon].autoScope);
    ImGui::Combo("Bone", &config.aimbot[currentWeapon].bone, "Nearest\0Best damage\0Head\0Neck\0Sternum\0Chest\0Stomach\0Pelvis\0");
    ImGui::NextColumn();
    ImGui::PushItemWidth(240.0f);
    ImGui::SliderFloat("Fov", &config.aimbot[currentWeapon].fov, 0.0f, 255.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
    ImGui::SliderFloat("Smooth", &config.aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
    ImGui::SliderFloat("Max aim inaccuracy", &config.aimbot[currentWeapon].maxAimInaccuracy, 0.0f, 1.0f, "%.5f", ImGuiSliderFlags_Logarithmic);
    ImGui::SliderFloat("Max shot inaccuracy", &config.aimbot[currentWeapon].maxShotInaccuracy, 0.0f, 1.0f, "%.5f", ImGuiSliderFlags_Logarithmic);
    ImGui::InputInt("Min damage", &config.aimbot[currentWeapon].minDamage);
    config.aimbot[currentWeapon].minDamage = std::clamp(config.aimbot[currentWeapon].minDamage, 0, 250);
    ImGui::Checkbox("Killshot", &config.aimbot[currentWeapon].killshot);
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
        ImGui::Begin("Triggerbot", &window.triggerbot, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "All\0Pistols\0Heavy\0SMG\0Rifles\0Zeus x27\0");
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
        static constexpr const char* pistols[]{ "All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

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
        static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

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
        static constexpr const char* smgs[]{ "All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

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
        static constexpr const char* rifles[]{ "All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

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
    ImGui::Checkbox("Enabled", &config.triggerbot[currentWeapon].enabled);
    ImGui::Separator();
    ImGui::hotkey("Hold Key", config.triggerbotHoldKey);
    ImGui::Checkbox("Friendly fire", &config.triggerbot[currentWeapon].friendlyFire);
    ImGui::Checkbox("Scoped only", &config.triggerbot[currentWeapon].scopedOnly);
    ImGui::Checkbox("Ignore flash", &config.triggerbot[currentWeapon].ignoreFlash);
    ImGui::Checkbox("Ignore smoke", &config.triggerbot[currentWeapon].ignoreSmoke);
    ImGui::SetNextItemWidth(85.0f);
    ImGui::Combo("Hitgroup", &config.triggerbot[currentWeapon].hitgroup, "All\0Head\0Chest\0Stomach\0Left arm\0Right arm\0Left leg\0Right leg\0");
    ImGui::PushItemWidth(220.0f);
    ImGui::SliderInt("Shot delay", &config.triggerbot[currentWeapon].shotDelay, 0, 250, "%d ms");
    ImGui::InputInt("Min damage", &config.triggerbot[currentWeapon].minDamage);
    config.triggerbot[currentWeapon].minDamage = std::clamp(config.triggerbot[currentWeapon].minDamage, 0, 250);
    ImGui::Checkbox("Killshot", &config.triggerbot[currentWeapon].killshot);
    ImGui::SliderFloat("Burst Time", &config.triggerbot[currentWeapon].burstTime, 0.0f, 0.5f, "%.3f s");

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderChamsWindow(Config& config, bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.chams)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Chams", &window.chams, windowFlags);
    }

    ImGui::hotkey("Toggle Key", config.getFeatures().chams.toggleKey, 80.0f);
    ImGui::hotkey("Hold Key", config.getFeatures().chams.holdKey, 80.0f);
    ImGui::Separator();

    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);

    static int material = 1;

    if (ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0Hands\0Backtrack\0Sleeves\0"))
        material = 1;

    ImGui::PopID();

    ImGui::SameLine();

    if (material <= 1)
        ImGuiCustom::arrowButtonDisabled("##left", ImGuiDir_Left);
    else if (ImGui::ArrowButton("##left", ImGuiDir_Left))
        --material;

    ImGui::SameLine();
    ImGui::Text("%d", material);
    ImGui::SameLine();

    if (material >= int(config.getFeatures().chams.chamsMaterials[currentCategory].materials.size()))
        ImGuiCustom::arrowButtonDisabled("##right", ImGuiDir_Right);
    else if (ImGui::ArrowButton("##right", ImGuiDir_Right))
        ++material;

    ImGui::SameLine();

    auto& chams{ config.getFeatures().chams.chamsMaterials[currentCategory].materials[material - 1] };

    ImGui::Checkbox("Enabled", &chams.enabled);
    ImGui::Separator();
    ImGui::Checkbox("Health based", &chams.healthBased);
    ImGui::Checkbox("Blinking", &chams.blinking);

    int material_ = static_cast<int>(chams.material);
    ImGui::Combo("Material", &material_, [](void*, int idx, const char** out_text) {
        *out_text = toString(static_cast<ChamsMaterial>(idx)).data();
        return true;
    }, nullptr, ChamsMaterials::numberOfMaterials);
    chams.material = static_cast<ChamsMaterial>(material_);

    ImGui::Checkbox("Wireframe", &chams.wireframe);
    ImGui::Checkbox("Cover", &chams.cover);
    ImGui::Checkbox("Ignore-Z", &chams.ignorez);
    ImGuiCustom::colorPicker("Color", chams.color);

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
        ImGui::Begin("Style", &window.style, windowFlags);
    }

    ImGui::PushItemWidth(150.0f);
    if (ImGui::Combo("Menu style", &config.style.menuStyle, "Classic\0One window\0"))
        window = { };
    if (ImGui::Combo("Menu colors", &config.style.menuColors, "Dark\0Light\0Classic\0Custom\0"))
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

void GUI::renderConfigWindow(const OtherInterfaces& interfaces, const Memory& memory, Config& config, bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.config)
            return;
        ImGui::SetNextWindowSize({ 320.0f, 0.0f });
        if (!ImGui::Begin("Config", &window.config, windowFlags)) {
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
        if (ImGui::InputTextWithHint("", "config name", &buffer, ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (currentConfig != -1)
                config.rename(currentConfig, buffer);
        }
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::PushItemWidth(100.0f);

        if (ImGui::Button("Open config directory"))
            config.openConfigDir();

        if (ImGui::Button("Create config", { 100.0f, 25.0f }))
            config.add(interfaces, memory, buffer.c_str());

        if (ImGui::Button("Reset config", { 100.0f, 25.0f }))
            ImGui::OpenPopup("Config to reset");

        if (ImGui::BeginPopup("Config to reset")) {
            static constexpr const char* names[]{ "Whole", "Aimbot", "Triggerbot", "Backtrack", "Glow", "Chams", "ESP", "Visuals", "Inventory Changer", "Sound", "Style", "Misc" };
            for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                if (i == 1) ImGui::Separator();

                ResetConfigurator configurator;

                if (ImGui::Selectable(names[i])) {
                    switch (i) {
                    case 0: config.reset(interfaces, memory); updateColors(config); config.getFeatures().misc.updateClanTag(true); config.getFeatures().inventoryChanger.scheduleHudUpdate(); break;
                    case 1: config.aimbot = { }; break;
                    case 2: config.triggerbot = { }; break;
                    case 3: config.getFeatures().backtrack.configure(configurator); break;
                    case 4: config.getFeatures().glow.resetConfig(); break;
                    case 5: config.getFeatures().chams.configure(configurator); break;
                    case 6: config.streamProofESP = { }; break;
                    case 7: config.getFeatures().visuals.resetConfig(); break;
                    case 8: config.getFeatures().inventoryChanger.reset(memory); config.getFeatures().inventoryChanger.scheduleHudUpdate(); break;
                    case 9: config.getFeatures().sound.configure(configurator); break;
                    case 10: config.style = { }; updateColors(config); break;
                    case 11: config.getFeatures().misc.resetConfig(); config.getFeatures().misc.updateClanTag(true); break;
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (currentConfig != -1) {
            if (ImGui::Button("Load selected", { 100.0f, 25.0f })) {
                config.load(interfaces, memory, currentConfig, incrementalLoad);
                updateColors(config);
                config.getFeatures().inventoryChanger.scheduleHudUpdate();
                config.getFeatures().misc.updateClanTag(true);
            }
            if (ImGui::Button("Save selected", { 100.0f, 25.0f }))
                config.save(interfaces, memory, currentConfig);
            if (ImGui::Button("Delete selected", { 100.0f, 25.0f })) {
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

void GUI::renderGuiStyle2(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, Config& config) noexcept
{
    ImGui::Begin("Osiris", nullptr, windowFlags | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip)) {
        if (ImGui::BeginTabItem("Aimbot")) {
            renderAimbotWindow(config, true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Triggerbot")) {
            renderTriggerbotWindow(config, true);
            ImGui::EndTabItem();
        }
        config.getFeatures().backtrack.tabItem();
        config.getFeatures().glow.tabItem();
        if (ImGui::BeginTabItem("Chams")) {
            renderChamsWindow(config, true);
            ImGui::EndTabItem();
        }
        StreamProofESP::tabItem(config);
        config.getFeatures().visuals.tabItem();
        config.getFeatures().inventoryChanger.tabItem(memory);
        config.getFeatures().sound.tabItem();
        if (ImGui::BeginTabItem("Style")) {
            renderStyleWindow(config, true);
            ImGui::EndTabItem();
        }
        config.getFeatures().misc.tabItem(config.getFeatures().visuals, config.getFeatures().inventoryChanger, config.getFeatures().glow);
        if (ImGui::BeginTabItem("Config")) {
            renderConfigWindow(interfaces, memory, config, true);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}
