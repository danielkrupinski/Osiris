#define _USE_MATH_DEFINES
#include <cmath>
#include <Windows.h>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/Entity.h"
#include "../SDK/WeaponId.h"
#include "Triggerbot.h"

void Triggerbot::run(UserCmd* cmd) noexcept
{
    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if (localPlayer->getProperty<float>("m_flNextAttack") > memory.globalVars->serverTime())
        return;

    const auto activeWeapon = interfaces.entityList->getEntityFromHandle(localPlayer->getProperty<int>("m_hActiveWeapon"));
    if (!activeWeapon || !activeWeapon->getProperty<int>("m_iClip1") || activeWeapon->getProperty<float>("m_flNextPrimaryAttack") > memory.globalVars->serverTime())
        return;

    auto weaponIndex = getWeaponIndex(activeWeapon->getProperty<WeaponId>("m_iItemDefinitionIndex"));
    if (!weaponIndex)
        return;

    if (!config.triggerbot.weapons[weaponIndex].enabled)
        weaponIndex = 0;

    static float lastTime{ 0.0f };
    const auto now = memory.globalVars->serverTime();

    if (lastTime > now)
        lastTime = 0.0f;

    if (config.triggerbot.weapons[weaponIndex].enabled) {
        if ((GetAsyncKeyState(config.triggerbot.weapons[weaponIndex].key) || !config.triggerbot.weapons[weaponIndex].onKey)
            && now - lastTime >= config.triggerbot.weapons[weaponIndex].shotDelay / 1000.0f) {

            constexpr auto degreesToRadians = [](float degrees) noexcept { return degrees * static_cast<float>(M_PI) / 180; };
            constexpr float maxRange{ 8192.0f };
            Vector viewAngles{ cos(degreesToRadians(cmd->viewangles.x)) * cos(degreesToRadians(cmd->viewangles.y)) * maxRange,
                               cos(degreesToRadians(cmd->viewangles.x)) * sin(degreesToRadians(cmd->viewangles.y)) * maxRange,
                              -sin(degreesToRadians(cmd->viewangles.x)) * maxRange };
            static Trace trace;
            interfaces.engineTrace->traceRay({ localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles }, 0x46004009, { localPlayer }, trace);
            if (trace.entity->getClientClass()->classId == ClassId::CSPlayer
                && (config.triggerbot.weapons[weaponIndex].friendlyFire
                    || trace.entity->isEnemy())
                && !trace.entity->getProperty<bool>("m_bGunGameImmunity")
                && (!config.triggerbot.weapons[weaponIndex].hitgroup
                    || trace.hitgroup == config.triggerbot.weapons[weaponIndex].hitgroup)
                && (config.triggerbot.weapons[weaponIndex].ignoreSmoke
                    || !memory.lineGoesThroughSmoke(localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles, 1))
                && (config.triggerbot.weapons[weaponIndex].ignoreFlash
                    || !localPlayer->getProperty<float>("m_flFlashDuration"))
                && (!config.triggerbot.weapons[weaponIndex].scopedOnly
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
