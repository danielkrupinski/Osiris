#define _USE_MATH_DEFINES
#include <cmath>

#include "Aimbot.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"

static QAngle calculateAngleBetween(Vector source, Vector destination)
{
    QAngle angles;
    Vector delta = source - destination;

    if (delta.y == 0.0f && delta.x == 0.0f)
    {
        angles.pitch = (delta.z > 0.0f) ? 270.0f : 90.0f; // Pitch (up/down)
        angles.yaw = 0.0f;  //yaw left/right
    }
    else
    {
        angles.pitch = atan2(-delta.z, delta.length2D()) * -180 / M_PI;
        angles.yaw = atan2(delta.y, delta.x) * 180 / M_PI;

        if (angles.yaw > 90.0f) angles.yaw -= 180.0f;
        else if (angles.yaw < 90.0f) angles.yaw += 180.0f;
        else if (angles.yaw == 90) angles.yaw = 0.0f;
    }

    angles.roll = 0.0f;
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
    float bestFov = 999999.9f;
    int bestTarget = 0;

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
    auto entity = interfaces.entityList->getClientEntity(findTarget(cmd));
    auto angle = calculateAngleBetween((*memory.localPlayer)->getEyePosition(), entity->getBonePosition(8));
    cmd->viewangles = angle;
}
