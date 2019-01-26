#define _USE_MATH_DEFINES
#include <cmath>

#include "Aimbot.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"

static constexpr int getBoneId() noexcept
{
    return 8 - config.aimbot.bone;
}

static Vector calculateAngleBetween(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept
{
    Vector delta = source - destination;
    float hyp = delta.length2D();
    constexpr auto radiansToDegrees = [](float radians) noexcept { return radians * 180 / static_cast<float>(M_PI); };
    Vector angles;
    angles.x = radiansToDegrees(atanf(delta.z / hyp));
    angles.y = radiansToDegrees(atanf(delta.y / delta.x));
    angles.z = 0.0f;

    if (delta.x >= 0.0)
        angles.y += 180.0f;

    angles -= viewAngles;

    angles.normalize();
    return angles;
}

static float getFov(const Vector& viewAngle, const Vector& aimAngle) noexcept
{
    Vector delta = aimAngle - viewAngle;
    delta.normalize();
    return delta.length2D();
}

static int findTarget(UserCmd* cmd) noexcept
{
    auto bestFov = config.aimbot.fov;
    auto bestTarget = 0;
    auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

    for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
        auto entity = interfaces.entityList->getEntity(i);
        if (!entity || entity == localPlayer || entity->isDormant() || !entity->isAlive() || !entity->isEnemy() || entity->isImmune())
            continue;
        auto angle = calculateAngleBetween(localPlayer->getEyeOrigin(), entity->getBonePosition(getBoneId()), cmd->viewangles);
        auto fov = angle.length2D();// getFov(cmd->viewangles, angle);
        if (fov < bestFov) {
            bestFov = fov;
            bestTarget = i;
        }
    }
    return bestTarget;
}

void Aimbot::run(UserCmd* cmd)
{
    if (config.aimbot.enabled && cmd->buttons & 1) {
        if (auto target = findTarget(cmd)) {
            auto entity = interfaces.entityList->getEntity(target);
            auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
            auto angle = calculateAngleBetween(localPlayer->getEyeOrigin(), entity->getBonePosition(getBoneId()), cmd->viewangles);
            //angle -= cmd->viewangles;
            angle /= config.aimbot.smooth;
            cmd->viewangles += angle;
            if (!config.aimbot.silent)
                interfaces.engine->setViewAngles(cmd->viewangles);
        }
    }
}
