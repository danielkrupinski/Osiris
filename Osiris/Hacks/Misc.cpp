#include <mutex>
#include <numeric>
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
#include "../SDK/WeaponData.h"
#include "EnginePrediction.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"
#include "../SDK/GameEvent.h"
#include "../SDK/FrameStage.h"
#include "../SDK/Client.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/WeaponSystem.h"
#include "../SDK/WeaponData.h"
#include "../GUI.h"



void Misc::edgejump(UserCmd* cmd) noexcept
{
    if (!config->misc.edgejump || !GetAsyncKeyState(config->misc.edgejumpkey))
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto mt = localPlayer->moveType(); mt == MoveType::LADDER || mt == MoveType::NOCLIP)
        return;

    if ((EnginePrediction::getFlags() & 1) && !(localPlayer->flags() & 1))
        cmd->buttons |= UserCmd::IN_JUMP;
}

void Misc::slowwalk(UserCmd* cmd) noexcept
{
    if (!config->misc.slowwalk || !GetAsyncKeyState(config->misc.slowwalkKey))
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon)
        return;

    const auto weaponData = activeWeapon->getWeaponData();
    if (!weaponData)
        return;

    const float maxSpeed = (localPlayer->isScoped() ? weaponData->maxSpeedAlt : weaponData->maxSpeed) / 3;

    if (cmd->forwardmove && cmd->sidemove) {
        const float maxSpeedRoot = maxSpeed * static_cast<float>(M_SQRT1_2);
        cmd->forwardmove = cmd->forwardmove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
        cmd->sidemove = cmd->sidemove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
    } else if (cmd->forwardmove) {
        cmd->forwardmove = cmd->forwardmove < 0.0f ? -maxSpeed : maxSpeed;
    } else if (cmd->sidemove) {
        cmd->sidemove = cmd->sidemove < 0.0f ? -maxSpeed : maxSpeed;
    }
}

void Misc::inverseRagdollGravity() noexcept
{
    static auto ragdollGravity = interfaces->cvar->findVar("cl_ragdoll_gravity");
    ragdollGravity->setValue(config->visuals.inverseRagdollGravity ? -600 : 600);
}

void Misc::updateClanTag(bool tagChanged) noexcept
{

    if (config->misc.clocktag) {
        const auto time = std::time(nullptr);
        const auto localTime = std::localtime(&time);
        const auto timeString = '[' + std::to_string(localTime->tm_hour) + ':' + std::to_string(localTime->tm_min) + ':' + std::to_string(localTime->tm_sec) + ']';
        memory->setClanTag(timeString.c_str(), timeString.c_str());
    }

    if (config->misc.customClanTag) {
        static std::string clanTag;

        if (tagChanged) {
            clanTag = config->misc.clanTag;
            if (!isblank(clanTag.front()) && !isblank(clanTag.back()))
                clanTag.push_back(' ');
        }

        static auto lastTime{ 0.0f };
        if (memory->globalVars->realtime - lastTime < 0.6f)
            return;
        lastTime = memory->globalVars->realtime;

        if (config->misc.animatedClanTag && !clanTag.empty())
            std::rotate(std::begin(clanTag), std::next(std::begin(clanTag)), std::end(clanTag));

        memory->setClanTag(clanTag.c_str(), clanTag.c_str());

    }
}

void Misc::spectatorList() noexcept
{
    if (!config->misc.spectatorList.enabled)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    interfaces->surface->setTextFont(Surface::font);

    if (config->misc.spectatorList.rainbow)
        interfaces->surface->setTextColor(rainbowColor(memory->globalVars->realtime, config->misc.spectatorList.rainbowSpeed));
    else
        interfaces->surface->setTextColor(config->misc.spectatorList.color);

    const auto [width, height] = interfaces->surface->getScreenSize();

    auto textPositionY = static_cast<int>(0.5f * height);

    for (int i = 1; i <= interfaces->engine->getMaxClients(); ++i) {
        const auto entity = interfaces->entityList->getEntity(i);
        if (!entity || entity->isDormant() || entity->isAlive() || entity->getObserverTarget() != localPlayer.get())
            continue;

        PlayerInfo playerInfo;

        if (!interfaces->engine->getPlayerInfo(i, playerInfo))
            continue;

        if (wchar_t name[128]; MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
            const auto [textWidth, textHeight] = interfaces->surface->getTextSize(Surface::font, name);
            interfaces->surface->setTextPosition(width - textWidth - 5, textPositionY);
            textPositionY -= textHeight;
            interfaces->surface->printText(name);
        }
    }
}

void Misc::sniperCrosshair() noexcept
{
    static auto showSpread = interfaces->cvar->findVar("weapon_debug_spread_show");
    showSpread->setValue(config->misc.sniperCrosshair && localPlayer && !localPlayer->isScoped() ? 3 : 0);
}

void Misc::recoilCrosshair() noexcept
{
    static auto recoilCrosshair = interfaces->cvar->findVar("cl_crosshair_recoil");
    recoilCrosshair->setValue(config->misc.recoilCrosshair ? 1 : 0);
}

