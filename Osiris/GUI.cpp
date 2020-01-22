#include <fstream>
#include <string>
#include <ShlObj.h>
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_stdlib.h"

#include "imguiCustom.h"

#include "GUI.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/SkinChanger.h"
#include "Hacks/Visuals.h"
#include "Hooks.h"
#include "SDK/InputSystem.h"

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

GUI::GUI() noexcept
{
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(FindWindowW(L"Valve001", NULL));

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    style.ScrollbarSize = 9.0f;

    ImGuiIO& io = ImGui::GetIO();
	ImFontConfig fontConfig;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    if (PWSTR pathToFonts; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &pathToFonts))) {
        const std::filesystem::path path{ pathToFonts };
        CoTaskMemFree(pathToFonts);

		static ImWchar ranges[] = {
		0x0020, 0x007E, // Basic Latin

		0x00A0, 0x00FF, // Latin-1 Supplement

				0x0100, 0x017F, // Latin Extended-A
		0x0180, 0x024F, // Latin Extended-B
		0x0370, 0x03FF, // Greek and Coptic
		0x0400, 0x04FF, // Cyrillic
		0x0500, 0x052F, // Cyrillic Supplementary
		0
		};

		static ImWchar KaiGenGothicCNRegular_ranges[] = {
			0x3000, 0x30FF, // Punctuations, Hiragana, Katakana
			0x31F0, 0x31FF, // Katakana Phonetic Extensions
			0xFF00, 0xFFEF, // Half-width characters
			0x4E00, 0x9FAF, // CJK Ideograms
			0
		};

		fonts.tahoma = io.Fonts->AddFontFromFileTTF((path / "tahoma.ttf").string().c_str(), 15.0f, &fontConfig, ranges);
		fonts.segoeui = io.Fonts->AddFontFromFileTTF((path / "segoeui.ttf").string().c_str(), 15.0f, &fontConfig, ranges);
		fonts.kaigengothic = io.Fonts->AddFontFromFileTTF((path / "KaiGenGothicCN-Regular.ttf").string().c_str(), 15.0f, &fontConfig, KaiGenGothicCNRegular_ranges);
		fontConfig.MergeMode = true;
		io.Fonts->Build();
    }
}

void GUI::render() noexcept
{
    if (!config.style.menuStyle) {
        renderMenuBar();
        renderAimbotWindow();
        renderAntiAimWindow();
        renderTriggerbotWindow();
        renderBacktrackWindow();
        renderGlowWindow();
        renderChamsWindow();
        renderEspWindow();
        renderVisualsWindow();
        renderSkinChangerWindow();
        renderSoundWindow();
        renderStyleWindow();
        renderMiscWindow();
        renderReportbotWindow();
        renderConfigWindow();
    } else {
        renderGuiStyle2();
    }
}

void GUI::updateColors() const noexcept
{
    switch (config.style.menuColors) {
    case 0: ImGui::StyleColorsDark(); break;
    case 1: ImGui::StyleColorsLight(); break;
    case 2: ImGui::StyleColorsClassic(); break;
    }
}

void GUI::hotkey(int& key) noexcept
{
    constexpr bool stringDisplayTest = true;

    if constexpr (stringDisplayTest)
        key ? ImGui::Text("[%s]", interfaces.inputSystem->virtualKeyToString(key)) : ImGui::TextUnformatted("[KEY]");
    else
        key ? ImGui::Text("[0x%x]", key) : ImGui::TextUnformatted("[KEY]");

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Press any key to change the keybind");
        ImGuiIO& io = ImGui::GetIO();
		for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); ++i)
            if (ImGui::IsKeyPressed(i) && i != config.misc.menuKey)
                key = i != VK_ESCAPE ? i : 0;

		for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); ++i)
            if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != config.misc.menuKey)
                key = i + (i > 1 ? 2 : 1);
    }
}

void GUI::renderMenuBar() noexcept
{
    if (ImGui::BeginMainMenuBar()) {
        ImGui::MenuItem("Aimbot", nullptr, &window.aimbot);
        ImGui::MenuItem("Anti-Aim", nullptr, &window.antiAim);
        ImGui::MenuItem("Triggerbot", nullptr, &window.triggerbot);
        ImGui::MenuItem("Backtrack", nullptr, &window.backtrack);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Abuse lag compensation to move players back in time");
        }
        ImGui::MenuItem("Glow", nullptr, &window.glow);
        ImGui::MenuItem("Chams", nullptr, &window.chams);
        ImGui::MenuItem("ESP", nullptr, &window.esp);
        ImGui::MenuItem("Visuals", nullptr, &window.visuals);
        ImGui::MenuItem("Skinchanger", nullptr, &window.skinChanger);
        ImGui::MenuItem("Sound", nullptr, &window.sound);
        ImGui::MenuItem("Style", nullptr, &window.style);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("The style of the cheat menu");
        }
        ImGui::MenuItem("Misc", nullptr, &window.misc);
        ImGui::MenuItem("Reportbot", nullptr, &window.reportbot);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Automatically report players on the current server for selected actions");
        }
        ImGui::MenuItem("Configs", nullptr, &window.config);
        ImGui::EndMainMenuBar();
    }
}

