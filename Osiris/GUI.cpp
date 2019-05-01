#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <Windows.h>

#include "imgui/imgui.h"
#include "GUI.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/Visuals.h"
#include "Hooks.h"

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

void GUI::render() noexcept
{
    renderMenuBar();
    renderAimbotWindow();
    renderTriggerbotWindow();
    renderGlowWindow();
    renderChamsWindow();
    renderEspWindow();
    renderVisualsWindow();
    renderKnifeChangerWindow();
    renderMiscWindow();
    renderConfigWindow();
}

void GUI::checkboxedColorPicker(const std::string& name, bool* enable, float* color) noexcept
{
    ImGui::Checkbox(("##" + name).c_str(), enable);
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::PushID(0);
    bool openPopup = ImGui::ColorButton(("##" + name).c_str(), ImColor{ color[0], color[1], color[2] }, ImGuiColorEditFlags_NoTooltip);
    ImGui::PopID();
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::Text(name.c_str());
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
    key ? ImGui::Text("[ 0x%x ]", key) : ImGui::Text("[ key ]");
    if (ImGui::IsItemHovered()) {
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
        ImGui::MenuItem("Triggerbot", nullptr, &window.triggerbot);
        ImGui::MenuItem("Glow", nullptr, &window.glow);
        ImGui::MenuItem("Chams", nullptr, &window.chams);
        ImGui::MenuItem("Esp", nullptr, &window.esp);
        ImGui::MenuItem("Visuals", nullptr, &window.visuals);
        ImGui::MenuItem("Knife changer", nullptr, &window.knifeChanger);
        ImGui::MenuItem("Misc", nullptr, &window.misc);
        ImGui::MenuItem("Config", nullptr, &window.config);
        ImGui::EndMainMenuBar();
    }
}

void GUI::renderAimbotWindow() noexcept
{
    if (window.aimbot) {
        ImGui::SetNextWindowSize({ 260.0f, 420.0f });
        ImGui::Begin("Aimbot", &window.aimbot, windowFlags);
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
            ImGui::Combo("", &currentPistol, "Glock-18\0P2000\0USP-S\0Dual Berettas\0P250\0Tec-9\0Five-Seven\0CZ-75\0Desert Eagle\0Revolver\0");
            currentWeapon = currentPistol + 1;
            break;
        }
        case 2: {
            static int currentHeavy{ 0 };
            ImGui::Combo("", &currentHeavy, "Nova\0XM1014\0Sawed-off\0MAG-7\0M249\0Negev\0");
            currentWeapon = currentHeavy + 11;
            break;
        }
        case 3: {
            static int currentSmg{ 0 };
            ImGui::Combo("", &currentSmg, "Mac-10\0MP9\0MP7\0MP5-SD\0UMP-45\0P90\0PP-Bizon\0");
            currentWeapon = currentSmg + 17;
            break;
        }
        case 4: {
            static int currentRifle{ 0 };
            ImGui::Combo("", &currentRifle, "Galil AR\0Famas\0AK-47\0M4A4\0M4A1-S\0SSG-08\0SG-553\0AUG\0AWP\0G3SG1\0SCAR-20\0");
            currentWeapon = currentRifle + 24;
            break;
        }
        }
        ImGui::PopID();
        ImGui::Checkbox("Enabled", &config.aimbot.weapons[currentWeapon].enabled);
        ImGui::Checkbox("Silent", &config.aimbot.weapons[currentWeapon].silent);
        ImGui::Checkbox("Friendly fire", &config.aimbot.weapons[currentWeapon].friendlyFire);
        ImGui::Checkbox("Visible only", &config.aimbot.weapons[currentWeapon].visibleOnly);
        ImGui::Checkbox("Scoped only", &config.aimbot.weapons[currentWeapon].scopedOnly);
        ImGui::Checkbox("Ignore flash", &config.aimbot.weapons[currentWeapon].ignoreFlash);
        ImGui::Checkbox("Ignore smoke", &config.aimbot.weapons[currentWeapon].ignoreSmoke);
        ImGui::Checkbox("Auto shot", &config.aimbot.weapons[currentWeapon].autoShot);
        ImGui::Checkbox("Recoil-based fov", &config.aimbot.weapons[currentWeapon].recoilbasedFov);
        ImGui::Combo("Bone", &config.aimbot.weapons[currentWeapon].bone, "Nearest\0Head\0Neck\0Sternum\0Chest\0Stomach\0Pelvis\0");
        ImGui::PushItemWidth(240.0f);
        ImGui::PushID(5);
        ImGui::SliderFloat("", &config.aimbot.weapons[currentWeapon].fov, 0.0f, 255.0f, "Fov: %.2f");
        ImGui::PopID();
        ImGui::PushID(6);
        ImGui::SliderFloat("", &config.aimbot.weapons[currentWeapon].smooth, 1.0f, 100.0f, "Smooth: %.2f");
        ImGui::PopID();
        ImGui::PushID(7);
        ImGui::SliderFloat("", &config.aimbot.weapons[currentWeapon].recoilControlX, 0.0f, 1.0f, "Recoil control x: %.2f");
        ImGui::PopID();
        ImGui::PushID(9);
        ImGui::SliderFloat("", &config.aimbot.weapons[currentWeapon].recoilControlY, 0.0f, 1.0f, "Recoil control y: %.2f");
        ImGui::PopID();
        ImGui::End();
    }
}

