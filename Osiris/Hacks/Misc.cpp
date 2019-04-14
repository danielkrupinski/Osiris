#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../Netvars.h"
#include "Misc.h"

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
        if (activeWeapon && activeWeapon->isPistol() && activeWeapon->getProperty<float>("m_flNextPrimaryAttack") > memory.globalVars->serverTime()) {
            if (activeWeapon->getProperty<WeaponId>("m_iItemDefinitionIndex") == WeaponId::Revolver)
                cmd->buttons &= ~UserCmd::IN_ATTACK2;
            else
                cmd->buttons &= ~UserCmd::IN_ATTACK;
        }
    }
}

void Misc::revealRanks() noexcept
{
    //static int param[3]{ };
    //memory.revealRanks(param);
    interfaces.client->dispatchUserMessage(50, 0, 0, nullptr);
}

void Misc::animateClanTag(const char* tag) noexcept
{
    static float lastTime{ 0.0f };
    static std::string clanTag;

    if (tag) {
        clanTag = tag;
        if (!isblank(clanTag.front()) && !isblank(clanTag.back()))
            clanTag.push_back(' ');
    }

    if (config.misc.animatedClanTag && clanTag.length() > 1
        && memory.globalVars->realtime - lastTime > 0.5f) {
        std::rotate(std::begin(clanTag), std::next(std::begin(clanTag)), std::end(clanTag));
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

        static unsigned font = interfaces.surface->createFont();
        static bool init = interfaces.surface->setFontGlyphSet(font, "Tahoma", 12, 700, 0, 0, 128);
        interfaces.surface->setTextFont(font);
        interfaces.surface->setTextColor(51, 153, 255, 255);

        const auto [width, height] = interfaces.surface->getScreenSize();

        int spectatorsCount{ 0 };

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

void Misc::sniperCrosshair() noexcept
{
    static auto showSpread = interfaces.cvar->findVar("weapon_debug_spread_show");
    showSpread->setValue(config.misc.sniperCrosshair ? 3 : 0);
}

void Misc::recoilCrosshair() noexcept
{
    static auto recoilCrosshair = interfaces.cvar->findVar("cl_crosshair_recoil");
    recoilCrosshair->setValue(config.misc.recoilCrosshair ? 1 : 0);
}

void Misc::watermark() noexcept
{
    if (config.misc.watermark) {
        static unsigned font = interfaces.surface->createFont();
        static bool init = interfaces.surface->setFontGlyphSet(font, "Tahoma", 12, 700, 0, 0, 128);
        interfaces.surface->setTextFont(font);
        interfaces.surface->setTextColor(51, 153, 255, 255);
        interfaces.surface->setTextPosition(0, 0);
        interfaces.surface->printText(L"Osiris");
    }
}
