#pragma once

#include <string_view>

#include "../JsonForward.h"
#include "../Memory.h"

namespace Sound
{
    void modulateSound(const ClientInterfaces& clientInterfaces, const Memory& memory, std::string_view name, int entityIndex, float& volume) noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;
}