void Misc::watermark() noexcept
{
    if (config->misc.watermark.enabled) {
        interfaces->surface->setTextFont(Surface::font);

        if (config->misc.watermark.rainbow)
            interfaces->surface->setTextColor(rainbowColor(memory->globalVars->realtime, config->misc.watermark.rainbowSpeed));
        else
            interfaces->surface->setTextColor(config->misc.watermark.color);

        interfaces->surface->setTextPosition(5, 0);
        interfaces->surface->printText(L"BRCheats.org");

        static auto frameRate = 1.0f;
        frameRate = 0.9f * frameRate + 0.1f * memory->globalVars->absoluteFrameTime;
        const auto [screenWidth, screenHeight] = interfaces->surface->getScreenSize();
        std::wstring fps{ L"FPS: " + std::to_wstring(static_cast<int>(1 / frameRate)) };
        const auto [fpsWidth, fpsHeight] = interfaces->surface->getTextSize(Surface::font, fps.c_str());
        interfaces->surface->setTextPosition(screenWidth - fpsWidth - 5, 0);
        interfaces->surface->printText(fps.c_str());

        float latency = 0.0f;
        if (auto networkChannel = interfaces->engine->getNetworkChannel(); networkChannel && networkChannel->getLatency(0) > 0.0f)
            latency = networkChannel->getLatency(0);

        std::wstring ping{ L"PING: " + std::to_wstring(static_cast<int>(latency * 1000)) + L" ms" };
        const auto pingWidth = interfaces->surface->getTextSize(Surface::font, ping.c_str()).first;
        interfaces->surface->setTextPosition(screenWidth - pingWidth - 5, fpsHeight);
        interfaces->surface->printText(ping.c_str());
    }
}

void Misc::prepareRevolver(UserCmd* cmd) noexcept
{
    constexpr auto timeToTicks = [](float time) {  return static_cast<int>(0.5f + time / memory->globalVars->intervalPerTick); };
    constexpr float revolverPrepareTime{ 0.234375f };

    static float readyTime;
    if (config->misc.prepareRevolver && localPlayer && (!config->misc.prepareRevolverKey || GetAsyncKeyState(config->misc.prepareRevolverKey))) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver) {
            if (!readyTime) readyTime = memory->globalVars->serverTime() + revolverPrepareTime;
            auto ticksToReady = timeToTicks(readyTime - memory->globalVars->serverTime() - interfaces->engine->getNetworkChannel()->getLatency(0));
            if (ticksToReady > 0 && ticksToReady <= timeToTicks(revolverPrepareTime))
                cmd->buttons |= UserCmd::IN_ATTACK;
            else
                readyTime = 0.0f;
        }
    }
}

void Misc::fastPlant(UserCmd* cmd) noexcept
{
    if (config->misc.fastPlant) {
        static auto plantAnywhere = interfaces->cvar->findVar("mp_plant_c4_anywhere");

        if (plantAnywhere->getInt())
            return;

        if (!localPlayer || !localPlayer->isAlive() || localPlayer->inBombZone())
            return;

        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (!activeWeapon || activeWeapon->getClientClass()->classId != ClassId::C4)
            return;

        cmd->buttons &= ~UserCmd::IN_ATTACK;

        constexpr float doorRange{ 200.0f };
        Vector viewAngles{ cos(degreesToRadians(cmd->viewangles.x)) * cos(degreesToRadians(cmd->viewangles.y)) * doorRange,
                           cos(degreesToRadians(cmd->viewangles.x)) * sin(degreesToRadians(cmd->viewangles.y)) * doorRange,
                          -sin(degreesToRadians(cmd->viewangles.x)) * doorRange };
        Trace trace;
        interfaces->engineTrace->traceRay({ localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles }, 0x46004009, localPlayer.get(), trace);

        if (!trace.entity || trace.entity->getClientClass()->classId != ClassId::PropDoorRotating)
            cmd->buttons &= ~UserCmd::IN_USE;
    }
}

