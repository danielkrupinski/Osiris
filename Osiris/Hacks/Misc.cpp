#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../Netvars.h"
#include "Misc.h"

void Misc::bunnyHop(UserCmd* cmd) noexcept
{
    if (config.misc.bunnyHop) {
        if (cmd->buttons & UserCmd::IN_JUMP) {
            auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
            if (!(localPlayer->getProperty<int>("m_fFlags") & 1))
                cmd->buttons &= ~UserCmd::IN_JUMP;
        }
    }
}

void Misc::inverseRagdollGravity() noexcept
{
    static auto ragdollGravity = interfaces.cvar->findVar("cl_ragdoll_gravity");
    ragdollGravity->setValue(config.visuals.inverseRagdollGravity ? -600 : 600);
}

void Misc::autoPistol(UserCmd* cmd) noexcept
{
    if (config.misc.autoPistol) {
        const auto activeWeapon = interfaces.entityList->getEntityFromHandle(
            interfaces.entityList->getEntity(
                interfaces.engine->getLocalPlayer())->getProperty<int>("m_hActiveWeapon"));
        if (activeWeapon && activeWeapon->isPistol()) {
            static bool wasInAttackLastTick{ false };
            if (activeWeapon->getProperty<WeaponId>("m_iItemDefinitionIndex") == WeaponId::Revolver && wasInAttackLastTick) {
                if (cmd->buttons & UserCmd::IN_ATTACK2 && activeWeapon->getProperty<float>("m_flNextPrimaryAttack") <= memory.globalVars->currenttime)
                    cmd->buttons &= ~UserCmd::IN_ATTACK2;
            }
            else if (cmd->buttons & UserCmd::IN_ATTACK && wasInAttackLastTick)
                cmd->buttons &= ~UserCmd::IN_ATTACK;
            wasInAttackLastTick = cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2);
        }
    }
}

void Misc::revealRanks() noexcept
{
    int param[3]{ };
    reinterpret_cast<void(__cdecl*)(int*)>(memory.revealRanks)(param);
}

void Misc::animateClanTag(bool update, const std::string_view tag) noexcept
{
    static float lastTime{ 0.0f };
    static std::string clanTag;

    if (update) {
        clanTag = tag;
        if (clanTag.front() != ' ' || clanTag.back() != ' ')
            clanTag.push_back(' ');
    }

    if (config.misc.animatedClanTag && clanTag.length() > 1
        && memory.globalVars->realtime - lastTime > 0.5f) {
        std::rotate(std::begin(clanTag), std::begin(clanTag) + 1, std::end(clanTag));
        setClanTag(clanTag.c_str());
        lastTime = memory.globalVars->realtime;
    }
}

void Misc::spectatorList() noexcept
{
    if (config.misc.spectatorList && interfaces.engine->isInGame()) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        if (!localPlayer->isAlive())
            return;

        interfaces.surface->setTextColor(51, 153, 255, 255);

        const auto [width, height] = interfaces.surface->getScreenSize();

        auto spectatorsCount{ 0 };

        for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
            auto entity = interfaces.entityList->getEntity(i);
            if (!entity || entity->isAlive() || entity->isDormant())
                continue;

            static PlayerInfo playerInfo;

            if (interfaces.engine->getPlayerInfo(i, playerInfo)) {
                auto target = interfaces.entityList->getEntityFromHandle(entity->getProperty<int>("m_hObserverTarget"));

                if (target == localPlayer) {
                    static wchar_t name[128];
                    if (MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                        interfaces.surface->setTextPosition(static_cast<int>(0.85 * width), static_cast<int>(0.7 * height - spectatorsCount * 20));
                        interfaces.surface->printText(name);
                        spectatorsCount++;
                    }
                }
            }
        }
    }
}
