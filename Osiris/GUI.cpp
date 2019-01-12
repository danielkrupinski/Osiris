#include <sstream>
#include <Windows.h>

#include "imgui/imgui.h"
#include "GUI.h"
#include "Config.h"

void GUI::render() noexcept
{
    renderMenuBar();
    renderAimbotWindow();
    renderTriggerbotWindow();
    renderGlowWindow();
    renderChamsWindow();
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

void GUI::hotkey(int* key) const noexcept
{
    ImGui::Text((std::ostringstream{ } << "[ " << (*key ? std::to_string(*key) : "key") << " ]").str().c_str());
    if (ImGui::IsItemHovered()) {
        ImGuiIO& io = ImGui::GetIO();
        for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
            if (ImGui::IsKeyPressed(i) && i != VK_INSERT)
                *key = i != VK_ESCAPE ? i : 0;
    }
}

void GUI::renderMenuBar() noexcept
{
    if (ImGui::BeginMainMenuBar()) {
        ImGui::MenuItem("Aimbot", nullptr, &window.aimbot);
        ImGui::MenuItem("Triggerbot", nullptr, &window.triggerbot);
        ImGui::MenuItem("Glow", nullptr, &window.glow);
        ImGui::MenuItem("Chams", nullptr, &window.chams);
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
        ImGui::SetNextWindowSize(ImVec2(165.0f, 137.0f));
        ImGui::Begin("Aimbot", &window.aimbot, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Checkbox("Enabled", &config.aimbot.enabled);
        ImGui::Checkbox("Silent", &config.aimbot.silent);
        ImGui::PushItemWidth(150.0f);
        ImGui::SliderFloat("##Fov", &config.aimbot.fov, 0.0f, 180.0f, "Fov: %.2f");
        ImGui::SliderFloat("##Smooth", &config.aimbot.smooth, 1.0f, 100.0f, "Smooth: %.2f");
        ImGui::End();
    }
}

void GUI::renderTriggerbotWindow() noexcept
{
    if (window.triggerbot) {
        ImGui::SetNextWindowSize(ImVec2(180.0f, 115.0f));
        ImGui::Begin("Triggerbot", &window.triggerbot, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Checkbox("Enabled", &config.triggerbot.enabled);
        ImGui::Checkbox("On key", &config.triggerbot.onKey);
        ImGui::SameLine();
        hotkey(&config.triggerbot.key);
        ImGui::PushItemWidth(160.0f);
        ImGui::SliderInt("##Shot delay", &config.triggerbot.shotDelay, 0, 250, "Shot delay: %d ms");
        ImGui::End();
    }
}


void GUI::renderGlowWindow() noexcept
{
    if (window.glow) {
        ImGui::SetNextWindowSize(ImVec2(320.0f, 170.0f));
        ImGui::Begin("Glow", &window.glow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
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
        ImGui::SetNextWindowSize(ImVec2(330.0f, 160.0f));
        ImGui::Begin("Chams", &window.chams, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
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
        ImGui::End();
    }
}

void GUI::renderVisualsWindow() noexcept
{

}

void GUI::renderMiscWindow() noexcept
{
    if (window.misc) {
        ImGui::SetNextWindowSize(ImVec2(250.0f, 345.0f));
        ImGui::Begin("Misc", &window.misc, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PushItemWidth(160.0f);
        ImGui::Checkbox("Bunny hop", &config.misc.bunnyHop);
        char buf[16];
        std::strcpy(buf, config.misc.clanTag.c_str());
        ImGui::InputText("Clantag", buf, IM_ARRAYSIZE(buf));
        config.misc.clanTag = buf;
        ImGui::Checkbox("Disable post-processing", &config.misc.disablePostProcessing);
        ImGui::SliderInt("##Flash reduction", &config.misc.flashReduction, 0, 100, "Flash reduction: %d%%");
        ImGui::Checkbox("Inverse ragdoll gravity", &config.misc.inverseRagdollGravity);
        ImGui::Checkbox("Fast duck", &config.misc.fastDuck);
        ImGui::Checkbox("No smoke", &config.misc.noSmoke);
        if (!config.misc.noSmoke)
            ImGui::Checkbox("Wireframe smoke", &config.misc.wireframeSmoke);
        ImGui::Combo("Skybox", &config.misc.skybox, "Default\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam");
        ImGui::SliderInt("##Viewmodel FOV", &config.misc.viewmodelFov, -60, 60, "Viewmodel FOV: %d");
        ImGui::ColorEdit3("World color", config.misc.worldColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
        ImGui::End();
    }
}
