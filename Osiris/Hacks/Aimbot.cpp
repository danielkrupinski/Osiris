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

static Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept
{
    Vector delta = destination - source;
    constexpr auto radiansToDegrees = [](float radians) noexcept { return radians * 180 / static_cast<float>(M_PI); };
    Vector angles{ radiansToDegrees(atan2f(-delta.z, std::hypotf(delta.x, delta.y))) - viewAngles.x,
                   radiansToDegrees(atan2f(delta.y, delta.x)) - viewAngles.y };
    angles.normalize();
    return angles;
}

static int findTarget(UserCmd* cmd) noexcept
{
    auto bestFov = config.aimbot.fov;
    auto bestTarget = 0;
    auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    auto localPlayerEyeOrigin = localPlayer->getEyeOrigin();

    for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
        auto entity = interfaces.entityList->getEntity(i);
        if (!entity || entity == localPlayer || entity->isDormant() || !entity->isAlive() || !entity->isEnemy() || entity->isImmune())
            continue;
        auto angle = calculateRelativeAngle(localPlayerEyeOrigin, entity->getBonePosition(getBoneId()), cmd->viewangles);
        auto fov = std::hypotf(angle.x, angle.y);
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
            auto angle = calculateRelativeAngle(localPlayer->getEyeOrigin(), entity->getBonePosition(getBoneId()), cmd->viewangles);
            auto distance = std::hypotf(angle.x, angle.y);
            angle /= config.aimbot.smooth;
            cmd->viewangles += angle;
            if (!config.aimbot.silent || distance > 1.0f)
                interfaces.engine->setViewAngles(cmd->viewangles);
        }
    }
}