void Misc::drawBombTimer() noexcept
{
    if (config->misc.bombTimer.enabled) {
        for (int i = interfaces->engine->getMaxClients(); i <= interfaces->entityList->getHighestEntityIndex(); i++) {
            Entity* entity = interfaces->entityList->getEntity(i);
            if (!entity || entity->isDormant() || entity->getClientClass()->classId != ClassId::PlantedC4 || !entity->c4Ticking())
                continue;

            constexpr unsigned font{ 0xc1 };
            interfaces->surface->setTextFont(font);
            interfaces->surface->setTextColor(255, 255, 255);
            auto drawPositionY{ interfaces->surface->getScreenSize().second / 8 };
            auto bombText{ (std::wstringstream{ } << L"Plantado no bomb " << (!entity->c4BombSite() ? 'A' : 'B') << L" : " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(entity->c4BlowTime() - memory->globalVars->currenttime, 0.0f) << L" s").str() };
            const auto bombTextX{ interfaces->surface->getScreenSize().first / 2 - static_cast<int>((interfaces->surface->getTextSize(font, bombText.c_str())).first / 2) };
            interfaces->surface->setTextPosition(bombTextX, drawPositionY);
            drawPositionY += interfaces->surface->getTextSize(font, bombText.c_str()).second;
            interfaces->surface->printText(bombText.c_str());

            const auto progressBarX{ interfaces->surface->getScreenSize().first / 3 };
            const auto progressBarLength{ interfaces->surface->getScreenSize().first / 3 };
            constexpr auto progressBarHeight{ 5 };

            interfaces->surface->setDrawColor(50, 50, 50);
            interfaces->surface->drawFilledRect(progressBarX - 3, drawPositionY + 2, progressBarX + progressBarLength + 3, drawPositionY + progressBarHeight + 8);
            if (config->misc.bombTimer.rainbow)
                interfaces->surface->setDrawColor(rainbowColor(memory->globalVars->realtime, config->misc.bombTimer.rainbowSpeed));
            else
                interfaces->surface->setDrawColor(config->misc.bombTimer.color);

            static auto c4Timer = interfaces->cvar->findVar("mp_c4timer");

            interfaces->surface->drawFilledRect(progressBarX, drawPositionY + 5, static_cast<int>(progressBarX + progressBarLength * std::clamp(entity->c4BlowTime() - memory->globalVars->currenttime, 0.0f, c4Timer->getFloat()) / c4Timer->getFloat()), drawPositionY + progressBarHeight + 5);

            if (entity->c4Defuser() != -1) {
                if (PlayerInfo playerInfo; interfaces->engine->getPlayerInfo(interfaces->entityList->getEntityFromHandle(entity->c4Defuser())->index(), playerInfo)) {
                    if (wchar_t name[128];  MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                        drawPositionY += interfaces->surface->getTextSize(font, L" ").second;
                        const auto defusingText{ (std::wstringstream{ } << name << L" esta defusando: " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(entity->c4DefuseCountDown() - memory->globalVars->currenttime, 0.0f) << L" s").str() };

                        interfaces->surface->setTextPosition((interfaces->surface->getScreenSize().first - interfaces->surface->getTextSize(font, defusingText.c_str()).first) / 2, drawPositionY);
                        interfaces->surface->printText(defusingText.c_str());
                        drawPositionY += interfaces->surface->getTextSize(font, L" ").second;

                        interfaces->surface->setDrawColor(50, 50, 50);
                        interfaces->surface->drawFilledRect(progressBarX - 3, drawPositionY + 2, progressBarX + progressBarLength + 3, drawPositionY + progressBarHeight + 8);
                        interfaces->surface->setDrawColor(0, 255, 0);
                        interfaces->surface->drawFilledRect(progressBarX, drawPositionY + 5, progressBarX + static_cast<int>(progressBarLength * (std::max)(entity->c4DefuseCountDown() - memory->globalVars->currenttime, 0.0f) / (interfaces->entityList->getEntityFromHandle(entity->c4Defuser())->hasDefuser() ? 5.0f : 10.0f)), drawPositionY + progressBarHeight + 5);

                        drawPositionY += interfaces->surface->getTextSize(font, L" ").second;
                        const wchar_t* canDefuseText;

                        if (entity->c4BlowTime() >= entity->c4DefuseCountDown()) {
                            canDefuseText = L"Consegue Defusar";
                            interfaces->surface->setTextColor(0, 255, 0);
                        } else {
                            canDefuseText = L"Nao consegue defusar";
                            interfaces->surface->setTextColor(255, 0, 0);
                        }

                        interfaces->surface->setTextPosition((interfaces->surface->getScreenSize().first - interfaces->surface->getTextSize(font, canDefuseText).first) / 2, drawPositionY);
                        interfaces->surface->printText(canDefuseText);
                    }
                }
            }
            break;
        }
    }
}

void Misc::stealNames() noexcept
{
    if (!config->misc.nameStealer)
        return;

    if (!localPlayer)
        return;

    static std::vector<int> stolenIds;

    for (int i = 1; i <= memory->globalVars->maxClients; ++i) {
        const auto entity = interfaces->entityList->getEntity(i);

        if (!entity || entity == localPlayer.get())
            continue;

        PlayerInfo playerInfo;
        if (!interfaces->engine->getPlayerInfo(entity->index(), playerInfo))
            continue;

        if (playerInfo.fakeplayer || std::find(stolenIds.cbegin(), stolenIds.cend(), playerInfo.userId) != stolenIds.cend())
            continue;

        if (changeName(false, (std::string{ playerInfo.name } +'\x1').c_str(), 1.0f))
            stolenIds.push_back(playerInfo.userId);

        return;
    }
    stolenIds.clear();
}

void Misc::disablePanoramablur() noexcept
{
    static auto blur = interfaces->cvar->findVar("@panorama_disable_blur");
    blur->setValue(config->misc.disablePanoramablur);
}

void Misc::quickReload(UserCmd* cmd) noexcept
{
    if (config->misc.quickReload) {
        static Entity* reloadedWeapon{ nullptr };

        if (reloadedWeapon) {
            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (interfaces->entityList->getEntityFromHandle(weaponHandle) == reloadedWeapon) {
                    cmd->weaponselect = reloadedWeapon->index();
                    cmd->weaponsubtype = reloadedWeapon->getWeaponSubType();
                    break;
                }
            }
            reloadedWeapon = nullptr;
        }

        if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && activeWeapon->isInReload() && activeWeapon->clip() == activeWeapon->getWeaponData()->maxClip) {
            reloadedWeapon = activeWeapon;

            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (auto weapon{ interfaces->entityList->getEntityFromHandle(weaponHandle) }; weapon && weapon != reloadedWeapon) {
                    cmd->weaponselect = weapon->index();
                    cmd->weaponsubtype = weapon->getWeaponSubType();
                    break;
                }
            }
        }
    }
}

bool Misc::changeName(bool reconnect, const char* newName, float delay) noexcept
{
    static auto exploitInitialized{ false };

    static auto name{ interfaces->cvar->findVar("name") };

    if (reconnect) {
        exploitInitialized = false;
        return false;
    }

    if (!exploitInitialized && interfaces->engine->isInGame()) {
        if (PlayerInfo playerInfo; localPlayer && interfaces->engine->getPlayerInfo(localPlayer->index(), playerInfo) && (!strcmp(playerInfo.name, "?empty") || !strcmp(playerInfo.name, "\n\xAD\xAD\xAD"))) {
            exploitInitialized = true;
        } else {
            name->onChangeCallbacks.size = 0;
            name->setValue("\n\xAD\xAD\xAD");
            return false;
        }
    }

    static auto nextChangeTime{ 0.0f };
    if (nextChangeTime <= memory->globalVars->realtime) {
        name->setValue(newName);
        nextChangeTime = memory->globalVars->realtime + delay;
        return true;
    }
    return false;
}

