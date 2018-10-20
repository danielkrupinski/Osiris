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

        if (ImGui::BeginMenu(xorstr_("Misc"))) {
            window.misc = true;
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(xorstr_("Config"))) {
            if (ImGui::MenuItem(xorstr_("Load")))
                config.load();
            if (ImGui::MenuItem(xorstr_("Save")))
                config.save();
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    const ImGuiIO& io = ImGui::GetIO();

    if (window.glow) {
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 8.0f, io.DisplaySize.y / 3.0f));
        ImGui::Begin(xorstr_("Glow"), &window.glow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PushItemWidth(io.DisplaySize.x / 16.0f);
        ImGui::Checkbox(xorstr_("Enabled"), &config.visuals.glow.enabled);
        ImGui::Checkbox(xorstr_("Enemies only"), &config.visuals.glow.enemiesOnly);
        ImGui::SliderFloat(xorstr_("Thickness"), &config.visuals.glow.thickness, 0.0f, 1.0f);
        ImGui::SliderFloat(xorstr_("Alpha"), &config.visuals.glow.alpha, 0.0f, 1.0f);
        ImGui::ColorEdit3(xorstr_("Allies color"), config.visuals.glow.alliesColor, ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit3(xorstr_("Enemies color"), config.visuals.glow.enemiesColor, ImGuiColorEditFlags_NoInputs);
        ImGui::End();
    }

    if (window.misc) {
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 8.0f, io.DisplaySize.y / 3.0f));
        ImGui::Begin(xorstr_("Misc"), &window.misc, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Checkbox(xorstr_("Bunny hop"), &config.misc.bunnyHop);
        ImGui::End();
    }
}
