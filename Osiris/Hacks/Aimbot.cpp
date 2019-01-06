#define _USE_MATH_DEFINES
#include <cmath>

#include "Aimbot.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"

static QAngle calculateAngleBetween(const Vector& source, const Vector& destination) noexcept
{
    Vector delta = source - destination;
    delta /= config.aimbot.smooth;
    float hyp = delta.length();
    QAngle angles;
    angles.pitch = atanf(delta.z / hyp) * 180 / M_PI;
    angles.yaw = atanf(delta.y / delta.x) * 180 / M_PI;
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

    for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
        auto entity = interfaces.entityList->getClientEntity(i);
        if (!entity || entity->isDormant() || !entity->isAlive() || !entity->isEnemy())
            continue;
        auto angle = calculateAngleBetween((*memory.localPlayer)->getEyePosition(), entity->getBonePosition(8));
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
            auto angle = calculateAngleBetween((*memory.localPlayer)->getEyePosition(), entity->getBonePosition(8));
            cmd->viewangles = angle;
            if (!config.aimbot.silent)
                interfaces.engine->setViewAngles(angle);
        }
    }
}