void GUI::renderTriggerbotWindow() noexcept
{
    if (window.triggerbot) {
        ImGui::SetNextWindowSize({ 250.0f, 270.0f });
        ImGui::Begin("Triggerbot", &window.triggerbot, windowFlags);
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
            ImGui::Combo("", &currentPistol, "Glock-18\0P2000\0USP-S\0Dual Berettas\0P250\0Tec-9\0Five-Seven\0CZ-75\0Desert Eagle\0Revolver\0");
            currentWeapon = currentPistol + 1;
            break;
        }
        case 2: {
            static int currentHeavy{ 0 };
            ImGui::Combo("", &currentHeavy, "Nova\0XM1014\0Sawed-off\0MAG-7\0M249\0Negev\0");
            currentWeapon = currentHeavy + 11;
            break;
        }
        case 3: {
            static int currentSmg{ 0 };
            ImGui::Combo("", &currentSmg, "Mac-10\0MP9\0MP7\0MP5-SD\0UMP-45\0P90\0PP-Bizon\0");
            currentWeapon = currentSmg + 17;
            break;
        }
        case 4: {
            static int currentRifle{ 0 };
            ImGui::Combo("", &currentRifle, "Galil AR\0Famas\0AK-47\0M4A4\0M4A1-S\0SSG-08\0SG-553\0AUG\0AWP\0G3SG1\0SCAR-20\0");
            currentWeapon = currentRifle + 24;
            break;
        }
        }
        ImGui::PopID();
        ImGui::Checkbox("Enabled", &config.triggerbot.weapons[currentWeapon].enabled);
        ImGui::Checkbox("On key", &config.triggerbot.weapons[currentWeapon].onKey);
        ImGui::SameLine();
        hotkey(config.triggerbot.weapons[currentWeapon].key);
        ImGui::Checkbox("Friendly fire", &config.triggerbot.weapons[currentWeapon].friendlyFire);
        ImGui::Checkbox("Scoped only", &config.triggerbot.weapons[currentWeapon].scopedOnly);
        ImGui::Checkbox("Ignore flash", &config.triggerbot.weapons[currentWeapon].ignoreFlash);
        ImGui::Checkbox("Ignore smoke", &config.triggerbot.weapons[currentWeapon].ignoreSmoke);
        ImGui::PushItemWidth(85.0f);
        ImGui::Combo("Hitgroup", &config.triggerbot.weapons[currentWeapon].hitgroup, "All\0Head\0Chest\0Stomach\0Left arm\0Right arm\0Left leg\0Right leg\0");
        ImGui::PushItemWidth(220.0f);
        ImGui::SliderInt("", &config.triggerbot.weapons[currentWeapon].shotDelay, 0, 250, "Shot delay: %d ms");
        ImGui::End();
    }
}

