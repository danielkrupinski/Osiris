#pragma once

#include "Config.h"

namespace ImGuiCustom {
    void colorPicker(const char* name, float color[3], bool* enable = nullptr, bool* rainbow = nullptr, float* rainbowSpeed = nullptr) noexcept;
    void colorPicker(const char* name, Config::ColorToggle& colorConfig) noexcept;
	void colorPicker_bars(const char* name, float color[3], bool* enable, bool* rainbow, float* rainbowSpeed, bool* percentageBased) noexcept;
    void colorPicker_bars(const char* name, Config::ColorToggle& colorConfig) noexcept;
}
