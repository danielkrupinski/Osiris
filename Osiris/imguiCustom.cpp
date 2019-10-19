#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "imguiCustom.h"

void ImGuiCustom::colorPicker(const char* name, bool* enable, float color[3]) noexcept
{
    ImGui::PushID(name);
    ImGui::Checkbox("##check", enable);
    ImGui::SameLine(0.0f, 5.0f);
    bool openPopup = ImGui::ColorButton("##btn", ImColor{ color[0], color[1], color[2] }, ImGuiColorEditFlags_NoTooltip);
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::TextUnformatted(name);

    if (openPopup)
        ImGui::OpenPopup("##popup");

    if (ImGui::BeginPopup("##popup")) {
        ImGui::ColorPicker3("##picker", color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);
        ImGui::EndPopup();
    }
    ImGui::PopID();
}
