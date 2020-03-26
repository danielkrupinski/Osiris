#pragma once

#include "../Memory.h"
#include "../Interfaces.h"
#include "../SDK/FrameStage.h"
#include "../SDK/UserCmd.h"
#include "../SDK/WeaponId.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/Entity.h"
#include "../SDK/Client.h"
#include "../SDK/GameEvent.h"
#include "../SDK/GlobalVars.h"

namespace Misc
{
    void edgejump(UserCmd* cmd) noexcept;
    void slowwalk(UserCmd* cmd) noexcept;
    void inverseRagdollGravity() noexcept;
    void updateClanTag(bool = false) noexcept;
    void spectatorList() noexcept;
    void sniperCrosshair() noexcept;
    void recoilCrosshair() noexcept;
    void watermark() noexcept;
    void prepareRevolver(UserCmd*) noexcept;
    void fastPlant(UserCmd*) noexcept;
    void drawBombTimer() noexcept;
    void stealNames() noexcept;
    void disablePanoramablur() noexcept;
    void quickReload(UserCmd*) noexcept;
    bool changeName(bool, const char*, float) noexcept;
    void bunnyHop(UserCmd*) noexcept;
    void fakeBan(bool = false) noexcept;
    void nadePredict() noexcept;
    void quickHealthshot(UserCmd*) noexcept;
    void fixTabletSignal() noexcept;
    void fakePrime() noexcept;
    void killMessage(GameEvent& event) noexcept;

    constexpr void fixMovement(UserCmd* cmd, float yaw) noexcept
    {
        if (config.misc.fixMovement) {
            float oldYaw = yaw + (yaw < 0.0f ? 360.0f : 0.0f);
            float newYaw = cmd->viewangles.y + (cmd->viewangles.y < 0.0f ? 360.0f : 0.0f);
            float yawDelta = newYaw < oldYaw ? fabsf(newYaw - oldYaw) : 360.0f - fabsf(newYaw - oldYaw);
            yawDelta = 360.0f - yawDelta;

            const float forwardmove = cmd->forwardmove;
            const float sidemove = cmd->sidemove;
            cmd->forwardmove = std::cos(degreesToRadians(yawDelta)) * forwardmove + std::cos(degreesToRadians(yawDelta + 90.0f)) * sidemove;
            cmd->sidemove = std::sin(degreesToRadians(yawDelta)) * forwardmove + std::sin(degreesToRadians(yawDelta + 90.0f)) * sidemove;
        }
    }

    constexpr void antiAfkKick(UserCmd* cmd) noexcept
    {
        if (config.misc.antiAfkKick && cmd->commandNumber % 2)
            cmd->buttons |= 1 << 26;
    }

    constexpr void fixAnimationLOD(FrameStage stage) noexcept
    {
        if (config.misc.fixAnimationLOD && stage == FrameStage::RENDER_START) {
            for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
                if (i == interfaces.engine->getLocalPlayer()) continue;
                Entity* entity = interfaces.entityList->getEntity(i);
                if (!entity || entity->isDormant() || !entity->isAlive()) continue;
                *reinterpret_cast<int*>(entity + 0xA28) = 0;
                *reinterpret_cast<int*>(entity + 0xA30) = memory.globalVars->framecount;
            }
        }
    }

    constexpr void autoPistol(UserCmd* cmd) noexcept
    {
        if (config.misc.autoPistol) {
            const auto activeWeapon = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getActiveWeapon();
            if (activeWeapon && activeWeapon->isPistol() && activeWeapon->nextPrimaryAttack() > memory.globalVars->serverTime()) {
                if (activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver)
                    cmd->buttons &= ~UserCmd::IN_ATTACK2;
                else
                    cmd->buttons &= ~UserCmd::IN_ATTACK;
            }
        }
    }

    constexpr void chokePackets(bool& sendPacket) noexcept
    {
        if (!config.misc.chokedPacketsKey || GetAsyncKeyState(config.misc.chokedPacketsKey))
            sendPacket = interfaces.engine->getNetworkChannel()->chokedPackets >= config.misc.chokedPackets;
    }

    constexpr void autoReload(UserCmd* cmd) noexcept
    {
        if (config.misc.autoReload) {
            const auto activeWeapon = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getActiveWeapon();
            if (activeWeapon && getWeaponIndex(activeWeapon->itemDefinitionIndex2()) && !activeWeapon->clip())
                cmd->buttons &= ~(UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2);
        }
    }

    constexpr void revealRanks(UserCmd* cmd) noexcept
    {
        if (config.misc.revealRanks && cmd->buttons & UserCmd::IN_SCORE)
            interfaces.client->dispatchUserMessage(50, 0, 0, nullptr);
    }

    constexpr void autoStrafe(UserCmd* cmd) noexcept
    {
        if (auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
            config.misc.autoStrafe
            && !(localPlayer->flags() & 1)
            && localPlayer->moveType() != MoveType::NOCLIP) {
            if (cmd->mousedx < 0)
                cmd->sidemove = -450.0f;
            else if (cmd->mousedx > 0)
                cmd->sidemove = 450.0f;
        }
    }

    constexpr void removeCrouchCooldown(UserCmd* cmd) noexcept
    {
        if (config.misc.fastDuck)
            cmd->buttons |= UserCmd::IN_BULLRUSH;
    }

    constexpr void moonwalk(UserCmd* cmd) noexcept
    {
        if (config.misc.moonwalk && interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->moveType() != MoveType::LADDER)
            cmd->buttons ^= UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT;
    }

    constexpr void playHitSound(GameEvent& event) noexcept
    {
        if (!config.misc.hitSound)
            return;

        if (const auto localIdx = interfaces.engine->getLocalPlayer(); interfaces.engine->getPlayerForUserID(event.getInt("attacker")) != localIdx || interfaces.engine->getPlayerForUserID(event.getInt("userid")) == localIdx)
            return;

        constexpr std::array hitSounds{
            "play physics/metal/metal_solid_impact_bullet2",
            "play buttons/arena_switch_press_02",
            "play training/timer_bell",
            "play physics/glass/glass_impact_bullet1"
        };

        if (static_cast<std::size_t>(config.misc.hitSound - 1) < hitSounds.size())
            interfaces.engine->clientCmdUnrestricted(hitSounds[config.misc.hitSound - 1]);
    }
}