void Misc::bunnyHop2(UserCmd* cmd) noexcept
{
    if (!localPlayer)
        return;

    static auto wasLastTimeOnGround{ localPlayer->flags() & 1 };

    if (config->misc.bunnyHop && !(localPlayer->flags() & 1) && localPlayer->moveType() != MoveType::LADDER && !wasLastTimeOnGround)
        cmd->buttons &= ~UserCmd::IN_JUMP;

    wasLastTimeOnGround = localPlayer->flags() & 1;
}

void Misc::bunnyHop(UserCmd* cmd) noexcept
{
    static int hopsHit = 0;

    if (!config->misc.bunnyHuman || !localPlayer)
        return;

    //if (config->misc.bunnyHop && !(localPlayer->flags() & 1) && localPlayer->moveType() != MoveType::LADDER && !wasLastTimeOnGround)
    //    cmd->buttons &= ~UserCmd::IN_JUMP;

    if (localPlayer->moveType() != MoveType::LADDER) {
        if (cmd->buttons & UserCmd::IN_JUMP && !(localPlayer->flags() & 1)) {
            cmd->buttons &= ~UserCmd::IN_JUMP;
        }
        else if ((hopsHit >= config->misc.bhopMinHits && rand() % 100 + 1 > config->misc.bhopHitchance) || hopsHit >= config->misc.bhopMaxHits) {
            cmd->buttons &= ~UserCmd::IN_JUMP;
            hopsHit = 0;
        }
        else {
            hopsHit++;
        }
    }
}

void Misc::fakeBan(bool set) noexcept
{
    static bool shouldSet = false;

    if (set)
        shouldSet = set;

    if (shouldSet && interfaces->engine->isInGame() && changeName(false, std::string{ " \x1\xB" }.append(std::string{ static_cast<char>(config->misc.banColor + 1) }).append(config->misc.banText).append("\n\x1").c_str(), 5.0f))
        shouldSet = false;
}

void Misc::setName(bool set) noexcept
{
    static bool shouldSet = false;
    static bool shouldResetOrigName = false;

    if (set)
        shouldSet = set;

    if (shouldSet && changeName(false, std::string{ "" }.append(config->misc.customName).c_str(), 5.0f) && !(config->misc.customName.c_str() == NULL))
    {
        shouldSet = false;
        shouldResetOrigName = true;
    }
}

