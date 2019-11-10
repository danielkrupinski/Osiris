#pragma once
#include "../SDK/Vector.h"
struct UserCmd;
struct Vector;

namespace AntiAim {
    void run(UserCmd*, const Vector&, const Vector&, bool&) noexcept;
}
void VectorAngles(const Vector& forward, QAngle& angles);
void AngleVectors(const QAngle& angles, Vector& forward);
void CorrectMovement(Vector vOldAngles, UserCmd* pCmd, float fOldForward, float fOldSidemove);
int distance(Vector a, Vector b);
Vector CalcAngle(Vector src, Vector dst);
