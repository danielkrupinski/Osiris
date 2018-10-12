#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "GUI.h"
#include "Memory.h"
#include <string>
#include "Interfaces.h"

GUI::GUI()
{
    
}

void GUI::render()
{
    ImGui::SetNextWindowSize(ImVec2(650.0f, 500.0f));
    ImGui::Begin("Osiris", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

    static enum { AIMBOT, VISUALS, MISC } tab{ MISC };
 
    ImGui::PushItemWidth(165);
    if (ImGui::BeginChild("#tabs", ImVec2(0.0f, 460.0f), true)) {
        switch (tab) {
        case AIMBOT:
            if (interfaces.engineClient->IsInGame())
                ImGui::Text(std::to_string(memory.localPlayer->getHealth()).c_str());
            break;
        case VISUALS:
            ImGui::Text("Visuals tab welcomes!");
            break;
        case MISC:
            ImGui::Text("Misc tab welcomes!");
            break;
        }
    }
    
    ImGui::EndChild();
    ImGui::PopItemWidth();

    if (ImGui::Button("Aimbot", ImVec2(207.0f, 20.0f)))
        tab = AIMBOT;
    ImGui::SameLine();

    if (ImGui::Button("Visuals", ImVec2(207.0f, 20.0f)))
        tab = VISUALS;
    ImGui::SameLine();

    if (ImGui::Button("Misc", ImVec2(207.0f, 20.0f)))
        tab = MISC;

    ImGui::End();
}