void Misc::fakeItem(bool set) noexcept
{
    static auto name{ interfaces->cvar->findVar("name") };
    static auto disconnect{ interfaces->cvar->findVar("disconnect") };

    static int shouldSet = 0;

    std::string playercolor;
    std::string color;
    std::string team;
    std::string star;
    std::string stattrak;
    std::string skinName;
    std::string item;

    if (set)
        if (shouldSet == 0)
            shouldSet = 1;

    if (shouldSet == 1)
    {
        if (config->misc.fakeItemRarity == 0)
            color = "\x08"; // Consumer Grade(White)
        else if (config->misc.fakeItemRarity == 1)
            color = "\x0D"; // Industrial Grade(Light blue)
        else if (config->misc.fakeItemRarity == 2)
            color = "\x0B"; // Mil-Spec(Blue)
        else if (config->misc.fakeItemRarity == 3)
            color = "\x03"; // Restricted(Purple)
        else if (config->misc.fakeItemRarity == 4)
            color = "\x0E"; // Classified(Pink)
        else if (config->misc.fakeItemRarity == 5)
            color = "\x02"; // Covert(Red)
        else if (config->misc.fakeItemRarity == 6)
            color = "\x10"; // Contrabanned(Orange / Gold)

        if (config->misc.fakeItemTeam == 1)
            team = "\x09";
        else
            team = "\x0B";

        if (config->misc.selectedFakeItemFlags[3])
            star = "★ ";
        else
            star = "";

        if (config->misc.selectedFakeItemFlags[2])
            stattrak = "StatTrak™ ";
        else
            stattrak = "";

        if (!config->misc.fakeItemName.empty())
            skinName.append(" | ").append(config->misc.fakeItemName);
        else
            skinName = "";

        if (config->misc.fakeItemType == 0)
            item = "AK-47";
        else if (config->misc.fakeItemType == 1)
            item = "AUG";
        else if (config->misc.fakeItemType == 2)
            item = "AWP";
        else if (config->misc.fakeItemType == 3)
            item = "Faca Baioneta";
        else if (config->misc.fakeItemType == 4)
            item = "Faca Bowie";
        else if (config->misc.fakeItemType == 5)
            item = "Canivete Borboleta";
        else if (config->misc.fakeItemType == 6)
            item = "CZ75";
        else if (config->misc.fakeItemType == 7)
            item = "Faca Clássica";
        else if (config->misc.fakeItemType == 8)
            item = "Desert Eagle";
        else if (config->misc.fakeItemType == 9)
            item = "Dual Berettas";
        else if (config->misc.fakeItemType == 10)
            item = "FAMAS";
        else if (config->misc.fakeItemType == 11)
            item = "Canivete Falchion";
        else if (config->misc.fakeItemType == 12)
            item = "FiveSeveN";
        else if (config->misc.fakeItemType == 13)
            item = "Flip Knife";
        else if (config->misc.fakeItemType == 14)
            item = "G3SG1";
        else if (config->misc.fakeItemType == 15)
            item = "Galil AR";
        else if (config->misc.fakeItemType == 16)
            item = "Glock-18";
        else if (config->misc.fakeItemType == 17)
            item = "Gut Knife";
        else if (config->misc.fakeItemType == 18)
            item = "Huntsman Knife";
        else if (config->misc.fakeItemType == 19)
            item = "Karambit";
        else if (config->misc.fakeItemType == 20)
            item = "M249";
        else if (config->misc.fakeItemType == 21)
            item = "M4A1-S";
        else if (config->misc.fakeItemType == 22)
            item = "M4A4";
        else if (config->misc.fakeItemType == 23)
            item = "M9 Baioneta";
        else if (config->misc.fakeItemType == 24)
            item = "MAC-10";
        else if (config->misc.fakeItemType == 25)
            item = "MAG-7";
        else if (config->misc.fakeItemType == 26)
            item = "MP5-SD";
        else if (config->misc.fakeItemType == 27)
            item = "MP7";
        else if (config->misc.fakeItemType == 28)
            item = "MP9";
        else if (config->misc.fakeItemType == 29)
            item = "Navaja Knife";
        else if (config->misc.fakeItemType == 30)
            item = "Negev";
        else if (config->misc.fakeItemType == 31)
            item = "Nomad Knife";
        else if (config->misc.fakeItemType == 32)
            item = "P2000";
        else if (config->misc.fakeItemType == 33)
            item = "P250";
        else if (config->misc.fakeItemType == 34)
            item = "P90";
        else if (config->misc.fakeItemType == 35)
            item = "PP-Bizon";
        else if (config->misc.fakeItemType == 36)
            item = "Paracord Knife";
        else if (config->misc.fakeItemType == 37)
            item = "SCAR-20";
        else if (config->misc.fakeItemType == 38)
            item = "SG 553";
        else if (config->misc.fakeItemType == 39)
            item = "Cano-Curto";
        else if (config->misc.fakeItemType == 40)
            item = "Adagas Sombrias";
        else if (config->misc.fakeItemType == 41)
            item = "Faca Esqueleto";
        else if (config->misc.fakeItemType == 42)
            item = "Stiletto Knife";
        else if (config->misc.fakeItemType == 43)
            item = "Survival Knife";
        else if (config->misc.fakeItemType == 44)
            item = "Talon Knife";
        else if (config->misc.fakeItemType == 45)
            item = "Tec-9";
        else if (config->misc.fakeItemType == 46)
            item = "UMP-45";
        else if (config->misc.fakeItemType == 47)
            item = "USP-S";
        else if (config->misc.fakeItemType == 48)
            item = "Ursus Knife";
        else if (config->misc.fakeItemType == 49)
            item = "XM1014";
        else if (config->misc.fakeItemType == 50)
            item = "Hand Wraps";
        else if (config->misc.fakeItemType == 51)
            item = "Moto Gloves";
        else if (config->misc.fakeItemType == 52)
            item = "Specialist Gloves";
        else if (config->misc.fakeItemType == 53)
            item = "Sport Gloves";
        else if (config->misc.fakeItemType == 54)
            item = "Bloodhound Gloves";
        else if (config->misc.fakeItemType == 55)
            item = "Hydra Gloves";
        else if (config->misc.fakeItemType == 56)
            item = "Driver Gloves";

        if (config->misc.fakeItemPlayerColor == 0)
            playercolor = "\x09"; // Yellow
        else if (config->misc.fakeItemPlayerColor == 1)
            playercolor = "\x04"; // Green
        else if (config->misc.fakeItemPlayerColor == 2)
            playercolor = "\x0D"; // Blue
        else if (config->misc.fakeItemPlayerColor == 3)
            playercolor = "\x03"; // Purple
        else if (config->misc.fakeItemPlayerColor == 4)
            playercolor = "\x10"; // Orange

        if (config->misc.fakeItemMessageType == 0)
        {
            if (interfaces->engine->isInGame() && changeName(false, std::string{ "\n \x1\xB" }.append(playercolor).append("• • ").append(team).append(config->misc.fakeItemPlayerName).append("\x01 abriu uma caixa e encontrou: \x1\xB").append(color).append(star).append(stattrak).append(item).append(skinName).append("\n ").append("\x1").c_str(), 5.0f))
                shouldSet = 2;
        }
        else
        {
            if (interfaces->engine->isInGame() && changeName(false, std::string{ "\n \x1\xB" }.append(playercolor).append("• • ").append(team).append(config->misc.fakeItemPlayerName).append("\x01 recebeu em uma troca: \x1\xB").append(color).append(star).append(stattrak).append(item).append(skinName).append("\n ").append("\x1").c_str(), 5.0f))
                shouldSet = 2;
        }
    }

    if (shouldSet == 2)
    {
        if (config->misc.selectedFakeItemFlags[1])
            disconnect->setValue(1);
        shouldSet = 0;
    }
}

void Misc::nadePredict() noexcept
{
    static auto nadeVar{ interfaces->cvar->findVar("cl_grenadepreview") };

    nadeVar->onChangeCallbacks.size = 0;
    nadeVar->setValue(config->misc.nadePredict);
}

