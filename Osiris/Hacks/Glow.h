#pragma once

namespace Glow
{
    void render() noexcept;
    void clearCustomObjects() noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;
}
