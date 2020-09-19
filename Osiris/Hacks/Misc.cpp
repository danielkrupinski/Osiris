#include <mutex>
#include <numeric>
#include <sstream>
#include <random>

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
#include "../SDK/Panorama.h"
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
    if (config->visuals.inverseRagdollGravityCustomize)
        ragdollGravity->setValue(config->visuals.inverseRagdollGravity ? config->visuals.inverseRagdollGravityValue : 600);
    else
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
    if (config->misc.clanTagStyle == 0)
        memory->setClanTag("", "");
    if (config->misc.clanTagStyle == 1) {
        if (memory->globalVars->realtime - lastTime < 1.0f)
            return;

        const auto time = std::time(nullptr);
        const auto localTime = std::localtime(&time);
        char s[11];
        s[0] = '\0';
        sprintf_s(s, "[%02d:%02d:%02d]", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
        lastTime = memory->globalVars->realtime;
        memory->setClanTag(s, s);
    } else if (config->misc.clanTagStyle == 2) {
        if (memory->globalVars->realtime - lastTime < config->misc.customClanTagSpeed)
            return;

        if (config->misc.animatedClanTag && !clanTag.empty()) {
            const auto offset = Helpers::utf8SeqLen(clanTag[0]);
            if (offset != -1)
                std::rotate(clanTag.begin(), clanTag.begin() + offset, clanTag.end());
        }
        lastTime = memory->globalVars->realtime;
        memory->setClanTag(clanTag.c_str(), clanTag.c_str());
    }
    else if (config->misc.clanTagStyle == 3) {
        static std::vector<std::string> OsirisBETA
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
            "OsirsB",
            "OsirsB^",
            "OsirsBE",
            "OsirsBE^",
            "OsirsBET",
            "OsirsBET^",
            "OsirsBETA ^_^",
            "OsirsBET^",
            "OsirsBET",
            "OsirsBE^",
            "OsirsBE",
            "OsirsB^",
            "OsirsB",
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
        float currentTime = memory->globalVars->currenttime * config->misc.OsirisBETAClanTagSpeed;

        if (lastTime != currentTime)
        {
            std::string buf = OsirisBETA[(int)currentTime % OsirisBETA.size()];
            memory->setClanTag(buf.c_str(), buf.c_str());
            lastTime = currentTime;
        }

    }
    else if (config->misc.clanTagStyle == 4) {
        if (config->misc.customMultiClanTag.empty())
            return;

        std::istringstream Stream(config->misc.customMultiClanTag);
        std::vector <std::string> ClanTags;
        std::string ClanTag;

        while (std::getline(Stream, ClanTag, '\n'))
            if (ClanTag.length() > 0)
                ClanTags.push_back(ClanTag);
        
        static float lastTime = 0.0f;
        float currentTime = memory->globalVars->currenttime * config->misc.customMultiClanTagSpeed;

        if (lastTime != currentTime && ClanTags.size() > 0)
        {
            std::string buf = ClanTags[(int)currentTime % ClanTags.size()];
            memory->setClanTag(buf.c_str(), buf.c_str());
            lastTime = currentTime;
        }
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
        interfaces->surface->setTextColor(rainbowColor(config->misc.spectatorList.rainbowSpeed));
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

static void drawCrosshair(ImDrawList* drawList, const ImVec2& pos, ImU32 color, float thickness) noexcept
{
    drawList->Flags &= ~ImDrawListFlags_AntiAliasedLines;

    // dot
    drawList->AddRectFilled(pos - ImVec2{ 1, 1 }, pos + ImVec2{ 2, 2 }, color & IM_COL32_A_MASK);
    drawList->AddRectFilled(pos, pos + ImVec2{ 1, 1 }, color);

    // left
    drawList->AddRectFilled(ImVec2{ pos.x - 11, pos.y - 1 }, ImVec2{ pos.x - 3, pos.y + 2 }, color & IM_COL32_A_MASK);
    drawList->AddRectFilled(ImVec2{ pos.x - 10, pos.y }, ImVec2{ pos.x - 4, pos.y + 1 }, color);

    // right
    drawList->AddRectFilled(ImVec2{ pos.x + 4, pos.y - 1 }, ImVec2{ pos.x + 12, pos.y + 2 }, color & IM_COL32_A_MASK);
    drawList->AddRectFilled(ImVec2{ pos.x + 5, pos.y }, ImVec2{ pos.x + 11, pos.y + 1 }, color);

    // top (left with swapped x/y offsets)
    drawList->AddRectFilled(ImVec2{ pos.x - 1, pos.y - 11 }, ImVec2{ pos.x + 2, pos.y - 3 }, color & IM_COL32_A_MASK);
    drawList->AddRectFilled(ImVec2{ pos.x, pos.y - 10 }, ImVec2{ pos.x + 1, pos.y - 4 }, color);

    // bottom (right with swapped x/y offsets)
    drawList->AddRectFilled(ImVec2{ pos.x - 1, pos.y + 4 }, ImVec2{ pos.x + 2, pos.y + 12 }, color & IM_COL32_A_MASK);
    drawList->AddRectFilled(ImVec2{ pos.x, pos.y + 5 }, ImVec2{ pos.x + 1, pos.y + 11 }, color);

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
    out = ImFloor(out);
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
    if (config->misc.watermark.enabled) {
        std::string watermark = "OsirisBETA";

        if (interfaces->engine->isInGame() && config->misc.watermarkNickname) {
            PlayerInfo playerInfo;
            auto nickname = interfaces->engine->getPlayerInfo(localPlayer->index(), playerInfo);
            watermark.append(" | ").append(playerInfo.name);
        };

        if (config->misc.watermarkUsername) {
            auto username = getenv("USERNAME");
            watermark.append(" | ").append(username);
        }

        if (config->misc.watermarkFPS) {
            static auto frameRate = 1.0f;
            frameRate = 0.9f * frameRate + 0.1f * memory->globalVars->absoluteFrameTime;
            watermark.append(" | FPS: ").append(std::to_string(static_cast<int>(1 / frameRate)));
        }

        if (config->misc.watermarkPing) {
            float latency = 0.0f;
            if (auto networkChannel = interfaces->engine->getNetworkChannel(); networkChannel && networkChannel->getLatency(0) > 0.0f)
                latency = networkChannel->getLatency(0);
            watermark.append(" | Ping: ").append(std::to_string(static_cast<int>(latency * 1000))).append(" ms");
        }

        if (config->misc.watermarkTickrate) {
            auto tickRate = 1.0f / memory->globalVars->intervalPerTick;
            watermark.append(" | ").append(std::to_string(static_cast<int>(tickRate))).append(" tick");
        }

        if (config->misc.watermarkTime) {
            const auto time = std::time(nullptr);
            const auto localTime = std::localtime(&time);
            std::ostringstream timeShow;
            timeShow << std::setfill('0') << std::setw(2) << localTime->tm_hour << ":" << std::setw(2) << localTime->tm_min << ":" << std::setw(2) << localTime->tm_sec;
            watermark.append(" | ").append(timeShow.str());
        }

        auto rainbow = rainbowColor(config->misc.watermark.rainbowSpeed);

        auto posX = config->misc.watermarkPosX * ImGui::GetIO().DisplaySize.x;
        auto posY = config->misc.watermarkPosY * ImGui::GetIO().DisplaySize.y;
        ImGuiCond nextFlag = ImGuiCond_None;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f }, ImGuiCond_Always);
        if (ImGui::IsMouseDown(0))
            nextFlag |= ImGuiCond_Once;
        else
            nextFlag |= ImGuiCond_Always;
        ImGui::SetNextWindowPos({ posX ,posY }, nextFlag);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        if (!gui->open)
            windowFlags |= ImGuiWindowFlags_NoInputs;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
        ImGui::Begin("OsirisBETA", nullptr, windowFlags);
        ImGui::PopStyleVar();

        auto [x, y] = ImGui::GetWindowPos();
        auto [w, h] = ImGui::GetWindowSize();
        auto ds = ImGui::GetIO().DisplaySize;
        if (x > (ds.x - w) && y > (ds.y - h)) {
            x = ds.x - w;
            y = ds.y - h;
        }
        else if (x > (ds.x - w) && y <= (ds.y - h))
            x = ds.x - w;
        else if (x <= (ds.x - w) && y > (ds.y - h))
            y = ds.y - h;

        if (x < 0 && y < 0) {
            x = 0;
            y = 0;
        }
        else if (x < 0 && y >= 0)
            x = 0;
        else if (x >= 0 && y < 0)
            y = 0;
        x /= ds.x;
        y /= ds.y;

        config->misc.watermarkPosX = x;
        config->misc.watermarkPosY = y;

        ImGui::SetWindowFontScale(config->misc.watermarkScale);
        if (config->misc.watermark.rainbow)
            ImGui::TextColored({std::get<0>(rainbow), std::get<1>(rainbow) ,std::get<2>(rainbow), 1.0f }, watermark.c_str());
        else
            ImGui::TextColored({ config->misc.watermark.color[0], config->misc.watermark.color[1] ,config->misc.watermark.color[2], 1.0f }, watermark.c_str());

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
    if (!config->misc.fastPlant)
        return;

    static auto plantAnywhere = interfaces->cvar->findVar("mp_plant_c4_anywhere");

    if (plantAnywhere->getInt())
        return;

    if (!localPlayer || !localPlayer->isAlive() || (localPlayer->inBombZone() && localPlayer->flags() & 1))
        return;

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon || activeWeapon->getClientClass()->classId != ClassId::C4)
        return;

    cmd->buttons &= ~UserCmd::IN_ATTACK;

    constexpr auto doorRange = 200.0f;

    Trace trace;
    const auto startPos = localPlayer->getEyePosition();
    const auto endPos = startPos + Vector::fromAngle(cmd->viewangles) * doorRange;
    interfaces->engineTrace->traceRay({ startPos, endPos }, 0x46004009, localPlayer.get(), trace);

    if (!trace.entity || trace.entity->getClientClass()->classId != ClassId::PropDoorRotating)
        cmd->buttons &= ~UserCmd::IN_USE;
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
    static auto wasLastTimeOnGround{ localPlayer->flags() & 1 };
    static int hopsHit = 0;

    if (!localPlayer || !config->misc.bunnyHop)
        return;

    if (config->misc.bhopHitchanceEnable) {
        if (localPlayer->moveType() != MoveType::LADDER) {
            if (cmd->buttons & UserCmd::IN_JUMP && !(localPlayer->flags() & 1))
                cmd->buttons &= ~UserCmd::IN_JUMP;
            else if ((hopsHit >= config->misc.bhopMinHits && rand() % 100 + 1 > config->misc.bhopHitchance) || hopsHit >= config->misc.bhopMaxHits) {
                cmd->buttons &= ~UserCmd::IN_JUMP;
                hopsHit = 0;
            }
            else
                hopsHit++;
        }

    }
    else {
        if (!(localPlayer->flags() & 1) && localPlayer->moveType() != MoveType::LADDER && !wasLastTimeOnGround)
            cmd->buttons &= ~UserCmd::IN_JUMP;

        wasLastTimeOnGround = localPlayer->flags() & 1;
    }
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

float RandomFloat(float min, float max) noexcept
{
    return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
}

struct Record {
    Vector origin;
    float simulationTime;
    matrix3x4 matrix[256];
};

void Misc::chokePackets(bool& sendPacket, UserCmd* cmd) noexcept
{
    bool doFakeLag{ false };
    auto position = localPlayer->getAbsOrigin();
    auto velocity = localPlayer->velocity();
    auto extrapolatedVelocity = sqrt(sqrt(velocity.x * velocity.y * velocity.z));
    Record record{ };
    record.origin = localPlayer->getAbsOrigin();
    record.simulationTime = localPlayer->simulationTime();
    localPlayer->setupBones(record.matrix, 256, 0x7FF00, memory->globalVars->currenttime);
    auto& records = record;
    float distanceDifToServerSide = sqrt(sqrt(records.origin.x * records.origin.y * records.origin.z));

    if (config->misc.chokedPackets != 0)
    {
        if ((config->misc.chokedPacketsShooting && cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2))
            || (config->misc.chokedPacketsStanding && !(cmd->buttons & (UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT)))
            || (config->misc.chokedPacketsMoving && cmd->buttons & (UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT))
            || (config->misc.chokedPacketsAir && !(localPlayer->flags() & 1)))
            doFakeLag = true;
        else
            doFakeLag = false;
    }

    if (doFakeLag)
    {
        config->misc.pingBasedChokedVal = config->misc.chokedPacketsTicks;
        if (config->misc.pingBasedChoked)
            if (auto networkChannel = interfaces->engine->getNetworkChannel(); networkChannel && networkChannel->getLatency(0) > 0.0f)
                config->misc.pingBasedChokedVal = std::clamp(static_cast<int>(networkChannel->getLatency(0) * 64), 0, 16);

        if (config->misc.chokedPackets == 1)
            sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= config->misc.pingBasedChokedVal;
        if (config->misc.chokedPackets == 2) {
            auto requiredPacketsToBreakLagComp = 65 / extrapolatedVelocity;
            if (requiredPacketsToBreakLagComp <= config->misc.pingBasedChokedVal && requiredPacketsToBreakLagComp <= 16)
                sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= requiredPacketsToBreakLagComp;
            else
                sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= 16;
        }
        if (config->misc.chokedPackets == 3) {
            float lagTicks = RandomFloat(static_cast<float>(config->misc.pingBasedChokedVal), 16);
            sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= lagTicks;
        }
        if (config->misc.chokedPackets == 4) {
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
        && (!config->misc.autoStrafeKey || GetAsyncKeyState(config->misc.autoStrafeKey))
        && !(localPlayer->flags() & 1)
        && localPlayer->moveType() != MoveType::NOCLIP) {
        if (config->misc.autoStrafeStyle == 0) {
            if (cmd->mousedx < 0)
                cmd->sidemove = -450.0f;
            else if (cmd->mousedx > 0)
                cmd->sidemove = 450.0f;
        }
        else if (config->misc.autoStrafeStyle == 1) {
            float flYawBhop = (localPlayer->velocity().length() > 50.f) ? (30.f * fabsf(30.f / localPlayer->velocity().length())) : 0.f;
            if (cmd->mousedx < 0) {
                cmd->viewangles.y -= flYawBhop;
                cmd->sidemove = -450.0f;
            }
            else if (cmd->mousedx > 0) {
                cmd->viewangles.y += flYawBhop;
                cmd->sidemove = 450.0f;
            }
        }
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

void Misc::oppositeHandKnife(FrameStage stage) noexcept
{
    if (GetAsyncKeyState(config->misc.oppositeHandKnifeBind) & 1)
        config->misc.oppositeHandKnife = !config->misc.oppositeHandKnife;

    if (!config->misc.oppositeHandKnife)
        return;

    if (!localPlayer)
        return;

    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    static const auto cl_righthand = interfaces->cvar->findVar("cl_righthand");
    static bool original;

    if (stage == FrameStage::RENDER_START) {
        original = cl_righthand->getInt();

        if (const auto activeWeapon = localPlayer->getActiveWeapon()) {
            if (const auto classId = activeWeapon->getClientClass()->classId; classId == ClassId::Knife || classId == ClassId::KnifeGG)
                cl_righthand->setValue(!original);
        }
    } else {
        cl_righthand->setValue(original);
    }
}

static std::vector<std::uint64_t> reportedPlayers;
static int reportbotRound;

void Misc::runReportbot() noexcept
{
    if (!config->misc.reportbot.enabled)
        return;

    if (!localPlayer)
        return;

    static auto lastReportTime = 0.0f;

    if (lastReportTime + config->misc.reportbot.delay > memory->globalVars->realtime)
        return;

    if (reportbotRound >= config->misc.reportbot.rounds)
        return;

    for (int i = 1; i <= interfaces->engine->getMaxClients(); ++i) {
        const auto entity = interfaces->entityList->getEntity(i);

        if (!entity || entity == localPlayer.get())
            continue;

        if (config->misc.reportbot.target != 2 && (entity->isOtherEnemy(localPlayer.get()) ? config->misc.reportbot.target != 0 : config->misc.reportbot.target != 1))
            continue;

        PlayerInfo playerInfo;
        if (!interfaces->engine->getPlayerInfo(i, playerInfo))
            continue;

        if (playerInfo.fakeplayer || std::find(reportedPlayers.cbegin(), reportedPlayers.cend(), playerInfo.xuid) != reportedPlayers.cend())
            continue;

        std::string report;

        if (config->misc.reportbot.textAbuse)
            report += "textabuse,";
        if (config->misc.reportbot.griefing)
            report += "grief,";
        if (config->misc.reportbot.wallhack)
            report += "wallhack,";
        if (config->misc.reportbot.aimbot)
            report += "aimbot,";
        if (config->misc.reportbot.other)
            report += "speedhack,";

        if (!report.empty()) {
            memory->submitReport(std::to_string(playerInfo.xuid).c_str(), report.c_str());
            lastReportTime = memory->globalVars->realtime;
            reportedPlayers.push_back(playerInfo.xuid);
        }
        return;
    }

    reportedPlayers.clear();
    ++reportbotRound;
}

void Misc::resetReportbot() noexcept
{
    reportbotRound = 0;
    reportedPlayers.clear();
}

void Misc::preserveKillfeed(bool roundStart) noexcept
{
    if (!config->misc.preserveKillfeed.enabled)
        return;

    static auto nextUpdate = 0.0f;

    if (roundStart) {
        nextUpdate = memory->globalVars->realtime + 10.0f;
        return;
    }

    if (nextUpdate > memory->globalVars->realtime)
        return;

    nextUpdate = memory->globalVars->realtime + 2.0f;

    const auto deathNotice = memory->findHudElement(memory->hud, "CCSGO_HudDeathNotice");
    if (!deathNotice)
        return;

    const auto deathNoticePanel = (*(UIPanel**)(*(deathNotice - 5 + 22) + 4));
    const auto childPanelCount = deathNoticePanel->getChildCount();

    for (int i = 0; i < childPanelCount; ++i) {
        const auto child = deathNoticePanel->getChild(i);
        if (!child)
            continue;

        if (child->hasClass("DeathNotice_Killer") && (!config->misc.preserveKillfeed.onlyHeadshots || child->hasClass("DeathNoticeHeadShot")))
            child->setAttributeFloat("SpawnTime", memory->globalVars->currenttime);
    }
}


void Misc::jumpbug(UserCmd* cmd) noexcept {
    if (!config->misc.jumpbug || !localPlayer || !localPlayer->isAlive())
        return;

    static bool bhopWasEnabled = true;
    bool JumpDone;

    bool unduck = true;

    float max_radius = static_cast<float>(M_PI) * 2;
    float step = max_radius / 128;
    float xThick = 23;

    if (GetAsyncKeyState(config->misc.jumpbugkey) && (localPlayer->flags() & 1) && !(EnginePrediction::getFlags() & 1)) {

        if (config->misc.bunnyHop) {
            config->misc.bunnyHop = false;
            bhopWasEnabled = false;
        }

        if (unduck) {
            JumpDone = false;
            if (config->misc.jumpbughold)
                cmd->buttons |= UserCmd::IN_JUMP; // If you want to hold JB key only.
            else
                cmd->buttons &= ~UserCmd::IN_DUCK;
            unduck = false;
        }

        Vector pos = localPlayer->origin();

        for (float a = 0.f; a < max_radius; a += step) {
            Vector pt;
            pt.x = (xThick * cos(a)) + pos.x;
            pt.y = (xThick * sin(a)) + pos.y;
            pt.z = pos.z;

            Vector pt2 = pt;
            pt2.z -= 6;

            Trace target;

            interfaces->engineTrace->traceRay({ pt, pt2 }, 0x1400B, localPlayer.get(), target);

            if (target.fraction != 1.0f && target.fraction != 0.0f) {
                JumpDone = true;
                cmd->buttons |= UserCmd::IN_DUCK;
                cmd->buttons &= ~UserCmd::IN_JUMP;
                unduck = true;
            }
        }
        for (float a = 0.f; a < max_radius; a += step) {
            Vector pt;
            pt.x = ((xThick - 2.f) * cos(a)) + pos.x;
            pt.y = ((xThick - 2.f) * sin(a)) + pos.y;
            pt.z = pos.z;

            Vector pt2 = pt;
            pt2.z -= 6;

            Trace target;

            interfaces->engineTrace->traceRay({ pt, pt2 }, 0x1400B, localPlayer.get(), target);

            if (target.fraction != 1.f && target.fraction != 0.f) {
                JumpDone = true;
                cmd->buttons |= UserCmd::IN_DUCK;
                cmd->buttons &= ~UserCmd::IN_JUMP;
                unduck = true;
            }
        }
        for (float a = 0.f; a < max_radius; a += step) {
            Vector pt;
            pt.x = ((xThick - 20.f) * cos(a)) + pos.x;
            pt.y = ((xThick - 20.f) * sin(a)) + pos.y;
            pt.z = pos.z;

            Vector pt2 = pt;
            pt2.z -= 6;

            Trace target;

            interfaces->engineTrace->traceRay({ pt, pt2 }, 0x1400B, localPlayer.get(), target);

            if (target.fraction != 1.f && target.fraction != 0.f) {
                JumpDone = true;
                cmd->buttons |= UserCmd::IN_DUCK;
                cmd->buttons &= ~UserCmd::IN_JUMP;
                unduck = true;
            }
        }
    }
    else if (!bhopWasEnabled) {
        config->misc.bunnyHop = true;
        bhopWasEnabled = true;
    }
}

void Misc::teamDamageCounter(GameEvent* event) noexcept {
    if (!event || !interfaces->engine->isInGame())
        return;

    if (!config->misc.teamDamageCounter)
        return;

    if (interfaces->engine->getPlayerForUserID(event->getInt("attacker")) == localPlayer) { // we attacked.
        int victim = interfaces->engine->getPlayerForUserID(event->getInt("userid"));

        if (victim == localPlayer) return; // did damage to ourself, does not count.

        Entity* ent = interfaces->entityList->getEntity(victim);

        if (ent && !ent->isOtherEnemy(localPlayer.get())) { // teammate got hurt
            switch (fnv::hashRuntime(event->getName())) {
            case fnv::hash("player_hurt"):
                teamDamage += event->getInt("dmg_health");
                break;
            case fnv::hash("player_death"):
                teamKills += 1;
                break;
            }

            memory->debugMsg("[ Friendly Fire ] Kills: <%d> Damage: <%d>)\n", teamKills, teamDamage);
        }
    }
}

void Misc::drawAimbotFov() noexcept {
    if (config->misc.drawAimbotFov && interfaces->engine->isInGame()) {
        if (!localPlayer || !localPlayer->isAlive() || !localPlayer->getActiveWeapon()) return;
        int weaponId = getWeaponIndex(localPlayer->getActiveWeapon()->itemDefinitionIndex2());
        if (!config->aimbot[weaponId].enabled) weaponId = 0;
        if (!config->aimbot[weaponId].enabled) return;
        auto [width, heigth] = interfaces->surface->getScreenSize();
        if (config->aimbot[weaponId].silent)
            interfaces->surface->setDrawColor(255, 10, 10, 255);
        else interfaces->surface->setDrawColor(10, 255, 10, 255);
        float radius = std::tan(degreesToRadians(config->aimbot[weaponId].fov / 2.f)) / std::tan(degreesToRadians(config->misc.actualFov / 2.f)) * width;
        interfaces->surface->drawOutlinedCircle(width / 2, heigth / 2, (int)(radius * 0.776), 100); // idk how to fix it correctly
    }
}

void Misc::showVelocity() noexcept
{
    if (!config->visuals.showvelocity.enabled || !localPlayer || !localPlayer->isAlive())
        return;

    float velocity = localPlayer->velocity().length2D();
    std::wstring velocitywstr{ L"(" + std::to_wstring(static_cast<int>(velocity)) + L")" };

    interfaces->surface->setTextFont(Surface::font);
    if (config->visuals.showvelocity.rainbow)
        interfaces->surface->setTextColor(rainbowColor(config->visuals.showvelocity.rainbowSpeed));
    else
        interfaces->surface->setTextColor((config->visuals.showvelocity.color));

    const auto [width, height] = interfaces->surface->getScreenSize();
    config->visuals.showvelocityResX = width;
    config->visuals.showvelocityResY = height;
    if (config->visuals.showvelocityM)
        interfaces->surface->setTextPosition(config->visuals.showvelocityPosX, config->visuals.showvelocityPosY);
    else
        interfaces->surface->setTextPosition(width / 2 - 6, height - 200);
    interfaces->surface->printText(velocitywstr);
}

void Misc::drawBombDamage() noexcept
{
    if (!config->misc.bombDamage) return;

    //No Alive return since it is useful if you want to call it out to a mate that he will die
    if (!localPlayer) return;

    for (int i = interfaces->engine->getMaxClients(); i <= interfaces->entityList->getHighestEntityIndex(); i++)
    {
        auto entity = interfaces->entityList->getEntity(i);
        if (!entity || entity->isDormant() || entity->getClientClass()->classId != ClassId::PlantedC4 || !entity->
            c4Ticking())
            continue;

        auto vecBombDistance = entity->origin() - localPlayer->origin();

        const auto d = (vecBombDistance.length() - 75.68f) / 789.2f;
        auto flDamage = 450.7f * exp(-d * d);

        if (localPlayer->armor() > 0)
        {
            auto flNew = flDamage * 0.5f;
            auto flArmor = (flDamage - flNew) * 0.5f;

            if (flArmor > localPlayer->armor())
            {
                flArmor = localPlayer->armor() * 2.f;
                flNew = flDamage - flArmor;
            }

            flDamage = flNew;
        }

        //Could get the specator target here as well and set the color based on the spaceted player
        //I'm too lazy for that tho, green while you are dead just looks nicer
        if (localPlayer->isAlive() && max(ceilf(flDamage), 0) >= localPlayer->health())
            interfaces->surface->setTextColor(255, 0, 0);
        else
            interfaces->surface->setTextColor(0, 255, 0);

        auto bombDmgText{ (std::wstringstream{} << L"Bomb Damage: " << max(ceilf(flDamage), 0)).str() };

        constexpr unsigned font{ 0xc1 };
        interfaces->surface->setTextFont(font);

        auto drawPositionY{ interfaces->surface->getScreenSize().second / 8 };
        const auto bombDmgX{
            interfaces->surface->getScreenSize().first / 2 - static_cast<int>((interfaces
                                                                              ->surface->getTextSize(
                                                                                  font, bombDmgText.c_str())).first / 2)
        };

        drawPositionY -= interfaces->surface->getTextSize(font, bombDmgText.c_str()).second;

        interfaces->surface->setTextPosition(bombDmgX, drawPositionY);
        interfaces->surface->printText(bombDmgText.c_str());
    }
}

void Misc::playerBlocker(UserCmd* cmd) noexcept
{
    if (config->misc.playerBlocker && GetAsyncKeyState(config->misc.playerBlockerKey)) {
        float bestdist = 250.f;
        int index = -1;
        for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
            auto entity = interfaces->entityList->getEntity(i);

            if (!entity)
                continue;

            if (!entity->isAlive() || entity->isDormant() || entity == localPlayer.get())
                continue;

            float dist;

            double distance;
            distance = sqrt(((int)localPlayer->origin().x - (int)entity->origin().x) * ((int)localPlayer->origin().x - (int)entity->origin().x) +
                ((int)localPlayer->origin().y - (int)entity->origin().y) * ((int)localPlayer->origin().y - (int)entity->origin().y) +
                ((int)localPlayer->origin().z - (int)entity->origin().z) * ((int)localPlayer->origin().z - (int)entity->origin().z));
            dist = (float)abs(round(distance));

            if (dist < bestdist)
            {
                bestdist = dist;
                index = i;
            }
        }

        if (index == -1)
            return;

        auto target = interfaces->entityList->getEntity(index);

        if (!target)
            return;


        Vector delta = target->origin() - localPlayer->origin();
        Vector angles{ radiansToDegrees(atan2f(-delta.z, std::hypotf(delta.x, delta.y))), radiansToDegrees(atan2f(delta.y, delta.x)) };
        angles.normalize();

        angles.y -= localPlayer->eyeAngles().y;
        angles.normalize();
        angles.y = std::clamp(angles.y, -180.f, 180.f);

        if (angles.y < -1.0f)
            cmd->sidemove = 450.f;
        else if (angles.y > 1.0f)
            cmd->sidemove = -450.f;

    }
}

void Misc::doorSpam(UserCmd* cmd) noexcept {

    if (!localPlayer || !config->misc.doorSpam || localPlayer->isDefusing())
        return;

    constexpr auto doorRange = 200.0f;

    Trace trace;
    const auto startPos = localPlayer->getEyePosition();
    const auto endPos = startPos + Vector::fromAngle(cmd->viewangles) * doorRange;
    interfaces->engineTrace->traceRay({ startPos, endPos }, 0x46004009, localPlayer.get(), trace);

    if (trace.entity && trace.entity->getClientClass()->classId == ClassId::PropDoorRotating)
        if (cmd->buttons & UserCmd::IN_USE && cmd->tickCount & 1)
            cmd->buttons &= ~UserCmd::IN_USE;
}

void Misc::chatSpam() noexcept
{
    static int lastSpam = 0;

    const int curTime = (int)memory->globalVars->currenttime;

    if (!config->misc.chatSpam || lastSpam == curTime || curTime % config->misc.chatSpamDelay != 0)
        return;

    lastSpam = curTime;

    if (!localPlayer || localPlayer->team() == 0)
        return;

    std::istringstream Stream(config->misc.chatSpamPhrases);
    std::vector <std::string> Phrases;
    std::string Phrase;

    while (std::getline(Stream, Phrase, '\n'))
        if (Phrase.length() > 0)
            Phrases.push_back(Phrase);

    if (Phrases.size() > 0)
    {
        static int phraseId = 0;

        if (config->misc.chatSpamRandom)
        {
            std::mt19937 gen(std::random_device().operator()());
            std::uniform_int_distribution<int> uid(0, Phrases.size() - 1);
            const int randNum = uid(gen);
            const std::string Cmd = "say \"" + Phrases[randNum] + "\"";

            interfaces->engine->clientCmdUnrestricted(Cmd.c_str());

            phraseId = randNum + 1;
        }
        else
        {
            if (phraseId >= static_cast<int>(Phrases.size()))
                phraseId = 0;

            const std::string Cmd = "say \"" + Phrases[phraseId++] + "\"";

            interfaces->engine->clientCmdUnrestricted(Cmd.c_str());
        }
    }
}

void __fastcall hkCheckFileCRCsWithServer(void* ecx, void* edx)
{
    // dont do anything.. to prevent CRC check from happening
    return;
}

void Misc::pure() noexcept
{
    if (config->misc.svpurebypassOLD)
    {
        hkCheckFileCRCsWithServer(memory->CheckFileCRCsWithServer, hkCheckFileCRCsWithServer);
    }

}

void Misc::MLP() noexcept
{
    memory->debugMsg("\n                                         __  _.--''``''--..\n                                        /  \\/              `.\n                                       ( /  \\___             \\\n                                       | |      `'-.    __    \\\n                         ___           ( '.       __`.'` \\`.   \\\n                  ___   (   `.         /\\    ,. .'__\\  / `:_\\   \\\n                 (   `-. `.   `.      /  \\_ __.` / .-| |'.|= \\   |\n                  `-.   `-.`.   `.   :    r__,' ( (WW| \\W)j   |  |\n                     `.    `.\\   _\\  |    |    \\_\\_`/   ``-. /  /\n            .--'''```-_`-,   `  (  | |    |            \\__/.` .`\n           /         ( ```    __ \\  \\|    \\    -,______.-'/  /                      \n          /           `-._   (  `.\\  '.    \\      /'.   .' .'\n         /         ,.--''>`-. `-. `   |     `.   (  |  /  /   _\n        (         |     / 8Y8`_: `_:. |       \\   \\ | l  ( ,:' \\\n        |        .'    |     (   (    (        \\   |\\ |   \\  )  )\n        |       .'     | 8Y8  `---:.__-\\        \\  | `.    `` .'\n        |       |      |     8Y8        \\        \\ j   `'-..-'\n        |       '.    / \\        /       |        |\n        |        |.--'   |      /-,______|        |\n        l        |     _/      /     .-. |       / \\\n         \\       '.   /       /     ( (` /      /   \\\n      __  \\       |   |      |      |\\ ``    _.'     )\n    .' /   \\      |__/|      |      | `-._.-'(       |\n    | (_    |      |  |      |      |      |  \\      |\n    '   `'-`       |  |       \\     |       \\  `.___/\n     `-.._________/    \\_______)     \\_______) Fluttershy\n\nFrendship is magic\n\n");
    memory->debugMsg("   ____    ___                      ____                                                      \n  /\\  _`\\ /\\_ \\                    /\\  _`\\\n  \\ \\ \\L\\ \\//\\ \\      __     __  __\\ \\ \\/\\ \\     __     __  __\n   \\ \\ ,__/ \\ \\ \\   /'__`\\  /\\ \\/\\ \\\\ \\ \\ \\ \\  /'__`\\  /\\ \\/\\ \\\n    \\ \\ \\/   \\_\\ \\_/\\ \\L\\.\\_\\ \\ \\_\\ \\\\ \\ \\_\\ \\/\\ \\L\\.\\_\\ \\ \\_\\ \\\n     \\ \\_\\   /\\____\\ \\__/.\\_\\\\/`____ \\\\ \\____/\\ \\__/.\\_\\\\/`____ \\\n      \\/_/   \\/____/\\/__/\\/_/ `/___/> \\\\/___/  \\/__/\\/_/ `/___/> \\\n                                 /\\___/                     /\\___/\n                                 \\/__/                      \\/__/\n\n");
    std::string cmd = "showconsole";
    interfaces->engine->clientCmdUnrestricted(cmd.c_str());
}

void Misc::setName(bool set) noexcept
{
    static bool shouldSet = false;

    if (set)
        shouldSet = set;

    if (shouldSet && changeName(false, std::string{ "" }.append(config->misc.customName).c_str(), 5.0f) && !(config->misc.customName.c_str() == NULL))
        shouldSet = false;
}

void Misc::fakeItem(bool set) noexcept
{
    static auto name{ interfaces->cvar->findVar("name") };

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
        switch (config->misc.fakeItemRarity)
        {
            case 0: color = "\x08"; break; // Consumer Grade(White)
            case 1: color = "\x0D"; break; // Industrial Grade(Light blue)
            case 2: color = "\x0B"; break; // Mil-Spec(Blue)
            case 3: color = "\x03"; break; // Restricted(Purple)
            case 4: color = "\x0E"; break; // Classified(Pink)
            case 5: color = "\x02"; break; // Covert(Red)
            case 6: color = "\x10"; break; // Contrabanned(Orange / Gold)
        }

        team = (config->misc.fakeItemTeam == 1) ? "\x09" : "\x0B";
        star = config->misc.fakeItemFlagsStar ? "★ " : "";
        stattrak = config->misc.fakeItemFlagsST ? "StatTrak™ " : "";

        if (!config->misc.fakeItemName.empty())
            skinName.append(" | ").append(config->misc.fakeItemName);
        else
            skinName = "";

        switch (config->misc.fakeItemType)
        {
            case 0: item = "AK-47"; break;
            case 1: item = "AUG"; break;
            case 2: item = "AWP"; break;
            case 3: item = "Bayonet"; break;
            case 4: item = "Bowie Knife"; break;
            case 5: item = "Butterfly Knife"; break;
            case 6: item = "CZ75"; break;
            case 7: item = "Classic Knife"; break;
            case 8: item = "Desert Eagle"; break;
            case 9: item = "Dual Berettas"; break;
            case 10: item = "FAMAS"; break;
            case 11: item = "Falchion Knife"; break;
            case 12: item = "FiveSeveN"; break;
            case 13: item = "Flip Knife"; break;
            case 14: item = "G3SG1"; break;
            case 15: item = "Galil AR"; break;
            case 16: item = "Glock-18"; break;
            case 17: item = "Gut Knife"; break;
            case 18: item = "Huntsman Knife"; break;
            case 19: item = "Karambit"; break;
            case 20: item = "M249"; break;
            case 21: item = "M4A1-S"; break;
            case 22: item = "M4A4"; break;
            case 23: item = "M9 Bayonet"; break;
            case 24: item = "MAC-10"; break;
            case 25: item = "MAG-7"; break;
            case 26: item = "MP5-SD"; break;
            case 27: item = "MP7"; break;
            case 28: item = "MP9"; break;
            case 29: item = "Navaja Knife"; break;
            case 30: item = "Negev"; break;
            case 31: item = "Nomad Knife"; break;
            case 32: item = "Nova"; break;
            case 33: item = "P2000"; break;
            case 34: item = "P250"; break;
            case 35: item = "P90"; break;
            case 36: item = "PP-Bizon"; break;
            case 37: item = "Paracord Knife"; break;
            case 38: item = "R8 Revolver"; break;
            case 39: item = "SCAR-20"; break;
            case 40: item = "SG 553"; break;
            case 41: item = "SSG 08"; break;
            case 42: item = "Sawed-Off"; break;
            case 43: item = "Shadow Daggers"; break;
            case 44: item = "Skeleton Knife"; break;
            case 45: item = "Spectral Shiv"; break;
            case 46: item = "Stiletto Knife"; break;
            case 47: item = "Survival Knife"; break;
            case 48: item = "Talon Knife"; break;
            case 49: item = "Tec-9"; break;
            case 50: item = "UMP-45"; break;
            case 51: item = "USP-S"; break;
            case 52: item = "Ursus Knife"; break;
            case 53: item = "XM1014"; break;
            case 54: item = "Hand Wraps"; break;
            case 55: item = "Moto Gloves"; break;
            case 56: item = "Specialist Gloves"; break;
            case 57: item = "Sport Gloves"; break;
            case 58: item = "Bloodhound Gloves"; break;
            case 59: item = "Hydra Gloves"; break;
            case 60: item = "Driver Gloves"; break;
        }

        switch (config->misc.fakeItemPlayerColor)
        {
            case 0: playercolor = "\x09"; break; // Yellow
            case 1: playercolor = "\x04"; break; // Green
            case 2: playercolor = "\x0D"; break; // Blue
            case 3: playercolor = "\x03"; break; // Purple
            case 4: playercolor = "\x10"; break; // Orange
        }

        if (interfaces->engine->isInGame() && changeName(false, std::string{ "\n \x1\xB" }.append(playercolor).append("• • ").append(team).append(config->misc.fakeItemPlayerName).append(config->misc.fakeItemMessageType == 0 ? "\x01 has opened a container and found: \x1\xB" : "\x01 has recieved in trade: \x1\xB").append(color).append(star).append(stattrak).append(item).append(skinName).append("\n ").append("\x1").c_str(), 5.0f))
            shouldSet = 2;
    }

    if (shouldSet == 2)
        shouldSet = 0;
}

void Misc::buyBot(GameEvent* event) noexcept
{
    const char* primaryList[] = {
        "",
        "buy famas; buy galilar; ",
        "buy m4a1; buy ak47; ",
        "buy ssg08; ",
        "buy aug; buy sg556; ",
        "buy awp; ",
        "buy scar20; buy g3sg1; ",

        "buy mp9; buy mac10; ",
        "buy mp7; buy mp5sd; ",
        "buy ump45; ",
        "buy p90; ",
        "buy bizon; ",

        "buy nova; ",
        "buy xm1014; ",
        "buy mag7; buy sawedoff; ",
        "buy m249; ",
        "buy negev; "
    };

    const char* secondaryList[] = {
        "",
        "buy hkp2000; buy glock; ",
        "buy p250; ",
        "buy elite; ",
        "buy fiveseven; buy tec9; ",
        "buy deagle; buy revolver; "
    };

    static bool start = false;

    std::string buy = "";

    if (event) {
        switch (fnv::hashRuntime(event->getName())) {
        case fnv::hash("round_start"):
            if (!config->misc.buyBot)
                return;

            if (config->misc.buyBotPrimary != 0)
                buy += primaryList[config->misc.buyBotPrimary];
            if (config->misc.buyBotSecondary != 0)
                buy += secondaryList[config->misc.buyBotSecondary];

            buy += config->misc.buyBotVest ? (config->misc.buyBotVestHelm ? "buy vesthelm; " : "buy vest; ") : ""; // Kevlar Vest + Helmet
            buy += config->misc.buyBotTaser ? "buy taser 34; " : ""; // Zeus x27
            buy += config->misc.buyBotDefuser ? "buy defuser; " : ""; // Defuse Kit

            buy += config->misc.buyBotMolotov ? "buy incgrenade; buy molotov; " : "";  // Incendiary/Molotov
            buy += config->misc.buyBotDecoy ? "buy decoy; " : ""; // Decoy
            buy += config->misc.buyBotFlashbang ? (config->misc.buyBotFlashbangX2 ? "buy flashbang; buy flashbang; " : "buy flashbang; ") : ""; // Flashbang
            buy += config->misc.buyBotHE ? "buy hegrenade; " : ""; // High Explosive
            buy += config->misc.buyBotSmoke ? "buy smokegrenade; " : ""; // Smoke

            if (buy != "")
                interfaces->engine->clientCmdUnrestricted(buy.c_str());
            break;
        }
    }
}

void Misc::customViewmodelPosition(FrameStage stage) noexcept {

    if (!localPlayer)
        return;

    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    static const auto view_x = interfaces->cvar->findVar("viewmodel_offset_x");
    static const auto view_y = interfaces->cvar->findVar("viewmodel_offset_y");
    static const auto view_z = interfaces->cvar->findVar("viewmodel_offset_z");
    static const auto sv_minspec = interfaces->cvar->findVar("sv_competitive_minspec");
    static const auto cl_righthand = interfaces->cvar->findVar("cl_righthand");
    static float viewmodel_x_orig = view_x->getFloat();
    static float viewmodel_y_orig = view_y->getFloat();
    static float viewmodel_z_orig = view_z->getFloat();

    *(int*)((DWORD)&sv_minspec->onChangeCallbacks + 0xC) = 0;

    sv_minspec->setValue(config->misc.customViewmodelToggle ? 0 : 1);

    if (stage == FrameStage::RENDER_START) {

        if (const auto activeWeapon = localPlayer->getActiveWeapon()) {
            if (const auto classId = activeWeapon->getClientClass()->classId; classId == ClassId::Knife || classId == ClassId::KnifeGG) {
                view_x->setValue(config->misc.viewmodel_x_knife);
                view_y->setValue(config->misc.viewmodel_y_knife);
                view_z->setValue(config->misc.viewmodel_z_knife);
            }
            else if (activeWeapon && classId != ClassId::C4) {
                view_x->setValue(config->misc.viewmodel_x);
                view_y->setValue(config->misc.viewmodel_y);
                view_z->setValue(config->misc.viewmodel_z);
                cl_righthand->setValue(config->misc.customViewmodelSwitchHand ? 0 : 1);
                if (GetAsyncKeyState(config->misc.customViewmodelSwitchHandBind) & 1)
                    config->misc.customViewmodelSwitchHand = !config->misc.customViewmodelSwitchHand;
            }
            else {
                view_x->setValue(viewmodel_x_orig);
                view_y->setValue(viewmodel_y_orig);
                view_z->setValue(viewmodel_z_orig);
            }
        }
    }
    else {
        view_x->setValue(viewmodel_x_orig);
        view_y->setValue(viewmodel_y_orig);
        view_z->setValue(viewmodel_z_orig);
    }
}

void Misc::viewBob(FrameStage stage) noexcept {
    if (!localPlayer)
        return;

    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    static const auto view_bob = interfaces->cvar->findVar("cl_use_new_headbob");

    if (stage == FrameStage::RENDER_START)
        view_bob->setValue(config->misc.view_bob ? 0 : 1);
};

void Misc::fakeDuck(UserCmd* cmd, bool& sendPacket) noexcept
{
    auto state = localPlayer->getAnimstate()->duckAmount;
    static int fakeDuckShotState = 0;

    if (!config->misc.fakeDuck)
        return;

    if (config->misc.fakeDuckKey != 0) {
        if (!GetAsyncKeyState(config->misc.fakeDuckKey)) {
            config->misc.fakeDucking = false;
            return;
        }
        else
            config->misc.fakeDucking = true;
    }

    if (config->misc.fakeDucking) {
        if (cmd->buttons & UserCmd::IN_ATTACK || fakeDuckShotState != 0) {
            if (state > 0.2 && fakeDuckShotState == 0) {
                sendPacket = true; // clear up sendPacket for fakeduck going up to choke
                cmd->buttons |= UserCmd::IN_BULLRUSH;
                cmd->buttons |= UserCmd::IN_DUCK;
                cmd->buttons &= ~UserCmd::IN_ATTACK;
                fakeDuckShotState = 1;
            }
            else if (state > 0.2 && fakeDuckShotState == 1) {
                sendPacket = false;
                cmd->buttons |= UserCmd::IN_BULLRUSH;
                cmd->buttons &= ~UserCmd::IN_DUCK;
                cmd->buttons &= ~UserCmd::IN_ATTACK;
                fakeDuckShotState = 1;
            }
            else if (state <= 0.2 && fakeDuckShotState == 1) {
                sendPacket = false;
                cmd->buttons |= UserCmd::IN_BULLRUSH;
                cmd->buttons &= ~UserCmd::IN_DUCK;
                cmd->buttons |= UserCmd::IN_ATTACK;
                fakeDuckShotState = 2;
            }
            else if (fakeDuckShotState == 2) {
                sendPacket = false;
                cmd->buttons |= UserCmd::IN_BULLRUSH;
                cmd->buttons |= UserCmd::IN_DUCK;
                fakeDuckShotState = 3;
            }
            else if (fakeDuckShotState == 3) {
                sendPacket = true;
                cmd->buttons |= UserCmd::IN_BULLRUSH;
                cmd->buttons |= UserCmd::IN_DUCK;
                fakeDuckShotState = 0;
            }
        }
        else {
            cmd->buttons |= UserCmd::IN_BULLRUSH;
            cmd->buttons |= UserCmd::IN_DUCK;
            fakeDuckShotState = 0;
        }
    }
    else
        fakeDuckShotState = 0;
}

void Misc::DrawInaccuracy(ImDrawList* draw)noexcept
{
    const auto [w, h] = interfaces->surface->getScreenSize();

    if (!config->misc.drawInaccuracy.enabled)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    const auto Acweapon = localPlayer->getActiveWeapon();
    if (!Acweapon || Acweapon->getWeaponType() == WeaponType::Knife || Acweapon->getWeaponType() == WeaponType::Grenade)
        return;

    const float Inaccuracy = Acweapon->getInaccuracy() * 500.0f;
    if (Inaccuracy <= 0.0f)
        return;

    float r, g, b, a;
    if (config->misc.drawInaccuracy.rainbow) {
        auto rainbow = rainbowColor(config->misc.drawInaccuracy.rainbowSpeed);
        r = std::get<0>(rainbow);
        g = std::get<1>(rainbow);
        b = std::get<2>(rainbow);
    }
    else {
        r = config->misc.drawInaccuracy.color[0];
        g = config->misc.drawInaccuracy.color[1];
        b = config->misc.drawInaccuracy.color[2];
    }
    a = config->misc.drawInaccuracy.color[3];

    draw->AddCircle(ImVec2(static_cast<float>(w) / 2.0f, static_cast<float>(h) / 2.0f), Inaccuracy, ImGui::GetColorU32(ImVec4(r, g, b, a)), static_cast<int>(config->misc.drawInaccuracy.thickness));
}