void Misc::quickHealthshot(UserCmd* cmd) noexcept
{
    if (!localPlayer)
        return;

    static bool inProgress{ false };

    if (GetAsyncKeyState(config->misc.quickHealthshotKey))
        inProgress = true;

    if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && inProgress) {
        if (activeWeapon->getClientClass()->classId == ClassId::Healthshot && localPlayer->nextAttack() <= memory->globalVars->serverTime() && activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime())
            cmd->buttons |= UserCmd::IN_ATTACK;
        else {
            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (const auto weapon{ interfaces->entityList->getEntityFromHandle(weaponHandle) }; weapon && weapon->getClientClass()->classId == ClassId::Healthshot) {
                    cmd->weaponselect = weapon->index();
                    cmd->weaponsubtype = weapon->getWeaponSubType();
                    return;
                }
            }
        }
        inProgress = false;
    }
}

void Misc::fixTabletSignal() noexcept
{
    if (config->misc.fixTabletSignal && localPlayer) {
        if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && activeWeapon->getClientClass()->classId == ClassId::Tablet)
            activeWeapon->tabletReceptionIsBlocked() = false;
    }
}

void Misc::fakePrime() noexcept
{
    static bool lastState = false;

    if (config->misc.fakePrime != lastState) {
        lastState = config->misc.fakePrime;

        if (DWORD oldProtect; VirtualProtect(memory->fakePrime, 1, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            constexpr uint8_t patch[]{ 0x74, 0xEB };
            *memory->fakePrime = patch[config->misc.fakePrime];
            VirtualProtect(memory->fakePrime, 1, oldProtect, nullptr);
        }
    }
}

void Misc::killMessage(GameEvent& event) noexcept
{
    if (!config->misc.killMessage)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    std::string cmd = "say \"";
    cmd += config->misc.killMessageString;
    cmd += '"';
    interfaces->engine->clientCmdUnrestricted(cmd.c_str());
}

void Misc::fixMovement(UserCmd* cmd, float yaw) noexcept
{
    if (config->antiAim.general.fixMovement) {
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

void Misc::antiAfkKick(UserCmd* cmd) noexcept
{
    if (config->misc.antiAfkKick && cmd->commandNumber % 2)
        cmd->buttons |= 1 << 26;
}



void Misc::fixAnimationLOD(FrameStage stage) noexcept
{
    if (config->misc.fixAnimationLOD && stage == FrameStage::RENDER_START) {
        if (!localPlayer)
            return;

        for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
            Entity* entity = interfaces->entityList->getEntity(i);
            if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive()) continue;
            *reinterpret_cast<int*>(entity + 0xA28) = 0;
            *reinterpret_cast<int*>(entity + 0xA30) = memory->globalVars->framecount;
        }
    }
}

void Misc::autoPistol(UserCmd* cmd) noexcept
{
    if (config->misc.autoPistol && localPlayer) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && activeWeapon->isPistol() && activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime()) {
            if (activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver)
                cmd->buttons &= ~UserCmd::IN_ATTACK2;
            else
                cmd->buttons &= ~UserCmd::IN_ATTACK;
        }
    }
}

float Misc::RandomFloat(float min, float max) noexcept
{
    return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
}

void Misc::chokePackets(bool& sendPacket, UserCmd* cmd) noexcept
{
    bool doFakeLag{ false };
    auto position = localPlayer->getAbsOrigin();
    auto velocity = localPlayer->velocity();
    auto extrapolatedVelocity = sqrt(sqrt(velocity.x * velocity.y * velocity.z));
    auto& records = config->globals.serverPos;
    float distanceDifToServerSide = sqrt(sqrt(records.origin.x * records.origin.y * records.origin.z));

    if (config->misc.fakeLagMode != 0)
    {
        if ((config->misc.fakeLagSelectedFlags[0] && cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2))
            || (config->misc.fakeLagSelectedFlags[1] && !(cmd->buttons & (UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT)))
            || (config->misc.fakeLagSelectedFlags[2] && cmd->buttons & (UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT))
            || (config->misc.fakeLagSelectedFlags[3] && !(localPlayer->flags() & 1)))
            doFakeLag = true;
        else
            doFakeLag = false;
    }

    if (doFakeLag)
    {
        if (config->misc.fakeLagMode == 1)
            sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= config->misc.fakeLagTicks;
        if (config->misc.fakeLagMode == 2)
        {
            auto requiredPacketsToBreakLagComp = 65 / extrapolatedVelocity;
            if (!(requiredPacketsToBreakLagComp > config->misc.fakeLagTicks) && requiredPacketsToBreakLagComp <= 16)
                sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= requiredPacketsToBreakLagComp;
            else
                sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= 16;
        }
        if (config->misc.fakeLagMode == 3)
        {
            float lagTicks = RandomFloat(config->misc.fakeLagTicks, 16);
            sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= lagTicks;
        }
        if (config->misc.fakeLagMode == 4)
        {
            if (distanceDifToServerSide < 64.f && interfaces->engine->getNetworkChannel()->chokedPackets < 16)
                sendPacket = false;
            else
                sendPacket = true;
        }
    }
}
        
void Misc::autoReload(UserCmd* cmd) noexcept
{
    if (config->misc.autoReload && localPlayer) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && getWeaponIndex(activeWeapon->itemDefinitionIndex2()) && !activeWeapon->clip())
            cmd->buttons &= ~(UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2);
    }
}

void Misc::revealRanks(UserCmd* cmd) noexcept
{
    if (config->misc.revealRanks && cmd->buttons & UserCmd::IN_SCORE)
        interfaces->client->dispatchUserMessage(50, 0, 0, nullptr);
}

