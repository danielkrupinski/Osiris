#pragma once

struct UserCmd;
struct Vector;


namespace Ragebot
{
    //bool hitchance(Vector2D angle, float chance, Entity* entity);
    Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
    void run(UserCmd*) noexcept;

    bool can_shoot(const float time, UserCmd*);

    void Autostop(UserCmd* cmd) noexcept;

    void autoZeus(UserCmd* cmd) noexcept;




    struct Enemies {
        int id;
        int health;
        float distance;

        bool operator<(const Enemies& enemy) const noexcept {
            if (health != enemy.health)
                return health < enemy.health;
            return distance < enemy.distance;
        }
        Enemies(int id, int health, float distance) noexcept : id(id), health(health), distance(distance) { }
    };
}