void GUI::renderGlowWindow() noexcept
{
    if (window.glow) {
        ImGui::SetNextWindowSize({ 250.0f, 220.0f });
        ImGui::Begin("Glow", &window.glow, windowFlags);
        static int currentCategory{ 0 };
        ImGui::PushItemWidth(110.0f);
        ImGui::PushID(0);
        ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0C4\0Planted C4\0Chickens\0");
        ImGui::PopID();
        static int currentItem{ 0 };
        if (currentCategory <= 3) {
            ImGui::SameLine();
            static int currentType{ 0 };
            ImGui::PushID(1);
            ImGui::Combo("", &currentType, "Visible\0Occluded\0");
            ImGui::PopID();
            currentItem = currentCategory * 2 + currentType;
        }
        else {
            currentItem = currentCategory + 4;
        }

        ImGui::Checkbox("Enabled", &config.glow[currentItem].enabled);
        ImGui::Checkbox("Health based", &config.glow[currentItem].healthBased);
        bool openPopup = ImGui::ColorButton("Color", ImVec4{ config.glow[currentItem].color }, ImGuiColorEditFlags_NoTooltip);
        ImGui::SameLine(0.0f, 5.0f);
        ImGui::Text("Color");
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
        ImGui::PushItemWidth(220.0f);
        ImGui::PushID(4);
        ImGui::SliderFloat("", &config.glow[currentItem].thickness, 0.0f, 1.0f, "Thickness: %.2f");
        ImGui::PopID();
        ImGui::PushID(5);
        ImGui::SliderFloat("", &config.glow[currentItem].alpha, 0.0f, 1.0f, "Alpha: %.2f");
        ImGui::PopID();
        ImGui::PushID(6);
        ImGui::SliderInt("", &config.glow[currentItem].style, 0, 3, "Style: %d");
        ImGui::PopID();
        ImGui::End();
    }
}

void GUI::renderChamsWindow() noexcept
{
    if (window.chams) {
        ImGui::SetNextWindowSize({ 250.0f, 220.0f });
        ImGui::Begin("Chams", &window.chams, windowFlags);
        static int currentCategory{ 0 };
        ImGui::PushItemWidth(110.0f);
        ImGui::PushID(0);
        ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0Hands\0");
        ImGui::PopID();
        static int currentItem{ 0 };

        if (currentCategory <= 3) {
            ImGui::SameLine();
            static int currentType{ 0 };
            ImGui::PushID(1);
            ImGui::Combo("", &currentType, "Visible\0Occluded\0");
            ImGui::PopID();
            currentItem = currentCategory * 2 + currentType;
        }
        else {
            currentItem = currentCategory + 4;
        }

        ImGui::Checkbox("Enabled", &config.chams[currentItem].enabled);
        ImGui::Checkbox("Health based", &config.chams[currentItem].healthBased);
        ImGui::Combo("Material", &config.chams[currentItem].material, "Normal\0Flat\0Palm\0Platinum\0Glass\0Chrome\0Crystal\0Gold\0Plastic\0Blinking\0");
        ImGui::Checkbox("Wireframe", &config.chams[currentItem].wireframe);

        bool openPopup = ImGui::ColorButton("Color", ImVec4{ config.chams[currentItem].color }, ImGuiColorEditFlags_NoTooltip);
        ImGui::SameLine(0.0f, 5.0f);
        ImGui::Text("Color");
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
        ImGui::End();
    }
}

void GUI::renderEspWindow() noexcept
{
    if (window.esp) {
        ImGui::SetNextWindowSize({ 250.0f, 190.0f });
        ImGui::Begin("Esp", &window.esp, windowFlags);
        static int currentCategory{ 0 };
        ImGui::PushItemWidth(110.0f);
        ImGui::PushID(0);
        ImGui::Combo("", &currentCategory, "Allies\0Enemies\0");
        ImGui::PopID();
        ImGui::SameLine();
        static int currentType{ 0 };
        ImGui::PushID(1);
        ImGui::Combo("", &currentType, "Visible\0Occluded\0");
        ImGui::PopID();
        int currentItem = currentCategory * 2 + currentType;
        ImGui::Checkbox("Enabled", &config.esp[currentItem].enabled);
        checkboxedColorPicker("Snaplines", &config.esp[currentItem].snaplines, config.esp[currentItem].snaplinesColor);
        checkboxedColorPicker("Box", &config.esp[currentItem].box, config.esp[currentItem].boxColor);
        ImGui::End();
    }
}

