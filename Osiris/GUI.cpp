#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "xorstr.hpp"

#include "GUI.h"
#include "Memory.h"
#include <string>
#include "Interfaces.h"
#include "Config.h"

GUI::GUI()
{

}

void GUI::render()
{
    static struct {
        bool glow{ false };
        bool misc{ false };
    } window;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu(xorstr_("Aimbot"))) {
            ImGui::EndMenu();
        }

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
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 7.5f, io.DisplaySize.y / 3.0f));
        ImGui::Begin(xorstr_("Glow"), &window.glow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PushItemWidth(io.DisplaySize.x / 16.0f);
        ImGui::Checkbox(xorstr_("Enabled"), &config.visuals.glow.enabled);
        ImGui::Checkbox(xorstr_("Enemies only"), &config.visuals.glow.enemiesOnly);
        ImGui::SliderFloat(xorstr_("Thickness"), &config.visuals.glow.thickness, 0.0f, 1.0f);
        ImGui::SliderFloat(xorstr_("Alpha"), &config.visuals.glow.alpha, 0.0f, 1.0f);
        ImGui::SliderInt(xorstr_("Style"), &config.visuals.glow.style, 0, 3);
        ImGui::ColorEdit3(xorstr_("Allies color"), config.visuals.glow.alliesColor, ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit3(xorstr_("Enemies color"), config.visuals.glow.enemiesColor, ImGuiColorEditFlags_NoInputs);
        ImGui::End();
    }

    if (window.misc) {
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 6.0f, io.DisplaySize.y / 3.0f));
        ImGui::Begin(xorstr_("Misc"), &window.misc, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Checkbox(xorstr_("Bunny hop"), &config.misc.bunnyHop);
        ImGui::Checkbox(xorstr_("Disable post-processing"), &config.misc.disablePostProcessing);
        static const char* skyboxes[]{ "Default", "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "vietnam" };
        static int currentSkybox{ 0 };
        ImGui::Combo("Skybox", &currentSkybox, skyboxes, IM_ARRAYSIZE(skyboxes));
        config.misc.skybox = skyboxes[currentSkybox];
        ImGui::End();
    }
}