void GUI::renderAimbotWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.aimbot)
            return;
        ImGui::SetNextWindowSize({ 600.0f, 0.0f });
        ImGui::Begin("Aimbot", &window.aimbot, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "All\0Pistols\0Heavy\0SMGs\0Rifles\0");
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
        static constexpr const char* pistols[]{ "All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-SeveN", "CZ75 Auto", "Desert Eagle", "R8 Revolver" };

        ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
            if (config.aimbot[idx ? idx : 35].enabled) {
                static std::string name;
                name = pistols[idx];
                *out_text = name.append(" *").c_str();
            }
            else {
                *out_text = pistols[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(pistols));

        currentWeapon = currentPistol ? currentPistol : 35;
        break;
    }
    case 2: {
        static int currentHeavy{ 0 };
        static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-Off", "MAG-7", "M249", "Negev" };

        ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
            if (config.aimbot[idx ? idx + 10 : 36].enabled) {
                static std::string name;
                name = heavies[idx];
                *out_text = name.append(" *").c_str();
            }
            else {
                *out_text = heavies[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(heavies));

        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
        break;
    }
    case 3: {
        static int currentSmg{ 0 };
        static constexpr const char* smgs[]{ "All", "MAC-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

        ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
            if (config.aimbot[idx ? idx + 16 : 37].enabled) {
                static std::string name;
                name = smgs[idx];
                *out_text = name.append(" *").c_str();
            }
            else {
                *out_text = smgs[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(smgs));

        currentWeapon = currentSmg ? currentSmg + 16 : 37;
        break;
    }
    case 4: {
        static int currentRifle{ 0 };
        static constexpr const char* rifles[]{ "All", "Galil AR", "FAMAS", "AK-47", "M4A4", "M4A1-S", "SSG 08", "SG 553", "AUG", "AWP", "G3SG1", "SCAR-20" };

        ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
            if (config.aimbot[idx ? idx + 23 : 38].enabled) {
                static std::string name;
                name = rifles[idx];
                *out_text = name.append(" *").c_str();
            }
            else {
                *out_text = rifles[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(rifles));

        currentWeapon = currentRifle ? currentRifle + 23 : 38;
        break;
    }
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::Checkbox("Enabled", &config.aimbot[currentWeapon].enabled);
    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 220.0f);
    ImGui::Checkbox("On Key", &config.aimbot[currentWeapon].onKey);
    ImGui::SameLine();
    hotkey(config.aimbot[currentWeapon].key);
    ImGui::SameLine();
    ImGui::PushID(2);
    ImGui::PushItemWidth(70.0f);
    ImGui::Combo("", &config.aimbot[currentWeapon].keyMode, "Hold\0Toggle\0");
    ImGui::PopItemWidth();
    ImGui::PopID();
    ImGui::Checkbox("Aimlock", &config.aimbot[currentWeapon].aimlock);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Aimbot locks on to targets");
    }
    ImGui::Checkbox("Silent", &config.aimbot[currentWeapon].silent);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("No snapping to targets (clientside only)");
    }
    ImGui::Checkbox("Friendly Fire", &config.aimbot[currentWeapon].friendlyFire);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Aimbot treats allies as enemies");
    }
    ImGui::Checkbox("Visible Only", &config.aimbot[currentWeapon].visibleOnly);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Aimbot only aims at visible targets");
    }
    ImGui::Checkbox("Scoped Only", &config.aimbot[currentWeapon].scopedOnly);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Aimbot only aims when you are scoped in");
    }
    ImGui::Checkbox("Ignore Flash", &config.aimbot[currentWeapon].ignoreFlash);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Aimbot aims when you are flashed");
    }
    ImGui::Checkbox("Ignore Smoke", &config.aimbot[currentWeapon].ignoreSmoke);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Aimbot aims when you are smoked");
    }
    ImGui::Checkbox("Auto Shoot", &config.aimbot[currentWeapon].autoShot);
    ImGui::Checkbox("Auto Scope", &config.aimbot[currentWeapon].autoScope);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Automatically scope in before shooting");
    }
    ImGui::Checkbox("Recoil-Based FoV", &config.aimbot[currentWeapon].recoilbasedFov);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Aimbot uses recoil as an FoV origin");
    }
    ImGui::Combo("Bone", &config.aimbot[currentWeapon].bone, "Nearest\0Best Damage\0Head\0Neck\0Sternum\0Chest\0Stomach\0Pelvis\0");
    ImGui::NextColumn();
    ImGui::PushItemWidth(240.0f);
    ImGui::SliderFloat("FoV", &config.aimbot[currentWeapon].fov, 0.0f, 255.0f, "%.2f", 2.5f);
    ImGui::SliderFloat("Smoothing", &config.aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Smooths out aimbot movement to appear more human-like");
    }
    ImGui::SliderFloat("Recoil Control X", &config.aimbot[currentWeapon].recoilControlX, 0.0f, 1.0f, "%.2f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Horizontal Recoil Control factor");
    }
    ImGui::SliderFloat("Recoil Control Y", &config.aimbot[currentWeapon].recoilControlY, 0.0f, 1.0f, "%.2f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Vertical Recoil Control factor");
    }
    ImGui::SliderFloat("Max Aim Inaccuracy", &config.aimbot[currentWeapon].maxAimInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("How accurately the aimbot can shoot before being disabled");
    }
    ImGui::SliderFloat("Max Shot Inaccuracy", &config.aimbot[currentWeapon].maxShotInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("How accurate aimbot shots must be before the aimbot is disabled");
    }
    ImGui::Checkbox("Standalone RCS", &config.aimbot[currentWeapon].standaloneRCS);
    if (config.aimbot[currentWeapon].standaloneRCS)
    {
        ImGui::Combo("Mode", &config.aimbot[currentWeapon].rcsStyle, "Normal\0With Ignore Shots\0");
    }
    ImGui::InputInt("Min. Damage", &config.aimbot[currentWeapon].minDamage);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("The amount of damage the aimbot must be able to do before shooting");
    }
    config.aimbot[currentWeapon].minDamage = std::clamp(config.aimbot[currentWeapon].minDamage, 0, 250);
    ImGui::InputInt("Hit Chance", &config.aimbot[currentWeapon].hitChance);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Aimbot only shoots if it can hit the target");
    }
    config.aimbot[currentWeapon].hitChance = std::clamp(config.aimbot[currentWeapon].hitChance, 0, 100);
    ImGui::Checkbox("Killshot", &config.aimbot[currentWeapon].killshot);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Aimbot will only shoot if it can 1-hit kill enemies through walls");
    }
    ImGui::Checkbox("Between Shots", &config.aimbot[currentWeapon].betweenShots);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Aimbot will continue to lock on to other targets after kills");
    }
    ImGui::Checkbox("Velocity Extrapolation", &config.aimbot[currentWeapon].velocityExtrapolation);
    ImGui::Checkbox("FoV Circle", &config.aimbot[currentWeapon].aimbotCircle);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Draws the FoV you set");
    }
    ImGui::SliderFloat("Ignore Shots", &config.aimbot[currentWeapon].shotsFired, 0.0f, 10.0f, "%1.0f");
    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderAntiAimWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.antiAim)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Anti-Aim", &window.antiAim, windowFlags);
    }
    ImGui::Checkbox("Enabled", &config.antiAim.enabled);
    ImGui::Checkbox("##pitch", &config.antiAim.pitch);
    ImGui::SameLine();
    ImGui::SliderFloat("Pitch", &config.antiAim.pitchAngle, -89.0f, 89.0f, "%.2f");
    ImGui::Checkbox("Yaw", &config.antiAim.yaw);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderTriggerbotWindow(bool contentOnly) noexcept
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
    ImGui::Combo("", &currentCategory, "All\0Pistols\0Heavy\0SMGs\0Rifles\0Zeus x27\0");
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
        static constexpr const char* pistols[]{ "All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-SeveN", "CZ75 Auto", "Desert Eagle", "R8 Revolver" };

        ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
            if (config.triggerbot[idx ? idx : 35].enabled) {
                static std::string name;
                name = pistols[idx];
                *out_text = name.append(" *").c_str();
            }
            else {
                *out_text = pistols[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(pistols));

        currentWeapon = currentPistol ? currentPistol : 35;
        break;
    }
    case 2: {
        static int currentHeavy{ 0 };
        static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-Off", "MAG-7", "M249", "Negev" };

        ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
            if (config.triggerbot[idx ? idx + 10 : 36].enabled) {
                static std::string name;
                name = heavies[idx];
                *out_text = name.append(" *").c_str();
            }
            else {
                *out_text = heavies[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(heavies));

        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
        break;
    }
    case 3: {
        static int currentSmg{ 0 };
        static constexpr const char* smgs[]{ "All", "MAC-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

        ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
            if (config.triggerbot[idx ? idx + 16 : 37].enabled) {
                static std::string name;
                name = smgs[idx];
                *out_text = name.append(" *").c_str();
            }
            else {
                *out_text = smgs[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(smgs));

        currentWeapon = currentSmg ? currentSmg + 16 : 37;
        break;
    }
    case 4: {
        static int currentRifle{ 0 };
        static constexpr const char* rifles[]{ "All", "Galil AR", "FAMAS", "AK-47", "M4A4", "M4A1-S", "SSG 08", "SG 553", "AUG", "AWP", "G3SG1", "SCAR-20" };

        ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
            if (config.triggerbot[idx ? idx + 23 : 38].enabled) {
                static std::string name;
                name = rifles[idx];
                *out_text = name.append(" *").c_str();
            }
            else {
                *out_text = rifles[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(rifles));

        currentWeapon = currentRifle ? currentRifle + 23 : 38;
        break;
    }
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::Checkbox("Enabled", &config.triggerbot[currentWeapon].enabled);
    ImGui::Separator();
    ImGui::Checkbox("On Key", &config.triggerbot[currentWeapon].onKey);
    ImGui::SameLine();
    hotkey(config.triggerbot[currentWeapon].key);
    ImGui::Checkbox("Friendly Fire", &config.triggerbot[currentWeapon].friendlyFire);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Triggerbot treats allies as enemies");
    }
    ImGui::Checkbox("Scoped Only", &config.triggerbot[currentWeapon].scopedOnly);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Triggerbot only shoots when you are scoped in");
    }
    ImGui::Checkbox("Ignore Flash", &config.triggerbot[currentWeapon].ignoreFlash);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Triggerbot shoots when you are flashed");
    }
    ImGui::Checkbox("Ignore Smoke", &config.triggerbot[currentWeapon].ignoreSmoke);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Triggerbot shoots when you are smoked");
    }
    ImGui::SetNextItemWidth(85.0f);
    ImGui::Combo("Hitgroup", &config.triggerbot[currentWeapon].hitgroup, "All\0Head\0Chest\0Stomach\0Left Arm\0Right Arm\0Left Leg\0Right Leg\0");
    ImGui::PushItemWidth(220.0f);
    ImGui::SliderInt("", &config.triggerbot[currentWeapon].shotDelay, 0, 250, "Shot Delay: %d ms");
    ImGui::InputInt("Min. Damage", &config.triggerbot[currentWeapon].minDamage, 0, 250);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("The amount of damage the triggerbot must be able to do before shooting");
    }
    ImGui::Checkbox("Killshot", &config.triggerbot[currentWeapon].killshot);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Triggerbot will only shoot if it can do damage equal to or more than the target's health");
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderBacktrackWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.backtrack)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Backtrack", &window.backtrack, windowFlags);
    }
    ImGui::Checkbox("Enabled", &config.backtrack.enabled);
    ImGui::Checkbox("Ignore Smoke", &config.backtrack.ignoreSmoke);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Backtrack targets when they are smoked");
    }
    ImGui::Checkbox("Recoil-Based FoV", &config.backtrack.recoilBasedFov);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Backtrack uses recoil as an FoV origin");
    }
    ImGui::Checkbox("Draw All Ticks", &config.backtrack.drawAllTicks);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shows all backtrack records");
    }
    ImGui::Checkbox("Ping Based", &config.backtrack.pingBased);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Uses your ping to determine how far to backtrack");
    }
    if (!config.backtrack.pingBased) {
        ImGui::PushItemWidth(220.0f);
        ImGui::SliderInt("", &config.backtrack.timeLimit, 1, 200, "Amount: %d ms");
        ImGui::PopItemWidth();
    }
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderGlowWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.glow)
            return;
        ImGui::SetNextWindowSize({ 450.0f, 0.0f });
        ImGui::Begin("Glow", &window.glow, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local Player\0Weapons\0C4\0Planted C4\0Chickens\0Defuse Kits\0Projectiles\0Hostages\0Ragdolls\0");
    ImGui::PopID();
    static int currentItem{ 0 };
    if (currentCategory <= 3) {
        ImGui::SameLine();
        static int currentType{ 0 };
        ImGui::PushID(1);
        ImGui::Combo("", &currentType, "All\0Visible\0Occluded\0");
        ImGui::PopID();
        currentItem = currentCategory * 3 + currentType;
    }
    else {
        currentItem = currentCategory + 8;
    }

    ImGui::SameLine();
    ImGui::Checkbox("Enabled", &config.glow[currentItem].enabled);
    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 150.0f);
    ImGui::Checkbox("Health-Based", &config.glow[currentItem].healthBased);

    ImGuiCustom::colorPicker("Colour", config.glow[currentItem].color.color, nullptr, &config.glow[currentItem].color.rainbow, &config.glow[currentItem].color.rainbowSpeed);

    ImGui::NextColumn();
    ImGui::PushItemWidth(220.0f);
    ImGui::SliderFloat("Thickness", &config.glow[currentItem].thickness, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Alpha", &config.glow[currentItem].alpha, 0.0f, 1.0f, "%.2f");
    ImGui::SliderInt("Style", &config.glow[currentItem].style, 0, 3);
    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderChamsWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.chams)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Chams", &window.chams, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local Player\0Weapons\0Sleeves\0Hands\0Backtrack\0");
    ImGui::PopID();
    static int currentItem{ 0 };

    if (currentCategory <= 3) {
        ImGui::SameLine();
        static int currentType{ 0 };
        ImGui::PushID(1);
        ImGui::Combo("", &currentType, "All\0Visible\0Occluded\0");
        ImGui::PopID();
        currentItem = currentCategory * 3 + currentType;
    }
    else {
        currentItem = currentCategory + 8;
    }

    ImGui::SameLine();
    static auto material{ 1 };
    ImGui::InputInt("##mat", &material, 1, 2);
    material = std::clamp(material, 1, 2);
    ImGui::SameLine();
    auto& chams{ config.chams[currentItem].materials[material - 1] };

    ImGui::Checkbox("Enabled", &chams.enabled);
    ImGui::Separator();
    ImGui::Checkbox("Health-Based", &chams.healthBased);
    ImGui::Checkbox("Blinking", &chams.blinking);
    ImGui::Combo("Material", &chams.material, "Normal\0Flat\0Animated\0Platinum\0Glass\0Chrome\0Crystal\0Silver\0Gold\0Plastic\0");
    ImGui::Checkbox("Wireframe", &chams.wireframe);
    ImGuiCustom::colorPicker("Colour", chams.color.color, nullptr, &chams.color.rainbow, &chams.color.rainbowSpeed);
    ImGui::SetNextItemWidth(220.0f);
    ImGui::SliderFloat("Alpha", &chams.alpha, 0.0f, 1.0f, "%.2f");

    if (!contentOnly) {
        ImGui::End();
    }
}

