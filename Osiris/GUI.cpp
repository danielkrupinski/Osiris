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
        bool none{ true };
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

    if (window.glow) {
        ImGui::SetNextWindowSize(ImVec2(650.0f, 500.0f));
        ImGui::Begin(xorstr_("Glow"), &window.glow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Checkbox(xorstr_("Glow"), &config.visuals.glow.enabled);
        ImGui::SliderFloat(xorstr_("Glow thickness"), &config.visuals.glow.thickness, 0.0f, 1.0f);
        ImGui::SliderFloat(xorstr_("Glow alpha"), &config.visuals.glow.alpha, 0.0f, 1.0f);
        ImGui::ColorEdit3(xorstr_("Glow color"), config.visuals.glow.color, ImGuiColorEditFlags_NoInputs);
        ImGui::End();
    }

    if (window.misc) {
        ImGui::SetNextWindowSize(ImVec2(650.0f, 500.0f));
        ImGui::Begin(xorstr_("Misc"), &window.misc, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Checkbox(xorstr_("Bunny hop"), &config.misc.bunnyHop);
        ImGui::End();
    }
}
