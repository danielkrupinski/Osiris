#include <fstream>
#include <functional>
#include <string>
#include <ShlObj.h>
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_stdlib.h"

#include "imguiCustom.h"

#include "GUI.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/Reportbot.h"
#include "Hacks/SkinChanger.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "SDK/InputSystem.h"

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

GUI::GUI() noexcept
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    style.ScrollbarSize = 9.0f;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    if (PWSTR pathToFonts; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &pathToFonts))) {
        const std::filesystem::path path{ pathToFonts };
        CoTaskMemFree(pathToFonts);

        static constexpr ImWchar ranges[]{ 0x0020, 0xFFFF, 0 };
        ImFontConfig cfg;
        cfg.OversampleV = 3;

        fonts.tahoma = io.Fonts->AddFontFromFileTTF((path / "tahoma.ttf").string().c_str(), 15.0f, &cfg, ranges);
        fonts.segoeui = io.Fonts->AddFontFromFileTTF((path / "segoeui.ttf").string().c_str(), 15.0f, &cfg, ranges);
    }
}



void GUI::render() noexcept
{
    if (!config->style.menuStyle) {
        renderGuiStyle2();
    }
    else {
        renderGuiStyle2();
    }
}

void GUI::updateColors() const noexcept
{
    switch (config->style.menuColors) {
    case 0: ImGui::StyleColorsDark(); break;
    case 1: ImGui::StyleColorsLight(); break;
    case 2: ImGui::StyleColorsClassic(); break;
    }
}

void GUI::hotkey(int& key) noexcept
{
    key ? ImGui::Text("[ %s ]", interfaces->inputSystem->virtualKeyToString(key)) : ImGui::TextUnformatted("[ key ]");

    if (!ImGui::IsItemHovered())
        return;

    ImGui::SetTooltip("Press any key to change keybind");
    ImGuiIO& io = ImGui::GetIO();
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
        if (ImGui::IsKeyPressed(i) && i != config->misc.menuKey)
            key = i != VK_ESCAPE ? i : 0;

    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
        if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != config->misc.menuKey)
            key = i + (i > 1 ? 2 : 1);
}