void GUI::renderEspWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.esp)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("ESP", &window.esp, windowFlags);
    }

    static int currentCategory = 0;
    static int currentItem = 0;

    if (ImGui::ListBoxHeader("##", { 125.0f, 300.0f })) {
        static constexpr const char* players[]{ "All", "Visible", "Occluded" };

        ImGui::Text("Allies");
        ImGui::Indent();
        ImGui::PushID("Allies");
        ImGui::PushFont(fonts.segoeui);

        for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
            bool isSelected = currentCategory == 0 && currentItem == i;

            if ((i == 0 || !config.esp.players[0].enabled) && ImGui::Selectable(players[i], isSelected)) {
                currentItem = i;
                currentCategory = 0;
            }
        }

        ImGui::PopFont();
        ImGui::PopID();
        ImGui::Unindent();
        ImGui::Text("Enemies");
        ImGui::Indent();
        ImGui::PushID("Enemies");
        ImGui::PushFont(fonts.segoeui);

        for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
            bool isSelected = currentCategory == 1 && currentItem == i;

            if ((i == 0 || !config.esp.players[3].enabled) && ImGui::Selectable(players[i], isSelected)) {
                currentItem = i;
                currentCategory = 1;
            }
        }

        ImGui::PopFont();
        ImGui::PopID();
        ImGui::Unindent();
        if (bool isSelected = currentCategory == 2; ImGui::Selectable("Weapons", isSelected))
            currentCategory = 2;

        ImGui::Text("Projectiles");
        ImGui::Indent();
        ImGui::PushID("Projectiles");
        ImGui::PushFont(fonts.segoeui);
        static constexpr const char* projectiles[]{ "Flashbang", "HE Grenade", "Breach Charge", "Bump Mine", "Decoy Grenade", "Molotov", "TA Grenade", "Smoke Grenade", "Snowball" };

        for (int i = 0; i < IM_ARRAYSIZE(projectiles); i++) {
            bool isSelected = currentCategory == 3 && currentItem == i;

            if (ImGui::Selectable(projectiles[i], isSelected)) {
                currentItem = i;
                currentCategory = 3;
            }
        }

        ImGui::PopFont();
        ImGui::PopID();
        ImGui::Unindent();

        ImGui::Text("Dangerzone");
        ImGui::Indent();
        ImGui::PushID("Dangerzone");
        ImGui::PushFont(fonts.segoeui);
        static constexpr const char* dangerZone[]{ "Sentries", "Drones", "Cash", "Cash Dufflebag", "Pistol Case", "Light Case", "Heavy Case", "Explosive Case", "Tools Case", "Full Armor", "Armor", "Helmet", "Parachute", "Briefcase", "Tablet Upgrade", "ExoJump", "Ammobox", "Radar Jammer" };

        for (int i = 0; i < IM_ARRAYSIZE(dangerZone); i++) {
            bool isSelected = currentCategory == 4 && currentItem == i;

            if (ImGui::Selectable(dangerZone[i], isSelected)) {
                currentItem = i;
                currentCategory = 4;
            }
        }

        ImGui::PopFont();
        ImGui::PopID();
        ImGui::ListBoxFooter();
    }
    ImGui::SameLine();
    if (ImGui::BeginChild("##child", { 400.0f, 0.0f })) {
        switch (currentCategory) {
        case 0:
        case 1: {
            int selected = currentCategory * 3 + currentItem;
            ImGui::Checkbox("Enabled", &config.esp.players[selected].enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("Font", &config.esp.players[selected].font, 1, 294);
            config.esp.players[selected].font = std::clamp(config.esp.players[selected].font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 200.0f };
            ImGuiCustom::colorPicker("Snaplines", config.esp.players[selected].snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Box", config.esp.players[selected].box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config.esp.players[selected].boxType, "2D\0""2D Cornered\0""3D\0""3D Cornered\0");
            ImGuiCustom::colorPicker("View Angles", config.esp.players[selected].eyeTraces);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Health", config.esp.players[selected].health);
            ImGuiCustom::colorPicker("Head Dot", config.esp.players[selected].headDot);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Health Bar", config.esp.players[selected].healthBar);
            ImGuiCustom::colorPicker("Name", config.esp.players[selected].name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Armour", config.esp.players[selected].armor);
            ImGuiCustom::colorPicker("Money", config.esp.players[selected].money);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Armour Bar", config.esp.players[selected].armorBar);
            ImGuiCustom::colorPicker("Outline", config.esp.players[selected].outline);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Distance", config.esp.players[selected].distance);
            ImGuiCustom::colorPicker("Active Weapon", config.esp.players[selected].activeWeapon);
            ImGui::SameLine(spacing);
            ImGui::Checkbox("Dead ESP", &config.esp.players[selected].deadesp);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Shows ESP whenever you're dead");
            }
            ImGui::SliderFloat("Max Distance", &config.esp.players[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        case 2: {
            ImGui::Checkbox("Enabled", &config.esp.weapon.enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("Font", &config.esp.weapon.font, 1, 294);
            config.esp.weapon.font = std::clamp(config.esp.weapon.font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 200.0f };
            ImGuiCustom::colorPicker("Snaplines", config.esp.weapon.snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Box", config.esp.weapon.box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config.esp.weapon.boxType, "2D\0""2D Cornered\0""3D\0""3D Cornered\0");
            ImGuiCustom::colorPicker("Name", config.esp.weapon.name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Outline", config.esp.weapon.outline);
            ImGuiCustom::colorPicker("Distance", config.esp.weapon.distance);
            ImGui::SliderFloat("Max Distance", &config.esp.weapon.maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        case 3: {
            ImGui::Checkbox("Enabled", &config.esp.projectiles[currentItem].enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("Font", &config.esp.projectiles[currentItem].font, 1, 294);
            config.esp.projectiles[currentItem].font = std::clamp(config.esp.projectiles[currentItem].font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 200.0f };
            ImGuiCustom::colorPicker("Snaplines", config.esp.projectiles[currentItem].snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Box", config.esp.projectiles[currentItem].box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config.esp.projectiles[currentItem].boxType, "2D\0""2D Cornered\0""3D\0""3D Cornered\0");
            ImGuiCustom::colorPicker("Name", config.esp.projectiles[currentItem].name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Outline", config.esp.projectiles[currentItem].outline);
            ImGuiCustom::colorPicker("Distance", config.esp.projectiles[currentItem].distance);
            ImGui::SliderFloat("Max Distance", &config.esp.projectiles[currentItem].maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        case 4: {
            int selected = currentItem;
            ImGui::Checkbox("Enabled", &config.esp.dangerZone[selected].enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("Font", &config.esp.dangerZone[selected].font, 1, 294);
            config.esp.dangerZone[selected].font = std::clamp(config.esp.dangerZone[selected].font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 200.0f };
            ImGuiCustom::colorPicker("Snaplines", config.esp.dangerZone[selected].snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Box", config.esp.dangerZone[selected].box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config.esp.dangerZone[selected].boxType, "2D\0""2D Cornered\0""3D\0""3D Cornered\0");
            ImGuiCustom::colorPicker("Name", config.esp.dangerZone[selected].name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Outline", config.esp.dangerZone[selected].outline);
            ImGuiCustom::colorPicker("Distance", config.esp.dangerZone[selected].distance);
            ImGui::SliderFloat("Max Distance", &config.esp.dangerZone[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        }

        ImGui::EndChild();
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderVisualsWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.visuals)
            return;
        ImGui::SetNextWindowSize({ 680.0f, 0.0f });
        ImGui::Begin("Visuals", &window.visuals, windowFlags);
    }
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 280.0f);
    ImGui::Checkbox("Disable Post-Processing", &config.visuals.disablePostProcessing);
    ImGui::Checkbox("Disable HUD Blur", &config.visuals.disablePanoramablur);
    ImGui::Checkbox("Inverse Ragdoll Gravity", &config.visuals.inverseRagdollGravity);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Reverses the gravity of ragdolls");
    }
    if (config.visuals.inverseRagdollGravity)
    {
        ImGui::PushID(0);
        ImGui::SliderInt("", &config.visuals.inverseRagdollGravityValue, -2400, 2400, "Gravity Value: %d");
        ImGui::PopID();
    }
    ImGui::Checkbox("Physics Timescale", &config.visuals.ragdollTimescale);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Controls the speed of ragdolls and other physic based objects");
    }
    if (config.visuals.ragdollTimescale) {
        ImGui::PushID(1);
        ImGui::SliderInt("", &config.visuals.ragdollTimescaleValue, 0, 10, "Value: %d");
        ImGui::PopID();
    }
    ImGui::Checkbox("No Fog", &config.visuals.noFog);
    ImGui::Checkbox("No 3D Sky", &config.visuals.no3dSky);
    ImGui::Checkbox("No Aim Punch", &config.visuals.noAimPunch);
    ImGui::Checkbox("No View Punch", &config.visuals.noViewPunch);
    ImGui::Checkbox("No Viewmodel Sway", &config.visuals.viewBob);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Only on movement");
    }
    ImGui::Checkbox("No Hands", &config.visuals.noHands);
    ImGui::Checkbox("No Sleeves", &config.visuals.noSleeves);
    ImGui::Checkbox("No Weapons", &config.visuals.noWeapons);
    ImGui::Checkbox("No Smoke", &config.visuals.noSmoke);
    ImGui::Checkbox("No Blur", &config.visuals.noBlur);
    ImGui::Checkbox("No Scope Overlay", &config.visuals.noScopeOverlay);
    ImGui::Checkbox("No Scope Zoom", &config.visuals.noZoom);
    ImGui::Checkbox("No Grass", &config.visuals.noGrass);
    ImGui::Checkbox("No Shadows", &config.visuals.noShadows);
    ImGui::Checkbox("Wireframe Smoke", &config.visuals.wireframeSmoke);
    ImGui::NextColumn();
    ImGui::Checkbox("Zoom", &config.visuals.zoom);
    ImGui::SameLine();
    hotkey(config.visuals.zoomKey);
    ImGui::Checkbox("Thirdperson", &config.visuals.thirdperson);
    ImGui::SameLine();
    hotkey(config.visuals.thirdpersonKey);
    ImGui::PushItemWidth(290.0f);
    ImGui::PushID(2);
    ImGui::SliderInt("", &config.visuals.thirdpersonDistance, 0, 1000, "Thirdperson Distance: %d");
    ImGui::PopID();
    ImGui::PushID(3);
    ImGui::SliderInt("", &config.visuals.fov, -60, 60, "FoV: %d");
    ImGui::PopID();
    ImGui::PushID(4);
    ImGui::SliderInt("", &config.visuals.viewmodelFov, -60, 60, "Viewmodel FoV: %d");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("How close your viewmodels are to your screen");
    }
    ImGui::PopID();
    ImGui::PushID(5);
    bool viewModelSwitch = 0;
    ImGui::Checkbox("Viewmodel Offsets", &config.visuals.viewModel);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("The positions of viewmodels");
    }
    if (!config.visuals.viewModel) {
        config.visuals.viewModelKnifeEnabled = 0;
    }
    if (!config.visuals.viewModelKnife) {
        viewModelSwitch = 0;
    }
    if (config.visuals.viewModelKnife) {
        viewModelSwitch = 1;
    }
    if (config.visuals.viewModel && !config.visuals.viewModelKnife && !viewModelSwitch) {
        config.visuals.viewModelKnifeEnabled = 1;
        ImGui::SameLine();
        ImGui::Checkbox("Knife Offsets", &config.visuals.viewModelKnife);
        ImGui::PushItemWidth(280.0f);
        ImGui::PushID(6);
        ImGui::SliderFloat("", &config.visuals.viewModel_x, -20, 20, "X: %.2f");
        ImGui::PopID();
        ImGui::PushID(7);
        ImGui::SliderFloat("", &config.visuals.viewModel_y, -20, 20, "Y: %.2f");
        ImGui::PopID();
        ImGui::PushID(8);
        ImGui::SliderFloat("", &config.visuals.viewModel_z, -20, 20, "Z: %.2f");
        ImGui::PopID();
    }
    if (config.visuals.viewModelKnife && viewModelSwitch && config.visuals.viewModel) {
        config.visuals.viewModelKnifeEnabled = 1;
        ImGui::SameLine();
        ImGui::Checkbox("Knife Offsets", &config.visuals.viewModelKnife);
        ImGui::PushItemWidth(280.0f);
        ImGui::PushID(9);
        ImGui::SliderFloat("", &config.visuals.viewModel_x_Knife, -20, 20, "X: %.2f");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Up/Down");
        }
        ImGui::PopID();
        ImGui::PushID(10);
        ImGui::SliderFloat("", &config.visuals.viewModel_y_Knife, -20, 20, "Y: %.2f");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Left/Right");
        }
        ImGui::PopID();
        ImGui::PushID(11);
        ImGui::SliderFloat("", &config.visuals.viewModel_z_Knife, -20, 20, "Z: %.2f");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("To/From");
        }
        ImGui::PopID();
        ImGui::Checkbox("Flip Knife Hand", &config.visuals.viewModelFlipKnifeHand);
    }
    ImGui::SliderInt("", &config.visuals.farZ, 0, 2000, "Far Z: %d");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Useful for drawing distant objects such as buildings when fog is disabled");
    }
    ImGui::PopID();
    ImGui::PushID(12);
    ImGui::SliderInt("", &config.visuals.flashReduction, 0, 100, "Flash Reduction: %d%%");
    ImGui::PopID();
    if (!config.visuals.fullBright) {
        ImGui::PushID(13);
        ImGui::SliderFloat("", &config.visuals.brightness, 0.0f, 1.0f, "Brightness: %.2f");
        ImGui::PopID();
    }
    else {
        ImGui::PushID(14);
        ImGui::SliderFloat("", &config.visuals.brightness, 0.0f, 0.0f, "Disabled for Full Bright");
        ImGui::PopID();
    }
    ImGui::PopItemWidth();
    ImGui::Checkbox("Full Bright", &config.visuals.fullBright);
    ImGui::Checkbox("Colour Correction", &config.visuals.colorCorrection.enabled);
    ImGui::SameLine();
    bool ccPopup = ImGui::Button("Edit");

    if (ccPopup)
        ImGui::OpenPopup("##popup");

    if (ImGui::BeginPopup("##popup")) {
        ImGui::VSliderFloat("##1", { 40.0f, 160.0f }, &config.visuals.colorCorrection.blue, 0.0f, 1.0f, "Blue\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##2", { 40.0f, 160.0f }, &config.visuals.colorCorrection.red, 0.0f, 1.0f, "Red\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##3", { 40.0f, 160.0f }, &config.visuals.colorCorrection.mono, 0.0f, 1.0f, "Mono\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##4", { 40.0f, 160.0f }, &config.visuals.colorCorrection.saturation, 0.0f, 1.0f, "Sat.\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##5", { 40.0f, 160.0f }, &config.visuals.colorCorrection.ghost, 0.0f, 1.0f, "Ghost\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##6", { 40.0f, 160.0f }, &config.visuals.colorCorrection.green, 0.0f, 1.0f, "Green\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##7", { 40.0f, 160.0f }, &config.visuals.colorCorrection.yellow, 0.0f, 1.0f, "Yellow\n%.3f"); ImGui::SameLine();
        ImGui::EndPopup();
    }
    ImGui::Combo("Skybox", &config.visuals.skybox, "Default\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0");
    ImGuiCustom::colorPicker("World Colour", config.visuals.world);
    ImGuiCustom::colorPicker("Sky Colour", config.visuals.sky);
    ImGui::Checkbox("Spinning Deagle Inspect", &config.visuals.deagleSpinner);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Always plays the rare spinning Deagle inspect whenever inspected");
    }
    ImGui::Combo("Screen Effect", &config.visuals.screenEffect, "None\0Drone Cam\0Drone Cam With Noise\0Underwater\0Healthboost\0Dangerzone\0");
    ImGui::Combo("Hitmarker", &config.visuals.hitMarker, "None\0Drone Cam\0Drone Cam With Noise\0Underwater\0Healthboost\0Dangerzone\0Classic\0");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shows an effect whenever a target is hit");
    }
    ImGui::SliderFloat("Hitmarker Time", &config.visuals.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
    ImGui::Checkbox("Damage Indicator", &config.visuals.hitMarkerDamageIndicator);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("How much damage is dealt whenever targets are hit");
    }
    ImGui::SameLine();
    ImGui::Checkbox("Customize", &config.visuals.hitMarkerDamageIndicatorCustomize);
    if (config.visuals.hitMarkerDamageIndicator && config.visuals.hitMarkerDamageIndicatorCustomize) {
        ImGui::InputInt("Font", &config.visuals.hitMarkerDamageIndicatorFont, 1, 294);
        ImGui::InputInt("Alpha", &config.visuals.hitMarkerDamageIndicatorAlpha, 1, 1000);
        ImGui::InputInt("Distance", &config.visuals.hitMarkerDamageIndicatorDist, -100, 100);
        ImGui::InputInt("X", &config.visuals.hitMarkerDamageIndicatorTextX, -100, 100);
        ImGui::InputInt("Y", &config.visuals.hitMarkerDamageIndicatorTextY, -100, 100);
        ImGui::PushID(15);
        ImGui::SliderFloat(" ", &config.visuals.hitMarkerDamageIndicatorRatio, -1.0f, 1.0f, "Z: %.2f");
        ImGui::PopID();
    }
    ImGui::Combo("Player Model (T)", &config.visuals.playerModelT, "Default\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0");
    ImGui::Combo("Player Model (CT)", &config.visuals.playerModelCT, "Default\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0");
    ImGui::Columns(1);

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderSkinChangerWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.skinChanger)
            return;
        ImGui::SetNextWindowSize({ 700.0f, 0.0f });
        ImGui::Begin("nSkinz", &window.skinChanger, windowFlags);
    }

    static auto itemIndex = 0;

    ImGui::PushItemWidth(110.0f);
    ImGui::Combo("##1", &itemIndex, [](void* data, int idx, const char** out_text) {
        *out_text = game_data::weapon_names[idx].name;
        return true;
        }, nullptr, IM_ARRAYSIZE(game_data::weapon_names), 5);
    ImGui::PopItemWidth();

    auto& selected_entry = config.skinChanger[itemIndex];
    selected_entry.definition_vector_index = itemIndex;

    {
        ImGui::SameLine();
        ImGui::Checkbox("Enabled", &selected_entry.enabled);
        ImGui::Separator();
        ImGui::Columns(2, nullptr, false);
        ImGui::InputInt("Seed", &selected_entry.seed);
        ImGui::InputInt("StatTrak", &selected_entry.stat_trak);
        ImGui::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

        ImGui::Combo("Paint Kit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) {
            *out_text = (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits)[idx].name.c_str();
            return true;
            }, nullptr, (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits).size(), 10);

        ImGui::Combo("Quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text) {
            *out_text = game_data::quality_names[idx].name;
            return true;
            }, nullptr, IM_ARRAYSIZE(game_data::quality_names), 5);

        if (itemIndex == 0) {
            ImGui::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = game_data::knife_names[idx].name;
                return true;
                }, nullptr, IM_ARRAYSIZE(game_data::knife_names), 5);
        }
        else if (itemIndex == 1) {
            ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = game_data::glove_names[idx].name;
                return true;
                }, nullptr, IM_ARRAYSIZE(game_data::glove_names), 5);
        }
        else {
            static auto unused_value = 0;
            selected_entry.definition_override_vector_index = 0;
            ImGui::Combo("Unavailable", &unused_value, "For knives and gloves only\0");
        }

        ImGui::InputText("Name Tag", selected_entry.custom_name, 32);
    }

    ImGui::NextColumn();

    {
        ImGui::PushID("sticker");

        static auto selectedStickerSlot = 0;

        ImGui::PushItemWidth(-1);

        ImGui::ListBox("", &selectedStickerSlot, [](void* data, int idx, const char** out_text) {
            static char elementName[64];
            auto kit_vector_index = config.skinChanger[itemIndex].stickers[idx].kit_vector_index;
            sprintf_s(elementName, "#%d (%s)", idx + 1, SkinChanger::stickerKits[kit_vector_index].name.c_str());
            *out_text = elementName;
            return true;
            }, nullptr, 5, 5);

        ImGui::PopItemWidth();

        auto& selected_sticker = selected_entry.stickers[selectedStickerSlot];

        ImGui::Combo("Sticker Kit", &selected_sticker.kit_vector_index, [](void* data, int idx, const char** out_text) {
            *out_text = SkinChanger::stickerKits[idx].name.c_str();
            return true;
            }, nullptr, SkinChanger::stickerKits.size(), 10);

        ImGui::SliderFloat("Wear", &selected_sticker.wear, FLT_MIN, 1.0f, "%.10f", 5.0f);
        ImGui::SliderFloat("Scale", &selected_sticker.scale, 0.1f, 5.0f);
        ImGui::SliderFloat("Rotation", &selected_sticker.rotation, 0.0f, 360.0f);

        ImGui::PopID();
    }
    selected_entry.update();

    ImGui::Columns(1);

    ImGui::Separator();

    if (ImGui::Button("Apply", { 130.0f, 30.0f }))
        SkinChanger::scheduleHudUpdate();

    ImGui::TextUnformatted("nSkinz by namazso");

    if (!contentOnly)
        ImGui::End();
}


