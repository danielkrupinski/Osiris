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
    ImGui::SetNextWindowSize(ImVec2(650.0f, 500.0f));
    ImGui::Begin(xorstr_("Osiris"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

    static enum { AIMBOT, VISUALS, MISC } tab{ MISC };
 
    ImGui::PushItemWidth(165);
    if (ImGui::BeginChild(xorstr_("#tabs"), ImVec2(0.0f, 460.0f), true)) {
        switch (tab) {
        case AIMBOT:
            if (interfaces.engineClient->IsInGame())
                ImGui::Text(xorstr_("Player health: %d"), (*memory.localPlayer)->getHealth());
            break;
        case VISUALS:
            ImGui::Text(xorstr_("Visuals tab welcomes!"));
            break;
        case MISC:
            ImGui::Checkbox(xorstr_("Bunny hop"), &config.misc.bunnyHop);
            ImGui::Checkbox(xorstr_("Auto strafe"), &config.misc.autoStrafe);
            break;
        }
    }
    
    ImGui::EndChild();
    ImGui::PopItemWidth();

    if (ImGui::Button(xorstr_("Aimbot"), ImVec2(207.0f, 20.0f)))
        tab = AIMBOT;
    ImGui::SameLine();

    if (ImGui::Button(xorstr_("Visuals"), ImVec2(207.0f, 20.0f)))
        tab = VISUALS;
    ImGui::SameLine();

    if (ImGui::Button(xorstr_("Misc"), ImVec2(207.0f, 20.0f)))
        tab = MISC;

    ImGui::End();
}
