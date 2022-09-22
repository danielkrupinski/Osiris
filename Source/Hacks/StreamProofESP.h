#pragma once

#include <Config.h>
#include "../Memory.h"

namespace StreamProofESP
{
    void render(const Memory& memory, Config& config) noexcept;
    void updateInput(Config& config) noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem(Config& config) noexcept;
    void drawGUI(Config& config, bool contentOnly) noexcept;
}
