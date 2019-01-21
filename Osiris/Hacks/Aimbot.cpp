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
    switch (config.aimbot.bone) {
    case 0:
        return 8;
    case 1:
        return 7;
    case 2:
        return 6;
    case 3:
        return 5;
    case 4:
        return 4;
    case 5:
        return 3;
    default:
        return 8;
    }
}

static QAngle calculateAngleBetween(const Vector& source, const Vector& destination) noexcept
{
    Vector delta = source - destination;
    delta /= config.aimbot.smooth;
    float hyp = delta.length2D();
    constexpr auto radiansToDegrees = [](float radians) noexcept { return radians * 180 / static_cast<float>(M_PI); };
    QAngle angles;
    angles.pitch = radiansToDegrees(atanf(delta.z / hyp));
    angles.yaw = radiansToDegrees(atanf(delta.y / delta.x));
    angles.roll = 0.0f;

    if (delta.x >= 0.0)
        angles.yaw += 180.0f;

    angles.normalize();
    return angles;
}

static float getFov(const QAngle& viewAngle, const QAngle& aimAngle) noexcept
{
    QAngle delta = aimAngle - viewAngle;
    delta.normalize();
    return sqrtf(powf(delta.pitch, 2.0f) + powf(delta.yaw, 2.0f));
}

static int findTarget(UserCmd* cmd) noexcept
{
    auto bestFov = config.aimbot.fov;
    auto bestTarget = 0;
    auto localPlayer = interfaces.entityList->getClientEntity(interfaces.engine->getLocalPlayer());

    for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
        auto entity = interfaces.entityList->getClientEntity(i);
        if (!entity || entity == localPlayer || entity->isDormant() || !entity->isAlive() || !entity->isEnemy() || entity->isImmune())
            continue;
        auto angle = calculateAngleBetween(localPlayer->getEyeOrigin(), entity->getBonePosition(getBoneId()));
        auto fov = getFov(cmd->viewangles, angle);
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
            auto entity = interfaces.entityList->getClientEntity(target);
            auto localPlayer = interfaces.entityList->getClientEntity(interfaces.engine->getLocalPlayer());
            auto angle = calculateAngleBetween(localPlayer->getEyeOrigin(), entity->getBonePosition(getBoneId()));
            cmd->viewangles = angle;
            if (!config.aimbot.silent)
                interfaces.engine->setViewAngles(angle);
        }
    }
}
