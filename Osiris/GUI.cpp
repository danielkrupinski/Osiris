#include <sstream>
#include <Windows.h>

#include "imgui/imgui.h"
#include "GUI.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/Visuals.h"

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

void GUI::render() noexcept
{
    renderMenuBar();
    renderAimbotWindow();
    renderTriggerbotWindow();
    renderGlowWindow();
    renderChamsWindow();
    renderVisualsWindow();
    renderMiscWindow();
}

void GUI::checkBoxAndColorPicker(const std::string_view name, bool* enable, float* color) const noexcept
{
    ImGui::Checkbox(std::string{ "##" }.append(name).c_str(), enable);
    ImGui::SameLine(0.0f, 5.0f);
    bool openPopup = ImGui::ColorButton(std::string{ "##" }.append(name).append("ColorButton").c_str(), ImColor{ color[0], color[1], color[2] }, ImGuiColorEditFlags_NoTooltip);
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::Text(name.data());
    if (openPopup)
        ImGui::OpenPopup(std::string{ "##" }.append(name).append("Popup").c_str());
    if (ImGui::BeginPopup(std::string{ "##" }.append(name).append("Popup").c_str())) {
        ImGui::ColorPicker3(std::string{ "##" }.append(name).append("Picker").c_str(), color, ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_NoSidePreview);
        ImGui::EndPopup();
    }
}

void GUI::hotkey(int& key) const noexcept
{
    ImGui::Text((std::ostringstream{ } << "[ " << (key ? std::to_string(key) : "key") << " ]").str().c_str());
    if (ImGui::IsItemHovered()) {
        ImGuiIO& io = ImGui::GetIO();
        for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
            if (ImGui::IsKeyPressed(i) && i != VK_INSERT)
                key = i != VK_ESCAPE ? i : 0;
    }
}

