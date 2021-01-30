#pragma once

struct UserCmd;
struct Vector;

namespace AntiAim
{
    void run(UserCmd*, const Vector&, const Vector&, bool&) noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;
}
