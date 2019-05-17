#include <sstream>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../Netvars.h"
#include "Misc.h"
#include "../SDK/Surface.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/NetworkChannel.h"

void Misc::inverseRagdollGravity() noexcept
{
    static auto ragdollGravity = interfaces.cvar->findVar("cl_ragdoll_gravity");
    ragdollGravity->setValue(config.visuals.inverseRagdollGravity ? -600 : 600);
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
    showSpread->setValue(config.misc.sniperCrosshair && !interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<bool>("m_bIsScoped") ? 3 : 0);
}

void Misc::recoilCrosshair() noexcept
{
    static auto recoilCrosshair = interfaces.cvar->findVar("cl_crosshair_recoil");
    recoilCrosshair->setValue(config.misc.recoilCrosshair ? 1 : 0);
}

void Misc::watermark() noexcept
{
    if (config.misc.watermark) {
        constexpr unsigned font{ 0x1d }; // builtin font from vgui_spew_fonts
        interfaces.surface->setTextFont(font);
        interfaces.surface->setTextColor(sinf(0.6f * memory.globalVars->realtime) * 127 + 128,
                                         sinf(0.6f * memory.globalVars->realtime + 2.0f) * 127 + 128,
                                         sinf(0.6f * memory.globalVars->realtime + 4.0f) * 127 + 128,
                                         255.0f);

        interfaces.surface->setTextPosition(5, 0);
        interfaces.surface->printText(L"Osiris");

        static auto frameRate = 1.0f;
        frameRate = 0.9f * frameRate + 0.1f * memory.globalVars->absoluteFrameTime;
        const auto [screenWidth, screenHeight] = interfaces.surface->getScreenSize();
        std::wstring fps{ (std::wostringstream{ } << "FPS: " << static_cast<int>(1 / frameRate)).str().c_str() };
        const auto [fpsWidth, fpsHeight] = interfaces.surface->getTextSize(font, fps.c_str());
        interfaces.surface->setTextPosition(screenWidth - fpsWidth - 5, 0);
        interfaces.surface->printText(fps.c_str());

        float latency = 0.0f;
        if (auto networkChannel = interfaces.engine->getNetworkChannel(); networkChannel->getLatency(0) > 0.0f)
            latency = networkChannel->getLatency(0);

        std::wstring ping{ (std::wostringstream{ } << "PING: " << static_cast<int>(latency * 1000) << " ms").str().c_str() };
        const auto pingWidth = interfaces.surface->getTextSize(font, ping.c_str()).first;
        interfaces.surface->setTextPosition(screenWidth - pingWidth - 5, fpsHeight);
        interfaces.surface->printText(ping.c_str());
    }
}
