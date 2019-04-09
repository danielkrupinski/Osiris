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
    if (config.triggerbot.enabled) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        if (localPlayer->getProperty<float>("m_flNextAttack") > memory.globalVars->serverTime())
            return;

        const auto activeWeapon = interfaces.entityList->getEntityFromHandle(localPlayer->getProperty<int>("m_hActiveWeapon"));
            if (!activeWeapon || !activeWeapon->getProperty<int>("m_iClip1")
                || !getWeaponIndex(activeWeapon->getProperty<WeaponId>("m_iItemDefinitionIndex")))
                return;

        static auto lastTime = memory.globalVars->realtime;
        auto now = memory.globalVars->realtime;

        if ((GetAsyncKeyState(config.triggerbot.key) || !config.triggerbot.onKey)
            && now - lastTime >= config.triggerbot.shotDelay / 1000.0f) {

            if (config.triggerbot.rayTracing) {
                constexpr auto degreesToRadians = [](float degrees) noexcept { return degrees * static_cast<float>(M_PI) / 180; };
                constexpr float maxRange{ 8192.0f };
                Vector viewAngles{ cos(degreesToRadians(cmd->viewangles.x)) * cos(degreesToRadians(cmd->viewangles.y)) * maxRange,
                                   cos(degreesToRadians(cmd->viewangles.x)) * sin(degreesToRadians(cmd->viewangles.y)) * maxRange,
                                  -sin(degreesToRadians(cmd->viewangles.x)) * maxRange };
                static Trace trace;
                interfaces.engineTrace->traceRay({ localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles }, 0x46004009, { localPlayer }, trace);
                if (trace.entity->getClientClass()->classId == ClassId::CSPlayer && trace.entity->isEnemy())
                    cmd->buttons |= UserCmd::IN_ATTACK;
            }
            else {
                auto inCrosshair = localPlayer->getProperty<int>("m_bHasDefuser", 92);
                if (inCrosshair > 0 && inCrosshair <= 64) {
                    auto target = interfaces.entityList->getEntity(inCrosshair);
                    if (target->isEnemy() && !target->getProperty<bool>("m_bGunGameImmunity")) {
                        cmd->buttons |= UserCmd::IN_ATTACK;
                    }
                }
            }

            lastTime = now;
        }
    }
}