void GUI::renderSoundWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.sound)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Sound", &window.sound, windowFlags);
    }
    ImGui::SliderInt("Chicken Volume", &config.sound.chickenVolume, 0, 200, "%d%%");

    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::Combo("", &currentCategory, "Local Player\0Allies\0Enemies\0");
    ImGui::PopItemWidth();
    ImGui::SliderInt("Master Volume", &config.sound.players[currentCategory].masterVolume, 0, 200, "%d%%");
    ImGui::SliderInt("Headshot Volume", &config.sound.players[currentCategory].headshotVolume, 0, 200, "%d%%");
    ImGui::SliderInt("Weapon Volume", &config.sound.players[currentCategory].weaponVolume, 0, 200, "%d%%");
    ImGui::SliderInt("Footstep Volume", &config.sound.players[currentCategory].footstepVolume, 0, 200, "%d%%");

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderStyleWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.style)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Style", &window.style, windowFlags);
    }

    ImGui::PushItemWidth(150.0f);
    if (ImGui::Combo("Menu Style", &config.style.menuStyle, "Classic\0One Window\0"))
        window = { };
    if (ImGui::Combo("Menu Colours", &config.style.menuColors, "Dark\0Light\0Classic\0Custom\0"))
        updateColors();
    ImGui::PopItemWidth();

    if (config.style.menuColors == 3) {
        ImGuiStyle& style = ImGui::GetStyle();
        for (int i = 0; i < ImGuiCol_COUNT; i++) {
            if (i && i & 3) ImGui::SameLine(220.0f * (i & 3));

            ImGuiCustom::colorPicker(ImGui::GetStyleColorName(i), (float*)&style.Colors[i]);
        }
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderMiscWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.misc)
            return;
        ImGui::SetNextWindowSize({ 580.0f, 0.0f });
        ImGui::Begin("Misc", &window.misc, windowFlags);
    }
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 230.0f);
    ImGui::TextUnformatted("Menu Key");
    ImGui::SameLine();
    hotkey(config.misc.menuKey);
    ImGui::Checkbox("Anti-AFK Kick", &config.misc.antiAfkKick);
    ImGui::Checkbox("Bunnyhop", &config.misc.bunnyHop);
    ImGui::Checkbox("Autostrafe", &config.misc.autoStrafe);
    ImGui::SameLine();
    hotkey(config.misc.autoStrafeKey);
    if (config.misc.autoStrafe)
    {
        ImGui::Combo("Style", &config.misc.autostrafestyle, "Legit\0Normal\0");
    }
    ImGui::Checkbox("Edge Jump", &config.misc.edgeJump);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Automatically jumps whenever near an edge");
    }
    ImGui::SameLine();
    hotkey(config.misc.edgeJumpKey);
    ImGui::Checkbox("Door Spam", &config.misc.useSpam);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Removes the cooldown between opening doors");
    }
    ImGui::Checkbox("Slow Walk", &config.misc.slowWalk);
    ImGui::SameLine();
    hotkey(config.misc.slowWalkKey);
    ImGui::Checkbox("Fast Duck", &config.misc.fastDuck);
    ImGui::TextUnformatted("Fake Duck Key");
    ImGui::SameLine();
    hotkey(config.misc.fakeDuckKey);
    ImGui::Checkbox("Slidewalk", &config.misc.moonwalk);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Freezes the player model's lower body animations");
    }
    ImGui::Checkbox("Sniper Crosshair", &config.misc.sniperCrosshair);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Draws the crosshair while holding sniper rifles");
    }
    ImGui::SameLine();
    ImGui::Checkbox("In Scope", &config.misc.sniperCrosshairInscope);
    ImGui::Checkbox("Recoil Crosshair", &config.misc.recoilCrosshair);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Crosshair follows weapon recoil patterns");
    }
    ImGui::Checkbox("Auto Pistol", &config.misc.autoPistol);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Makes pistols automatically fire");
    }
    ImGui::Checkbox("Auto Reload", &config.misc.autoReload);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Whenever weapons are empty");
    }
    ImGui::Checkbox("Auto Accept", &config.misc.autoAccept);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Automatically accept comp matches");
    }
    ImGui::Checkbox("Radar Hack", &config.misc.radarHack);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shows enemy positions on the radar");
    }
    ImGui::Checkbox("Rank Revealer", &config.misc.revealRanks);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shows player ranks on the scoreboard in comp modes");
    }
    ImGui::Checkbox("Money Revealer", &config.misc.revealMoney);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shows enemies' money on the scoreboard");
    }
    ImGui::Checkbox("Suspect Revealer", &config.misc.revealSuspect);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shows The Suspect's info in Overwatch cases");
    }
    ImGuiCustom::colorPicker("Spectator List", config.misc.spectatorList);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shows who is spectating you and other players");
    }
    ImGuiCustom::colorPicker("Watermark", config.misc.watermark);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shows the cheat name, your ping, and your FPS at the top of your screen");
    }
    ImGui::Checkbox("Fix Animation LOD", &config.misc.fixAnimationLOD);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Fixes aimbot inaccuracy for players behind you");
    }
    ImGui::Checkbox("Fix Bone Matrix", &config.misc.fixBoneMatrix);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Corrects the client's bone matrix to be closer to the server's");
    }
    ImGui::Checkbox("Fix Movement", &config.misc.fixMovement);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Fixes the movement issue while using silent aim and walking/bhopping");
    }
    ImGui::Checkbox("Disable Model Occlusion", &config.misc.disableModelOcclusion);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Draws player models behind thick objects");
    }
    ImGui::SliderFloat("Aspect Ratio", &config.misc.aspectRatio, 0.0f, 5.0f, "%.2f");
    ImGui::NextColumn();
    if (config.misc.customClanTag) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }
    if (config.misc.customClanTag) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
    if (ImGui::Checkbox("Custom Clantag", &config.misc.customClanTag))
        Misc::updateClanTag(true);
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(0);
    if (ImGui::InputText("", config.misc.clanTag, IM_ARRAYSIZE(config.misc.clanTag)))
        Misc::updateClanTag(true);
    ImGui::PopID();
    if (config.misc.customClanTag)
    {
        if (ImGui::Checkbox("Animated", &config.misc.animatedClanTag))
            Misc::updateClanTag(true);
    }
    ImGui::Checkbox("Clock Clantag", &config.misc.clocktag);
    ImGui::Checkbox("Chat Spam", &config.misc.chatSpam);
    ImGui::SameLine();
    ImGui::Checkbox("Random", &config.misc.randomChatSpam);
    ImGui::SliderInt("Delay", &config.misc.chatSpamDelay, 0, 60, "%d s");
    ImGui::Text("Text");
    ImGui::PushID(1);
    ImGui::InputTextMultiline("", config.misc.chatSpamText, IM_ARRAYSIZE(config.misc.chatSpamText), { 280.f, 120.f });
    ImGui::PopID();
    ImGui::Checkbox("Killsay", &config.misc.killMessage);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Messages chat whenever an enemy is killed");
    }
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(2);
    ImGui::InputText("", &config.misc.killMessageString);
    ImGui::PopID();
    ImGui::Checkbox("Namestealer", &config.misc.nameStealer);
    ImGui::PushID(3);
    ImGui::InputText("", config.misc.voteText, IM_ARRAYSIZE(config.misc.voteText));
    ImGui::PopID();
    ImGui::SameLine();
    if (ImGui::Button("Fake Vote"))
        Misc::fakeVote(true);
    ImGui::PushID(4);
    ImGui::SetNextItemWidth(100.0f);
    ImGui::Combo("", &config.misc.banColor, "White\0Red\0Purple\0Green\0Light Green\0Turquoise\0Light Red\0Gray\0Yellow\0Gray 2\0Light Blue\0Gray/Purple\0Blue\0Pink\0Dark Orange\0Orange\0");
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(5);
    ImGui::InputText("", config.misc.banText, IM_ARRAYSIZE(config.misc.banText));
    ImGui::PopID();
    ImGui::SameLine();
    if (ImGui::Button("Fake Ban"))
        Misc::fakeBan(true);
    ImGui::Checkbox("Fake Prime", &config.misc.fakePrime);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shows the Prime status in the menu");
    }
    ImGui::Checkbox("Fast Plant", &config.misc.fastPlant);
    ImGuiCustom::colorPicker("Bomb Timer", config.misc.bombTimer);
    ImGui::Checkbox("Quick Reload", &config.misc.quickReload);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Performs a quick switch during reloading for faster reloads");
    }
    ImGui::Checkbox("R8 Preparer", &config.misc.prepareRevolver);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Readies the R8 to shoot as soon as you click");
    }
    ImGui::SameLine();
    hotkey(config.misc.prepareRevolverKey);
    ImGui::Combo("Hitsound", &config.misc.hitSound, "None\0Metal\0Gamesense\0Bell\0Glass\0Bubble\0CoD\0Fatality\0Custom\0");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Plays a sound whenever an enemy is hit (custom hitsounds go in the csgo/sound folder)");
    }
    ImGui::SetNextItemWidth(90.0f);
    ImGui::InputInt("Fakelag", &config.misc.chokedPackets, 1, 5);
    config.misc.chokedPackets = std::clamp(config.misc.chokedPackets, 0, 64);
    ImGui::SameLine();
    hotkey(config.misc.chokedPacketsKey);
    ImGui::Text("Quick Healthshot");
    ImGui::SameLine();
    hotkey(config.misc.quickHealthshotKey);
    ImGui::Checkbox("Grenade Prediction", &config.misc.nadePredict);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shows prediction lines for grenades whenever they are readied");
    }
    ImGui::Checkbox("Fix Tablet Signal", &config.misc.fixTabletSignal);
    ImGui::SetNextItemWidth(120.0f);
    ImGui::SliderFloat("Max Angle Delta", &config.misc.maxAngleDelta, 0.0f, 255.0f, "%.2f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Maximum viewangle changes per tick (set to 30 to avoid VAC authentication errors)");
    }

    if (ImGui::Button("Unload"))
        hooks.restore();
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Unloads the cheat from the game");
    }

    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderReportbotWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.reportbot)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Reportbot", &window.reportbot, windowFlags);
    }
    ImGui::Checkbox("Enabled", &config.reportbot.enabled);
    ImGui::Combo("Target", &config.reportbot.target, "Enemies\0Allies\0All\0");
    ImGui::InputInt("Delay (s)", &config.reportbot.delay, 1, 5);
    config.reportbot.delay = (std::max)(config.reportbot.delay, 0);
    ImGui::Checkbox("Aimbot", &config.reportbot.aimbot);
    ImGui::Checkbox("Wallhack", &config.reportbot.wallhack);
    ImGui::Checkbox("Other", &config.reportbot.other);
    ImGui::Checkbox("Griefing", &config.reportbot.griefing);
    ImGui::Checkbox("Voice Abuse", &config.reportbot.voiceAbuse);
    ImGui::Checkbox("Text Abuse", &config.reportbot.textAbuse);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderConfigWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.config)
            return;
        ImGui::SetNextWindowSize({ 290.0f, 190.0f });
        ImGui::Begin("Configs", &window.config, windowFlags);
    }

    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 170.0f);

    ImGui::PushItemWidth(160.0f);

    constexpr auto& configItems = config.getConfigs();
    static int currentConfig = -1;

    if (static_cast<size_t>(currentConfig) >= configItems.size())
        currentConfig = -1;

    static std::string buffer;

    if (ImGui::ListBox("", &currentConfig, [](void* data, int idx, const char** out_text) {
        auto& vector = *static_cast<std::vector<std::string>*>(data);
        *out_text = vector[idx].c_str();
        return true;
        }, &configItems, configItems.size(), 5) && currentConfig != -1)
        buffer = configItems[currentConfig];

        ImGui::PushID(0);
        if (ImGui::InputText("", &buffer, ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (currentConfig != -1)
                config.rename(currentConfig, buffer.c_str());
        }
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::PushItemWidth(100.0f);

        if (ImGui::Button("Create Config", { 100.0f, 25.0f }))
            config.add(buffer.c_str());

        if (ImGui::Button("Reset Config", { 100.0f, 25.0f }))
            ImGui::OpenPopup("Part to Reset");

        if (ImGui::BeginPopup("Part to Reset")) {
            static constexpr const char* names[]{ "Everything", "Aimbot", "Anti-Aim", "Triggerbot", "Backtrack", "Glow", "Chams", "ESP", "Visuals", "Skinchanger", "Sound", "Style", "Misc", "Reportbot" };
            for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                if (i == 1) ImGui::Separator();

                if (ImGui::Selectable(names[i])) {
                    switch (i) {
                    case 0: config.reset(); updateColors(); Misc::updateClanTag(true); SkinChanger::scheduleHudUpdate(); break;
                    case 1: config.aimbot = { }; break;
                    case 2: config.triggerbot = { }; break;
                    case 3: config.backtrack = { }; break;
                    case 4: config.antiAim = { }; break;
                    case 5: config.glow = { }; break;
                    case 6: config.chams = { }; break;
                    case 7: config.esp = { }; break;
                    case 8: config.visuals = { }; break;
                    case 9: config.skinChanger = { }; SkinChanger::scheduleHudUpdate(); break;
                    case 10: config.sound = { }; break;
                    case 11: config.style = { }; updateColors(); break;
                    case 12: config.misc = { };  Misc::updateClanTag(true); break;
                    case 13: config.reportbot = { }; break;
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (currentConfig != -1) {
            if (ImGui::Button("Load Selected", { 100.0f, 25.0f })) {
                config.load(currentConfig);
                updateColors();
                SkinChanger::scheduleHudUpdate();
                Misc::updateClanTag(true);
            }
            if (ImGui::Button("Save Selected", { 100.0f, 25.0f }))
                config.save(currentConfig);
            if (ImGui::Button("Delete Selected", { 100.0f, 25.0f }))
                config.remove(currentConfig);
        }
        ImGui::Columns(1);
        if (!contentOnly)
            ImGui::End();
}

void GUI::renderGuiStyle2() noexcept
{
    ImGui::SetNextWindowSize({ 800.0f, 0.0f });
    ImGui::Begin("Osiris", nullptr, windowFlags | ImGuiWindowFlags_NoTitleBar);

    if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable)) {
        if (ImGui::BeginTabItem("Aimbot")) {
            renderAimbotWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Anti-Aim")) {
            renderAntiAimWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Triggerbot")) {
            renderTriggerbotWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Backtrack")) {
            renderBacktrackWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Glow")) {
            renderGlowWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Chams")) {
            renderChamsWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("ESP")) {
            renderEspWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Visuals")) {
            renderVisualsWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Skinchanger")) {
            renderSkinChangerWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Sound")) {
            renderSoundWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Style")) {
            renderStyleWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Misc")) {
            renderMiscWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Reportbot")) {
            renderReportbotWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Configs")) {
            renderConfigWindow(true);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}
