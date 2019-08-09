#include <sstream>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../Netvars.h"
#include "Misc.h"
#include "../SDK/ConVar.h"
#include "../SDK/Surface.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/NetworkChannel.h"

void Misc::inverseRagdollGravity() noexcept
{
    static auto ragdollGravity = interfaces.cvar->findVar("cl_ragdoll_gravity");
    ragdollGravity->setValue(config.visuals.inverseRagdollGravity ? -600 : 600);
}

void Misc::updateClanTag(bool tagChanged) noexcept
{
    if (config.misc.customClanTag) {
        static std::string clanTag;

        if (tagChanged) {
            clanTag = config.misc.clanTag;
            if (!isblank(clanTag.front()) && !isblank(clanTag.back()))
                clanTag.push_back(' ');
        }

        static auto lastTime{ 0.0f };
        if (memory.globalVars->realtime - lastTime < 0.6f) return;
        lastTime = memory.globalVars->realtime;

        if (config.misc.animatedClanTag && !clanTag.empty())
            std::rotate(std::begin(clanTag), std::next(std::begin(clanTag)), std::end(clanTag));

        memory.setClanTag(clanTag.c_str(), clanTag.c_str());
    }
}

void Misc::spectatorList() noexcept
{
    if (config.misc.spectatorList && interfaces.engine->isInGame()) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        if (!localPlayer->isAlive())
            return;

        interfaces.surface->setTextFont(Surface::font);
        interfaces.surface->setTextColor(51, 153, 255, 255);

        const auto [width, height] = interfaces.surface->getScreenSize();

        int textPositionY{ static_cast<int>(0.5f * height) };

        for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
            auto entity = interfaces.entityList->getEntity(i);
            if (!entity || entity->isAlive() || entity->isDormant())
                continue;

            static PlayerInfo playerInfo;

            if (interfaces.engine->getPlayerInfo(i, playerInfo) && entity->getObserverTarget() == localPlayer) {
                static wchar_t name[128];
                if (MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                    const auto [textWidth, textHeight] = interfaces.surface->getTextSize(Surface::font, name);
                    interfaces.surface->setTextPosition(width - textWidth - 5, textPositionY);
                    textPositionY -= textHeight;
                    interfaces.surface->printText(name);
                }
            }
        }
    }
}

void Misc::sniperCrosshair() noexcept
{
    static auto showSpread = interfaces.cvar->findVar("weapon_debug_spread_show");
    showSpread->setValue(config.misc.sniperCrosshair && !interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->isScoped() ? 3 : 0);
}

void Misc::recoilCrosshair() noexcept
{
    static auto recoilCrosshair = interfaces.cvar->findVar("cl_crosshair_recoil");
    recoilCrosshair->setValue(config.misc.recoilCrosshair ? 1 : 0);
}

void Misc::watermark() noexcept
{
    if (config.misc.watermark) {
        interfaces.surface->setTextFont(Surface::font);
        interfaces.surface->setTextColor(sinf(0.6f * memory.globalVars->realtime) * 127 + 128,
                                         sinf(0.6f * memory.globalVars->realtime + 2.0f) * 127 + 128,
                                         sinf(0.6f * memory.globalVars->realtime + 4.0f) * 127 + 128,
                                         255.0f);

        interfaces.surface->setTextPosition(5, 0);
        interfaces.surface->printText(L"YeetHooks");

        static auto frameRate = 1.0f;
        frameRate = 0.9f * frameRate + 0.1f * memory.globalVars->absoluteFrameTime;
        const auto [screenWidth, screenHeight] = interfaces.surface->getScreenSize();
        std::wstring fps{ L"FPS: " + std::to_wstring(static_cast<int>(1 / frameRate)) };
        const auto [fpsWidth, fpsHeight] = interfaces.surface->getTextSize(Surface::font, fps.c_str());
        interfaces.surface->setTextPosition(screenWidth - fpsWidth - 5, 0);
        interfaces.surface->printText(fps.c_str());

        float latency = 0.0f;
        if (auto networkChannel = interfaces.engine->getNetworkChannel(); networkChannel && networkChannel->getLatency(0) > 0.0f)
            latency = networkChannel->getLatency(0);

        std::wstring ping{ L"PING: " + std::to_wstring(static_cast<int>(latency * 1000)) + L" ms" };
        const auto pingWidth = interfaces.surface->getTextSize(Surface::font, ping.c_str()).first;
        interfaces.surface->setTextPosition(screenWidth - pingWidth - 5, fpsHeight);
        interfaces.surface->printText(ping.c_str());
    }
}

