﻿#include <mutex>
#include <numeric>
#include <sstream>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../Netvars.h"

#include "EnginePrediction.h"
#include "Misc.h"

#include "../SDK/Client.h"
#include "../SDK/ConVar.h"
#include "../SDK/Entity.h"
#include "../SDK/FrameStage.h"
#include "../SDK/GameEvent.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/Localize.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/Surface.h"
#include "../SDK/UserCmd.h"
#include "../SDK/WeaponData.h"
#include "../SDK/WeaponSystem.h"

#include "../GUI.h"
#include "../Helpers.h"
#include "../GameData.h"

#include "../imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"

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
    static std::string clanTag;

    if (tagChanged) {
        clanTag = config->misc.clanTag;
        if (!clanTag.empty() && clanTag.front() != ' ' && clanTag.back() != ' ')
            clanTag.push_back(' ');
        return;
    }
    
    static auto lastTime = 0.0f;

    if (config->misc.clocktag)
    {
        if (memory->globalVars->realtime - lastTime < 1.0f)
            return;

        const auto time = std::time(nullptr);
        const auto localTime = std::localtime(&time);
        char s[11];
        s[0] = '\0';
        sprintf_s(s, "[%02d:%02d:%02d]", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
        lastTime = memory->globalVars->realtime;
        memory->setClanTag(s, s);
    } 
    else if (config->misc.customClanTag) {
        if (memory->globalVars->realtime - lastTime < 0.6f)
            return;

        if (config->misc.animatedClanTag && !clanTag.empty()) {
            const auto offset = Helpers::utf8SeqLen(clanTag[0]);
            if (offset != -1)
                std::rotate(clanTag.begin(), clanTag.begin() + offset, clanTag.end());
        }
        lastTime = memory->globalVars->realtime;
        memory->setClanTag(clanTag.c_str(), clanTag.c_str());
    }
    else if(config->misc.osirisClanTag){
		static std::vector<std::string> ClanTags
		{
			"",
			"O",
			"O^",
			"Os",
			"Os^",
			"Osi",
			"Osi^",
			"Osir",
			"Osir^",
			"Osirs ",
			"Osirs^",
			"Osirs.",
			"Osirs.^",
			"Osirs.c",
			"Osirs.c^",
			"Osirs.cc ^_^",
			"Osirs.c^",
			"Osirs.c", //  ^
			"Osirs.^",
			"Osirs.",
			"Osirs^",
			"Osirs",
			"Osir^",
			"Osir",
			"Osi^",
			"Osi",
			"Os^",
			"Os",
			"O^",
			"O",
			""
		};

		static float lastTime = 0.0f;
		float currentTime = memory->globalVars->currenttime * 2.0f;

		if (lastTime != currentTime)
		{
			std::string buf = ClanTags[(int)currentTime % ClanTags.size()];
			memory->setClanTag(buf.c_str(), "amaterasu");
			lastTime = currentTime;
		}

    }
}

