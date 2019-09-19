#include <functional>
#include <string>
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"

#include "GUI.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/SkinChanger.h"
#include "Hacks/Visuals.h"
#include "Hooks.h"

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

GUI::GUI() noexcept
{
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(FindWindowW(L"Valve001", NULL));

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.GrabMinSize = 7.0f;
    style.GrabRounding = 5.0f;
    style.FrameRounding = 5.0f;
    style.PopupRounding = 5.0f;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    static ImWchar ranges[] = { 0x0020, 0x00FF, 0x0100, 0x017f, 0 };
    char buffer[MAX_PATH];
    if (GetWindowsDirectoryA(buffer, MAX_PATH))
        io.Fonts->AddFontFromFileTTF(strcat(buffer, "/Fonts/Tahoma.ttf"), 16.0f, nullptr, ranges);
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

void GUI::checkboxedColorPicker(const std::string& name, bool* enable, float* color) noexcept
{
    ImGui::Checkbox(("##" + name).c_str(), enable);
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::PushID(0);
    bool openPopup = ImGui::ColorButton(("##" + name).c_str(), ImColor{ color[0], color[1], color[2] }, ImGuiColorEditFlags_NoTooltip);
    ImGui::PopID();
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::TextUnformatted(name.c_str());
    ImGui::PushID(1);
    if (openPopup)
        ImGui::OpenPopup(("##" + name).c_str());
    if (ImGui::BeginPopup(("##" + name).c_str())) {
        ImGui::PushID(2);
        ImGui::ColorPicker3(("##" + name).c_str(), color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);
        ImGui::PopID();
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void GUI::hotkey(int& key) noexcept
{
    key ? ImGui::Text("[ 0x%x ]", key) : ImGui::TextUnformatted("[ key ]");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Press any key to change keybind");
        ImGuiIO& io = ImGui::GetIO();
        for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
            if (ImGui::IsKeyPressed(i) && i != config.misc.menuKey)
                key = i != VK_ESCAPE ? i : 0;

        for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
            if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != config.misc.menuKey)
                key = i + (i > 1 ? 2 : 1);
    }
}

void GUI::renderMenuBar() noexcept
{
    if (ImGui::BeginMainMenuBar()) {
        ImGui::MenuItem("Aimbot", nullptr, &window.aimbot);
        ImGui::MenuItem("Anti aim", nullptr, &window.antiAim);
        ImGui::MenuItem("Triggerbot", nullptr, &window.triggerbot);
        ImGui::MenuItem("Backtrack", nullptr, &window.backtrack);
        ImGui::MenuItem("Glow", nullptr, &window.glow);
        ImGui::MenuItem("Chams", nullptr, &window.chams);
        ImGui::MenuItem("Esp", nullptr, &window.esp);
        ImGui::MenuItem("Visuals", nullptr, &window.visuals);
        ImGui::MenuItem("Skin changer", nullptr, &window.skinChanger);
        ImGui::MenuItem("Sound", nullptr, &window.sound);
        ImGui::MenuItem("Style", nullptr, &window.style);
        ImGui::MenuItem("Misc", nullptr, &window.misc);
        ImGui::MenuItem("Reportbot", nullptr, &window.reportbot);
        ImGui::MenuItem("Config", nullptr, &window.config);
        ImGui::EndMainMenuBar();
    }
}

void GUI::renderAimbotWindow() noexcept
{
    if (window.aimbot) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 600.0f, 0.0f });
            ImGui::Begin("Aimbot", &window.aimbot, windowFlags);
        }
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

            ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
                if (config.aimbot[idx ? idx : 35].enabled) {
                    static std::string name;
                    name = pistols[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = pistols[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(pistols));

            currentWeapon = currentPistol ? currentPistol : 35;
            break;
        }
        case 2: {
            static int currentHeavy{ 0 };
            static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

            ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
                if (config.aimbot[idx ? idx + 10 : 36].enabled) {
                    static std::string name;
                    name = heavies[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = heavies[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(heavies));

            currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
            break;
        }
        case 3: {
            static int currentSmg{ 0 };
            static constexpr const char* smgs[]{ "All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

            ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
                if (config.aimbot[idx ? idx + 16 : 37].enabled) {
                    static std::string name;
                    name = smgs[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = smgs[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(smgs));

            currentWeapon = currentSmg ? currentSmg + 16 : 37;
            break;
        }
        case 4: {
            static int currentRifle{ 0 };
            static constexpr const char* rifles[]{ "All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

            ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
                if (config.aimbot[idx ? idx + 23 : 38].enabled) {
                    static std::string name;
                    name = rifles[idx];
                    *out_text = name.append(" *").c_str();
                } else {
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
        ImGui::Checkbox("On key", &config.aimbot[currentWeapon].onKey);
        ImGui::SameLine();
        hotkey(config.aimbot[currentWeapon].key);
        ImGui::SameLine();
        ImGui::PushID(2);
        ImGui::PushItemWidth(70.0f);
        ImGui::Combo("", &config.aimbot[currentWeapon].keyMode, "Hold\0Toggle\0");
        ImGui::PopItemWidth();
        ImGui::PopID();
        ImGui::Checkbox("Aimlock", &config.aimbot[currentWeapon].aimlock);
        ImGui::Checkbox("Silent", &config.aimbot[currentWeapon].silent);
        ImGui::Checkbox("Friendly fire", &config.aimbot[currentWeapon].friendlyFire);
        ImGui::Checkbox("Visible only", &config.aimbot[currentWeapon].visibleOnly);
        ImGui::Checkbox("Scoped only", &config.aimbot[currentWeapon].scopedOnly);
        ImGui::Checkbox("Ignore flash", &config.aimbot[currentWeapon].ignoreFlash);
        ImGui::Checkbox("Ignore smoke", &config.aimbot[currentWeapon].ignoreSmoke);
        ImGui::Checkbox("Auto shot", &config.aimbot[currentWeapon].autoShot);
        ImGui::Checkbox("Auto scope", &config.aimbot[currentWeapon].autoScope);
        ImGui::Checkbox("Recoil-based fov", &config.aimbot[currentWeapon].recoilbasedFov);
        ImGui::Combo("Bone", &config.aimbot[currentWeapon].bone, "Nearest\0Best damage\0Head\0Neck\0Sternum\0Chest\0Stomach\0Pelvis\0");
        ImGui::NextColumn();
        ImGui::PushItemWidth(240.0f);
        ImGui::SliderFloat("Fov", &config.aimbot[currentWeapon].fov, 0.0f, 255.0f, "%.2f");
        ImGui::SliderFloat("Smooth", &config.aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
        ImGui::SliderFloat("Recoil control x", &config.aimbot[currentWeapon].recoilControlX, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Recoil control y", &config.aimbot[currentWeapon].recoilControlY, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Max aim inaccuracy", &config.aimbot[currentWeapon].maxAimInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
        ImGui::SliderFloat("Max shot inaccuracy", &config.aimbot[currentWeapon].maxShotInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
        ImGui::InputInt("Min damage", &config.aimbot[currentWeapon].minDamage);
        config.aimbot[currentWeapon].minDamage = std::clamp(config.aimbot[currentWeapon].minDamage, 0, 250);
        ImGui::Checkbox("Killshot", &config.aimbot[currentWeapon].killshot);
        ImGui::Checkbox("Between shots", &config.aimbot[currentWeapon].betweenShots);
        ImGui::Columns(1);
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderAntiAimWindow() noexcept
{
    if (window.antiAim) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 0.0f, 0.0f });
            ImGui::Begin("Anti aim", &window.antiAim, windowFlags);
        }
        ImGui::Checkbox("Enabled", &config.antiAim.enabled);
        ImGui::Checkbox("##pitch", &config.antiAim.pitch);
        ImGui::SameLine();
        ImGui::SliderFloat("Pitch", &config.antiAim.pitchAngle, -89.0f, 89.0f, "%.2f");
        ImGui::Checkbox("Yaw", &config.antiAim.yaw);
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderTriggerbotWindow() noexcept
{
    if (window.triggerbot) {
        if (!config.style.menuStyle) {
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

            ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
                if (config.triggerbot[idx ? idx : 35].enabled) {
                    static std::string name;
                    name = pistols[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = pistols[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(pistols));

            currentWeapon = currentPistol ? currentPistol : 35;
            break;
        }
        case 2: {
            static int currentHeavy{ 0 };
            static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

            ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
                if (config.triggerbot[idx ? idx + 10 : 36].enabled) {
                    static std::string name;
                    name = heavies[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = heavies[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(heavies));

            currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
            break;
        }
        case 3: {
            static int currentSmg{ 0 };
            static constexpr const char* smgs[]{ "All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

            ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
                if (config.triggerbot[idx ? idx + 16 : 37].enabled) {
                    static std::string name;
                    name = smgs[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = smgs[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(smgs));

            currentWeapon = currentSmg ? currentSmg + 16 : 37;
            break;
        }
        case 4: {
            static int currentRifle{ 0 };
            static constexpr const char* rifles[]{ "All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

            ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
                if (config.triggerbot[idx ? idx + 23 : 38].enabled) {
                    static std::string name;
                    name = rifles[idx];
                    *out_text = name.append(" *").c_str();
                } else {
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
        ImGui::Checkbox("On key", &config.triggerbot[currentWeapon].onKey);
        ImGui::SameLine();
        hotkey(config.triggerbot[currentWeapon].key);
        ImGui::Checkbox("Friendly fire", &config.triggerbot[currentWeapon].friendlyFire);
        ImGui::Checkbox("Scoped only", &config.triggerbot[currentWeapon].scopedOnly);
        ImGui::Checkbox("Ignore flash", &config.triggerbot[currentWeapon].ignoreFlash);
        ImGui::Checkbox("Ignore smoke", &config.triggerbot[currentWeapon].ignoreSmoke);
        ImGui::SetNextItemWidth(85.0f);
        ImGui::Combo("Hitgroup", &config.triggerbot[currentWeapon].hitgroup, "All\0Head\0Chest\0Stomach\0Left arm\0Right arm\0Left leg\0Right leg\0");
        ImGui::PushItemWidth(220.0f);
        ImGui::SliderInt("", &config.triggerbot[currentWeapon].shotDelay, 0, 250, "Shot delay: %d ms");
        ImGui::InputInt("Min damage", &config.triggerbot[currentWeapon].minDamage);
        config.triggerbot[currentWeapon].minDamage = std::clamp(config.triggerbot[currentWeapon].minDamage, 0, 250);
        ImGui::Checkbox("Killshot", &config.triggerbot[currentWeapon].killshot);
        
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderBacktrackWindow() noexcept
{
    if (window.backtrack) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 0.0f, 0.0f });
            ImGui::Begin("Backtrack", &window.backtrack, windowFlags);
        }
        ImGui::Checkbox("Enabled", &config.backtrack.enabled);
        ImGui::Checkbox("Ignore smoke", &config.backtrack.ignoreSmoke);
        ImGui::Checkbox("Recoil based fov", &config.backtrack.recoilBasedFov);
        ImGui::PushItemWidth(220.0f);
        ImGui::SliderInt("", &config.backtrack.timeLimit, 1, 200, "Time limit: %d ms");
        ImGui::PopItemWidth();
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderGlowWindow() noexcept
{
    if (window.glow) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 450.0f, 0.0f });
            ImGui::Begin("Glow", &window.glow, windowFlags);
        }
        static int currentCategory{ 0 };
        ImGui::PushItemWidth(110.0f);
        ImGui::PushID(0);
        ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0C4\0Planted C4\0Chickens\0Defuse kits\0Projectiles\0Hostages\0Ragdolls\0");
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
        ImGui::Checkbox("Health based", &config.glow[currentItem].healthBased);
        ImGui::Checkbox("Rainbow", &config.glow[currentItem].rainbow);
        bool openPopup = ImGui::ColorButton("Color", ImVec4{ config.glow[currentItem].color }, ImGuiColorEditFlags_NoTooltip);
        ImGui::SameLine(0.0f, 5.0f);
        ImGui::TextUnformatted("Color");
        ImGui::PushID(2);
        if (openPopup)
            ImGui::OpenPopup("");
        if (ImGui::BeginPopup("")) {
            ImGui::PushID(3);
            ImGui::ColorPicker3("", config.glow[currentItem].color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);
            ImGui::PopID();
            ImGui::EndPopup();
        }
        ImGui::PopID();
        ImGui::NextColumn();
        ImGui::PushItemWidth(220.0f);
        ImGui::SliderFloat("Thickness", &config.glow[currentItem].thickness, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Alpha", &config.glow[currentItem].alpha, 0.0f, 1.0f, "%.2f");
        ImGui::SliderInt("Style", &config.glow[currentItem].style, 0, 3);
        ImGui::Columns(1);
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderChamsWindow() noexcept
{
    if (window.chams) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 0.0f, 0.0f });
            ImGui::Begin("Chams", &window.chams, windowFlags);
        }
        static int currentCategory{ 0 };
        ImGui::PushItemWidth(110.0f);
        ImGui::PushID(0);
        ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0Hands\0Backtrack\0");
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
        ImGui::Checkbox("Enabled", &config.chams[currentItem].enabled);
        ImGui::Separator();
        ImGui::Checkbox("Health based", &config.chams[currentItem].healthBased);
        ImGui::Checkbox("Rainbow", &config.chams[currentItem].rainbow);
        ImGui::Checkbox("Blinking", &config.chams[currentItem].blinking);
        ImGui::Combo("Material", &config.chams[currentItem].material, "Normal\0Flat\0Animated\0Platinum\0Glass\0Chrome\0Crystal\0Silver\0Gold\0Plastic\0");
        ImGui::Checkbox("Wireframe", &config.chams[currentItem].wireframe);

        bool openPopup = ImGui::ColorButton("Color", ImVec4{ config.chams[currentItem].color }, ImGuiColorEditFlags_NoTooltip);
        ImGui::SameLine(0.0f, 5.0f);
        ImGui::TextUnformatted("Color");
        ImGui::PushID(2);
        if (openPopup)
            ImGui::OpenPopup("");
        if (ImGui::BeginPopup("")) {
            ImGui::PushID(3);
            ImGui::ColorPicker3("", config.chams[currentItem].color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);
            ImGui::PopID();
            ImGui::EndPopup();
        }
        ImGui::PopID();
        ImGui::PushItemWidth(220.0f);
        ImGui::PushID(4);
        ImGui::SliderFloat("", &config.chams[currentItem].alpha, 0.0f, 1.0f, "Alpha: %.2f");
        ImGui::PopID();
        if (!config.style.menuStyle) {
            ImGui::End();
        }
    }
}

void GUI::renderEspWindow() noexcept
{
    if (window.esp) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 0.0f, 0.0f });
            ImGui::Begin("Esp", &window.esp, windowFlags);
        }
        static int currentCategory{ 0 };
        ImGui::PushItemWidth(110.0f);
        ImGui::PushID(0);
        ImGui::Combo("", &currentCategory, "Allies\0Enemies\0");
        ImGui::PopID();
        ImGui::SameLine();
        static int currentType{ 0 };
        ImGui::PushID(1);
        ImGui::Combo("", &currentType, "All\0Visible\0Occluded\0");
        ImGui::PopID();
        int currentItem = currentCategory * 3 + currentType;
        ImGui::SameLine();
        ImGui::Checkbox("Enabled", &config.esp[currentItem].enabled);
        ImGui::SameLine(0.0f, 50.0f);
        ImGui::InputInt("Font", &config.esp[currentItem].font, 1, 294);
        config.esp[currentItem].font = std::clamp(config.esp[currentItem].font, 1, 294);
        
        ImGui::Separator();

        constexpr auto spacing{ 200.0f };
        checkboxedColorPicker("Snaplines", &config.esp[currentItem].snaplines, config.esp[currentItem].snaplinesColor);
        ImGui::SameLine(spacing);
        checkboxedColorPicker("Box", &config.esp[currentItem].box, config.esp[currentItem].boxColor);
        checkboxedColorPicker("Eye traces", &config.esp[currentItem].eyeTraces, config.esp[currentItem].eyeTracesColor);
        ImGui::SameLine(spacing);
        checkboxedColorPicker("Health", &config.esp[currentItem].health, config.esp[currentItem].healthColor);
        checkboxedColorPicker("Head dot", &config.esp[currentItem].headDot, config.esp[currentItem].headDotColor);
        ImGui::SameLine(spacing);
        checkboxedColorPicker("Health bar", &config.esp[currentItem].healthBar, config.esp[currentItem].healthBarColor);
        checkboxedColorPicker("Name", &config.esp[currentItem].name, config.esp[currentItem].nameColor);
        ImGui::SameLine(spacing);
        checkboxedColorPicker("Armor", &config.esp[currentItem].armor, config.esp[currentItem].armorColor);
        checkboxedColorPicker("Money", &config.esp[currentItem].money, config.esp[currentItem].moneyColor);
        ImGui::SameLine(spacing);
        checkboxedColorPicker("Armor bar", &config.esp[currentItem].armorBar, config.esp[currentItem].armorBarColor);

        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderVisualsWindow() noexcept
{
    if (window.visuals) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 520.0f, 0.0f });
            ImGui::Begin("Visuals", &window.visuals, windowFlags);
        }
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 210.0f);
        ImGui::Checkbox("Disable post-processing", &config.visuals.disablePostProcessing);
        ImGui::Checkbox("Inverse ragdoll gravity", &config.visuals.inverseRagdollGravity);
        ImGui::Checkbox("No fog", &config.visuals.noFog);
        ImGui::Checkbox("No 3d sky", &config.visuals.no3dSky);
        ImGui::Checkbox("No aim punch", &config.visuals.noAimPunch);
        ImGui::Checkbox("No view punch", &config.visuals.noViewPunch);
        ImGui::Checkbox("No hands", &config.visuals.noHands);
        ImGui::Checkbox("No sleeves", &config.visuals.noSleeves);
        ImGui::Checkbox("No weapons", &config.visuals.noWeapons);
        ImGui::Checkbox("No smoke", &config.visuals.noSmoke);
        ImGui::Checkbox("No blur", &config.visuals.noBlur);
        ImGui::Checkbox("No scope overlay", &config.visuals.noScopeOverlay);
        ImGui::Checkbox("No grass", &config.visuals.noGrass);
        ImGui::Checkbox("No shadows", &config.visuals.noShadows);
        ImGui::Checkbox("Wireframe smoke", &config.visuals.wireframeSmoke);
        ImGui::NextColumn();
        ImGui::Checkbox("Zoom", &config.visuals.zoom);
        ImGui::SameLine();
        hotkey(config.visuals.zoomKey);
        ImGui::Checkbox("Thirdperson", &config.visuals.thirdperson);
        ImGui::SameLine();
        hotkey(config.visuals.thirdpersonKey);
        ImGui::PushItemWidth(290.0f);
        ImGui::PushID(0);
        ImGui::SliderInt("", &config.visuals.thirdpersonDistance, 0, 1000, "Thirdperson distance: %d");
        ImGui::PopID();
        ImGui::PushID(1);
        ImGui::SliderInt("", &config.visuals.viewmodelFov, -60, 60, "Viewmodel FOV: %d");
        ImGui::PopID();
        ImGui::PushID(2);
        ImGui::SliderInt("", &config.visuals.fov, -60, 60, "FOV: %d");
        ImGui::PopID();
        ImGui::PushID(3);
        ImGui::SliderInt("", &config.visuals.farZ, 0, 2000, "Far Z: %d");
        ImGui::PopID();
        ImGui::PushID(4);
        ImGui::SliderInt("", &config.visuals.flashReduction, 0, 100, "Flash reduction: %d%%");
        ImGui::PopID();
        ImGui::PushID(5);
        ImGui::SliderFloat("", &config.visuals.brightness, 0.0f, 1.0f, "Brightness: %.2f");
        ImGui::PopID();
        ImGui::PopItemWidth();
        ImGui::Combo("Skybox", &config.visuals.skybox, "Default\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0");
        ImGui::ColorEdit3("World color", config.visuals.worldColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
        ImGui::Checkbox("Deagle spinner", &config.visuals.deagleSpinner);
        ImGui::Combo("Screen effect", &config.visuals.screenEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
        ImGui::Combo("Hit marker", &config.visuals.hitMarker, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
        ImGui::SliderFloat("Hit marker time", &config.visuals.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
        ImGui::Columns(1);

        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderSkinChangerWindow() noexcept
{
    if (window.skinChanger) {
        if (!config.style.menuStyle) {
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
            } else if (itemIndex == 1) {
                ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                    *out_text = game_data::glove_names[idx].name;
                    return true;
                }, nullptr, IM_ARRAYSIZE(game_data::glove_names), 5);
            } else {
                static auto unused_value = 0;
                selected_entry.definition_override_vector_index = 0;
                ImGui::Combo("Unavailable", &unused_value, "For knives or gloves\0");
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

        if (ImGui::Button("Update", { 130.0f, 30.0f }))
            SkinChanger::scheduleHudUpdate();

        ImGui::TextUnformatted("nSkinz by namazso");

        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderSoundWindow() noexcept
{
    if (window.sound) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 0.0f, 0.0f });
            ImGui::Begin("Sound", &window.sound, windowFlags);
        }
        ImGui::SliderInt("Chicken volume", &config.sound.chickenVolume, 0, 200, "%d%%");

        static int currentCategory{ 0 };
        ImGui::PushItemWidth(110.0f);
        ImGui::Combo("", &currentCategory, "Local player\0Allies\0Enemies\0");
        ImGui::PopItemWidth();
        ImGui::SliderInt("Master volume", &config.sound.players[currentCategory].masterVolume, 0, 200, "%d%%");
        ImGui::SliderInt("Headshot volume", &config.sound.players[currentCategory].headshotVolume, 0, 200, "%d%%");
        ImGui::SliderInt("Weapon volume", &config.sound.players[currentCategory].weaponVolume, 0, 200, "%d%%");
        ImGui::SliderInt("Footstep volume", &config.sound.players[currentCategory].footstepVolume, 0, 200, "%d%%");

        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderStyleWindow() noexcept
{
    if (window.style) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 0.0f, 0.0f });
            ImGui::Begin("Style", &window.style, windowFlags);
        }

        ImGui::PushItemWidth(150.0f);
        if (ImGui::Combo("Menu style", &config.style.menuStyle, "Classic\0One window\0"))
            window = { };
        if (ImGui::Combo("Menu colors", &config.style.menuColors, "Dark\0Light\0Classic\0Custom\0"))
            updateColors();
        ImGui::PopItemWidth();

        if (config.style.menuColors == 3) {
            ImGuiStyle& style = ImGui::GetStyle();
            for (int i = 0; i < ImGuiCol_COUNT; i++) {
                if (i && i % 4) ImGui::SameLine(220.0f * (i % 4));

                const char* name = ImGui::GetStyleColorName(i);
                ImGui::PushID(i);
                bool openPopup = ImGui::ColorButton("##colorbutton", style.Colors[i], ImGuiColorEditFlags_NoTooltip);
                ImGui::SameLine(0.0f, 5.0f);
                ImGui::TextUnformatted(name);
                if (openPopup)
                    ImGui::OpenPopup(name);
                if (ImGui::BeginPopup(name)) {
                    ImGui::ColorPicker3("##colorpicker", (float*)& style.Colors[i], ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);
                    ImGui::EndPopup();
                }
                ImGui::PopID();
            }
        }

        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderMiscWindow() noexcept
{
    if (window.misc) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 580.0f, 0.0f });
            ImGui::Begin("Misc", &window.misc, windowFlags);
        }
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 230.0f);
        ImGui::TextUnformatted("Menu key");
        ImGui::SameLine();
        hotkey(config.misc.menuKey);

        ImGui::Checkbox("Anti AFK kick", &config.misc.antiAfkKick);
        ImGui::Checkbox("Auto strafe", &config.misc.autoStrafe);
        ImGui::Checkbox("Bunny hop", &config.misc.bunnyHop);
        ImGui::Checkbox("Fast duck", &config.misc.fastDuck);
        ImGui::Checkbox("Moonwalk", &config.misc.moonwalk);
        ImGui::Checkbox("Sniper crosshair", &config.misc.sniperCrosshair);
        ImGui::Checkbox("Recoil crosshair", &config.misc.recoilCrosshair);
        ImGui::Checkbox("Auto pistol", &config.misc.autoPistol);
        ImGui::Checkbox("Auto reload", &config.misc.autoReload);
        ImGui::Checkbox("Auto accept", &config.misc.autoAccept);
        ImGui::Checkbox("Radar hack", &config.misc.radarHack);
        ImGui::Checkbox("Reveal ranks", &config.misc.revealRanks);
        ImGui::Checkbox("Spectator list", &config.misc.spectatorList);
        ImGui::Checkbox("Watermark", &config.misc.watermark);
        ImGui::Checkbox("Fix animation LOD", &config.misc.fixAnimationLOD);
        ImGui::Checkbox("Fix bone matrix", &config.misc.fixBoneMatrix);
        ImGui::Checkbox("Fix movement", &config.misc.fixMovement);
        ImGui::Checkbox("Disable model occlusion", &config.misc.disableModelOcclusion);
        ImGui::NextColumn();
        ImGui::Checkbox("Animated clan tag", &config.misc.animatedClanTag);
        ImGui::Checkbox("Custom clantag", &config.misc.customClanTag);
        ImGui::SameLine();
        ImGui::PushItemWidth(120.0f);
        ImGui::PushID(0);
        if (ImGui::InputText("", config.misc.clanTag, IM_ARRAYSIZE(config.misc.clanTag)))
            Misc::updateClanTag(true);
        ImGui::PopID();
        ImGui::Checkbox("Kill message", &config.misc.killMessage);
        ImGui::SameLine();
        ImGui::PushItemWidth(120.0f);
        ImGui::PushID(1);
        ImGui::InputText("", config.misc.killMessageString, IM_ARRAYSIZE(config.misc.killMessageString));
        ImGui::PopID();
        ImGui::Checkbox("Name stealer", &config.misc.nameStealer);
        ImGui::PushID(2);
        ImGui::InputText("", config.misc.voteText, IM_ARRAYSIZE(config.misc.voteText));
        ImGui::PopID();
        ImGui::SameLine();
        if (ImGui::Button("Setup fake vote"))
            Misc::fakeVote(true);

        ImGui::PushID(3);
        ImGui::SetNextItemWidth(100.0f);
        ImGui::Combo("", &config.misc.banColor, "White\0Red\0Purple\0Green\0Light green\0Turquoise\0Light red\0Gray\0Yellow\0Gray 2\0Light blue\0Gray/Purple\0Blue\0Pink\0Dark orange\0Orange\0");
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::PushID(4);
        ImGui::InputText("", config.misc.banText, IM_ARRAYSIZE(config.misc.banText));
        ImGui::PopID();
        ImGui::SameLine();
        if (ImGui::Button("Setup fake ban"))
            Misc::fakeBan(true);
        ImGui::Checkbox("Fast plant", &config.misc.fastPlant);
        ImGui::Checkbox("Bomb timer", &config.misc.bombTimer);
        ImGui::Checkbox("Quick reload", &config.misc.quickReload);
        ImGui::Checkbox("Prepare revolver", &config.misc.prepareRevolver);
        ImGui::SameLine();
        hotkey(config.misc.prepareRevolverKey);
        ImGui::Combo("Hit Sound", &config.misc.hitSound, "None\0Metal\0Gamesense\0Bell\0Glass\0");
        ImGui::PushItemWidth(90.0f);
        ImGui::InputInt("Choked packets", &config.misc.chokedPackets, 1, 5);
        config.misc.chokedPackets = std::clamp(config.misc.chokedPackets, 0, 64);
        ImGui::SameLine();
        hotkey(config.misc.chokedPacketsKey);
        ImGui::PushItemWidth(120.0f);
        ImGui::SliderFloat("Max angle delta", &config.misc.maxAngleDelta, 0.0f, 255.0f, "%.2f");
        ImGui::PushItemWidth(290.0f);

        if (ImGui::Button("Unhook"))
            hooks.restore();

        ImGui::Columns(1);
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderReportbotWindow() noexcept
{
    if (window.reportbot) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 0.0f, 0.0f });
            ImGui::Begin("Reportbot", &window.reportbot, windowFlags);
        }
        ImGui::Checkbox("Enabled", &config.reportbot.enabled);
        ImGui::Combo("Target", &config.reportbot.target, "Enemies\0Allies\0All");
        ImGui::InputInt("Delay (s)", &config.reportbot.delay, 1, 5);
        config.reportbot.delay = (std::max)(config.reportbot.delay, 0);
        ImGui::Checkbox("Aimbot", &config.reportbot.aimbot);
        ImGui::Checkbox("Wallhack", &config.reportbot.wallhack);
        ImGui::Checkbox("Other", &config.reportbot.other);
        ImGui::Checkbox("Griefing", &config.reportbot.griefing);
        ImGui::Checkbox("Voice abuse", &config.reportbot.voiceAbuse);
        ImGui::Checkbox("Text abuse", &config.reportbot.textAbuse);
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderConfigWindow() noexcept
{
    if (window.config) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 290.0f, 190.0f });
            ImGui::Begin("Config", &window.config, windowFlags);
        }

        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 170.0f);

        ImGui::PushItemWidth(160.0f);

        constexpr auto& configItems = config.getConfigs();
        static int currentConfig = -1;

        if (static_cast<size_t>(currentConfig) >= configItems.size())
            currentConfig = -1;

        static char buffer[16];

        if (ImGui::ListBox("", &currentConfig, [](void* data, int idx, const char** out_text) {
            auto& vector = *static_cast<std::vector<std::string>*>(data);
            *out_text = vector[idx].c_str();
            return true;
        }, &configItems, configItems.size(), 5) && currentConfig != -1)
            strcpy(buffer, configItems[currentConfig].c_str());

        ImGui::PushID(0);
        if (ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (currentConfig != -1)
                config.rename(currentConfig, buffer);
        }
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::PushItemWidth(100.0f);

        if (ImGui::Button("Create config", { 100.0f, 25.0f }))
            config.add(buffer);

        if (ImGui::Button("Reset config", { 100.0f, 25.0f }))
            ImGui::OpenPopup("Config to reset");

        if (ImGui::BeginPopup("Config to reset")) {
            static constexpr const char* names[]{ "Whole", "Aimbot", "Triggerbot", "Backtrack", "Anti aim", "Glow", "Chams", "Esp", "Visuals", "Skin changer", "Sound", "Style", "Misc", "Reportbot" };
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
            if (ImGui::Button("Load selected", { 100.0f, 25.0f })) {
                config.load(currentConfig);
                updateColors();
                SkinChanger::scheduleHudUpdate();
                Misc::updateClanTag(true);
            }
            if (ImGui::Button("Save selected", { 100.0f, 25.0f }))
                config.save(currentConfig);
            if (ImGui::Button("Delete selected", { 100.0f, 25.0f }))
                config.remove(currentConfig);
        }
        ImGui::Columns(1);
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderGuiStyle2() noexcept
{
    ImGui::SetNextWindowSize({ 800.0f, 0.0f });
    ImGui::Begin("Osiris", nullptr, windowFlags | ImGuiWindowFlags_NoTitleBar);

    if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable)) {
        if (ImGui::BeginTabItem("Aimbot")) {
            window = { };
            window.aimbot = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Anti aim")) {
            window = { };
            window.antiAim = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Triggerbot")) {
            window = { };
            window.triggerbot = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Backtrack")) {
            window = { };
            window.backtrack = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Glow")) {
            window = { };
            window.glow = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Chams")) {
            window = { };
            window.chams = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Esp")) {
            window = { };
            window.esp = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Visuals")) {
            window = { };
            window.visuals = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Skin changer")) {
            window = { };
            window.skinChanger = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Sound")) {
            window = { };
            window.sound = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Style")) {
            window = { };
            window.style = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Misc")) {
            window = { };
            window.misc = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Reportbot")) {
            window = { };
            window.reportbot = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Config")) {
            window = { };
            window.config = true;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

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

    ImGui::End();
}
