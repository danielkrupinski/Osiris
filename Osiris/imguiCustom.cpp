#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "imguiCustom.h"

void ImGuiCustom::colorPicker(const char* name, float color[3], bool* enable, bool* rainbow, float* rainbowSpeed, float rainbowSpectrum[3]) noexcept
{
    ImGui::PushID(name);
    if (enable) {
        ImGui::Checkbox("##check", enable);
        ImGui::SameLine(0.0f, 5.0f);
    }
    bool openPopup = ImGui::ColorButton("##btn", ImColor{ color[0], color[1], color[2] }, ImGuiColorEditFlags_NoTooltip);
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::TextUnformatted(name);

    if (openPopup)
        ImGui::OpenPopup("##popup");

    if (ImGui::BeginPopup("##popup")) {
        ImGui::ColorPicker3("##picker", color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);

        if (rainbow && rainbowSpeed) {
            ImGui::SameLine();

            if (ImGui::BeginChild("##child", { 100.0f, 0.0f })) {
                ImGui::Checkbox("Rainbow", rainbow);
                ImGui::PushItemWidth(50.0f);
                ImGui::InputFloat("Speed", rainbowSpeed, 0.0f, 0.0f, "%.1f");

                if (rainbowSpectrum) {
                    ImGui::InputFloat("Red", &rainbowSpectrum[0], 0.0f, 0.0f, "%.1f");
                    ImGui::InputFloat("Green", &rainbowSpectrum[1], 0.0f, 0.0f, "%.1f");
                    ImGui::InputFloat("Blue", &rainbowSpectrum[2], 0.0f, 0.0f, "%.1f");
                }
                ImGui::PopItemWidth();
                ImGui::EndChild();
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}
