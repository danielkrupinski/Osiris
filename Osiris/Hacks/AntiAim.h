#pragma once

struct UserCmd;

namespace AntiAim {
    void run(UserCmd*, float, bool&) noexcept;
}
