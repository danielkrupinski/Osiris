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
        interfaces.surface->setTextFont(Surface::font);
        interfaces.surface->setTextColor(sinf(0.6f * memory.globalVars->realtime) * 127 + 128,
                                         sinf(0.6f * memory.globalVars->realtime + 2.0f) * 127 + 128,
                                         sinf(0.6f * memory.globalVars->realtime + 4.0f) * 127 + 128,
                                         255.0f);

        interfaces.surface->setTextPosition(5, 0);
        interfaces.surface->printText(L"Osiris");

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
        if (!localPlayer->isAlive() || localPlayer->getProperty<bool>("m_bInBombZone")) return;

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

static bool plantingBomb = false;
static bool defusingBomb = false;
static bool haveDefusers = false;
static float plantedTime = 0.0f;
static float defusingTime = 0.0f;

void Misc::bombEvents(GameEvent* event) noexcept
{
    if (!config.misc.bombTimer) return;
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

void Misc::drawTextTimer() noexcept
{
    if(!config.misc.bombTimer || !interfaces.engine->isInGame()) return;
    auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if (!localPlayer) return;
    static auto screen = interfaces.surface->getScreenSize();
    if (plantingBomb) {
            std::wstring plantingText(L"Bomb is planting!");
            static auto text = interfaces.surface->getTextSize(Surface::font, plantingText.c_str());
            interfaces.surface->setTextFont(Surface::font);
            interfaces.surface->setTextPosition(5, (screen.second / 2) - (text.second / 2));
            if (localPlayer->getProperty<int>("m_iTeamNum") == 3) {
                interfaces.surface->setTextColor(255, 0, 0, 255);
            }
            else {
                interfaces.surface->setTextColor(0, 255, 0, 255);
            }
            interfaces.surface->printText(plantingText.c_str());
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
                if (localPlayer->getProperty<int>("m_iTeamNum") == 3) {
                    interfaces.surface->setTextColor(255, 0, 0, 255);
                }
                else {
                    interfaces.surface->setTextColor(0, 255, 0, 255);
                }
                interfaces.surface->printText(timerText.c_str());
                if(defusingBomb){
                    float defusedTime = (defusingTime + (haveDefusers ? 5 : 10));
                    float defuse = defusedTime - memory.globalVars->currenttime;
                    std::wstring defuseTimer(L"Defuse timer: ");
                    defuseTimer += std::to_wstring(defuse);
                    interfaces.surface->setTextFont(Surface::font);
                    interfaces.surface->setTextPosition(5, screen.second / 2);
                    if (localPlayer->getProperty<int>("m_iTeamNum") == 3) {
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

void Misc::drawAimbotFov() noexcept
{
    auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if (!config.misc.drawAimbotFov || actualFov == 0.0f || !interfaces.engine->isInGame() || !localPlayer || !localPlayer->getActiveWeapon()) return;
    int weaponId = getWeaponIndex(localPlayer->getActiveWeapon()->getProperty<WeaponId>("m_iItemDefinitionIndex"));
    if (!config.aimbot[weaponId].enabled) weaponId = 0;
    if (!config.aimbot[weaponId].enabled) return;
    auto screenSize = interfaces.surface->getScreenSize();
    if (config.aimbot[weaponId].silent) interfaces.surface->setDrawColor(255, 10, 10, 255);
    else interfaces.surface->setDrawColor(10, 255, 10, 255);
    float radius = std::tan(degreesToRadians(config.aimbot[weaponId].fov) / 2.f) / std::tan(degreesToRadians(actualFov) / 2.f) * screenSize.first;
    interfaces.surface->drawOutlinedCircle(screenSize.first / 2, screenSize.second / 2, radius, 100);
}

void Misc::killMessage(GameEvent* event) noexcept
{
    auto localPlayer = interfaces.engine->getLocalPlayer();
    if (config.misc.killMessage
        && interfaces.engine->getPlayerForUserID(event->getInt("attacker")) == localPlayer
        && interfaces.engine->getPlayerForUserID(event->getInt("userid")) != localPlayer)
    {
        std::string msg("say ");
        if (!event->getBool("headshot"))
        {
            msg += config.messages.kill;
            interfaces.engine->clientCmdUnrestricted(msg.c_str());
        }
        else
        {
            msg += config.messages.headshot;
            interfaces.engine->clientCmdUnrestricted(msg.c_str());
        }
    }
}
