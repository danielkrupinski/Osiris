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
#include "../SDK/Surface.h"
#include "../SDK/ConVar.h"

namespace Misc {
    void inverseRagdollGravity() noexcept;
    void updateClanTag(bool = false) noexcept;
    void spectatorList() noexcept;
    void sniperCrosshair() noexcept;
    void recoilCrosshair() noexcept;
    void watermark() noexcept;
    void prepareRevolver(UserCmd*) noexcept;
    void fastPlant(UserCmd*) noexcept;
    void drawTimer() noexcept;

    static float plantedTime = 0.0f;
    static float defusingTime = 0.0f;
    static bool plantingBomb = false;
    static bool defusingBomb = false;
    static bool haveDefusers = false;
        
    constexpr void fixMovement(UserCmd* cmd, float yaw) noexcept
    {
        if (config.misc.fixMovement) {
            float oldYaw = yaw + (yaw < 0.0f ? 360.0f : 0.0f);
            float newYaw = cmd->viewangles.y + (cmd->viewangles.y < 0.0f ? 360.0f : 0.0f);
            float yawDelta = newYaw < oldYaw ? fabsf(newYaw - oldYaw) : 360.0f - fabsf(newYaw - oldYaw);
            yawDelta = 360.0f - yawDelta;

            const float forwardmove = cmd->forwardmove;
            const float sidemove = cmd->sidemove;
            cmd->forwardmove = std::clamp(cos(degreesToRadians(yawDelta)) * forwardmove + cos(degreesToRadians(yawDelta + 90.0f)) * sidemove, -450.0f, 450.0f);
            cmd->sidemove = std::clamp(sin(degreesToRadians(yawDelta)) * forwardmove + sin(degreesToRadians(yawDelta + 90.0f)) * sidemove, -450.0f, 450.0f);
        }
    }

    constexpr void antiAfkKick(UserCmd* cmd) noexcept
    {
        if (config.misc.antiAfkKick && cmd->command_number % 2)
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
            if (activeWeapon && activeWeapon->isPistol() && activeWeapon->getProperty<float>("m_flNextPrimaryAttack") > memory.globalVars->serverTime()) {
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
            if (activeWeapon && getWeaponIndex(activeWeapon->itemDefinitionIndex2()) && !activeWeapon->getProperty<int>("m_iClip1"))
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
            && !(localPlayer->getProperty<int>("m_fFlags") & 1)
            && localPlayer->getProperty<MoveType>("m_nRenderMode", 1) != MoveType::NOCLIP) {
            if (cmd->mousedx < -20)
                cmd->sidemove = -450.0f;
            else if (cmd->mousedx > 20)
                cmd->sidemove = 450.0f;
        }
    }

    constexpr void bunnyHop(UserCmd* cmd) noexcept
    {
        if (auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
            config.misc.bunnyHop
            && !(localPlayer->getProperty<int>("m_fFlags") & 1)
            && localPlayer->getProperty<MoveType>("m_nRenderMode", 1) != MoveType::LADDER) {
            cmd->buttons &= ~UserCmd::IN_JUMP;
        }
    }

    constexpr void removeCrouchCooldown(UserCmd* cmd) noexcept
    {
        if (config.misc.fastDuck)
            cmd->buttons |= UserCmd::IN_BULLRUSH;
    }

    constexpr void playHitSound(GameEvent* event) noexcept
    {
        constexpr const char* hitSounds[]{
            "play physics/metal/metal_solid_impact_bullet2",
            "play buttons/arena_switch_press_02",
            "play training/timer_bell",
            "play physics/glass/glass_impact_bullet1"
        };

        if (config.misc.hitSound
            && interfaces.engine->getPlayerForUserID(event->getInt("attacker")) == interfaces.engine->getLocalPlayer())
            interfaces.engine->clientCmdUnrestricted(hitSounds[config.misc.hitSound - 1]);
    }

    constexpr void killMessage(GameEvent* event) noexcept
    {
        auto localPlayer = interfaces.engine->getLocalPlayer();
        if (config.misc.killMessage
            && interfaces.engine->getPlayerForUserID(event->getInt("attacker")) == localPlayer
            && interfaces.engine->getPlayerForUserID(event->getInt("userid")) != localPlayer)
            interfaces.engine->clientCmdUnrestricted("say Gotcha!");
    }

