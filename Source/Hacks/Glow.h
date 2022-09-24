#pragma once

#include "../JsonForward.h"
#include "../Memory.h"

#define OSIRIS_GLOW() true

namespace Glow
{
    void render(const Interfaces& interfaces, const Memory& memory) noexcept;
    void clearCustomObjects(const Memory& memory) noexcept;
    void updateInput() noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;
}
