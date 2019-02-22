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

void Misc::disablePostProcessing() noexcept
{
    if (*memory.disablePostProcessing != config.visuals.disablePostProcessing)
        *memory.disablePostProcessing = config.visuals.disablePostProcessing;
}

void Misc::skybox() noexcept
{
    constexpr std::string_view skyboxes[]{ "Default", "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "sky_csgo_night02", "sky_day02_05_hdr", "sky_day02_05", "sky_dust", "sky_l4d_rural02_ldr", "sky_venice", "vertigo_hdr", "vertigo", "vertigoblue_hdr", "vietnam" };

    if (config.visuals.skybox)
        reinterpret_cast<void(__fastcall*)(const char*)>(memory.loadSky)(skyboxes[config.visuals.skybox].data());
}

void Misc::setClanTag(const std::string& clanTag) noexcept
{
    reinterpret_cast<void(__fastcall*)(const char*, const char*)>(memory.setClanTag)(clanTag.c_str(), clanTag.c_str());
}

void Misc::inverseRagdollGravity() noexcept
{
    static auto ragdollGravity = interfaces.cvar->findVar("cl_ragdoll_gravity");
    ragdollGravity->setValue(config.visuals.inverseRagdollGravity ? -600 : 600);
}

void Misc::removeCrouchCooldown(UserCmd* cmd) noexcept
{
    if (config.misc.fastDuck) {
        constexpr int IN_BULLRUSH{ 1 << 22 };
        cmd->buttons |= IN_BULLRUSH;
    }
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
                if (cmd->buttons & UserCmd::IN_ATTACK && activeWeapon->getProperty<float>("m_flPostponeFireReadyTime") <= memory.globalVars->currenttime)
                    cmd->buttons &= ~UserCmd::IN_ATTACK;
                else if (cmd->buttons & UserCmd::IN_ATTACK2 && activeWeapon->getProperty<float>("m_flNextPrimaryAttack") <= memory.globalVars->realtime)
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

void Misc::spectatorsList() noexcept
{
    if (config.misc.spectatorsList) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        interfaces.surface->setTextColor(51, 153, 255, 255);

        int width, height;
        interfaces.surface->getScreenSize(width, height);

        auto spectatorsCount{ 0 };

        for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
            auto entity = interfaces.entityList->getEntity(i);
            if (!entity || entity->isAlive())
                continue;

            PlayerInfo playerInfo;

            if (interfaces.engine->getPlayerInfo(i, playerInfo)) {
                auto target = interfaces.entityList->getEntityFromHandle(entity->getProperty<int>("m_hObserverTarget"));

                if (target == localPlayer) {
                    static wchar_t name[128]{ };
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
