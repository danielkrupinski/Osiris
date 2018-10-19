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
    static enum { NONE, GLOW } window{ NONE };

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu(xorstr_("Aimbot"))) {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(xorstr_("Visuals"))) {
            if (ImGui::MenuItem(xorstr_("Glow")))
                window = GLOW;
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(xorstr_("Misc"))) {
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

    ImGui::SetNextWindowSize(ImVec2(650.0f, 500.0f));
    ImGui::Begin(xorstr_("Osiris"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

    switch (window) {
        case GLOW:
            ImGui::Checkbox(xorstr_("Glow"), &config.visuals.glow.enabled);
            ImGui::SliderFloat(xorstr_("Glow thickness"), &config.visuals.glow.thickness, 0.0f, 1.0f);
            ImGui::SliderFloat(xorstr_("Glow alpha"), &config.visuals.glow.alpha, 0.0f, 1.0f);
            ImGui::ColorEdit3(xorstr_("Glow color"), config.visuals.glow.color, ImGuiColorEditFlags_NoInputs);
            break;
        /*
        case VISUALS:
            ImGui::Checkbox(xorstr_("Glow"), &config.visuals.glow.enabled);
            ImGui::SliderFloat(xorstr_("Glow thickness"), &config.visuals.glow.thickness, 0.0f, 1.0f);
            ImGui::SliderFloat(xorstr_("Glow alpha"), &config.visuals.glow.alpha, 0.0f, 1.0f);
            ImGui::ColorEdit3(xorstr_("Glow color"), config.visuals.glow.color, ImGuiColorEditFlags_NoInputs);
            break;
        case MISC:
            ImGui::Checkbox(xorstr_("Bunny hop"), &config.misc.bunnyHop);
            ImGui::Text(xorstr_("Config"));
            if (ImGui::Button(xorstr_("Load")))
                config.load();
            if (ImGui::Button(xorstr_("Save")))
                config.save();
            break;
        */
        }

    ImGui::End();
}
