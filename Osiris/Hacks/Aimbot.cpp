#define _USE_MATH_DEFINES
#include <cmath>

#include "Aimbot.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"

static QAngle calculateAngleBetween(Vector source, Vector destination)
{
    Vector delta = source - destination;
    float hyp = delta.length();
    QAngle angles;
    angles.pitch = atanf(delta.z / hyp) * 180 / M_PI;
    angles.yaw = atanf(delta.y / delta.x) * 180 / M_PI; // 57.295779513082f);
    angles.roll = 0.0f;

    if (delta.x >= 0.0)
        angles.yaw += 180.0f;

    angles.normalize();
    return angles;
}

static float getFov(QAngle& viewAngle, QAngle& aimAngle)
{
    QAngle delta = aimAngle - viewAngle;
    delta.normalize();
    return sqrtf(powf(delta.pitch, 2.0f) + powf(delta.yaw, 2.0f));
}

static int findTarget(UserCmd* cmd)
{
    auto bestFov = 99999.9f;
    auto bestTarget = 0;

    for (int i = 1; i < interfaces.engine->getMaxClients(); i++) {
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
    if (config.aimbot.enabled && GetAsyncKeyState(VK_MENU)) {
        if (auto target = findTarget(cmd)) {
            auto entity = interfaces.entityList->getClientEntity(target);
            auto angle = calculateAngleBetween((*memory.localPlayer)->getEyePosition(), entity->getBonePosition(8));
            cmd->viewangles = angle;
            interfaces.engine->setViewAngles(angle);
        }
    }
}