void Misc::autoStrafe(UserCmd* cmd) noexcept
{
    if (localPlayer
        && config->misc.autoStrafe
        && !(localPlayer->flags() & 1)
        && localPlayer->moveType() != MoveType::NOCLIP) {
        if (cmd->mousedx < 0)
            cmd->sidemove = -450.0f;
        else if (cmd->mousedx > 0)
            cmd->sidemove = 450.0f;
    }
}

void Misc::removeCrouchCooldown(UserCmd* cmd) noexcept
{
    if (config->misc.fastDuck)
        cmd->buttons |= UserCmd::IN_BULLRUSH;
}

void Misc::moonwalk(UserCmd* cmd) noexcept
{
    if (config->misc.moonwalk && localPlayer && localPlayer->moveType() != MoveType::LADDER)
        cmd->buttons ^= UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT;
}

void Misc::playHitSound(GameEvent& event) noexcept
{
    if (!config->misc.hitSound)
        return;

    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    constexpr std::array hitSounds{
        "play physics/metal/metal_solid_impact_bullet2",
        "play buttons/arena_switch_press_02",
        "play training/timer_bell",
        "play physics/glass/glass_impact_bullet1"
    };

    if (static_cast<std::size_t>(config->misc.hitSound - 1) < hitSounds.size())
        interfaces->engine->clientCmdUnrestricted(hitSounds[config->misc.hitSound - 1]);
    else if (config->misc.hitSound == 5)
        interfaces->engine->clientCmdUnrestricted(("play " + config->misc.customHitSound).c_str());
}

void Misc::drawAimbotFOV() noexcept
{
    auto local = localPlayer.get();
    if (config->misc.drawAimbotFov)
    {
        if (!local || !local->isAlive()) return;
        int weaponId = getWeaponIndex(localPlayer->getActiveWeapon()->itemDefinitionIndex2());
        if (!config->aimbot[weaponId].enabled) weaponId = 0;
        if (!config->aimbot[weaponId].enabled) return;
        auto screenSize = interfaces->surface->getScreenSize();
        if (config->aimbot[weaponId].silent) interfaces->surface->setDrawColor(255, 10, 10, 255);
        else interfaces->surface->setDrawColor(10, 255, 10, 255);
        float radius = std::tan(degreesToRadians(config->aimbot[weaponId].fov) / 2.f) / std::tan(degreesToRadians(local->isScoped() ? local->fov() : (105 + config->visuals.fov)) / 2.f) * screenSize.first;
        interfaces->surface->drawOutlinedCircle(screenSize.first / 2, screenSize.second / 2, radius, 100);
    }

}

void Misc::killSound(GameEvent& event) noexcept
{
    if (!config->misc.killSound)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    constexpr std::array killSounds{
        "play physics/metal/metal_solid_impact_bullet2",
        "play buttons/arena_switch_press_02",
        "play training/timer_bell",
        "play physics/glass/glass_impact_bullet1"
    };

    if (static_cast<std::size_t>(config->misc.killSound - 1) < killSounds.size())
        interfaces->engine->clientCmdUnrestricted(killSounds[config->misc.killSound - 1]);
    else if (config->misc.killSound == 5)
        interfaces->engine->clientCmdUnrestricted(("play " + config->misc.customKillSound).c_str());
}

void Misc::fakeDuck(UserCmd* cmd, bool& sendPacket) noexcept
{
    auto state = localPlayer->getAnimstate();

    if (config->misc.fakeDuck)
    {
        if (config->misc.fakeDuckKey != 0) {
            if (!GetAsyncKeyState(config->misc.fakeDuckKey))
            {
                config->misc.fakeDucking = false;
                return;
            }
            else
                config->misc.fakeDucking = true;
        }

        if (config->misc.fakeDucking)
        {
            if (cmd->buttons & UserCmd::IN_ATTACK || config->misc.fakeDuckShotState != 0)
            {
                if (localPlayer->getAnimstate()->duckAmount > 0.2 && config->misc.fakeDuckShotState == 0)
                {
                    sendPacket = true; // clear up sendPacket for fakeduck going up to choke
                    cmd->buttons |= UserCmd::IN_BULLRUSH;
                    cmd->buttons |= UserCmd::IN_DUCK;
                    cmd->buttons &= ~UserCmd::IN_ATTACK;
                    config->misc.fakeDuckShotState = 1;
                }
                else if (localPlayer->getAnimstate()->duckAmount > 0.2 && config->misc.fakeDuckShotState == 1)
                {
                    sendPacket = false;
                    cmd->buttons |= UserCmd::IN_BULLRUSH;
                    cmd->buttons &= ~UserCmd::IN_DUCK;
                    cmd->buttons &= ~UserCmd::IN_ATTACK;
                    config->misc.fakeDuckShotState = 1;
                }
                else if (localPlayer->getAnimstate()->duckAmount <= 0.2 && config->misc.fakeDuckShotState == 1)
                {
                    sendPacket = false;
                    cmd->buttons |= UserCmd::IN_BULLRUSH;
                    cmd->buttons &= ~UserCmd::IN_DUCK;
                    cmd->buttons |= UserCmd::IN_ATTACK;
                    config->misc.fakeDuckShotState = 2;
                }
                else if (config->misc.fakeDuckShotState == 2)
                {
                    sendPacket = false;
                    cmd->buttons |= UserCmd::IN_BULLRUSH;
                    cmd->buttons |= UserCmd::IN_DUCK;
                    config->misc.fakeDuckShotState = 3;
                }
                else if (config->misc.fakeDuckShotState == 3)
                {
                    sendPacket = true;
                    cmd->buttons |= UserCmd::IN_BULLRUSH;
                    cmd->buttons |= UserCmd::IN_DUCK;
                    config->misc.fakeDuckShotState = 0;
                }
            }
            else
            {
                cmd->buttons |= UserCmd::IN_BULLRUSH;
                cmd->buttons |= UserCmd::IN_DUCK;
                config->misc.fakeDuckShotState = 0;
            }
        }
        else
        {
            config->misc.fakeDuckShotState = 0;
        }
    }
}

