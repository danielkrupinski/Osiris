#pragma once

#include "../Memory.h"
#include "../SDK/FrameStage.h"
#include "../SDK/UserCmd.h"

namespace Misc {
    void inverseRagdollGravity() noexcept;
    void animateClanTag(const char* = nullptr) noexcept;
    void spectatorList() noexcept;
    void sniperCrosshair() noexcept;
    void recoilCrosshair() noexcept;
    void watermark() noexcept;

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
            const auto activeWeapon = interfaces.entityList->getEntityFromHandle(
                interfaces.entityList->getEntity(
                    interfaces.engine->getLocalPlayer())->getProperty<int>("m_hActiveWeapon"));
            if (activeWeapon && activeWeapon->isPistol() && activeWeapon->getProperty<float>("m_flNextPrimaryAttack") > memory.globalVars->serverTime()) {
                if (activeWeapon->getProperty<WeaponId>("m_iItemDefinitionIndex") == WeaponId::Revolver)
                    cmd->buttons &= ~UserCmd::IN_ATTACK2;
                else
                    cmd->buttons &= ~UserCmd::IN_ATTACK;
            }
        }
    }

    constexpr void chokePackets(bool& sendPacket) noexcept
    {
        sendPacket = interfaces.engine->getNetworkChannel()->chokedPackets >= config.misc.chokedPackets;
    }

    constexpr void autoReload(UserCmd* cmd) noexcept
    {
        if (config.misc.autoReload) {
            const auto activeWeapon = interfaces.entityList->getEntityFromHandle(interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<int>("m_hActiveWeapon"));
            if (activeWeapon && getWeaponIndex(activeWeapon->getProperty<WeaponId>("m_iItemDefinitionIndex")) && !activeWeapon->getProperty<int>("m_iClip1"))
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
        if (config.misc.autoStrafe
            && !(interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<int>("m_fFlags") & 1)) {
            if (cmd->mousedx < 0)
                cmd->sidemove = -450.0f;
            else if (cmd->mousedx > 0)
                cmd->sidemove = 450.0f;
        }
    }

    constexpr void bunnyHop(UserCmd* cmd) noexcept
    {
        if (config.misc.bunnyHop
            && !(interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<int>("m_fFlags") & 1)) {
            cmd->buttons &= ~UserCmd::IN_JUMP;
        }
    }

    constexpr void removeCrouchCooldown(UserCmd* cmd) noexcept
    {
        if (config.misc.fastDuck)
            cmd->buttons |= UserCmd::IN_BULLRUSH;
    }

    constexpr void setClanTag(const char* clanTag) noexcept
    {
        memory.setClanTag(clanTag, clanTag);
    }
}
