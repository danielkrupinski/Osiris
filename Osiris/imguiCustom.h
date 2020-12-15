#pragma once

#include "Config.h"
#include "imgui/imgui.h"

struct ColorA;
struct ColorToggle;
struct ColorToggleRounding;
struct ColorToggleThickness;
struct ColorToggleThicknessRounding;

namespace ImGuiCustom
{
    void colorPopup(const char* name, std::array<float, 4>& color, bool* rainbow = nullptr, float* rainbowSpeed = nullptr, bool* enable = nullptr, float* thickness = nullptr, float* rounding = nullptr) noexcept;
    void colorPicker(const char* name, float color[3], bool* enable = nullptr, bool* rainbow = nullptr, float* rainbowSpeed = nullptr) noexcept;
    void colorPicker(const char* name, Config::ColorToggle& colorConfig) noexcept;
    void colorPicker(const char* name, ColorA& colorConfig, bool* enable = nullptr, float* thickness = nullptr) noexcept;
    void colorPicker(const char* name, ColorToggle& colorConfig) noexcept;
    void colorPicker(const char* name, ColorToggleRounding& colorConfig) noexcept;
    void colorPicker(const char* name, ColorToggleThickness& colorConfig) noexcept;
    void colorPicker(const char* name, ColorToggleThicknessRounding& colorConfig) noexcept;
    void arrowButtonDisabled(const char* id, ImGuiDir dir) noexcept;
}

namespace ImGui
{
    void progressBarFullWidth(float fraction, float height) noexcept;
    void textUnformattedCentered(const char* text) noexcept;
}
