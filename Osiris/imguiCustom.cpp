#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "imguiCustom.h"

void ImGuiCustom::colorPopup(const char* name, std::array<float, 4>& color, bool* rainbow, float* rainbowSpeed, bool* enable, float* thickness, float* rounding) noexcept
{
    ImGui::PushID(name);
    if (enable) {
        ImGui::Checkbox("##check", enable);
        ImGui::SameLine(0.0f, 5.0f);
    }
    bool openPopup = ImGui::ColorButton("##btn", color, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaPreview);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
            color = *(std::array<float, 4>*)payload->Data;
        ImGui::EndDragDropTarget();
    }
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::TextUnformatted(name);

    if (openPopup)
        ImGui::OpenPopup("##popup");

    if (ImGui::BeginPopup("##popup")) {
        ImGui::ColorPicker4("##picker", color.data(), ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar);

        if (rainbow || rainbowSpeed || thickness || rounding) {
            ImGui::SameLine();
            if (ImGui::BeginChild("##child", { 150.0f, 0.0f })) {
                if (rainbow)
                    ImGui::Checkbox("Rainbow", rainbow);
                ImGui::PushItemWidth(85.0f);
                if (rainbowSpeed)
                    ImGui::InputFloat("Speed", rainbowSpeed, 0.01f, 0.15f, "%.2f");

                if (rounding || thickness)
                    ImGui::Separator();

                if (rounding) {
                    ImGui::InputFloat("Rounding", rounding, 0.1f, 0.0f, "%.1f");
                    *rounding = std::max(*rounding, 0.0f);
                }

                if (thickness) {
                    ImGui::InputFloat("Thickness", thickness, 0.1f, 0.0f, "%.1f");
                    *thickness = std::max(*thickness, 1.0f);
                }

                ImGui::PopItemWidth();
                ImGui::EndChild();
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void ImGuiCustom::colorPicker(const char* name, float color[3], bool* enable, bool* rainbow, float* rainbowSpeed) noexcept
{
    ImGui::PushID(name);
    if (enable) {
        ImGui::Checkbox("##check", enable);
        ImGui::SameLine(0.0f, 5.0f);
    }
    bool openPopup = ImGui::ColorButton("##btn", color, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoAlpha);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
            std::copy((float*)payload->Data, (float*)payload->Data + 3, color);
        ImGui::EndDragDropTarget();
    }
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
                ImGui::SetNextItemWidth(50.0f);
                ImGui::InputFloat("Speed", rainbowSpeed, 0.0f, 0.0f, "%.1f");
                ImGui::EndChild();
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void ImGuiCustom::colorPicker(const char* name, Config::ColorToggle& colorConfig) noexcept
{
    colorPicker(name, colorConfig.color.data(), &colorConfig.enabled, &colorConfig.rainbow, &colorConfig.rainbowSpeed);
}

void ImGuiCustom::colorPicker(const char* name, ColorA& colorConfig, bool* enable, float* thickness) noexcept
{
    colorPopup(name, colorConfig.color, &colorConfig.rainbow, &colorConfig.rainbowSpeed, enable, thickness);
}

void ImGuiCustom::colorPicker(const char* name, ColorToggle& colorConfig) noexcept
{
    colorPopup(name, colorConfig.color, &colorConfig.rainbow, &colorConfig.rainbowSpeed, &colorConfig.enabled);
}

void ImGuiCustom::colorPicker(const char* name, ColorToggleRounding& colorConfig) noexcept
{
    colorPopup(name, colorConfig.color, &colorConfig.rainbow, &colorConfig.rainbowSpeed, &colorConfig.enabled, nullptr, &colorConfig.rounding);
}

void ImGuiCustom::colorPicker(const char* name, ColorToggleThickness& colorConfig) noexcept
{
    colorPopup(name, colorConfig.color, &colorConfig.rainbow, &colorConfig.rainbowSpeed, &colorConfig.enabled, &colorConfig.thickness);
}

void ImGuiCustom::colorPicker(const char* name, ColorToggleThicknessRounding& colorConfig) noexcept
{
    colorPopup(name, colorConfig.color, &colorConfig.rainbow, &colorConfig.rainbowSpeed, &colorConfig.enabled, &colorConfig.thickness, &colorConfig.rounding);
}

void ImGuiCustom::arrowButtonDisabled(const char* id, ImGuiDir dir) noexcept
{
    float sz = ImGui::GetFrameHeight();
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    ImGui::ArrowButtonEx(id, dir, { sz, sz }, ImGuiButtonFlags_Disabled);
    ImGui::PopStyleVar();
}

bool ImGuiCustom::MultiCombo(const char* label, const char** displayName, bool* data, int dataSize)noexcept 
{

	ImGui::PushID(label);

	char previewText[1024] = { 0 };
	char buf[1024] = { 0 };
	char buf2[1024] = { 0 };
	int currentPreviewTextLen = 0;
	float multicomboWidth = 115.f;

	for (int i = 0; i < dataSize; i++) {

		if (data[i] == true) {

			if (currentPreviewTextLen == 0)
				sprintf(buf, "%s", displayName[i]);
			else
				sprintf(buf, ", %s", displayName[i]);

			strcpy(buf2, previewText);
			sprintf(buf2 + currentPreviewTextLen, buf);
			ImVec2 textSize = ImGui::CalcTextSize(buf2);

			if (textSize.x > multicomboWidth) {

				sprintf(previewText + currentPreviewTextLen, "...");
				currentPreviewTextLen += (int)strlen("...");
				break;
			}

			sprintf(previewText + currentPreviewTextLen, buf);
			currentPreviewTextLen += (int)strlen(buf);
		}
	}

	if (currentPreviewTextLen > 0)
		previewText[currentPreviewTextLen] = NULL;
	else
		sprintf(previewText, " -");

	bool isDataChanged = false;

	if (ImGui::BeginCombo(label, previewText)) {

		for (int i = 0; i < dataSize; i++) {

			sprintf(buf, displayName[i]);

			if (ImGui::Selectable(buf, data[i], ImGuiSelectableFlags_DontClosePopups)) {

				data[i] = !data[i];
				isDataChanged = true;
			}
		}

		ImGui::EndCombo();
	}

	ImGui::PopID();
	return isDataChanged;
}