void GUI::renderAimbotWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.aimbot)
            return;
        ImGui::SetNextWindowSize({ 600.0f, 0.0f });
        ImGui::Begin("ล็อคเป้า", &window.aimbot, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "ทั้งหมด\0Pistols\0Heavy\0SMG\0Rifles\0");
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
        static constexpr const char* pistols[]{ "ทั้งหมด", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

        ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx : 35].enabled) {
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
        static constexpr const char* heavies[]{ "ทั้งหมด", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

        ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx + 10 : 36].enabled) {
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
        static constexpr const char* smgs[]{ "ทั้งหมด", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

        ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx + 16 : 37].enabled) {
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
        static constexpr const char* rifles[]{ "ทั้งหมด", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

        ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx + 23 : 38].enabled) {
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
    ImGui::Checkbox("เปิด", &config->aimbot[currentWeapon].enabled);
    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(125, 300.0f);
    ImGui::Checkbox("เลือกปุ่ม", &config->aimbot[currentWeapon].onKey);
    ImGui::SameLine();
    hotkey(config->aimbot[currentWeapon].key);
    ImGui::SameLine();
    ImGui::PushID(2);
    ImGui::PushItemWidth(70.0f);
    ImGui::Combo("", &config->aimbot[currentWeapon].keyMode, "กดค้าง\0เปิด/ปิด\0");
    ImGui::PopItemWidth();
    ImGui::PopID();
    ImGui::Checkbox("ล็อคเป้า ", &config->aimbot[currentWeapon].aimlock);
    ImGui::Checkbox("ไม่สบัดหน้า", &config->aimbot[currentWeapon].silent);
    ImGui::Checkbox("ล็อคเป้าเพื่อน", &config->aimbot[currentWeapon].friendlyFire);
    ImGui::Checkbox("ล็อคเป้าเฉพาะที่มองเห็น", &config->aimbot[currentWeapon].visibleOnly);
    ImGui::Checkbox("ล็อคเป้าเฉพาะสโคป", &config->aimbot[currentWeapon].scopedOnly);
    ImGui::Checkbox("ไม่ล็อคเป้าเมื่อโดนแฟลช", &config->aimbot[currentWeapon].ignoreFlash);
    ImGui::Checkbox("ไม่ล็อคเป้าเมื่อโดนควัน", &config->aimbot[currentWeapon].ignoreSmoke);
    ImGui::Checkbox("ยิงอัตโนมัติ ", &config->aimbot[currentWeapon].autoShot);
    ImGui::Checkbox("ยิงอัตโนมัติเมื่อใช้สโคป", &config->aimbot[currentWeapon].autoScope);
    ImGui::Combo("ตำแหน่งที่ใช้ล็อค", &config->aimbot[currentWeapon].bone, "ใกล้ที่สุด\0ดาเมจเยะสุด\0หัว\0คอ\0ลำตัว\0หน้าอก\0ท้อง\0กระดูกเชิงกราน\0");
    ImGui::NextColumn();
    ImGui::SliderFloat("ขอบเขตการมองเห็น", &config->aimbot[currentWeapon].fov, 0.0f, 255.0f, "%.2f", 2.5f);
    ImGui::SliderFloat("ความนุ่มนวล", &config->aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
    ImGui::SliderFloat("ความแม่นยำของการล็อค", &config->aimbot[currentWeapon].maxAimInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
    ImGui::SliderFloat("ความแม่นยำของการยิง", &config->aimbot[currentWeapon].maxShotInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
    ImGui::InputInt("ดาเมจต่ำสุด", &config->aimbot[currentWeapon].minDamage);
    config->aimbot[currentWeapon].minDamage = std::clamp(config->aimbot[currentWeapon].minDamage, 0, 250);
    ImGui::Checkbox("หนึ่งนัด หนึ่งศพ", &config->aimbot[currentWeapon].killshot);
    ImGui::Checkbox("ล็อคเป้าหมายอื่นๆหลังจากฆ่า", &config->aimbot[currentWeapon].betweenShots);
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
        ImGui::Begin("กันล็อคเป้า", &window.antiAim, windowFlags);
    }
    ImGui::Text("*ใช้งานได้ไม่ค่อยดี");
    ImGui::Checkbox("เปิด", &config->antiAim.enabled);
    ImGui::Checkbox("##pitch", &config->antiAim.pitch);
    ImGui::SameLine();
    ImGui::PushItemWidth(220.0f);
    ImGui::SliderFloat("องศา", &config->antiAim.pitchAngle, -89.0f, 89.0f, "%.2f");
    ImGui::Checkbox("องศา ", &config->antiAim.yaw);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderTriggerbotWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.triggerbot)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("ยิงอัตโนมัติ", &window.triggerbot, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "ทั้งหมด\0Pistols\0Heavy\0SMG\0Rifles\0Zeus x27\0");
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
        static constexpr const char* pistols[]{ "ทั้งหมด", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

        ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx : 35].enabled) {
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
        static constexpr const char* heavies[]{ "ทั้งหมด", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

        ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx + 10 : 36].enabled) {
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
        static constexpr const char* smgs[]{ "ทั้งหมด", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

        ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx + 16 : 37].enabled) {
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
        static constexpr const char* rifles[]{ "ทั้งหมด", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

        ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx + 23 : 38].enabled) {
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
    ImGui::Checkbox("เปิด", &config->triggerbot[currentWeapon].enabled);
    ImGui::Separator();
    ImGui::Checkbox("เลือกปุ่ม", &config->triggerbot[currentWeapon].onKey);
    ImGui::SameLine();
    hotkey(config->triggerbot[currentWeapon].key);
    ImGui::Checkbox("ยิงเพื่อนอัตโนมัติ", &config->triggerbot[currentWeapon].friendlyFire);
    ImGui::Checkbox("ยิงสโคปอัตโนมัติ", &config->triggerbot[currentWeapon].scopedOnly);
    ImGui::Checkbox("ยิงอัตโนมัติเมื่อโดนแฟลช", &config->triggerbot[currentWeapon].ignoreFlash);
    ImGui::Checkbox("ยิงอัตโนมัติเมื่อโดนควัน", &config->triggerbot[currentWeapon].ignoreSmoke);
    ImGui::SetNextItemWidth(85.0f);
    ImGui::Combo("ยิงเฉพาะส่วน", &config->triggerbot[currentWeapon].hitgroup, "ทั้งหมด\0หัว\0หน้าอก\0ท้อง\0แขนซ้าย\0แขนขวา\0ขาซ้าย\0ขาขวา\0");
    ImGui::PushItemWidth(220.0f);
    ImGui::SliderInt("ดีเลย์", &config->triggerbot[currentWeapon].shotDelay, 0, 250, "%d ms");
    ImGui::InputInt("ดาเมจต่ำสุด", &config->triggerbot[currentWeapon].minDamage);
    config->triggerbot[currentWeapon].minDamage = std::clamp(config->triggerbot[currentWeapon].minDamage, 0, 250);
    ImGui::Checkbox("หนึ่งนัด หนึ่งศพ", &config->triggerbot[currentWeapon].killshot);
    ImGui::SliderFloat("ยิงเป็นชุด", &config->triggerbot[currentWeapon].burstTime, 0.0f, 0.5f, "%.3f s");

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderBacktrackWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.backtrack)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("ยิงซ้ำรอย", &window.backtrack, windowFlags);
    }
    ImGui::Checkbox("เปิด", &config->backtrack.enabled);
    ImGui::Checkbox("ยิงซ้ำรอยเมื่อโดนควัน", &config->backtrack.ignoreSmoke);
    ImGui::Checkbox("แรงดีดตามการมองเห็น", &config->backtrack.recoilBasedFov);
    ImGui::PushItemWidth(220.0f);
    ImGui::SliderInt("เวลาที่จำกัด", &config->backtrack.timeLimit, 1, 200, "%d วินาที");
    ImGui::PopItemWidth();
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderGlowWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.glow)
            return;
        ImGui::SetNextWindowSize({ 450.0f, 0.0f });
        ImGui::Begin("ตัวละครเรืองแสง", &window.glow, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "พันธมิตร\0ศัตรู\0Planting\0Defusing\0Local player\0Weapons\0C4\0Planted C4\0Chickens\0Defuse kits\0Projectiles\0Hostages\0Ragdolls\0");
    ImGui::PopID();
    static int currentItem{ 0 };
    if (currentCategory <= 3) {
        ImGui::SameLine();
        static int currentType{ 0 };
        ImGui::PushID(1);
        ImGui::Combo("", &currentType, "ทั้งหมด\0หน้ากำแพง\0หลังกำแพง\0");
        ImGui::PopID();
        currentItem = currentCategory * 3 + currentType;
    } else {
        currentItem = currentCategory + 8;
    }

    ImGui::SameLine();
    ImGui::Checkbox("เปิด", &config->glow[currentItem].enabled);
    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(125, 300.0f);
    ImGui::Checkbox("เลือดในตัว", &config->glow[currentItem].healthBased);

    ImGuiCustom::colorPopup("สี", config->glow[currentItem].color, &config->glow[currentItem].rainbow, &config->glow[currentItem].rainbowSpeed);

    ImGui::PushItemWidth(220.0f);
    ImGui::Combo("Style", &config->glow[currentItem].style, "Default\0Rim3d\0Edge\0Edge Pulse\0");
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
        ImGui::Begin("สีตัวละคร", &window.chams, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "พันธมิตร\0ศัตรู\0Planting\0Defusing\0Local player\0Weapons\0Hands\0Backtrack\0Sleeves\0");
    ImGui::PopID();
    static int currentItem{ 0 };

    if (currentCategory <= 3) {
        ImGui::SameLine();
        static int currentType{ 0 };
        ImGui::PushID(1);
        ImGui::Combo("", &currentType, "ทั้งหมด\0หน้ากำแพง\0หลังกำแพง\0");
        ImGui::PopID();
        currentItem = currentCategory * 3 + currentType;
    } else {
        currentItem = currentCategory + 8;
    }

    ImGui::SameLine();
    static int material = 1;

    if (ImGui::ArrowButton("##left", ImGuiDir_Left) && material > 1)
        --material;
    ImGui::SameLine();
    ImGui::Text("%d", material);
    ImGui::SameLine();
    if (ImGui::ArrowButton("##right", ImGuiDir_Right) && material < int(config->chams[0].materials.size()))
        ++material;

    ImGui::SameLine();
    auto& chams{ config->chams[currentItem].materials[material - 1] };

    ImGui::Checkbox("เปิด", &chams.enabled);
    ImGui::Separator();
    ImGui::Checkbox("เลือดในตัว", &chams.healthBased);
    ImGui::Checkbox("กระพริบ", &chams.blinking);
    ImGui::Combo("รูปแบบ", &chams.material, "ปกติ\0เรียบ\0อนิเมชั่น\0แพลทินั่ม\0กระจก\0โครเมียม\0คริสตัล\0เงิน\0ทอง\0พลาสติก\0เรืองแสง\0");
    ImGui::Checkbox("โครงลวด", &chams.wireframe);
    ImGuiCustom::colorPopup("สี", chams.color, &chams.rainbow, &chams.rainbowSpeed);
    ImGui::SetNextItemWidth(220.0f);

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
        ImGui::Begin("รายละเอียดตัวละคร", &window.esp, windowFlags);
    }

    static int currentCategory = 0;
    static int currentItem = 0;

    if (ImGui::ListBoxHeader("##", { 125.0f, 300.0f })) {
        static constexpr const char* players[]{ "ทั้งหมด", "หน้ากำแพง", "หลังกำแพง" };

        ImGui::Text("พันธมิตร");
        ImGui::Indent();
        ImGui::PushID("Allies");
        ImGui::PushFont(fonts.tahoma);

        for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
            bool isSelected = currentCategory == 0 && currentItem == i;

            if ((i == 0 || !config->esp.players[0].enabled) && ImGui::Selectable(players[i], isSelected)) {
                currentItem = i;
                currentCategory = 0;
            }
        }

        ImGui::PopFont();
        ImGui::PopID();
        ImGui::Unindent();
        ImGui::Text("ศัตรู");
        ImGui::Indent();
        ImGui::PushID("Enemies");
        ImGui::PushFont(fonts.tahoma);

        for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
            bool isSelected = currentCategory == 1 && currentItem == i;

            if ((i == 0 || !config->esp.players[3].enabled) && ImGui::Selectable(players[i], isSelected)) {
                currentItem = i;
                currentCategory = 1;
            }
        }

        ImGui::PopFont();
        ImGui::PopID();
        ImGui::Unindent();
        if (bool isSelected = currentCategory == 2; ImGui::Selectable("อาวุธ", isSelected))
            currentCategory = 2;

        ImGui::Text("ระเบิด");
        ImGui::Indent();
        ImGui::PushID("Projectiles");
        ImGui::PushFont(fonts.tahoma);
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

        ImGui::Text("โซนอันตราย");
        ImGui::Indent();
        ImGui::PushID("Danger Zone");
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
            ImGui::Checkbox("เปิด", &config->esp.players[selected].enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("ขนาดตัวอักษร", &config->esp.players[selected].font, 1, 294);
            config->esp.players[selected].font = std::clamp(config->esp.players[selected].font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 185.0f };
            ImGuiCustom::colorPicker("มองเส้น", config->esp.players[selected].snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("กรอบ", config->esp.players[selected].box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config->esp.players[selected].boxType, "2 มิติ\0""2 มิติ *มุม\0""3 มิติ\0""3 มิติ *มุม\0");
            ImGuiCustom::colorPicker("ทิศทางการมอง", config->esp.players[selected].eyeTraces);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("เลือด", config->esp.players[selected].health);
            ImGuiCustom::colorPicker("จุดบนหัว", config->esp.players[selected].headDot);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("แถบเลือด", config->esp.players[selected].healthBar);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(70.f);
            ImGui::Combo("##HP side", &config->esp.players[selected].hpside, "ซ้าย\0ล่าง\0ขวา\0");
            ImGui::PushID("hotfix");
            ImGuiCustom::colorPicker("ชื่อ", config->esp.players[selected].name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("เกราะ", config->esp.players[selected].armor);
            ImGuiCustom::colorPicker("เงิน", config->esp.players[selected].money);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("แถบเกราะ", config->esp.players[selected].armorBar);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(70.f);
            ImGui::PopID();
            ImGui::Combo("##AR side", &config->esp.players[selected].armorside,"ซ้าย\0ล่าง\0ขวา\0");
            ImGuiCustom::colorPicker("ระยะห่าง", config->esp.players[selected].distance);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("อาวุธที่ใช้", config->esp.players[selected].activeWeapon);
            ImGui::Checkbox("เปิดตอนตาย", &config->esp.players[selected].deadesp);
            ImGui::SliderFloat("ระยะห่างสูงสุด", &config->esp.players[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        case 2: {
            ImGui::Checkbox("เปิด", &config->esp.weapon.enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("ขนาดตัวอักษร", &config->esp.weapon.font, 1, 294);
            config->esp.weapon.font = std::clamp(config->esp.weapon.font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 200.0f };
            ImGuiCustom::colorPicker("มองเส้น", config->esp.weapon.snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("กรอบ", config->esp.weapon.box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config->esp.weapon.boxType, "2 มิติ\0""2 มิติ *มุม\0""3 มิติ\0""3 มิติ *มุม\0");
            ImGuiCustom::colorPicker("ชื่อ", config->esp.weapon.name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("กระสุน", config->esp.weapon.ammo);
            ImGuiCustom::colorPicker("สีของกรอบ", config->esp.weapon.outline);
            ImGuiCustom::colorPicker("ระยะห่าง", config->esp.weapon.distance);
            ImGui::SliderFloat("ระยะห่างสูงสุด", &config->esp.weapon.maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        case 3: {
            ImGui::Checkbox("เปิด", &config->esp.projectiles[currentItem].enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("ขนาดตัวอักษร", &config->esp.projectiles[currentItem].font, 1, 294);
            config->esp.projectiles[currentItem].font = std::clamp(config->esp.projectiles[currentItem].font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 200.0f };
            ImGuiCustom::colorPicker("มองเส้น", config->esp.projectiles[currentItem].snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("กรอบ", config->esp.projectiles[currentItem].box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config->esp.projectiles[currentItem].boxType, "2 มิติ\0""2 มิติ *มุม\0""3 มิติ\0""3 มิติ *มุม\0");
            ImGuiCustom::colorPicker("ชื่อ", config->esp.projectiles[currentItem].name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("สีของกรอบ", config->esp.projectiles[currentItem].outline);
            ImGuiCustom::colorPicker("ระยะห่าง", config->esp.projectiles[currentItem].distance);
            ImGui::SliderFloat("ระยะห่างสูงสุด", &config->esp.projectiles[currentItem].maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        case 4: {
            int selected = currentItem;
            ImGui::Checkbox("เปิด", &config->esp.dangerZone[selected].enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("ขนาดตัวอักษร", &config->esp.dangerZone[selected].font, 1, 294);
            config->esp.dangerZone[selected].font = std::clamp(config->esp.dangerZone[selected].font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 200.0f };
            ImGuiCustom::colorPicker("มองเส้น", config->esp.dangerZone[selected].snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("กรอบ", config->esp.dangerZone[selected].box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config->esp.dangerZone[selected].boxType, "2 มิติ\0""2 มิติ *มุม\0""3 มิติ\0""3 มิติ *มุม\0");
            ImGuiCustom::colorPicker("ชื่อ", config->esp.dangerZone[selected].name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("สีของกรอบ", config->esp.dangerZone[selected].outline);
            ImGuiCustom::colorPicker("ระยะห่าง", config->esp.dangerZone[selected].distance);
            ImGui::SliderFloat("ระยะห่างสูงสุด", &config->esp.dangerZone[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
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
        ImGui::Begin("การมองเห็น", &window.visuals, windowFlags);
    }
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(125, 300.0f);
    ImGui::Combo("โมเดล โจร", &config->visuals.playerModelT, "ปกติ\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0");
    ImGui::Combo("โมเดล ตำรวจ", &config->visuals.playerModelCT, "ปกติ\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0");
    ImGui::Checkbox("ปิด โพส-โปรเซสซิ่ง", &config->visuals.disablePostProcessing);
    ImGui::Checkbox("ฆ่าแล้วตัวละครลอย", &config->visuals.inverseRagdollGravity);
    ImGui::Checkbox("ปิดหมอก", &config->visuals.noFog);
    ImGui::Checkbox("ปิดท้องฟ้า 3 มิติ", &config->visuals.no3dSky);
    ImGui::Checkbox("ปิดเอฟเฟคแรงดีด 1", &config->visuals.noAimPunch);
    ImGui::Checkbox("ปิดเอฟเฟคแรงดีด 2", &config->visuals.noViewPunch);
    ImGui::Checkbox("ปิดมือ", &config->visuals.noHands);
    ImGui::Checkbox("ปิดแขนเสื้อ", &config->visuals.noSleeves);
    ImGui::Checkbox("ปิดอาวุธ", &config->visuals.noWeapons);
    ImGui::Checkbox("ปิดควัน", &config->visuals.noSmoke);
    ImGui::Checkbox("ปิดเบลอ", &config->visuals.noBlur);
    ImGui::Checkbox("ปิดสโคป", &config->visuals.noScopeOverlay);
    ImGui::Checkbox("ปิดหญ้า", &config->visuals.noGrass);
    ImGui::Checkbox("ปิดเงา", &config->visuals.noShadows);
    ImGui::Checkbox("เปลี่ยนควันเป็นโครง", &config->visuals.wireframeSmoke);
    ImGui::NextColumn();
    ImGui::Checkbox("ซูม", &config->visuals.zoom);
    ImGui::SameLine();
    hotkey(config->visuals.zoomKey);
    ImGui::Checkbox("มุมมองบุคคลที่ 3", &config->visuals.thirdperson);
    ImGui::SameLine();
    hotkey(config->visuals.thirdpersonKey);
    ImGui::PushItemWidth(290.0f);
    ImGui::PushID(0);
    ImGui::SliderInt("", &config->visuals.thirdpersonDistance, 0, 1000, "ระยะมุมมองบุคคลที่ 3: %d");
    ImGui::PopID();
    ImGui::PushID(1);
    ImGui::SliderInt("", &config->visuals.viewmodelFov, -60, 60, "ขอบเขตมองเห็นตัวละคร: %d");
    ImGui::PopID();
    ImGui::PushID(2);
    ImGui::SliderInt("", &config->visuals.fov, -60, 60, "ขอบเขตการมองเห็น: %d");
    ImGui::PopID();
    ImGui::PushID(3);
    ImGui::SliderInt("", &config->visuals.farZ, 0, 2000, "เพิ่มระยะ (Danger Zone): %d");
    ImGui::PopID();
    ImGui::PushID(4);
    ImGui::SliderInt("", &config->visuals.flashReduction, 0, 100, "ลดแฟลช: %d%%");
    ImGui::PopID();
    ImGui::PushID(5);
    ImGui::SliderFloat("", &config->visuals.brightness, 0.0f, 1.0f, "แสง: %.2f");
    ImGui::PopID();
    ImGui::PopItemWidth();
    ImGui::Combo("ท้องฟ้า", &config->visuals.skybox, "Default\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0");
    ImGuiCustom::colorPicker("ปรับสีแม็พ", config->visuals.world);
    ImGuiCustom::colorPicker("ปรับสีท้องฟ้า", config->visuals.sky);
    ImGui::Checkbox("ควงปืน Deagle", &config->visuals.deagleSpinner);
    ImGui::Combo("เอฟเฟคหน้าจอ", &config->visuals.screenEffect, "ปกติ\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
    ImGui::Combo("เอฟเฟคตอนยิง", &config->visuals.hitEffect, "ปกติ\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
    ImGui::SliderFloat("ระยะเวลาเอฟเฟคตอนยิง", &config->visuals.hitEffectTime, 0.1f, 1.5f, "%.2fs");
    ImGui::Combo("ขึ้นกากบาทเมื่อยิงโดน", &config->visuals.hitMarker, "ปกติ\0กากบาท\0");
    ImGui::SliderFloat("ระยะเวลาขึ้นกากบาท", &config->visuals.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
    ImGui::Checkbox("แก้ไขสีหน้าจอ", &config->visuals.colorCorrection.enabled);
    ImGui::SameLine();
    bool ccPopup = ImGui::Button("แก้ไข");

    if (ccPopup)
        ImGui::OpenPopup("##popup");

    if (ImGui::BeginPopup("##popup")) {
        ImGui::VSliderFloat("##1", { 40.0f, 160.0f }, &config->visuals.colorCorrection.blue, 0.0f, 1.0f, "Blue\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##2", { 40.0f, 160.0f }, &config->visuals.colorCorrection.red, 0.0f, 1.0f, "Red\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##3", { 40.0f, 160.0f }, &config->visuals.colorCorrection.mono, 0.0f, 1.0f, "Mono\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##4", { 40.0f, 160.0f }, &config->visuals.colorCorrection.saturation, 0.0f, 1.0f, "Sat\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##5", { 40.0f, 160.0f }, &config->visuals.colorCorrection.ghost, 0.0f, 1.0f, "Ghost\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##6", { 40.0f, 160.0f }, &config->visuals.colorCorrection.green, 0.0f, 1.0f, "Green\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##7", { 40.0f, 160.0f }, &config->visuals.colorCorrection.yellow, 0.0f, 1.0f, "Yellow\n%.3f"); ImGui::SameLine();
        ImGui::EndPopup();
    }
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
        ImGui::Begin("เปลี่ยนสกิน", &window.skinChanger, windowFlags);
    }

    static auto itemIndex = 0;

    ImGui::PushItemWidth(110.0f);
    ImGui::Combo("##1", &itemIndex, [](void* data, int idx, const char** out_text) {
        *out_text = game_data::weapon_names[idx].name;
        return true;
        }, nullptr, IM_ARRAYSIZE(game_data::weapon_names), 5);
    ImGui::PopItemWidth();

    auto& selected_entry = config->skinChanger[itemIndex];
    selected_entry.itemIdIndex = itemIndex;

    {
        ImGui::SameLine();
        ImGui::Checkbox("เปิด", &selected_entry.enabled);
        ImGui::Separator();
        ImGui::Columns(2, nullptr, false);
        ImGui::InputInt("สุ่ม", &selected_entry.seed);
        ImGui::InputInt("จำนวนการฆ่า", &selected_entry.stat_trak);
        ImGui::SliderFloat("ความชัด", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

        ImGui::Combo("สกิน", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) {
            *out_text = (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits)[idx].name.c_str();
            return true;
            }, nullptr, (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits).size(), 10);

        ImGui::Combo("คุณภาพ", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text) {
            *out_text = game_data::quality_names[idx].name;
            return true;
            }, nullptr, IM_ARRAYSIZE(game_data::quality_names), 5);

        if (itemIndex == 0) {
            ImGui::Combo("มีด", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = game_data::knife_names[idx].name;
                return true;
                }, nullptr, IM_ARRAYSIZE(game_data::knife_names), 5);
        } else if (itemIndex == 1) {
            ImGui::Combo("ถุงมือ", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = game_data::glove_names[idx].name;
                return true;
                }, nullptr, IM_ARRAYSIZE(game_data::glove_names), 5);
        } else {
            static auto unused_value = 0;
            selected_entry.definition_override_vector_index = 0;
            ImGui::Combo("Unavailable", &unused_value, "For knives or gloves\0");
        }

        ImGui::InputText("ชื่อปืน", selected_entry.custom_name, 32);
    }

    ImGui::NextColumn();

    {
        ImGui::PushID("สติ๊กเกอร์");

        static auto selectedStickerSlot = 0;

        ImGui::PushItemWidth(-1);

        if (ImGui::ListBoxHeader("", 5)) {
            for (int i = 0; i < 5; ++i) {
                ImGui::PushID(i);

                const auto kit_vector_index = config->skinChanger[itemIndex].stickers[i].kit_vector_index;
                const std::string text = '#' + std::to_string(i + 1) + "  " + SkinChanger::stickerKits[kit_vector_index].name;

                if (ImGui::Selectable(text.c_str(), i == selectedStickerSlot))
                    selectedStickerSlot = i;

                ImGui::PopID();
            }
            ImGui::ListBoxFooter();
        }

        ImGui::PopItemWidth();

        auto& selected_sticker = selected_entry.stickers[selectedStickerSlot];

        ImGui::Combo("ลายสติ๊กเกอร์", &selected_sticker.kit_vector_index, [](void* data, int idx, const char** out_text) {
            *out_text = SkinChanger::stickerKits[idx].name.c_str();
            return true;
            }, nullptr, SkinChanger::stickerKits.size(), 10);

        ImGui::SliderFloat("ความชัด", &selected_sticker.wear, FLT_MIN, 1.0f, "%.10f", 5.0f);
        ImGui::SliderFloat("อัตราส่วน", &selected_sticker.scale, 0.1f, 5.0f);
        ImGui::SliderFloat("หมุน", &selected_sticker.rotation, 0.0f, 360.0f);

        ImGui::PopID();
    }
    selected_entry.update();

    ImGui::Columns(1);

    ImGui::Separator();

    if (ImGui::Button("อัพเดต", { 130.0f, 30.0f }))
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
        ImGui::Begin("เสียง", &window.sound, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(300.0f);
    ImGui::Combo("", &currentCategory, "ผู้เล่น\0พันธมิตร\0ศัตรู\0");
    ImGui::PopItemWidth();
    ImGui::PushItemWidth(300.0f);
    ImGui::SliderInt("เสียงหลัก", &config->sound.players[currentCategory].masterVolume, 0, 200, "%d%%");
    ImGui::SliderInt("เสียงยิงโดนหัว", &config->sound.players[currentCategory].headshotVolume, 0, 200, "%d%%");
    ImGui::SliderInt("เสียงอาวุธ", &config->sound.players[currentCategory].weaponVolume, 0, 200, "%d%%");
    ImGui::SliderInt("เสียงเท้า", &config->sound.players[currentCategory].footstepVolume, 0, 200, "%d%%");
    ImGui::SliderInt("เสียงไก่", &config->sound.chickenVolume, 0, 200, "%d%%");

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderStyleWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.style)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("ธีม", &window.style, windowFlags);
    }

    ImGui::PushItemWidth(300.0f);
    if (ImGui::Combo("สีเมนู", &config->style.menuColors, "มืด\0สว่าง\0คลาสสิค\0กำหนดเอง\0"))
        updateColors();
    ImGui::PopItemWidth();

    if (config->style.menuColors == 3) {
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
        ImGui::Begin("อื่นๆ", &window.misc, windowFlags);
    }
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(125, 300.0f);
    ImGui::TextUnformatted("เปิดเมนู");
    ImGui::SameLine();
    hotkey(config->misc.menuKey);

    ImGui::Checkbox("กันโดนเตะ AFK", &config->misc.antiAfkKick);
    ImGui::Checkbox("เอียงตัวอัตโนมัติ", &config->misc.autoStrafe);
    ImGui::Checkbox("กระโดดอัตโนมัติ", &config->misc.bunnyHop);
    ImGui::Checkbox("นั่งไว", &config->misc.fastDuck);
    ImGui::Checkbox("เดินไม่ขยับเท้า", &config->misc.moonwalk);
    ImGui::Checkbox("กระโดดตรงขอบอัตโนมัติ", &config->misc.edgejump);
    ImGui::SameLine();
    hotkey(config->misc.edgejumpkey);
    ImGui::Checkbox("เดินช้า", &config->misc.slowwalk);
    ImGui::SameLine();
    hotkey(config->misc.slowwalkKey);
    ImGui::Checkbox("เป้าสไนเปอร์", &config->misc.sniperCrosshair);
    ImGui::Checkbox("เป้าแรงดีด", &config->misc.recoilCrosshair);
    ImGui::Checkbox("ปืนพกอัตโนมัติ", &config->misc.autoPistol);
    ImGui::Checkbox("โหลดกระสุนอัตโนมัติ", &config->misc.autoReload);
    ImGui::Checkbox("กดยืนยันอัตโนมัติ", &config->misc.autoAccept);
    ImGui::Checkbox("เรด้า", &config->misc.radarHack);
    ImGui::Checkbox("แสดงแรงค์", &config->misc.revealRanks);
    ImGui::Checkbox("แสดงเงิน", &config->misc.revealMoney);
    ImGui::Checkbox("แสดงชื่อในโอเวอร์วอช", &config->misc.revealSuspect);
    ImGuiCustom::colorPicker("รายชื่อคนดู", config->misc.spectatorList);
    ImGuiCustom::colorPicker("ลายน้ำ", config->misc.watermark);
    ImGui::Checkbox("แก้อนิเมชั่น LOD", &config->misc.fixAnimationLOD);
    ImGui::Checkbox("แก้โครงกระดุก", &config->misc.fixBoneMatrix);
    ImGui::Checkbox("แก้การเคลื่อนไหว", &config->misc.fixMovement);
    ImGui::Checkbox("ปิดโมเดล occlusion", &config->misc.disableModelOcclusion);
    ImGui::SliderFloat("อัตราส่วนหน้าจอ", &config->misc.aspectratio, 0.0f, 5.0f, "%.2f");
    ImGui::NextColumn();
    ImGui::Checkbox("ปิดเบลอ", &config->misc.disablePanoramablur);
    ImGui::Checkbox("ชื่อแคลนขยับ", &config->misc.animatedClanTag);
    ImGui::Checkbox("เปลี่ยนชื่อแคลนเป็นเวลา", &config->misc.clocktag);
    ImGui::Checkbox("ชื่อแคลน", &config->misc.customClanTag);
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(0);
    if (ImGui::InputText("", config->misc.clanTag, sizeof(config->misc.clanTag)))
        Misc::updateClanTag(true);
    ImGui::PopID();
    ImGui::Checkbox("พิมพ์หลังจากฆ่า", &config->misc.killMessage);
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(1);
    ImGui::InputText("", &config->misc.killMessageString);
    ImGui::PopID();
    ImGui::Checkbox("ขโมยชื่อ", &config->misc.nameStealer);
    ImGui::PushID(3);
    ImGui::SetNextItemWidth(100.0f);
    ImGui::Combo("", &config->misc.banColor, "White\0Red\0Purple\0Green\0Light green\0Turquoise\0Light red\0Gray\0Yellow\0Gray 2\0Light blue\0Gray/Purple\0Blue\0Pink\0Dark orange\0Orange\0");
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(4);
    ImGui::InputText("", &config->misc.banText);
    ImGui::PopID();
    ImGui::SameLine();
    if (ImGui::Button("เฟคแบน"))
        Misc::fakeBan(true);
    ImGui::Checkbox("วางระเบิดไว", &config->misc.fastPlant);
    ImGuiCustom::colorPicker("เวลาระเบิด", config->misc.bombTimer);
    ImGui::Checkbox("รีโหลดไว", &config->misc.quickReload);
    ImGui::Checkbox("เตรียมลูกโม่", &config->misc.prepareRevolver);
    ImGui::SameLine();
    hotkey(config->misc.prepareRevolverKey);
    ImGui::Combo("เสียงยิงโดน", &config->misc.hitSound, "None\0Metal\0Gamesense\0Bell\0Glass\0Custom\0");
    if (config->misc.hitSound == 5) {
        ImGui::InputText("Hit Sound filename", &config->misc.customHitSound);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("audio file must be put in csgo/sound/ directory");
    }
    ImGui::PushID(5);
    ImGui::Combo("เสียงฆ่า", &config->misc.killSound, "None\0Metal\0Gamesense\0Bell\0Glass\0Custom\0");
    if (config->misc.killSound == 5) {
        ImGui::InputText("Kill Sound filename", &config->misc.customKillSound);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("audio file must be put in csgo/sound/ directory");
    }
    ImGui::PopID();
    ImGui::SetNextItemWidth(90.0f);
    ImGui::InputInt("เฟคแล็ค", &config->misc.chokedPackets, 1, 5);
    config->misc.chokedPackets = std::clamp(config->misc.chokedPackets, 0, 64);
    ImGui::SameLine();
    hotkey(config->misc.chokedPacketsKey);
    ImGui::Text("เติมเลือดไว");
    ImGui::SameLine();
    hotkey(config->misc.quickHealthshotKey);
    ImGui::Checkbox("ทำนายผลระเบิด", &config->misc.nadePredict);
    ImGui::Checkbox("แก้ไขสัญญาณแท็บเล็ต", &config->misc.fixTabletSignal);
    ImGui::SetNextItemWidth(120.0f);
    ImGui::SliderFloat("มุมเดลต้าสูงสุด", &config->misc.maxAngleDelta, 0.0f, 255.0f, "%.2f");
    ImGui::Checkbox("เฟคไพร์ม", &config->misc.fakePrime);
    ImGui::Checkbox("รายการที่ซื้อ", &config->misc.purchaseList.enabled);
    ImGui::SameLine();

    ImGui::PushID("รายการที่ซื้อ");
    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::SetNextItemWidth(75.0f);
        ImGui::Combo("โหมด", &config->misc.purchaseList.mode, "รายละเอียด\0สรุป\0");
        ImGui::Checkbox("เฉพาะเวลาขยับไม่ได้", &config->misc.purchaseList.onlyDuringFreezeTime);
        ImGui::Checkbox("แสดงราคา", &config->misc.purchaseList.showPrices);
        ImGui::Checkbox("ปิดแถบหัวข้อ", &config->misc.purchaseList.noTitleBar);
        ImGui::EndPopup();
    }
    ImGui::PopID();

    if (ImGui::Button("ปิดใช้งานเมนู"))
        hooks->uninstall();

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
        ImGui::Begin("รีพอร์ตบอท", &window.reportbot, windowFlags);
    }
    ImGui::Checkbox("เปิด", &config->reportbot.enabled);
    ImGui::SameLine(0.0f, 50.0f);
    if (ImGui::Button("รีเซ็ต"))
        Reportbot::reset();
    ImGui::Separator();
    ImGui::PushItemWidth(300.0f);
    ImGui::Combo("เป้าหมาย", &config->reportbot.target, "ศัตรู\0พันธมิตร\0ทั้งหมด\0");
    ImGui::InputInt("ดีเลย์ (วิ)", &config->reportbot.delay);
    config->reportbot.delay = (std::max)(config->reportbot.delay, 1);
    ImGui::InputInt("รอบ", &config->reportbot.rounds);
    config->reportbot.rounds = (std::max)(config->reportbot.rounds, 1);
    ImGui::PopItemWidth();
    ImGui::Checkbox("การสื่อสารที่ไม่เหมาะสม", &config->reportbot.textAbuse);
    ImGui::Checkbox("เกรียน", &config->reportbot.griefing);
    ImGui::Checkbox("มองทะลุ", &config->reportbot.wallhack);
    ImGui::Checkbox("ล็อคเป้า   ", &config->reportbot.aimbot);
    ImGui::Checkbox("อื่นๆ", &config->reportbot.other);

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderConfigWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.config)
            return;
        ImGui::SetNextWindowSize({ 290.0f, 200.0f });
        ImGui::Begin("คอนฟิก", &window.config, windowFlags);
    }

    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(125, 300.0f);

    ImGui::PushItemWidth(308.0f);

    if (ImGui::Button("โหลดคอนฟิก", { 308.0f, 25.0f }))
        config->listConfigs();

    auto& configItems = config->getConfigs();
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
                config->rename(currentConfig, buffer.c_str());
        }
        ImGui::PopID();

        ImGui::PushItemWidth(100.0f);

        if (ImGui::Button("สร้างคอนฟิก", { 150.0f, 25.0f }))
            config->add(buffer.c_str());
        ImGui::SameLine();
        if (ImGui::Button("รีเซ็ทคอนฟิก", { 150.0f, 25.0f }))
            ImGui::OpenPopup("Config to reset");
        ImGui::Text(" ");
        if (ImGui::BeginPopup("Config to reset")) {
            static constexpr const char* names[]{ "Whole", "Aimbot", "Triggerbot", "Backtrack", "Anti aim", "Glow", "Chams", "Esp", "Visuals", "Skin changer", "Sound", "Style", "Misc", "Reportbot" };
            for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                if (i == 1) ImGui::Separator();

                if (ImGui::Selectable(names[i])) {
                    switch (i) {
                    case 0: config->reset(); updateColors(); Misc::updateClanTag(true); SkinChanger::scheduleHudUpdate(); break;
                    case 1: config->aimbot = { }; break;
                    case 2: config->triggerbot = { }; break;
                    case 3: config->backtrack = { }; break;
                    case 4: config->antiAim = { }; break;
                    case 5: config->glow = { }; break;
                    case 6: config->chams = { }; break;
                    case 7: config->esp = { }; break;
                    case 8: config->visuals = { }; break;
                    case 9: config->skinChanger = { }; SkinChanger::scheduleHudUpdate(); break;
                    case 10: config->sound = { }; break;
                    case 11: config->style = { }; updateColors(); break;
                    case 12: config->misc = { };  Misc::updateClanTag(true); break;
                    case 13: config->reportbot = { }; break;
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (currentConfig != -1) {
            if (ImGui::Button("โหลดที่เลือก", { 308.0f, 25.0f })) {
                config->load(currentConfig);
                updateColors();
                SkinChanger::scheduleHudUpdate();
                Misc::updateClanTag(true);
            }
            if (ImGui::Button("เซฟที่เลือก", { 308.0f, 25.0f }))
                config->save(currentConfig);
            if (ImGui::Button("ลบที่เลือก", { 308.0f, 25.0f }))
                config->remove(currentConfig);
        }
        ImGui::Columns(1);
        if (!contentOnly)
            ImGui::End();
}

void GUI::renderGuiStyle2() noexcept
{

    ImGui::SetNextWindowSize(ImVec2(1150, 525), ImGuiCond_FirstUseEver);
    static int iPage0 = 0;
    static int iPage1 = 0;
    static int iPage2 = 0;
    static int iPage3 = 0;
    ImGui::Begin("โอซีริส - Osiris", nullptr);
    {
        static int tabb = 0;
        {
            ImGui::PushFont(fonts.tahoma);
            const char* tabbname[] = {
                "อัพเดต",
                "ศูนย์เล็ง",
                "รายละเอียด",
                "ตั้งค่า"
            };
            ImGuiStyle* style = &ImGui::GetStyle();
            ImVec4* colors = style->Colors;
            for (int i = 0; i < ARRAYSIZE(tabbname) - 1; i++)
            {

                if (tabb == i) {
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_ButtonActive]);
                    if (ImGui::Button(tabbname[i], ImVec2(ImGui::GetWindowSize().x / (ARRAYSIZE(tabbname)) - 19, ImGui::GetWindowSize().y / 8)))
                        tabb = i;
                    ImGui::PopStyleColor();
                }
                else {
                    ImGui::SameLine();
                    if (ImGui::Button(tabbname[i], ImVec2(ImGui::GetWindowSize().x / (ARRAYSIZE(tabbname)) - 19, ImGui::GetWindowSize().y / 8)))
                        tabb = i;
                }

            }
            if (tabb == ARRAYSIZE(tabbname) - 1) {
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_ButtonActive]);
                if (ImGui::Button(tabbname[ARRAYSIZE(tabbname) - 1], ImVec2(ImGui::GetWindowSize().x / (ARRAYSIZE(tabbname)) - 44.5f, ImGui::GetWindowSize().y / 8)))
                    tabb = ARRAYSIZE(tabbname) - 1;
                ImGui::PopStyleColor();
            }
            else {
                ImGui::SameLine();
                if (ImGui::Button(tabbname[ARRAYSIZE(tabbname) - 1], ImVec2(ImGui::GetWindowSize().x / (ARRAYSIZE(tabbname)) - 44.5f, ImGui::GetWindowSize().y / 8)))
                    tabb = ARRAYSIZE(tabbname) - 1;
            }
            ImGui::PopFont();
        }
        ImGui::Separator();
        if (tabb == 0) {
            ImGui::PushItemWidth(110.0f);
            ImGui::TextUnformatted("อัพเดตเมื่อ: " __DATE__ " " __TIME__);
            ImGui::Text(" ");
            ImGui::Text("โอซีริส (Osiris)");
            ImGui::Text("1. ควรใช้ด้วยความระมัดระวังหากโดนแบนทางเราจะไม่รับผิดชอบใดๆทั้งสิ้น");
            ImGui::Text("2. โอซีริส เหมาะสำหรับใช้งานปกติ");
            ImGui::Text("3. โอซีริส เป็นโปรฟรี บางฟังก์ชั่นอาจใช้ไม่ได้ หรือ ไม่โหดเท่าโปรเติม");
            ImGui::Text("4. โอซีริส เป็นโอเพนซอร์ซ สามารถนำไปปรับแต่ง หรือ พัฒนาต่อได้");
            ImGui::Text(" ");
            ImGui::Text("เครดิต");
            if (ImGui::Button("Daniel Krupiński", { 200.0f, 30.0f }))
                system("start https://github.com/danielkrupinski/Osiris");
            ImGui::SameLine();
            if (ImGui::Button("NetnetZ", { 200.0f, 30.0f }))
                system("start https://github.com/NetnetZ/Osiris");
            if (ImGui::Button("TqzDev", { 200.0f, 30.0f }))
                system("start https://www.youtube.com/tqzdev");
            ImGui::SameLine();
            if (ImGui::Button("SSense", { 200.0f, 30.0f }))
                system("start https://ssense.ml/");
            ImGui::Text(" ");
            if (ImGui::Button("ปิดใช้งานเมนู", { 200.0f, 30.0f }))
                hooks->uninstall();
            ImGui::SameLine();
            if (ImGui::Button("ปิดเกม", { 200.0f, 30.0f }))
                system("taskkill /f /im csgo.exe");
        }
        if (tabb == 1) {
            const char* tabs[] = {
        "ล็อคเป้า",
        "ยิงอัตโนมัติ",
        "ยิงซ้ำรอย",
        "กันล็อคเป้า"
            };
            ImGui::BeginGroup();
            ImGuiStyle* style = &ImGui::GetStyle();
            ImVec4* colors = style->Colors;
            for (int i = 0; i < ARRAYSIZE(tabs); i++)
            {

                if (iPage1 == i) {
                    ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_ButtonActive]);
                    if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / (/*ARRAYSIZE(tabs)*/ 4 * 2) - 9, ImGui::GetWindowSize().y / 8)))
                        iPage1 = i;
                    ImGui::PopStyleColor();
                }
                else {
                    if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / (/*ARRAYSIZE(tabs)*/ 4 * 2) - 9, ImGui::GetWindowSize().y / 8)))
                        iPage1 = i;
                }

            }
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();

            switch (iPage1)
            {
            case 0:
                renderAimbotWindow(true);
                break;
            case 1:
                renderTriggerbotWindow(true);
                break;
            case 2:
                renderBacktrackWindow(true);
                break;
            case 3:
                renderAntiAimWindow(true);
                break;
            }
            ImGui::EndTabItem();
        }
        if (tabb == 2) {
            const char* tabs[] = {
        "สีตัวละคร",
        "รายละเอียดตัวละคร",
        "ตัวละครเรืองแสง",
        "การมองเห็น",
        "เปลี่ยนสกิน"
            };
            ImGui::BeginGroup();
            ImGuiStyle* style = &ImGui::GetStyle();
            ImVec4* colors = style->Colors;
            for (int i = 0; i < ARRAYSIZE(tabs); i++)
            {

                if (iPage2 == i) {
                    ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_ButtonActive]);
                    if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / (/*ARRAYSIZE(tabs)*/ 4 * 2) - 9, ImGui::GetWindowSize().y / 8)))
                        iPage2 = i;
                    ImGui::PopStyleColor();
                }
                else {
                    if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / (/*ARRAYSIZE(tabs)*/ 4 * 2) - 9, ImGui::GetWindowSize().y / 8)))
                        iPage2 = i;
                }

            }
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();

            switch (iPage2)
            {
            case 0:
                renderChamsWindow(true);
                break;
            case 1:
                renderEspWindow(true);
                break;
            case 2:
                renderGlowWindow(true);
                break;
            case 3:
                renderVisualsWindow(true);
                break;
            case 4:
                renderSkinChangerWindow(true);
                break;
            }
            ImGui::EndTabItem();
        }
        if (tabb == 3) {
            const char* tabs[] = {
        "ธีม",
        "เสียง",
        "อื่นๆ",
        "รีพอร์ตบอท",
        "คอนฟิก"
            };
            ImGui::BeginGroup();
            ImGuiStyle* style = &ImGui::GetStyle();
            ImVec4* colors = style->Colors;
            for (int i = 0; i < ARRAYSIZE(tabs); i++)
            {

                if (iPage3 == i) {
                    ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_ButtonActive]);
                    if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / (/*ARRAYSIZE(tabs)*/ 4 * 2) - 9, ImGui::GetWindowSize().y / 8)))
                        iPage3 = i;
                    ImGui::PopStyleColor();
                }
                else {
                    if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / (/*ARRAYSIZE(tabs)*/ 4 * 2) - 9, ImGui::GetWindowSize().y / 8)))
                        iPage3 = i;
                }

            }
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();

            switch (iPage3)
            {
            case 0:
                renderStyleWindow(true);
                break;
            case 1:
                renderSoundWindow(true);
                break;
            case 2:
                renderMiscWindow(true);
                break;
            case 3:
                renderReportbotWindow(true);
                break;
            case 4:
                renderConfigWindow(true);
                break;
            }
            ImGui::EndTabItem();
        }

    }

    ImGui::End();
}