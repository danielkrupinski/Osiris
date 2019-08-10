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
    const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };
    if (localPlayer->nextAttack() > memory.globalVars->serverTime())
        return;

    const auto activeWeapon{ localPlayer->getActiveWeapon() };
    if (!activeWeapon || !activeWeapon->clip() || activeWeapon->nextPrimaryAttack() > memory.globalVars->serverTime())
        return;

    auto weaponIndex{ getWeaponIndex(activeWeapon->itemDefinitionIndex2()) };
    if (!weaponIndex)
        return;

    if (!config.triggerbot[weaponIndex].enabled)
        weaponIndex = 0;

    static auto lastTime{ 0.0f };
    
    if (config.triggerbot[weaponIndex].enabled) {
        if (const auto now{ memory.globalVars->realtime };
            (GetAsyncKeyState(config.triggerbot[weaponIndex].key) || !config.triggerbot[weaponIndex].onKey)
            && now - lastTime >= config.triggerbot[weaponIndex].shotDelay / 1000.0f) {

            static auto weaponRecoilScale{ interfaces.cvar->findVar("weapon_recoil_scale") };
            auto aimPunch{ localPlayer->aimPunchAngle() * weaponRecoilScale->getFloat() };

            constexpr auto maxRange{ 8192.0f };
            Vector viewAngles{ cos(degreesToRadians(cmd->viewangles.x + aimPunch.x)) * cos(degreesToRadians(cmd->viewangles.y + aimPunch.y)) * maxRange,
                               cos(degreesToRadians(cmd->viewangles.x + aimPunch.x)) * sin(degreesToRadians(cmd->viewangles.y + aimPunch.y)) * maxRange,
                              -sin(degreesToRadians(cmd->viewangles.x + aimPunch.x)) * maxRange };
            static Trace trace;
            interfaces.engineTrace->traceRay({ localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles }, 0x46004009, { localPlayer }, trace);
            if (trace.entity && trace.entity->getClientClass()->classId == ClassId::CSPlayer
                && (config.triggerbot[weaponIndex].friendlyFire
                    || trace.entity->isEnemy())
                && !trace.entity->gunGameImmunity()
                && (!config.triggerbot[weaponIndex].hitgroup
                    || trace.hitgroup == HitGroup{ config.triggerbot[weaponIndex].hitgroup })
                && (config.triggerbot[weaponIndex].ignoreSmoke
                    || !memory.lineGoesThroughSmoke(localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles, 1))
                && (config.triggerbot[weaponIndex].ignoreFlash
                    || !localPlayer->flashDuration())
                && (!config.triggerbot[weaponIndex].scopedOnly
                    || !activeWeapon->isSniperRifle()
                    || localPlayer->isScoped())) {
                cmd->buttons |= UserCmd::IN_ATTACK;
                lastTime = 0.0f;
            } else {
                lastTime = now;
            }
        }
    }
}
