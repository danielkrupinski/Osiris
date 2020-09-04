#pragma once

struct UserCmd;
struct Vector;
class GameEvent;

namespace Aimbot
{
    Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
    void autoZeus(UserCmd* cmd) noexcept;
    void run(UserCmd*) noexcept;

    static float lastKillTime{ 0 };

    void handleKill(GameEvent& event) noexcept;
}
