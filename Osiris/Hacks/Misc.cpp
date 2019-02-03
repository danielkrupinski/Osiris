#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../Netvars.h"
#include "Misc.h"

void Misc::bunnyHop(UserCmd* cmd) noexcept
{
    constexpr int ON_GROUND{ 1 };
    constexpr int IN_JUMP{ 2 };
    if (config.misc.bunnyHop) {
        if (cmd->buttons & IN_JUMP) {
            auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
            if (*localPlayer->getFlags() & ON_GROUND)
                cmd->buttons |= IN_JUMP;
            else
                cmd->buttons &= ~IN_JUMP;
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
                interfaces.engine->getLocalPlayer())->getActiveWeaponHandle());
        if (activeWeapon && activeWeapon->isPistol()) {
            static bool wasInAttackLastTick{ false };
            if (activeWeapon->getItemDefinitionIndex() == WeaponId::Revolver && wasInAttackLastTick) {
                if (cmd->buttons & 1 && activeWeapon->getFireReadyTime() <= memory.globalVars->currenttime)
                    cmd->buttons &= ~1;
                else if (cmd->buttons & 1 << 11 && activeWeapon->getNextPrimaryAttack() <= memory.globalVars->realtime)
                    cmd->buttons &= ~(1 << 11);
            }
            else if (cmd->buttons & 1 && wasInAttackLastTick)
                cmd->buttons &= ~1;
            wasInAttackLastTick = cmd->buttons & (1 | 1 << 11);
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