void Misc::prepareRevolver(UserCmd* cmd) noexcept
{
    constexpr auto timeToTicks = [](float time) {  return static_cast<int>(0.5f + time / memory.globalVars->intervalPerTick); };
    constexpr float revolverPrepareTime{ 0.234375f };

    static float readyTime;
    if (config.misc.prepareRevolver && (!config.misc.prepareRevolverKey || GetAsyncKeyState(config.misc.prepareRevolverKey))) {
        const auto activeWeapon = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getActiveWeapon();
        if (activeWeapon && activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver) {
            if (!readyTime) readyTime = memory.globalVars->serverTime() + revolverPrepareTime;
            auto ticksToReady = timeToTicks(readyTime - memory.globalVars->serverTime() - interfaces.engine->getNetworkChannel()->getLatency(0));
            if (ticksToReady > 0 && ticksToReady <= timeToTicks(revolverPrepareTime))
                cmd->buttons |= UserCmd::IN_ATTACK;
            else
                readyTime = 0.0f;
        }
    }
}

void Misc::fastPlant(UserCmd* cmd) noexcept
{
    if (config.misc.fastPlant) {
        static auto plantAnywhere = interfaces.cvar->findVar("mp_plant_c4_anywhere");

        if (plantAnywhere->getInt()) return;

        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        if (!localPlayer->isAlive() || localPlayer->inBombZone()) return;

        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (!activeWeapon || activeWeapon->getClientClass()->classId != ClassId::C4)
            return;

        cmd->buttons &= ~UserCmd::IN_ATTACK;

        constexpr float doorRange{ 200.0f };
        Vector viewAngles{ cos(degreesToRadians(cmd->viewangles.x)) * cos(degreesToRadians(cmd->viewangles.y)) * doorRange,
                           cos(degreesToRadians(cmd->viewangles.x)) * sin(degreesToRadians(cmd->viewangles.y)) * doorRange,
                          -sin(degreesToRadians(cmd->viewangles.x)) * doorRange };
        static Trace trace;
        interfaces.engineTrace->traceRay({ localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles }, 0x46004009, localPlayer, trace);

        if (!trace.entity || trace.entity->getClientClass()->classId != ClassId::PropDoorRotating)
            cmd->buttons &= ~UserCmd::IN_USE;
    }
}