    constexpr void drawFov() noexcept
    {
        if (config.misc.drawFOV && interfaces.engine->isInGame()) {
            auto local = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
            if (!local || !local->isAlive()) return;
            if (!local->getActiveWeapon()) return;
            int weaponId = getWeaponIndex(local->getActiveWeapon()->getProperty<WeaponId>("m_iItemDefinitionIndex"));
            if (!config.aimbot[weaponId].enabled) weaponId = 0;
            if (!config.aimbot[weaponId].enabled) return;

            auto [width, height] = interfaces.surface->getScreenSize();

            auto actualFov = std::atanf((static_cast<float>(width) / static_cast<float>(height)) * 0.75f * std::tanf(degreesToRadians(local->getProperty<bool>("m_bIsScoped") ? static_cast<float>(local->fovStart()) : (static_cast<float>(local->fovStart()) + config.visuals.fov)) / 2.f));
            
            if (config.aimbot[weaponId].silent) interfaces.surface->setDrawColor(255, 10, 10, 255);
            else interfaces.surface->setDrawColor(10, 255, 10, 255);
            auto radius = std::tan(degreesToRadians(config.aimbot[weaponId].fov) / 2.f) / std::tanf(actualFov) * width;
            interfaces.surface->drawOutlinedCircle(width / 2, height / 2, static_cast<int>(radius), 20);
        }
    }

    constexpr void bombEvents(GameEvent* event) noexcept
    {
        if (!config.misc.drawBombTimer) return;
        switch (fnv::hashRuntime(event->getName())) {
        case fnv::hash("bomb_beginplant"):
            plantingBomb = true;
            break;
        case fnv::hash("bomb_abortplant"):
            plantingBomb = false;
            break;
        case fnv::hash("bomb_planted"):
            plantedTime = memory.globalVars->currenttime;
            plantingBomb = false;
            break;
        case fnv::hash("bomb_begindefuse"):
            defusingBomb = true;
            defusingTime = memory.globalVars->currenttime;
            haveDefusers = event->getBool("haskit");
            break;
        case fnv::hash("bomb_abortdefuse"):
            defusingTime = 0.0f;
            defusingBomb = false;
            break;
        case fnv::hash("round_start"):
        case fnv::hash("bomb_exploded"):
        case fnv::hash("bomb_defused"):
            plantingBomb = false;
            defusingBomb = false;
            haveDefusers = false;
            plantedTime = 0.0f;
            defusingTime = 0.0f;
            break;
        }
    }

    static void drawTimer() noexcept
    {

        if (!interfaces.engine->isInGame()) return;
        auto local = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        if (!local) return;
        static auto screen = interfaces.surface->getScreenSize();
        if (plantingBomb) {
            static auto text = interfaces.surface->getTextSize(Surface::font, L"Bomb is planting!");
            interfaces.surface->setTextFont(Surface::font);
            interfaces.surface->setTextPosition(5, (screen.second / 2) - (text.second / 2));
            if (local->getProperty<int>("m_iTeamNum") == 3) {
                interfaces.surface->setTextColor(255, 0, 0, 255);
            }
            else {
                interfaces.surface->setTextColor(0, 255, 0, 255);
            }
            interfaces.surface->printText(L"Bomb is planting!");
        }
        if (plantedTime > 0.0f) {
            float blowTime = plantedTime + interfaces.cvar->findVar("mp_c4timer")->getInt();
            float timer = blowTime - memory.globalVars->currenttime;

            if (timer > 0.0000f) {
                std::wstring timerText(L"Bomb timer: ");
                timerText += std::to_wstring(timer);
                static auto textSize = interfaces.surface->getTextSize(Surface::font, timerText.c_str());
                interfaces.surface->setTextFont(Surface::font);
                interfaces.surface->setTextPosition(5, (screen.second / 2) - textSize.second);
                if (local->getProperty<int>("m_iTeamNum") == 3) {
                    interfaces.surface->setTextColor(255, 0, 0, 255);
                }
                else {
                    interfaces.surface->setTextColor(0, 255, 0, 255);
                }
                interfaces.surface->printText(timerText.c_str());
                if (defusingBomb) {
                    float defusedTime = (defusingTime + (haveDefusers ? 5 : 10));
                    float defuse = defusedTime - memory.globalVars->currenttime;
                    std::wstring defuseTimer(L"Defuse timer: ");
                    defuseTimer += std::to_wstring(defuse);
                    interfaces.surface->setTextFont(Surface::font);
                    interfaces.surface->setTextPosition(5, screen.second / 2);
                    if (local->getProperty<int>("m_iTeamNum") == 3) {
                        interfaces.surface->setTextColor(0, 255, 0, 255);
                    }
                    else {
                        interfaces.surface->setTextColor(255, 0, 0, 255);
                    }
                    interfaces.surface->printText(defuseTimer.c_str());
                }
            }
        }
    }

}
