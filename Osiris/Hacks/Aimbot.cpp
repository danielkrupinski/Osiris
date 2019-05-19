#define _USE_MATH_DEFINES
#include <cmath>

#include "Aimbot.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"
#include "../SDK/WeaponId.h"
#include "../SDK/GlobalVars.h"

Vector Aimbot::calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept
{
    Vector delta = destination - source;
    constexpr auto radiansToDegrees = [](float radians) noexcept { return radians * 180 / static_cast<float>(M_PI); };
    Vector angles{ radiansToDegrees(atan2f(-delta.z, std::hypotf(delta.x, delta.y))) - viewAngles.x,
                   radiansToDegrees(atan2f(delta.y, delta.x)) - viewAngles.y };
    angles.normalize();
    return angles;
}

void Aimbot::run(UserCmd* cmd) noexcept
{
    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if (localPlayer->getProperty<float>("m_flNextAttack") > memory.globalVars->serverTime())
        return;

    const auto activeWeapon = interfaces.entityList->getEntityFromHandle(localPlayer->getProperty<int>("m_hActiveWeapon"));
    if (!activeWeapon || !activeWeapon->getProperty<int>("m_iClip1"))
        return;

    auto weaponIndex = getWeaponIndex(activeWeapon->getProperty<WeaponId>("m_iItemDefinitionIndex"));
    if (!weaponIndex)
        return;

    if (!config.aimbot[weaponIndex].enabled)
        weaponIndex = 0;

    if (!config.aimbot[weaponIndex].ignoreFlash && localPlayer->getProperty<float>("m_flFlashDuration"))
        return;

    if (config.aimbot[weaponIndex].onKey && !GetAsyncKeyState(config.aimbot[weaponIndex].key))
        return;

    if (config.aimbot[weaponIndex].enabled && (cmd->buttons & UserCmd::IN_ATTACK || config.aimbot[weaponIndex].autoShot)) {

        if (config.aimbot[weaponIndex].scopedOnly && activeWeapon->isSniperRifle() && !localPlayer->getProperty<bool>("m_bIsScoped"))
            return;

        auto bestFov = config.aimbot[weaponIndex].fov;
        Vector bestTarget{ };
        auto localPlayerEyePosition = localPlayer->getEyePosition();

        static auto weaponRecoilScale = interfaces.cvar->findVar("weapon_recoil_scale");
        auto aimPunch = localPlayer->getProperty<Vector>("m_aimPunchAngle") * weaponRecoilScale->getFloat();
        aimPunch.x *= config.aimbot[weaponIndex].recoilControlY;
        aimPunch.y *= config.aimbot[weaponIndex].recoilControlX;

        for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
            auto entity = interfaces.entityList->getEntity(i);
            if (!entity || entity == localPlayer || entity->isDormant() || !entity->isAlive()
                || !entity->isEnemy() && !config.aimbot[weaponIndex].friendlyFire || entity->getProperty<bool>("m_bGunGameImmunity"))
                continue;

            for (int j = 8; j >= 3; j--) {
                auto bonePosition = entity->getBonePosition(config.aimbot[weaponIndex].bone ? 9 - config.aimbot[weaponIndex].bone : j);
                if (config.aimbot[weaponIndex].visibleOnly && !entity->isVisible(bonePosition))
                    continue;

                auto angle = calculateRelativeAngle(localPlayerEyePosition, bonePosition, cmd->viewangles + (config.aimbot[weaponIndex].recoilbasedFov ? aimPunch : Vector{ }));
                auto fov = std::hypotf(angle.x, angle.y);
                if (fov < bestFov) {
                    bestFov = fov;
                    bestTarget = bonePosition;
                }
                if (config.aimbot[weaponIndex].bone)
                    break;
            }
        }

        if (bestTarget && (config.aimbot[weaponIndex].ignoreSmoke
            || !memory.lineGoesThroughSmoke(localPlayer->getEyePosition(), bestTarget, 1))) {
            static Vector lastAngles{ cmd->viewangles };
            static int lastCommand{ };

            if (lastCommand == cmd->command_number - 1 && lastAngles)
                cmd->viewangles = lastAngles;

            auto angle = calculateRelativeAngle(localPlayer->getEyePosition(), bestTarget, cmd->viewangles + aimPunch);
            bool clamped{ false };

            if (config.aimbot[weaponIndex].autoShot
                && (fabs(angle.x) > config.aimbot[weaponIndex].maxAngleDelta
                || fabs(angle.y) > config.aimbot[weaponIndex].maxAngleDelta)) {
                    angle.x = std::clamp(angle.x, -config.aimbot[weaponIndex].maxAngleDelta, config.aimbot[weaponIndex].maxAngleDelta);
                    angle.y = std::clamp(angle.y, -config.aimbot[weaponIndex].maxAngleDelta, config.aimbot[weaponIndex].maxAngleDelta);
                    clamped = true;
            }
            
            angle /= config.aimbot[weaponIndex].smooth;
            cmd->viewangles += angle;
            if (!config.aimbot[weaponIndex].silent)
                interfaces.engine->setViewAngles(cmd->viewangles);

            if (config.aimbot[weaponIndex].autoShot && activeWeapon->getProperty<float>("m_flNextPrimaryAttack") <= memory.globalVars->serverTime() && !clamped)
                cmd->buttons |= UserCmd::IN_ATTACK;

            if (clamped || config.aimbot[weaponIndex].smooth > 1.0f) lastAngles = cmd->viewangles;
            else lastAngles = Vector{ };

            lastCommand = cmd->command_number;
        }
    }
}
