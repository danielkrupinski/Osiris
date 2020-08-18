#pragma once

struct UserCmd;
struct Vector;
class GameEvent;

namespace Aimbot
{
    static float lastKillTime{ 0 };

    Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
    void handleKill(GameEvent& event) noexcept;
    void run(UserCmd*) noexcept;
}
