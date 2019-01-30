#define _USE_MATH_DEFINES
#include <cmath>

#include "Aimbot.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"

static constexpr int getCurrentWeaponConfigIndex(short itemDefinitionIndex) noexcept
{
    switch (itemDefinitionIndex) {
    case 1:
        return 9;
    case 2:
        return 4;
    case 3:
        return 7;
    case 4:
        return 1;
    case 7:
        return 26;
    case 8:
        return 31;
    case 9:
        return 32;
    case 10:
        return 25;
    case 11:
        return 33;
    case 13:
        return 24;
    case 14:
        return 15;
    case 16:
        return 27;
    case 17:
        return 17;
    case 19:
        return 22;
    case 24:
        return 21;
    case 25:
        return 12;
    case 26:
        return 23;
    case 27:
        return 14;
    case 28:
        return 16;
    case 29:
        return 13;
    case 30:
        return 6;
    case 32:
        return 2;
    case 33:
        return 19;
    case 34:
        return 18;
    case 35:
        return 11;
    case 36:
        return 5;
    case 38:
        return 34;
    case 39:
        return 30;
    case 40:
        return 29;
    case 60:
        return 28;
    case 61:
        return 3;
    case 63:
        return 8;
    case 64:
        return 10;
    default:
        return 0;
    }
}

static constexpr int getBoneId(int weaponIndex) noexcept
{
    return 8 - config.aimbot.weapons[weaponIndex].bone;
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

static int findTarget(UserCmd* cmd, int weaponIndex) noexcept
{
    auto bestFov = config.aimbot.weapons[weaponIndex].fov;
    auto bestTarget = 0;
    auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    auto localPlayerEyeOrigin = localPlayer->getEyeOrigin();

    for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
        auto entity = interfaces.entityList->getEntity(i);
        if (!entity || entity == localPlayer || entity->isDormant() || !entity->isAlive() || !entity->isEnemy() || entity->isImmune())
            continue;
        auto angle = calculateRelativeAngle(localPlayerEyeOrigin, entity->getBonePosition(getBoneId(weaponIndex)), cmd->viewangles);
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
    const auto activeWeapon = interfaces.entityList->getEntityFromHandle(interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getActiveWeaponHandle());
    if (!activeWeapon)
        return;

    auto weaponIndex = getCurrentWeaponConfigIndex(activeWeapon->getItemDefinitionIndex());

    if (!config.aimbot.weapons[weaponIndex].enabled)
        weaponIndex = 0;

    if (config.aimbot.weapons[weaponIndex].enabled && cmd->buttons & 1) {
        const auto activeWeapon = interfaces.entityList->getEntityFromHandle(
            interfaces.entityList->getEntity(
                interfaces.engine->getLocalPlayer())->getActiveWeaponHandle())->getItemDefinitionIndex();
        if (auto target = findTarget(cmd, weaponIndex)) {
            auto entity = interfaces.entityList->getEntity(target);
            auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
            auto angle = calculateRelativeAngle(localPlayer->getEyeOrigin(), entity->getBonePosition(getBoneId(weaponIndex)), cmd->viewangles);
            auto distance = std::hypotf(angle.x, angle.y);
            angle /= config.aimbot.weapons[weaponIndex].smooth;
            cmd->viewangles += angle;
            if (!config.aimbot.weapons[weaponIndex].silent || distance > 1.0f)
                interfaces.engine->setViewAngles(cmd->viewangles);
        }
    }
}