void GUI::renderMenuBar() noexcept
{
    if (ImGui::BeginMainMenuBar()) {
        ImGui::MenuItem("Aimbot", nullptr, &window.aimbot);
        ImGui::MenuItem("Triggerbot", nullptr, &window.triggerbot);
        ImGui::MenuItem("Glow", nullptr, &window.glow);
        ImGui::MenuItem("Chams", nullptr, &window.chams);
        ImGui::MenuItem("Visuals", nullptr, &window.visuals);
        ImGui::MenuItem("Misc", nullptr, &window.misc);

        if (ImGui::BeginMenu("Config")) {
            if (ImGui::MenuItem("Load"))
                config.load();
            if (ImGui::MenuItem("Save"))
                config.save();
            if (ImGui::MenuItem("Reset"))
                config.reset();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void GUI::renderAimbotWindow() noexcept
{
    if (window.aimbot) {
        ImGui::SetNextWindowSize({ 260.0f, 300.0f });
        ImGui::Begin("Aimbot", &window.aimbot, windowFlags);
        static int currentCategory{ 0 };
        ImGui::PushItemWidth(110.0f);
        ImGui::Combo("##category", &currentCategory, "All\0Pistols\0Heavy\0SMG\0Rifles");
        ImGui::SameLine();
        static int currentWeapon{ 0 };

        switch (currentCategory) {
        case 0:
            currentWeapon = 0;
            ImGui::NewLine();
            break;
        case 1: {
            static int currentPistol{ 0 };
            ImGui::Combo("##weapon_pistol", &currentPistol, "Glock-18\0P2000\0USP-S\0Dual Berettas\0P250\0Tec-9\0Five-Seven\0CZ-75\0Desert Eagle\0Revolver");
            currentWeapon = currentPistol + 1;
            break;
        }
        case 2: {
            static int currentHeavy{ 0 };
            ImGui::Combo("##weapon_heavy", &currentHeavy, "Nova\0XM1014\0Sawed-off\0MAG-7\0M249\0Negev");
            currentWeapon = currentHeavy + 11;
            break;
        }
        case 3: {
            static int currentSmg{ 0 };
            ImGui::Combo("##weapon_smg", &currentSmg, "Mac-10\0MP9\0MP7\0MP5-SD\0UMP-45\0P90\0PP-Bizon");
            currentWeapon = currentSmg + 17;
            break;
        }
        case 4: {
            static int currentRifle{ 0 };
            ImGui::Combo("##weapon_rifle", &currentRifle, "Galil AR\0Famas\0AK-47\0M4A4\0M4A1-S\0SSG-08\0SG-553\0AUG\0AWP\0G3SG1\0SCAR-20");
            currentWeapon = currentRifle + 24;
            break;
        }
        }
        ImGui::Checkbox("Enabled", &config.aimbot.weapons[currentWeapon].enabled);
        ImGui::Checkbox("Silent", &config.aimbot.weapons[currentWeapon].silent);
        ImGui::PushItemWidth(150.0f);
        ImGui::SliderFloat("##Fov", &config.aimbot.weapons[currentWeapon].fov, 0.0f, 255.0f, "Fov: %.2f");
        ImGui::SliderFloat("##Smooth", &config.aimbot.weapons[currentWeapon].smooth, 1.0f, 100.0f, "Smooth: %.2f");
        ImGui::Combo("Bone", &config.aimbot.weapons[currentWeapon].bone, "Head\0Neck\0Sternum\0Chest\0Stomach\0Pelvis");
        ImGui::SliderFloat("##Recoil Control X", &config.aimbot.weapons[currentWeapon].recoilControlX, 0.0f, 1.0f, "Recoil Control X: %.2f");
        ImGui::SliderFloat("##Recoil Control Y", &config.aimbot.weapons[currentWeapon].recoilControlY, 0.0f, 1.0f, "Recoil Control Y: %.2f");
        ImGui::End();
    }
}

void GUI::renderTriggerbotWindow() noexcept
{
    if (window.triggerbot) {
        ImGui::SetNextWindowSize({ 180.0f, 115.0f });
        ImGui::Begin("Triggerbot", &window.triggerbot, windowFlags);
        ImGui::Checkbox("Enabled", &config.triggerbot.enabled);
        ImGui::Checkbox("On key", &config.triggerbot.onKey);
        ImGui::SameLine();
        hotkey(config.triggerbot.key);
        ImGui::PushItemWidth(160.0f);
        ImGui::SliderInt("##Shot delay", &config.triggerbot.shotDelay, 0, 250, "Shot delay: %d ms");
        ImGui::End();
    }
}


void GUI::renderGlowWindow() noexcept
{
    if (window.glow) {
        ImGui::SetNextWindowSize({ 320.0f, 170.0f });
        ImGui::Begin("Glow", &window.glow, windowFlags);
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 170.0f);
        ImGui::PushItemWidth(150.0f);
        ImGui::Checkbox("Enabled", &config.glow.enabled);
        ImGui::SliderFloat("##Thickness", &config.glow.thickness, 0.0f, 1.0f, "Thickness: %.2f");
        ImGui::SliderFloat("##Alpha", &config.glow.alpha, 0.0f, 1.0f, "Alpha: %.2f");
        ImGui::SliderInt("##Style", &config.glow.style, 0, 3, "Style: %d");
        checkBoxAndColorPicker("Allies", &config.glow.allies, config.glow.alliesColor);
        ImGui::NextColumn();
        checkBoxAndColorPicker("Weapons", &config.glow.weapons, config.glow.weaponsColor);
        checkBoxAndColorPicker("C4", &config.glow.C4, config.glow.C4Color);
        checkBoxAndColorPicker("Planted C4", &config.glow.plantedC4, config.glow.plantedC4Color);
        checkBoxAndColorPicker("Chickens", &config.glow.chickens, config.glow.chickensColor);
        checkBoxAndColorPicker("Enemies", &config.glow.enemies, config.glow.enemiesColor);
        ImGui::End();
    }
}

void GUI::renderChamsWindow() noexcept
{
    if (window.chams) {
        ImGui::SetNextWindowSize({ 330.0f, 190.0f });
        ImGui::Begin("Chams", &window.chams, windowFlags);
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 150.0f);
        ImGui::Checkbox("Enabled", &config.chams.enabled);
        ImGui::PushItemWidth(120.0f);
        ImGui::SliderFloat("##Alpha", &config.chams.alpha, 0.0f, 1.0f, "Alpha: %.2f");
        ImGui::Checkbox("Flat", &config.chams.flat);
        ImGui::Checkbox("Wireframe", &config.chams.wireframe);
        ImGui::NextColumn();
        checkBoxAndColorPicker("Visible allies", &config.chams.visibleAllies, config.chams.visibleAlliesColor);
        checkBoxAndColorPicker("Occluded allies", &config.chams.occludedAllies, config.chams.occludedAlliesColor);
        checkBoxAndColorPicker("Visible enemies", &config.chams.visibleEnemies, config.chams.visibleEnemiesColor);
        checkBoxAndColorPicker("Occluded enemies", &config.chams.occludedEnemies, config.chams.occludedEnemiesColor);
        checkBoxAndColorPicker("Weapons", &config.chams.weapons, config.chams.weaponsColor);
        checkBoxAndColorPicker("Hands", &config.chams.hands, config.chams.handsColor);
        ImGui::End();
    }
}

void GUI::renderVisualsWindow() noexcept
{
    if (window.visuals) {
        ImGui::SetNextWindowSize({ 520.0f, 235.0f });
        ImGui::Begin("Visuals", &window.visuals, windowFlags);
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 210.0f);
        ImGui::Checkbox("Disable post-processing", &config.visuals.disablePostProcessing);
        ImGui::Checkbox("Inverse ragdoll gravity", &config.visuals.inverseRagdollGravity);
        ImGui::Checkbox("No visual recoil", &config.visuals.noVisualRecoil);
        ImGui::Checkbox("No hands", &config.visuals.noHands);
        ImGui::Checkbox("No sleeves", &config.visuals.noSleeves);
        ImGui::Checkbox("No weapons", &config.visuals.noWeapons);
        ImGui::Checkbox("No smoke", &config.visuals.noSmoke);
        ImGui::Checkbox("Wireframe smoke", &config.visuals.wireframeSmoke);
        ImGui::NextColumn();
        ImGui::Checkbox("Thirdperson", &config.visuals.thirdperson);
        ImGui::SameLine();
        hotkey(config.visuals.thirdpersonKey);
        ImGui::PushItemWidth(290.0f);
        ImGui::SliderInt("##Thirdperson distance", &config.visuals.thirdpersonDistance, 0, 1000, "Thirdperson distance: %d");
        ImGui::SliderInt("##Viewmodel FOV", &config.visuals.viewmodelFov, -60, 60, "Viewmodel FOV: %d");
        ImGui::SliderInt("##Flash reduction", &config.visuals.flashReduction, 0, 100, "Flash reduction: %d%%");
        ImGui::PopItemWidth();
        ImGui::Combo("Skybox", &config.visuals.skybox, "Default\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam");
        ImGui::ColorEdit3("World color", config.visuals.worldColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
        ImGui::End();
    }
}

void GUI::renderMiscWindow() noexcept
{
    if (window.misc) {
        ImGui::SetNextWindowSize({ 220.0f, 220.0f });
        ImGui::Begin("Misc", &window.misc, windowFlags);
        ImGui::Checkbox("Bunny hop", &config.misc.bunnyHop);
        static std::vector<char> buffer(16);
        ImGui::PushItemWidth(120.0f);
        ImGui::InputText("##Clantag", buffer.data(), buffer.size());
        ImGui::SameLine();
        if (ImGui::Button("Set clantag")) {
            Misc::setClanTag(buffer.data());
            Misc::animateClanTag(buffer.data());
        }
        ImGui::Checkbox("Animated clan tag", &config.misc.animatedClanTag);
        ImGui::Checkbox("Fast duck", &config.misc.fastDuck);
        ImGui::Checkbox("Auto pistol", &config.misc.autoPistol);
        ImGui::Checkbox("Radar hack", &config.misc.radarHack);
        if (ImGui::Button("Reveal ranks"))
            Misc::revealRanks();
        ImGui::End();
    }
}
