#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "GUI.h"

GUI::GUI()
{

}

void GUI::render()
{
    ImGui::StyleColorsLight();
    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = 0.9f;
    ImGui::SetNextWindowSize(ImVec2(650, 500));
    ImGui::Begin("Osiris", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    static enum { AIMBOT, VISUALS, MISC } tab{ MISC };
 
    ImGui::PushItemWidth(165);
    if (ImGui::BeginChild("##tabchild", ImVec2(0, 440), true)) {
        switch (tab) {
        case AIMBOT:
            break;
        case VISUALS:
            break;
        case MISC:
            break;
        }
    }
    
    ImGui::EndChild();
    ImGui::PopItemWidth();

    if (ImGui::Button("Aimbot", ImVec2(152, 20)))
        tab = AIMBOT;
    ImGui::SameLine();

    if (ImGui::Button("Visuals", ImVec2(152, 20)))
        tab = VISUALS;
    ImGui::SameLine();

    if (ImGui::Button("Misc", ImVec2(152, 20)))
        tab = MISC;

    ImGui::End();
}
