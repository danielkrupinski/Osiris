#pragma once

#include "Config.h"
#include "imgui/imgui.h"

namespace ImGuiCustom
{
    void colorPopup(const char* name, std::array<float, 4>& color, bool* rainbow = nullptr, float* rainbowSpeed = nullptr, bool* enable = nullptr, float* thickness = nullptr, float* rounding = nullptr) noexcept;
    void colorPicker(const char* name, float color[3], bool* enable = nullptr, bool* rainbow = nullptr, float* rainbowSpeed = nullptr) noexcept;
    void colorPicker(const char* name, Config::ColorToggle& colorConfig) noexcept;
    void arrowButtonDisabled(const char* id, ImGuiDir dir) noexcept;
}