void GUI::renderVisualsWindow() noexcept
{
    if (window.visuals) {
        ImGui::SetNextWindowSize({ 520.0f, 365.0f });
        ImGui::Begin("Visuals", &window.visuals, windowFlags);
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 210.0f);
        ImGui::Checkbox("Disable post-processing", &config.visuals.disablePostProcessing);
        ImGui::Checkbox("Inverse ragdoll gravity", &config.visuals.inverseRagdollGravity);
        ImGui::Checkbox("No fog", &config.visuals.noFog);
        ImGui::Checkbox("No 3d sky", &config.visuals.no3dSky);
        ImGui::Checkbox("No visual recoil", &config.visuals.noVisualRecoil);
        ImGui::Checkbox("No hands", &config.visuals.noHands);
        ImGui::Checkbox("No sleeves", &config.visuals.noSleeves);
        ImGui::Checkbox("No weapons", &config.visuals.noWeapons);
        ImGui::Checkbox("No smoke", &config.visuals.noSmoke);
        ImGui::Checkbox("No blur", &config.visuals.noBlur);
        ImGui::Checkbox("No scope overlay", &config.visuals.noScopeOverlay);
        ImGui::Checkbox("No grass", &config.visuals.noGrass);
        ImGui::Checkbox("Wireframe smoke", &config.visuals.wireframeSmoke);
        ImGui::NextColumn();
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
        ImGui::SliderInt("", &config.visuals.flashReduction, 0, 100, "Flash reduction: %d%%");
        ImGui::PopID();
        ImGui::PushID(3);
        ImGui::SliderFloat("", &config.visuals.brightness, 0.0f, 1.0f, "Brightness: %.2f");
        ImGui::PopID();
        ImGui::PopItemWidth();
        ImGui::Combo("Skybox", &config.visuals.skybox, "cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0");
        ImGui::ColorEdit3("World color", config.visuals.worldColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
        ImGui::End();
    }
}

void GUI::renderKnifeChangerWindow() noexcept
{
    if (window.knifeChanger) {
        ImGui::SetNextWindowSize({ 180.0f, 100.0f });
        ImGui::Begin("Knife changer", &window.knifeChanger, windowFlags);
        ImGui::PushItemWidth(130.0f);
        ImGui::Checkbox("Enabled", &config.knifeChanger.enabled);
        ImGui::Combo("Knife", &config.knifeChanger.knife, "Default\0Bayonet\0Bowie\0Butterfly\0Falchion\0Flip\0Gut\0Huntsman\0Karambit\0M9 Bayonet\0Shadow Daggers\0Navaja\0Stiletto\0Talon\0Ursus\0Golden\0Ghost\0");
        ImGui::End();
    }
}

void GUI::renderMiscWindow() noexcept
{
    if (window.misc) {
        ImGui::SetNextWindowSize({ 220.0f, 438.0f });
        ImGui::Begin("Misc", &window.misc, windowFlags);
        ImGui::Text("Menu key");
        ImGui::SameLine();
        hotkey(config.misc.menuKey);
        ImGui::Checkbox("Auto strafe", &config.misc.autoStrafe);
        ImGui::Checkbox("Bunny hop", &config.misc.bunnyHop);
        static char buffer[16];
        ImGui::PushItemWidth(120.0f);
        ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer));
        ImGui::SameLine();
        if (ImGui::Button("Set clantag")) {
            Misc::setClanTag(buffer);
            Misc::animateClanTag(buffer);
        }
        ImGui::Checkbox("Animated clan tag", &config.misc.animatedClanTag);
        ImGui::Checkbox("Fast duck", &config.misc.fastDuck);
        ImGui::Checkbox("Sniper crosshair", &config.misc.sniperCrosshair);
        ImGui::Checkbox("Recoil crosshair", &config.misc.recoilCrosshair);
        ImGui::Checkbox("Auto pistol", &config.misc.autoPistol);
        ImGui::Checkbox("Auto reload", &config.misc.autoReload);
        ImGui::Checkbox("Auto accept", &config.misc.autoAccept);
        ImGui::Checkbox("Radar hack", &config.misc.radarHack);
        ImGui::Checkbox("Reveal ranks", &config.misc.revealRanks);
        ImGui::Checkbox("Spectator list", &config.misc.spectatorList);
        ImGui::Checkbox("Watermark", &config.misc.watermark);
        if (ImGui::Button("Unhook"))
            hooks.restore();
        ImGui::End();
    }
}

void GUI::renderConfigWindow() noexcept
{
    if (window.config) {
        ImGui::SetNextWindowSize({ 290.0f, 190.0f });
        ImGui::Begin("Config", &window.config, windowFlags);

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
            config.reset();

        if (currentConfig != -1) {
            if (ImGui::Button("Load selected", { 100.0f, 25.0f }))
                config.load(currentConfig);
            if (ImGui::Button("Save selected", { 100.0f, 25.0f }))
                config.save(currentConfig);
            if (ImGui::Button("Delete selected", { 100.0f, 25.0f }))
                config.remove(currentConfig);
        }
        ImGui::End();
    }
}