void Misc::drawBombTimer() noexcept
{
	if (config.misc.bombTimer) {
		for (int i = interfaces.engine->getMaxClients(); i <= interfaces.entityList->getHighestEntityIndex(); i++) {
			Entity* entity = interfaces.entityList->getEntity(i);
			if (!entity || entity->isDormant() || entity->getClientClass()->classId != ClassId::PlantedC4 || !entity->c4Ticking())
				continue;

			static constexpr unsigned font{ 0xc1 };
			interfaces.surface->setTextFont(font);
			interfaces.surface->setTextColor(255.0f, 255.0f, 255.0f, 255.0f);
			auto drawPositionY{ interfaces.surface->getScreenSize().second / 8 };
			auto bombText{ (std::wstringstream{ } << L"Bomb on " << (!entity->c4BombSite() ? 'A' : 'B') << L" : " << std::setprecision(3) << (std::max)(entity->c4BlowTime() - memory.globalVars->currenttime, 0.0f) << L" s").str() };
			interfaces.surface->setTextPosition(interfaces.surface->getScreenSize().first / 2 - static_cast<int>((interfaces.surface->getTextSize(font, bombText.c_str())).first / 2), drawPositionY);
			drawPositionY += interfaces.surface->getTextSize(font, bombText.c_str()).second;
			interfaces.surface->printText(bombText.c_str());
			static auto progressBarX{ interfaces.surface->getScreenSize().first / 3 };
			static auto progressBarY{ drawPositionY + 5 };
			static auto progress{ (std::max)(entity->c4BlowTime() - memory.globalVars->currenttime, 0.0f) / 40.0f };
			static auto progressBarLength{ interfaces.surface->getScreenSize().first / 3 };
			static auto progressBarHeight{ 5 };
			interfaces.surface->setDrawColor(50, 50, 50, 255);
			interfaces.surface->drawFilledRect(progressBarX - 3, progressBarY - 3, static_cast<int>(progressBarX + progressBarLength) + 3, progressBarY + progressBarHeight + 3);
			interfaces.surface->setDrawColor(255, 140, 0, 255);
			interfaces.surface->drawFilledRect(progressBarX, progressBarY, static_cast<int>(progressBarX + progressBarLength * progress), progressBarY + progressBarHeight);
			progress = (std::max)(entity->c4BlowTime() - memory.globalVars->currenttime, 0.0f) / 40.0f;
			if (entity->c4Defuser() != -1) {
				interfaces.surface->setTextColor(255.0f, 255.0f, 255.0f, 255.0f);
				static PlayerInfo playerInfo;
				if (interfaces.engine->getPlayerInfo(interfaces.entityList->getEntityFromHandle(entity->c4Defuser())->index(), playerInfo)) {
					static wchar_t name[128];
					if (MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
						drawPositionY += interfaces.surface->getTextSize(font, L" ").second;
						interfaces.surface->setTextPosition(interfaces.surface->getScreenSize().first / 2 - static_cast<int>((interfaces.surface->getTextSize(font, (std::wstringstream{ } << name << L" is defusing: " << std::setprecision(4) << (std::max)(entity->c4DefuseCountDown() - memory.globalVars->currenttime, 0.0f) << L" s").str().c_str())).first / 2), drawPositionY);
						interfaces.surface->printText((std::wstringstream{ } << name << L" is defusing: " << std::setprecision(4) << (std::max)(entity->c4DefuseCountDown() - memory.globalVars->currenttime, 0.0f) << L" s").str().c_str());
						interfaces.surface->setTextColor(255.0f, 255.0f, 255.0f, 255.0f);
						drawPositionY += interfaces.surface->getTextSize(font, L" ").second;
						static auto progressBarX2{ interfaces.surface->getScreenSize().first / 3 };
						static auto progressBarY2{ drawPositionY + 5 };
						static auto progress2{ (std::max)(entity->c4DefuseCountDown() - memory.globalVars->currenttime, 0.0f) / (interfaces.entityList->getEntityFromHandle(entity->c4Defuser())->hasDefuser() ? 5.0f : 10.0f) };
						static auto progressBarLength2{ interfaces.surface->getScreenSize().first / 3 };
						static auto progressBarHeight2{ 5 };
						interfaces.surface->setDrawColor(50, 50, 50, 255);
						interfaces.surface->drawFilledRect(progressBarX2 - 3, progressBarY2 - 3, static_cast<int>(progressBarX2 + progressBarLength2) + 3, progressBarY2 + progressBarHeight2 + 3);
						interfaces.surface->setDrawColor(0, 255, 0, 255);
						interfaces.surface->drawFilledRect(progressBarX2, progressBarY2, progressBarX2 + static_cast<int>(progressBarLength2 * progress2), progressBarY2 + progressBarHeight2);
						progress2 = (std::max)(entity->c4DefuseCountDown() - memory.globalVars->currenttime, 0.0f) / (interfaces.entityList->getEntityFromHandle(entity->c4Defuser())->hasDefuser() ? 5.0f : 10.0f);
						drawPositionY += interfaces.surface->getTextSize(font, L" ").second;
						if ((std::max)(entity->c4BlowTime() - memory.globalVars->currenttime, 0.0f) > (std::max)(entity->c4DefuseCountDown() - memory.globalVars->currenttime, 0.0f)) {
							interfaces.surface->setTextPosition(interfaces.surface->getScreenSize().first / 2 - static_cast<int>(interfaces.surface->getTextSize(font, (std::wstringstream{ } << L"Can Defuse: TRUE").str().c_str()).first / 2), drawPositionY);
							interfaces.surface->printText((std::wstringstream{ } << L"Can Defuse: ").str().c_str());
							interfaces.surface->setTextColor(0.0f, 255.0f, 0.0f, 255.0f);
							interfaces.surface->printText((std::wstringstream{ } << L"TRUE").str().c_str());
						}
						else {
							interfaces.surface->setTextPosition(interfaces.surface->getScreenSize().first / 2 - static_cast<int>(interfaces.surface->getTextSize(font, (std::wstringstream{ } << L"Can Defuse: FALSE").str().c_str()).first / 2), drawPositionY);
							interfaces.surface->printText((std::wstringstream{ } << L"Can Defuse: ").str().c_str());
							interfaces.surface->setTextColor(255.0f, 0.0f, 0.0f, 255.0f);
							interfaces.surface->printText((std::wstringstream{ } << L"FALSE").str().c_str());
						}
					}
				}
			}
		}
	}
}