void Misc::teamDamageCounter(GameEvent* event) noexcept {


    if (!event || !interfaces->engine->isInGame()) return;
    int local = localPlayer->getUserId();
        if (interfaces->engine->getPlayerForUserID(event->getInt("attacker")) == local) { // we attacked.
        int victim = interfaces->engine->getPlayerForUserID(event->getInt("userid"));

        if (victim == local) return; // did damage to ourself, does not count.

        Entity* ent = interfaces->entityList->getEntity(victim);

        if (ent) { // teammate got hurt
            switch (fnv::hashRuntime(event->getName())) {
            case fnv::hash("player_hurt"):
                teamDamage += event->getInt("dmg_health");
                break;
            case fnv::hash("player_death"):
                teamKills += 1;
                break;
            }

            if (config->misc.showTeamDamage)
                memory->conColorMsg({ 0, 255, 55, 255 },"[ Fogo-Amigo ] Kills: <%d> Dano: <%d>)\n", teamKills, teamDamage);
        }
    }
}

void Misc::purchaseList(GameEvent* event) noexcept
{
    static std::mutex mtx;
    std::scoped_lock _{ mtx };

    static std::unordered_map<std::string, std::pair<std::vector<std::string>, int>> purchaseDetails;
    static std::unordered_map<std::string, int> purchaseTotal;
    static int totalCost;

    static auto freezeEnd = 0.0f;

    if (event) {
        switch (fnv::hashRuntime(event->getName())) {
        case fnv::hash("item_purchase"): {
            const auto player = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid")));

            if (player && localPlayer && memory->isOtherEnemy(player, localPlayer.get())) {
                const auto weaponName = event->getString("weapon");
                auto& purchase = purchaseDetails[player->getPlayerName(true)];

                if (const auto definition = memory->itemSystem()->getItemSchema()->getItemDefinitionByName(weaponName)) {
                    if (const auto weaponInfo = memory->weaponSystem->getWeaponInfo(definition->getWeaponId())) {
                        purchase.second += weaponInfo->price;
                        totalCost += weaponInfo->price;
                    }
                }
                std::string weapon = weaponName;

                if (weapon.starts_with("weapon_"))
                    weapon.erase(0, 7);
                else if (weapon.starts_with("item_"))
                    weapon.erase(0, 5);

                if (weapon.starts_with("smoke"))
                    weapon.erase(5);
                else if (weapon.starts_with("m4a1_s"))
                    weapon.erase(6);
                else if (weapon.starts_with("usp_s"))
                    weapon.erase(5);

                purchase.first.push_back(weapon);
                ++purchaseTotal[weapon];
            }
            break;
        }
        case fnv::hash("round_start"):
            freezeEnd = 0.0f;
            purchaseDetails.clear();
            purchaseTotal.clear();
            totalCost = 0;
            break;
        case fnv::hash("round_freeze_end"):
            freezeEnd = memory->globalVars->realtime;
            break;
        }
    }
    else {
        if (!config->misc.purchaseList.enabled)
            return;

        static const auto mp_buytime = interfaces->cvar->findVar("mp_buytime");

        if ((!interfaces->engine->isInGame() || freezeEnd != 0.0f && memory->globalVars->realtime > freezeEnd + (!config->misc.purchaseList.onlyDuringFreezeTime ? mp_buytime->getFloat() : 0.0f) || purchaseDetails.empty() || purchaseTotal.empty()) && !gui->open)
            return;

        ImGui::SetNextWindowSize({ 200.0f, 200.0f }, ImGuiCond_Once);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
        if (!gui->open)
            windowFlags |= ImGuiWindowFlags_NoInputs;
        if (config->misc.purchaseList.noTitleBar)
            windowFlags |= ImGuiWindowFlags_NoTitleBar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
        ImGui::Begin("Purchases", nullptr, windowFlags);
        ImGui::PopStyleVar();

        if (config->misc.purchaseList.mode == PurchaseList::Details) {
            for (const auto& [playerName, purchases] : purchaseDetails) {
                std::string s = std::accumulate(purchases.first.begin(), purchases.first.end(), std::string{ }, [](std::string s, const std::string& piece) { return s += piece + ", "; });
                if (s.length() >= 2)
                    s.erase(s.length() - 2);

                if (config->misc.purchaseList.showPrices)
                    ImGui::TextWrapped("%s $%d: %s", playerName.c_str(), purchases.second, s.c_str());
                else
                    ImGui::TextWrapped("%s: %s", playerName.c_str(), s.c_str());
            }
        }
        else if (config->misc.purchaseList.mode == PurchaseList::Summary) {
            for (const auto& purchase : purchaseTotal)
                ImGui::TextWrapped("%d x %s", purchase.second, purchase.first.c_str());

            if (config->misc.purchaseList.showPrices && totalCost > 0) {
                ImGui::Separator();
                ImGui::TextWrapped("Total: $%d", totalCost);
            }
        }
        ImGui::End();
    }
}