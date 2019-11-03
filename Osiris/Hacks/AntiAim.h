#pragma once

struct UserCmd;
struct Vector;

namespace AntiAim {
    void run(UserCmd*, const Vector&, const Vector&, bool&) noexcept;
}

void CorrectMovement(Vector vOldAngles, UserCmd* pCmd, float fOldForward, float fOldSidemove);
