#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "xorstr.hpp"

#include "GUI.h"
#include "Config.h"

void GUI::render()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem(xorstr_("Glow"))) {
            window.glow = true;
        }

        if (ImGui::MenuItem(xorstr_("Chams"))) {
            window.chams = true;
        }

        if (ImGui::MenuItem(xorstr_("Triggerbot"))) {
            window.triggerbot = true;
        }

        if (ImGui::MenuItem(xorstr_("Misc"))) {
            window.misc = true;
        }

        if (ImGui::BeginMenu(xorstr_("Config"))) {
            if (ImGui::MenuItem(xorstr_("Load")))
                config.load();
            if (ImGui::MenuItem(xorstr_("Save")))
                config.save();
            if (ImGui::MenuItem(xorstr_("Reset")))
                config.reset();
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    renderGlowWindow();
    renderChamsWindow();
    renderTriggerbotWindow();

    if (window.misc) {
        ImGui::SetNextWindowSize(ImVec2(250.0f, 240.0f));
        ImGui::Begin(xorstr_("Misc"), &window.misc, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PushItemWidth(120.0f);
        ImGui::Checkbox(xorstr_("Bunny hop"), &config.misc.bunnyHop);
        char buf[16];
        std::strcpy(buf, config.misc.clanTag.c_str());
        ImGui::InputText(xorstr_("Clantag"), buf, IM_ARRAYSIZE(buf));
        config.misc.clanTag = buf;
        ImGui::Checkbox(xorstr_("Disable post-processing"), &config.misc.disablePostProcessing);
        ImGui::SliderInt(xorstr_("Flash reduction"), &config.misc.flashReduction, 0, 255);
        ImGui::Checkbox(xorstr_("No smoke"), &config.misc.noSmoke);
        if (!config.misc.noSmoke)
            ImGui::Checkbox(xorstr_("Wireframe smoke"), &config.misc.wireframeSmoke);
        static const std::string skyboxes[]{ xorstr_("Default"), xorstr_("cs_baggage_skybox_"), xorstr_("cs_tibet"), xorstr_("embassy"), xorstr_("italy"), xorstr_("jungle"), xorstr_("nukeblank"), xorstr_("office"), xorstr_("sky_cs15_daylight01_hdr"), xorstr_("sky_cs15_daylight02_hdr"), xorstr_("sky_cs15_daylight03_hdr"), xorstr_("sky_cs15_daylight04_hdr"), xorstr_("sky_csgo_cloudy01"), xorstr_("sky_csgo_night_flat"), xorstr_("sky_csgo_night02"), xorstr_("sky_day02_05_hdr"), xorstr_("sky_day02_05"), xorstr_("sky_dust"), xorstr_("sky_l4d_rural02_ldr"), xorstr_("sky_venice"), xorstr_("vertigo_hdr"), xorstr_("vertigo"), xorstr_("vertigoblue_hdr"), xorstr_("vietnam") };
        constexpr std::string& currentSkybox = config.misc.skybox;

        if (ImGui::BeginCombo(xorstr_("Skybox"), currentSkybox.c_str())) {
            for (const auto& a : skyboxes) {
                bool isSelected = (currentSkybox == a);
                if (ImGui::Selectable(a.c_str(), isSelected))
                    currentSkybox = a;
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SliderInt(xorstr_("Viewmodel FOV"), &config.misc.viewmodelFov, 0, 130);
        ImGui::ColorEdit3(xorstr_("World color"), config.misc.worldColor, ImGuiColorEditFlags_NoInputs);
        ImGui::End();
    }
}

void GUI::renderGlowWindow()
{
    if (window.glow) {
        ImGui::SetNextWindowSize(ImVec2(300.0f, 260.0f));
        ImGui::Begin(xorstr_("Glow"), &window.glow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 170.0f);
        ImGui::PushItemWidth(80.0f);
        ImGui::Checkbox(xorstr_("Enabled"), &config.glow.enabled);
        ImGui::SliderFloat(xorstr_("Thickness"), &config.glow.thickness, 0.0f, 1.0f);
        ImGui::SliderFloat(xorstr_("Alpha"), &config.glow.alpha, 0.0f, 1.0f);
        ImGui::SliderInt(xorstr_("Style"), &config.glow.style, 0, 3);
        ImGui::Checkbox(xorstr_("Players"), &config.glow.players);
        ImGui::Checkbox(xorstr_("Enemies only"), &config.glow.enemiesOnly);
        ImGui::Checkbox(xorstr_("Weapons"), &config.glow.weapons);
        ImGui::Checkbox(xorstr_("C4"), &config.glow.C4);
        ImGui::Checkbox(xorstr_("Planted C4"), &config.glow.plantedC4);
        ImGui::Checkbox(xorstr_("Chickens"), &config.glow.chickens);
        ImGui::NextColumn();
        ImGui::ColorEdit3(xorstr_("Allies"), config.glow.alliesColor, ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit3(xorstr_("Enemies"), config.glow.enemiesColor, ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit3(xorstr_("Weapons"), config.glow.weaponsColor, ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit3(xorstr_("C4"), config.glow.C4Color, ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit3(xorstr_("Planted C4"), config.glow.plantedC4Color, ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit3(xorstr_("Chickens"), config.glow.chickensColor, ImGuiColorEditFlags_NoInputs);
        ImGui::End();
    }
}

void GUI::renderChamsWindow()
{
    if (window.chams) {
        ImGui::SetNextWindowSize(ImVec2(200.0f, 240.0f));
        ImGui::Begin(xorstr_("Chams"), &window.chams, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PushItemWidth(80.0f);
        ImGui::Checkbox(xorstr_("Enabled"), &config.chams.enabled);
        ImGui::SliderFloat(xorstr_("Alpha"), &config.chams.alpha, 0.0f, 1.0f);
        ImGui::Checkbox(xorstr_("Enemies only"), &config.chams.enemiesOnly);
        ImGui::Checkbox(xorstr_("Visible only"), &config.chams.visibleOnly);
        ImGui::ColorEdit3(xorstr_("Visible allies color"), config.chams.visibleAlliesColor, ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit3(xorstr_("Occluded allies color"), config.chams.occludedAlliesColor, ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit3(xorstr_("Visible enemies color"), config.chams.visibleEnemiesColor, ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit3(xorstr_("Occluded enemies color"), config.chams.occludedEnemiesColor, ImGuiColorEditFlags_NoInputs);
        ImGui::Checkbox(xorstr_("Wireframe"), &config.chams.wireframe);
        ImGui::End();
    }
}

void GUI::renderTriggerbotWindow()
{
    if (window.triggerbot) {
        ImGui::SetNextWindowSize(ImVec2(180.0f, 100.0f));
        ImGui::Begin(xorstr_("Triggerbot"), &window.triggerbot, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PushItemWidth(70.0f);
        ImGui::Checkbox(xorstr_("Enabled"), &config.triggerbot.enabled);
        ImGui::SliderInt(xorstr_("Shot delay"), &config.triggerbot.shotDelay, 0, 250, xorstr_("%d ms"));
        ImGui::End();
    }
}

void GUI::renderMiscWindow()
{

}
