#pragma once

struct UserCmd;

namespace AntiAim {
    static auto toggle{ 1.0f };

    void run(UserCmd*, bool&) noexcept;
    void drawYaw() noexcept;
}
