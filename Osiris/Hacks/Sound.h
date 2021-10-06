#pragma once

#include <string_view>

#include "../JsonForward.h"

#define OSIRIS_SOUND() true

namespace Sound
{
    void modulateSound(std::string_view name, int entityIndex, float& volume) noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;
}
