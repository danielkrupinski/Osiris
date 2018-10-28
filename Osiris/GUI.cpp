#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "xorstr.hpp"

#include "GUI.h"
#include "Config.h"

void GUI::render()
{
    static struct {
        bool glow{ false };
        bool misc{ false };
    } window;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu(xorstr_("Visuals"))) {
            if (ImGui::MenuItem(xorstr_("Glow")))
                window.glow = true;
            ImGui::EndMenu();
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

    const ImGuiIO& io = ImGui::GetIO();

    if (window.glow) {
        ImGui::SetNextWindowSize(ImVec2(170.67f, 360.0f));
        ImGui::Begin(xorstr_("Glow"), &window.glow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PushItemWidth(80);
        ImGui::Checkbox(xorstr_("Enabled"), &config.visuals.glow.enabled);
        ImGui::SliderFloat(xorstr_("Thickness"), &config.visuals.glow.thickness, 0.0f, 1.0f);
        ImGui::SliderFloat(xorstr_("Alpha"), &config.visuals.glow.alpha, 0.0f, 1.0f);
        ImGui::SliderInt(xorstr_("Style"), &config.visuals.glow.style, 0, 3);
        ImGui::Checkbox(xorstr_("Players"), &config.visuals.glow.players);
        ImGui::Checkbox(xorstr_("Enemies only"), &config.visuals.glow.enemiesOnly);
        ImGui::ColorEdit3(xorstr_("Allies color"), config.visuals.glow.alliesColor, ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit3(xorstr_("Enemies color"), config.visuals.glow.enemiesColor, ImGuiColorEditFlags_NoInputs);
        ImGui::Checkbox(xorstr_("Weapons"), &config.visuals.glow.weapons);
        ImGui::ColorEdit3(xorstr_("Weapons color"), config.visuals.glow.weaponsColor, ImGuiColorEditFlags_NoInputs);
        ImGui::Checkbox(xorstr_("Planted C4"), &config.visuals.glow.plantedC4);
        ImGui::ColorEdit3(xorstr_("Planted C4 color"), config.visuals.glow.plantedC4Color, ImGuiColorEditFlags_NoInputs);
        // ImGui::Checkbox(xorstr_("Defuse kits"), &config.visuals.glow.defuseKits);
        // ImGui::ColorEdit3(xorstr_("Defuse kits color"), config.visuals.glow.defuseKitsColor, ImGuiColorEditFlags_NoInputs);
        ImGui::Checkbox(xorstr_("Chickens"), &config.visuals.glow.chickens);
        ImGui::ColorEdit3(xorstr_("Chickens color"), config.visuals.glow.chickensColor, ImGuiColorEditFlags_NoInputs);
        ImGui::End();
    }

    if (window.misc) {
        ImGui::SetNextWindowSize(ImVec2(200.0f, 180.0f));
        ImGui::Begin(xorstr_("Misc"), &window.misc, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Checkbox(xorstr_("Bunny hop"), &config.misc.bunnyHop);
        ImGui::Checkbox(xorstr_("Disable post-processing"), &config.misc.disablePostProcessing);
        static const std::string skyboxes[]{ xorstr_("Default"), xorstr_("cs_baggage_skybox_"), xorstr_("cs_tibet"), xorstr_("embassy"), xorstr_("italy"), xorstr_("jungle"), xorstr_("nukeblank"), xorstr_("office"), xorstr_("sky_austria_dusk"), xorstr_("sky_cs15_daylight01_hdr"), xorstr_("sky_cs15_daylight02_hdr"), xorstr_("sky_cs15_daylight03_hdr"), xorstr_("sky_cs15_daylight04_hdr"), xorstr_("sky_csgo_cloudy01"), xorstr_("sky_csgo_night_flat"), xorstr_("sky_csgo_night02"), xorstr_("sky_day02_05_hdr"), xorstr_("sky_day02_05"), xorstr_("sky_dust"), xorstr_("sky_l4d_rural02_ldr"), xorstr_("sky_venice"), xorstr_("vertigo_hdr"), xorstr_("vertigo"), xorstr_("vertigoblue_hdr"), xorstr_("vietnam") };
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

        ImGui::SliderFloat(xorstr_("Viewmodel FOV"), &config.misc.viewmodelFov, 0.0f, 130.0f);
        ImGui::End();
    }
}
