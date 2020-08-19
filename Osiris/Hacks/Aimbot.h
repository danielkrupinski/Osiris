#pragma once

struct UserCmd;
struct Vector;

namespace Aimbot
{
    Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
    void run(UserCmd*) noexcept;

	struct Enemies
    {
        int id;
        int health;
        float distance;

        bool operator<(const Enemies& enemy) const noexcept
        {
            if (health != enemy.health)
                return health < enemy.health;
            return distance < enemy.distance;
        }
        Enemies(int id, int health, float distance) noexcept : id(id), health(health), distance(distance) { }
    };
}
