#pragma once

#include "Config.h"

namespace ImGuiCustom {
    void colorPicker(const char* name, float color[3], bool* enable = nullptr, bool* rainbow = nullptr, float* rainbowSpeed = nullptr) noexcept;
    void colorPicker(const char* name, Config::ColorToggle& colorConfig) noexcept;
	bool MultiCombo(const char* label, const char** displayName, bool* data, int dataSize);
}
