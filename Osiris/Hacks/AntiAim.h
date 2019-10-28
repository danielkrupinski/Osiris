#pragma once

struct UserCmd;
struct Vector;

namespace AntiAim {
    void run(UserCmd*, const Vector&, const Vector&, bool&) noexcept;
}
