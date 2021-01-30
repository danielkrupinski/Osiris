#pragma once

#include "../ConfigStructs.h"

struct UserCmd;
struct Vector;

namespace AntiAim
{
    void run(UserCmd*, const Vector&, const Vector&, bool&) noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;
}
