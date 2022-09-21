#pragma once

#include <Config.h>

namespace StreamProofESP
{
    void render(Config& config) noexcept;
    void updateInput(Config& config) noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem(Config& config) noexcept;
    void drawGUI(Config& config, bool contentOnly) noexcept;
}
