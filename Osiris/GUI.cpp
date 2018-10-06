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

    enum { AIMBOT, VISUALS, MISC } tab;

    ImGui::End();
}
