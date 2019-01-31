#define _USE_MATH_DEFINES
#include <cmath>

#include "Aimbot.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"
#include "../SDK/WeaponId.h"

static constexpr int getCurrentWeaponConfigIndex(WeaponId itemDefinitionIndex) noexcept
{
    switch (itemDefinitionIndex) {
    default: return 0;

    case WeaponId::Glock: return 1;
    case WeaponId::Hkp2000: return 2;
    case WeaponId::Usp_s: return 3;
    case WeaponId::Elite: return 4;
    case WeaponId::P250: return 5;
    case WeaponId::Tec9: return 6;
    case WeaponId::Fiveseven: return 7;
    case WeaponId::Cz75a: return 8;
    case WeaponId::Deagle: return 9;
    case WeaponId::Revolver: return 10;

    case WeaponId::Nova: return 11;
    case WeaponId::Xm1014: return 12;
    case WeaponId::Sawedoff: return 13;
    case WeaponId::Mag7: return 14;
    case WeaponId::M249: return 15;
    case WeaponId::Negev: return 16;

    case WeaponId::Mac10: return 17;
    case WeaponId::Mp9: return 18;
    case WeaponId::Mp7: return 19;
    case WeaponId::Mp5sd: return 20;
    case WeaponId::Ump45: return 21;
    case WeaponId::P90: return 22;
    case WeaponId::Bizon: return 23;

    case WeaponId::GalilAr: return 24;
    case WeaponId::Famas: return 25;
    case WeaponId::Ak47: return 26;
    case WeaponId::M4A1: return 27;
    case WeaponId::M4a1_s: return 28;
    case WeaponId::Ssg08: return 29;
    case WeaponId::Sg553: return 30;
    case WeaponId::Aug: return 31;
    case WeaponId::Awp: return 32;
    case WeaponId::G3SG1: return 33;
    case WeaponId::Scar20: return 34;
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
    if (!weaponIndex)
        return;

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