void Misc::spectatorList() noexcept
{
    auto& cfg = config->misc.spectatorList;
    if (cfg.enabled) {
        std::string name;
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
        if (!gui->open)
            windowFlags |= ImGuiWindowFlags_NoInputs;
        if(cfg.noBackGround)
            windowFlags |= ImGuiWindowFlags_NoBackground;
        if (cfg.noTittleBar)
            windowFlags |= ImGuiWindowFlags_NoTitleBar;
        if (!localPlayer && !gui->open)
            return;

        const auto size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, NULL, name.c_str());
        ImGui::SetNextWindowSize(ImVec2(300.0f, size.y + ImGui::GetFontSize() * 3.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
        ImGui::Begin("Spectator List", nullptr, windowFlags);
        ImGui::PopStyleVar();
        if (!localPlayer)
            return;

		if (!localPlayer->isAlive())      
        {
			if (!localPlayer->getObserverTarget())
				return;
			auto a = localPlayer.get();
			a = localPlayer->getObserverTarget();
		}

		for (int i = 1; i <= interfaces->engine->getMaxClients(); ++i) {
			const auto entity = interfaces->entityList->getEntity(i);
			if (!entity || entity->isDormant() || entity->isAlive() || entity->getObserverTarget() != localPlayer.get())
				continue;

			PlayerInfo playerInfo;
			if (!interfaces->engine->getPlayerInfo(i, playerInfo))
				continue;

			name += std::string(playerInfo.name) + "\n";
		}
        

        ImGui::SetCursorPos(ImVec2(ImGui::GetStyle().FramePadding.x, ImGui::GetStyle().FramePadding.y * 3 + ImGui::GetFontSize()));
        ImGui::Text(name.c_str());


        ImGui::End();
    }
}

static void drawCrosshair(ImDrawList* drawList, const ImVec2& pos, ImU32 color, float thickness) noexcept
{
    drawList->Flags &= ~ImDrawListFlags_AntiAliasedLines;

    drawList->AddLine(ImVec2{ pos.x, pos.y - 10 } + ImVec2{ 1.0f, 1.0f }, ImVec2{ pos.x, pos.y - 3 } + ImVec2{ 1.0f, 1.0f }, color & IM_COL32_A_MASK, thickness);
    drawList->AddLine(ImVec2{ pos.x, pos.y + 3 } + ImVec2{ 1.0f, 1.0f }, ImVec2{ pos.x, pos.y + 10 } + ImVec2{ 1.0f, 1.0f }, color & IM_COL32_A_MASK, thickness);

    drawList->AddLine(ImVec2{ pos.x - 10, pos.y } + ImVec2{ 1.0f, 1.0f }, ImVec2{ pos.x - 3, pos.y } + ImVec2{ 1.0f, 1.0f }, color & IM_COL32_A_MASK, thickness);
    drawList->AddLine(ImVec2{ pos.x + 3, pos.y } + ImVec2{ 1.0f, 1.0f }, ImVec2{ pos.x + 10, pos.y } + ImVec2{ 1.0f, 1.0f }, color & IM_COL32_A_MASK, thickness);

    drawList->AddLine({ pos.x, pos.y - 10 }, { pos.x, pos.y - 3 }, color, thickness);
    drawList->AddLine({ pos.x, pos.y + 3 }, { pos.x, pos.y + 10 }, color, thickness);

    drawList->AddLine({ pos.x - 10, pos.y }, { pos.x - 3, pos.y }, color, thickness);
    drawList->AddLine({ pos.x + 3, pos.y }, { pos.x + 10, pos.y }, color, thickness);

    drawList->Flags |= ImDrawListFlags_AntiAliasedLines;
}

void Misc::noscopeCrosshair(ImDrawList* drawList) noexcept
{
    if (!config->misc.noscopeCrosshair.enabled)
        return;

    GameData::Lock lock;
    const auto& local = GameData::local();

    if (!local.exists || !local.alive || !local.noScope)
        return;

    drawCrosshair(drawList, ImGui::GetIO().DisplaySize / 2, Helpers::calculateColor(config->misc.noscopeCrosshair), config->misc.noscopeCrosshair.thickness);
}


static bool worldToScreen(const Vector& in, ImVec2& out) noexcept
{
    const auto& matrix = GameData::toScreenMatrix();

    const auto w = matrix._41 * in.x + matrix._42 * in.y + matrix._43 * in.z + matrix._44;
    if (w < 0.001f)
        return false;

    out = ImGui::GetIO().DisplaySize / 2.0f;
    out.x *= 1.0f + (matrix._11 * in.x + matrix._12 * in.y + matrix._13 * in.z + matrix._14) / w;
    out.y *= 1.0f - (matrix._21 * in.x + matrix._22 * in.y + matrix._23 * in.z + matrix._24) / w;
    return true;
}

void Misc::recoilCrosshair(ImDrawList* drawList) noexcept
{
    if (!config->misc.recoilCrosshair.enabled)
        return;

    GameData::Lock lock;
    const auto& localPlayerData = GameData::local();

    if (!localPlayerData.exists || !localPlayerData.alive)
        return;

    if (!localPlayerData.shooting)
        return;

    if (ImVec2 pos; worldToScreen(localPlayerData.aimPunch, pos))
        drawCrosshair(drawList, pos, Helpers::calculateColor(config->misc.recoilCrosshair), config->misc.recoilCrosshair.thickness);
}

void Misc::watermark() noexcept
{
    if (config->misc.watermark) {
        static auto frameRate = 1.0f;
        frameRate = 0.9f * frameRate + 0.1f * memory->globalVars->absoluteFrameTime;
        float latency = 0.0f;
      
        if (auto networkChannel = interfaces->engine->getNetworkChannel(); networkChannel && networkChannel->getLatency(0) > 0.0f)
            latency = networkChannel->getLatency(0);
        const auto [w, h] = interfaces->surface->getScreenSize();
        ImGui::SetNextWindowPos(ImVec2());
            ImGui::SetNextWindowSize(ImVec2(300.0F, 80.0F));
            
            ImGui::Begin("WaterMark",nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);
            auto draw = ImGui::GetWindowDrawList();
            auto pos = ImGui::GetWindowPos();
            ImGui::Spacing();
            ImGui::BeginChild("##First",ImVec2(200.0F,75.0F));
            ImGui::Text("Fps %.1f", static_cast<int>(1 / frameRate));
            ImGui::SameLine();
            ImGui::Text("Ping %.1f",static_cast<int>(latency * 1000));
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::BeginChild("##Second",ImVec2(80.0f,75.0f));
            draw->AddText(gui->fonts.segoeui,26.0f, ImVec2(pos.x + 285.0f, pos.y + 2.0f),ImColor(),"O");

            ImGui::End();
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
            auto bombText{ (std::wstringstream{ } << L"Bomb on " << (!entity->c4BombSite() ? 'A' : 'B') << L" : " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(entity->c4BlowTime() - memory->globalVars->currenttime, 0.0f) << L" s").str() };
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
                interfaces->surface->setDrawColor(rainbowColor(config->misc.bombTimer.rainbowSpeed));
            else
                interfaces->surface->setDrawColor(config->misc.bombTimer.color);

            static auto c4Timer = interfaces->cvar->findVar("mp_c4timer");

            interfaces->surface->drawFilledRect(progressBarX, drawPositionY + 5, static_cast<int>(progressBarX + progressBarLength * std::clamp(entity->c4BlowTime() - memory->globalVars->currenttime, 0.0f, c4Timer->getFloat()) / c4Timer->getFloat()), drawPositionY + progressBarHeight + 5);

            if (entity->c4Defuser() != -1) {
                if (PlayerInfo playerInfo; interfaces->engine->getPlayerInfo(interfaces->entityList->getEntityFromHandle(entity->c4Defuser())->index(), playerInfo)) {
                    if (wchar_t name[128];  MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                        drawPositionY += interfaces->surface->getTextSize(font, L" ").second;
                        const auto defusingText{ (std::wstringstream{ } << name << L" is defusing: " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(entity->c4DefuseCountDown() - memory->globalVars->currenttime, 0.0f) << L" s").str() };

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
                            canDefuseText = L"Can Defuse";
                            interfaces->surface->setTextColor(0, 255, 0);
                        } else {
                            canDefuseText = L"Cannot Defuse";
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

void Misc::bunnyHop(UserCmd* cmd) noexcept
{
    if (!localPlayer)
        return;

    static auto wasLastTimeOnGround{ localPlayer->flags() & 1 };

    if (config->misc.bunnyHop && !(localPlayer->flags() & 1) && localPlayer->moveType() != MoveType::LADDER && !wasLastTimeOnGround)
        cmd->buttons &= ~UserCmd::IN_JUMP;

    wasLastTimeOnGround = localPlayer->flags() & 1;
}

void Misc::fakeBan(bool set) noexcept
{
    static bool shouldSet = false;

    if (set)
        shouldSet = set;

    if (shouldSet && interfaces->engine->isInGame() && changeName(false, std::string{ "\x1\xB" }.append(std::string{ static_cast<char>(config->misc.banColor + 1) }).append(config->misc.banText).append("\x1").c_str(), 5.0f))
        shouldSet = false;
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

    if (GetAsyncKeyState(config->misc.quickHealthshotKey) & 1)
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
    if (config->misc.fixMovement) {
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

void Misc::chokePackets(bool& sendPacket) noexcept
{
    if (!config->misc.chokedPacketsKey || GetAsyncKeyState(config->misc.chokedPacketsKey))
        sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= config->misc.chokedPackets;
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
                if (const auto definition = memory->itemSystem()->getItemSchema()->getItemDefinitionByName(event->getString("weapon"))) {
                    auto& purchase = purchaseDetails[player->getPlayerName()];
                    if (const auto weaponInfo = memory->weaponSystem->getWeaponInfo(definition->getWeaponId())) {
                        purchase.second += weaponInfo->price;
                        totalCost += weaponInfo->price;
                    }
                    const std::string weapon = interfaces->localize->findAsUTF8(definition->getItemBaseName());
                    ++purchaseTotal[weapon];
                    purchase.first.push_back(weapon);
                }
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
    } else {
        if (!config->misc.purchaseList.enabled)
            return;

        static const auto mp_buytime = interfaces->cvar->findVar("mp_buytime");

        if ((!interfaces->engine->isInGame() || freezeEnd != 0.0f && memory->globalVars->realtime > freezeEnd + (!config->misc.purchaseList.onlyDuringFreezeTime ? mp_buytime->getFloat() : 0.0f) || purchaseDetails.empty() || purchaseTotal.empty()) && !gui->open)
            return;

        ImGui::SetNextWindowSize({ 200.0f, 200.0f }, ImGuiCond_Once);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
        if (!gui->open)
            windowFlags |= ImGuiWindowFlags_NoInputs;
        if (config->misc.purchaseList.noTittleBar)
			windowFlags |= ImGuiWindowFlags_NoTitleBar; 
        if (config->misc.purchaseList.noBackGround)
			windowFlags |= ImGuiWindowFlags_NoBackground;

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
        } else if (config->misc.purchaseList.mode == PurchaseList::Summary) {
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

void Misc::StatusBar()noexcept
{
    auto & cfg = config->misc.Sbar;
    if (cfg.enabled == false)
        return;

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
    
    if (cfg.noTittleBar)
        windowFlags |= ImGuiWindowFlags_NoTitleBar;

    if (cfg.noBackGround)
        windowFlags |= ImGuiWindowFlags_NoBackground;
    if (!localPlayer && !gui->open)
        return;

    ImGui::SetNextWindowSize(ImVec2(200.0f, 200.0f), ImGuiCond_Once);
    ImGui::Begin("Status Bar",nullptr,windowFlags);
    if (localPlayer && localPlayer->isAlive()) {
        if (cfg.ShowPlayerRealViewAngles) {
            ImGui::Text("Pitch: %.1f", config->globalvars.viewangles.x);
            ImGui::Text("Yaw: %.1f", config->globalvars.viewangles.y);
        }

        if (cfg.ShowPlayerStatus)
        {
            std::string message = "LocalPlayer: ";
            auto local = GameData::local();

			if (localPlayer->flags() & PlayerFlags::ONGROUND)
				message += "OnGound;\n";
			if (!(localPlayer->flags() & PlayerFlags::ONGROUND))
				message += "InAir;\n";
			if (localPlayer->flags() & PlayerFlags::DUCKING)
				message += "Ducking;\n";
			if (localPlayer->flags() & PlayerFlags::GODMODE)
				message += "Cheater Mode On;\n";
			if (localPlayer->flags() & PlayerFlags::ONFIRE)
				message += "OnFire;\n";
			if (localPlayer->flags() & PlayerFlags::SWIM)
				message += "Swimming;\n";
			if (localPlayer->isDefusing())
				message += "Defusing;\n";
			if (localPlayer->isFlashed())
				message += "Flashed~;\n";
			if (localPlayer->inBombZone())
				message += "in BombZone~\n";
			if (local.shooting)
				message += "Shooting...\n";

            if(!cfg.noBackGround) //if no background draw a line will make GUI not good
           ImGui::Separator();//Draw A line
           ImGui::Text(message.c_str());

        }

        if (cfg.ShowGameGlobalVars) {
            ImGui::Text("CurTime: %.1f",memory->globalVars->currenttime);
            ImGui::Text("RealTime: %.1f",memory->globalVars->realtime);
        }
    }
    ImGui::End();
}

void Misc::DrawInaccuracy(ImDrawList* draw)noexcept
{
    const auto [w, h] = interfaces->surface->getScreenSize();

    if (!config->misc.drawInaccuracy)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    const auto Acweapon = localPlayer->getActiveWeapon();
    if (!Acweapon || Acweapon->isNade() || Acweapon->isKnife())
		return;

	const float Inaccuracy = Acweapon->getInaccuracy() * 500.0f;
	if (Inaccuracy <= 0.0f)
		return;

    //TODO: Add Slider in GUI.CPP to change color
	draw->AddCircle(ImVec2(static_cast<float>(w) / 2.0f, static_cast<float>(h) / 2.0f), Inaccuracy,
	/* Red */ImGui::GetColorU32(ImVec4(1.000f, 0.000f, 0.000f, 1.000f)), config->misc.drawInaccuracyThickness);
}

static int missedshots;
static float hitchance;
static int TotalShots;
static int HitShots;
void Misc::ShotsCout(GameEvent* event)noexcept
{
	if (!config->misc.ShotsCout.enabled)
		return;

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;

	if (config->misc.ShotsCout.noTittleBar)
		windowFlags |= ImGuiWindowFlags_NoTitleBar;

	if (config->misc.ShotsCout.noBackGround)
		windowFlags |= ImGuiWindowFlags_NoBackground;

	if (!gui->open)
		windowFlags |= ImGuiWindowFlags_NoInputs;

	ImGui::SetNextWindowSize({ 200.0f, 200.0f }, ImGuiCond_Once);
	ImGui::Begin("空枪输出", nullptr, windowFlags);

    if (localPlayer) {
        if (localPlayer->isAlive()) {
            PlayerInfo Info;
            interfaces->engine->getPlayerInfo(localPlayer, Info);
            auto acweapon = localPlayer->getActiveWeapon();


            if (event && acweapon && (!acweapon->isKnife() || !acweapon->isNade())) {

                switch (fnv::hashRuntime(event->getName())) {
                case fnv::hash("weapon_fire"): {
                    int userID = event->getInt("userid"); //get userID

                    if (userID == Info.userId) //ID is localId
                        TotalShots++; // total++ 
                    break;
                }
                case fnv::hash("player_hurt"): {
                    int userID2 = event->getInt("userid"); //get userid
                    if (userID2 != Info.userId)
                        HitShots++;
                    break;
                }
                }


                }
            }
        }

    if (TotalShots && HitShots) {
        missedshots = TotalShots - HitShots;
        hitchance = HitShots / TotalShots;
    }
	ImGui::Text("TotalShots: %.1f", TotalShots);
	ImGui::Text("HitShots: %.1f", HitShots);
	ImGui::Text("Missed Shots: %.1f", missedshots);
	ImGui::Text("Hitchance: %.1f", hitchance);

	ImGui::End();
}

void Misc::ShowAA(ImDrawList* draw)noexcept
{
    if (!config->antiAim.enabled || !config->antiAim.ShowAA)
        return;

    auto [w, h] = interfaces->surface->getScreenSize();


	if (!config->antiAim.inver)
	{
		draw->AddCircle(ImVec2((w / 2) - 50, (h / 2) + 5), 3,
			ImGui::GetColorU32(ImVec4(1.000f, 0.000f, 0.000f, 1.000f)), 0.0f);
	}
    else
    {
		draw->AddCircle(ImVec2((w / 2) + 50, (h / 2) + 5), 3,
			ImGui::GetColorU32(ImVec4(1.000f, 0.000f, 0.000f, 1.000f)), 0.0f);
    }

}
