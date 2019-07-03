#define _USE_MATH_DEFINES
#include <cmath>
#include <Windows.h>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/ConVar.h"
#include "../SDK/Entity.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/WeaponId.h"
#include "Triggerbot.h"

void Triggerbot::run(UserCmd* cmd) noexcept
{
    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if (localPlayer->getProperty<float>("m_flNextAttack") > memory.globalVars->serverTime())
        return;

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon || !activeWeapon->getProperty<int>("m_iClip1") || activeWeapon->getProperty<float>("m_flNextPrimaryAttack") > memory.globalVars->serverTime())
        return;

    auto weaponIndex = getWeaponIndex(activeWeapon->getProperty<WeaponId>("m_iItemDefinitionIndex"));
    if (!weaponIndex)
        return;

    if (!config.triggerbot[weaponIndex].enabled)
        weaponIndex = 0;

    static float lastTime{ 0.0f };
    const auto now = memory.globalVars->realtime;

    if (config.triggerbot[weaponIndex].enabled) {
        if ((GetAsyncKeyState(config.triggerbot[weaponIndex].key) || !config.triggerbot[weaponIndex].onKey)
            && now - lastTime >= config.triggerbot[weaponIndex].shotDelay / 1000.0f) {

            constexpr auto degreesToRadians = [](float degrees) noexcept { return degrees * static_cast<float>(M_PI) / 180; };
            constexpr float maxRange{ 8192.0f };

            static auto weaponRecoilScale = interfaces.cvar->findVar("weapon_recoil_scale");
            auto aimPunch = localPlayer->getProperty<Vector>("m_aimPunchAngle") * weaponRecoilScale->getFloat();

            Vector viewAngles{ cos(degreesToRadians(cmd->viewangles.x + aimPunch.x)) * cos(degreesToRadians(cmd->viewangles.y + aimPunch.y)) * maxRange,
                               cos(degreesToRadians(cmd->viewangles.x + aimPunch.x)) * sin(degreesToRadians(cmd->viewangles.y + aimPunch.y)) * maxRange,
                              -sin(degreesToRadians(cmd->viewangles.x + aimPunch.x)) * maxRange };
            static Trace trace;
            interfaces.engineTrace->traceRay({ localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles }, 0x46004009, { localPlayer }, trace);
            if (trace.entity && trace.entity->getClientClass()->classId == ClassId::CSPlayer
                && (config.triggerbot[weaponIndex].friendlyFire
                    || trace.entity->isEnemy())
                && !trace.entity->getProperty<bool>("m_bGunGameImmunity")
                && (!config.triggerbot[weaponIndex].hitgroup
                    || trace.hitgroup == config.triggerbot[weaponIndex].hitgroup)
                && (config.triggerbot[weaponIndex].ignoreSmoke
                    || !memory.lineGoesThroughSmoke(localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles, 1))
                && (config.triggerbot[weaponIndex].ignoreFlash
                    || !localPlayer->getProperty<float>("m_flFlashDuration"))
                && (!config.triggerbot[weaponIndex].scopedOnly
                    || !activeWeapon->isSniperRifle()
                    || activeWeapon->isSniperRifle() && localPlayer->getProperty<bool>("m_bIsScoped"))) {
                cmd->buttons |= UserCmd::IN_ATTACK;
                lastTime = 0.0f;
            }
            else {
                lastTime = now;
            }
        }
    }
}
