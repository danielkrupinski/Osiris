#pragma once

struct UserCmd;
struct Vector;

namespace AntiAim {
    void run(UserCmd*, bool&) noexcept;
}
