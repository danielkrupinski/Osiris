#include <algorithm>
#include <array>
#include <iomanip>
#include <mutex>
#include <numbers>
#include <numeric>
#include <sstream>
#include <vector>

#include "../imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_stdlib.h"

#include "../ConfigStructs.h"
#include "../InputUtil.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../ProtobufReader.h"
#include "../pstring.h"

#include "EnginePrediction.h"
#include "Misc.h"

#include "../SDK/ClassId.h"
#include "../SDK/Client.h"
#include "../SDK/ClientClass.h"
#include "../SDK/ClientMode.h"
#include "../SDK/ConVar.h"
#include "../SDK/Cvar.h"
#include "../SDK/Engine.h"
#include "../SDK/EngineTrace.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/FrameStage.h"
#include "../SDK/GameEvent.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/Localize.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/Panorama.h"
#include "../SDK/Platform.h"
#include "../SDK/UserCmd.h"
#include "../SDK/UtlVector.h"
#include "../SDK/Steam.h"
#include "../SDK/Vector.h"
#include "../SDK/WeaponData.h"
#include "../SDK/WeaponId.h"
#include "../SDK/WeaponSystem.h"

#include "../SDK/Utils.h"

#include "../GUI.h"
#include "../Helpers.h"
#include "../Hooks.h"
#include "../GameData.h"

#include "../imguiCustom.h"

struct PreserveKillfeed {
    bool enabled = false;
    bool onlyHeadshots = false;
};

struct OffscreenEnemies : ColorToggle {
    OffscreenEnemies() : ColorToggle{ 1.0f, 0.26f, 0.21f, 1.0f } {}
    HealthBar healthBar;
};

struct MiscConfig {
    MiscConfig() { clanTag[0] = '\0'; }

    KeyBind menuKey{ KeyBind::INSERT };
    bool antiAfkKick{ false };
    bool autoStrafe{ false };
    bool rageStrafe{ false };
    bool bunnyHop{ false };
    bool dzBunnyHop{ false };
    bool customClanTag{ false };
    int tagType{ 0 };
    float tagUpdateInterval{ 0.5f };
    std::vector<std::string> tagAnimationSteps{ "","" };
    bool fastDuck{ false };
    bool moonwalk{ false };
    bool edgejump{ false };
    bool fastshift{ false };
    int fastshiftspeed{ 3 };
    bool slowwalk{ false };
    bool blockbot{ false };
    bool autoPistol{ false };
    bool autoReload{ false };
    bool autoAccept{ false };
    bool radarHack{ false };
    bool revealRanks{ false };
    bool revealMoney{ false };
    bool revealSuspect{ false };
    bool revealVotes{ false };
    bool fixAnimationLOD{ false };
    bool fixBoneMatrix{ false };
    bool fixMovement{ false };
    bool disableModelOcclusion{ false };
    bool nameStealer{ false };
    bool disablePanoramablur{ false };
    bool killMessage{ false };
    bool nadePredict{ false };
    bool fixTabletSignal{ false };
    bool quickHealthshot{ false };
    bool fastPlant{ false };
    bool fastStop{ false };
    bool quickReload{ false };
    bool prepareRevolver{ false };
    bool oppositeHandKnife = false;
    PreserveKillfeed preserveKillfeed;
    char clanTag[16];
    KeyBind edgejumpkey;
    KeyBind fastshiftKey;
    KeyBind slowwalkKey;
    KeyBind blockbotKey;
    bool noscopeCrosshair{ false };
    ColorToggleThickness recoilCrosshair;

    struct SpectatorList {
        bool enabled = false;
        bool noTitleBar = false;
        ImVec2 pos;
        ImVec2 size{ 200.0f, 200.0f };
    };

    SpectatorList spectatorList;

    struct DamageList {
        bool enabled = false;
        bool noTitleBar = false;
        int maxRows = 3;
        ImVec2 pos;
        ImVec2 size{ 300.0f, 200.0f };
    };
    DamageList damageList;

    struct PurchaseList {
        bool enabled = false;
        bool onlyDuringFreezeTime = false;
        bool showPrices = false;
        bool noTitleBar = false;

        enum Mode {
            Details = 0,
            Summary
        };
        int mode = Details;

        ImVec2 pos;
        ImVec2 size{ 400.0f, 200.0f };
    };
    PurchaseList purchaseList;

    struct Watermark {
        bool enabled = false;
    };
    Watermark watermark;
    float aspectratio{ 0 };
    std::string killMessageString{ "Gotcha!" };
    int banColor{ 6 };
    std::string banText{ "Cheater has been permanently banned from official CS:GO servers." };
    ColorToggle3 bombTimer{ 1.0f, 0.55f, 0.0f };
    KeyBind prepareRevolverKey;
    float revolverPrepareTime{ 0.238288879f };
    int hitSound{ 0 };
    int chokedPackets{ 0 };
    KeyBind chokedPacketsKey;
    KeyBind quickHealthshotKey;
    float maxAngleDelta{ 255.0f };
    int killSound{ 0 };
    std::string customKillSound;
    std::string customHitSound;

    struct Reportbot {
        bool enabled = false;
        bool textAbuse = false;
        bool griefing = false;
        bool wallhack = true;
        bool aimbot = true;
        bool other = true;
        int target = 0;
        int delay = 1;
        int rounds = 1;
    } reportbot;

    OffscreenEnemies offscreenEnemies;
    bool autoDisconnect{ false };

    struct EventLog {
        bool enabled = false;
        bool localOnly = false;
        bool chat = false;
        bool console = false;

        bool deathLog = false;
        bool damageLog = false;
        bool bombLog = false;
    } eventLog;
} miscConfig;

bool Misc::shouldRevealMoney() noexcept
{
    return miscConfig.revealMoney;
}

bool Misc::shouldRevealSuspect() noexcept
{
    return miscConfig.revealSuspect;
}

bool Misc::shouldDisableModelOcclusion() noexcept
{
    return miscConfig.disableModelOcclusion;
}

bool Misc::shouldFixBoneMatrix() noexcept
{
    return miscConfig.fixBoneMatrix;
}

bool Misc::isRadarHackOn() noexcept
{
    return miscConfig.radarHack;
}

bool Misc::isMenuKeyPressed() noexcept
{
    return miscConfig.menuKey.isPressed();
}

float Misc::maxAngleDelta() noexcept
{
    return miscConfig.maxAngleDelta;
}

float Misc::aspectRatio() noexcept
{
    return miscConfig.aspectratio;
}

void Misc::edgejump(UserCmd* cmd) noexcept
{
    if (!miscConfig.edgejump || !miscConfig.edgejumpkey.isDown())
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto mt = localPlayer->moveType(); mt == MoveType::LADDER || mt == MoveType::NOCLIP)
        return;

    if ((EnginePrediction::getFlags() & 1) && !(localPlayer->flags() & 1))
        cmd->buttons |= UserCmd::IN_JUMP;
}

void Misc::fastshift(UserCmd* cmd) noexcept
{
    if (!miscConfig.fastshift || !miscConfig.fastshiftKey.isDown())
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon)
        return;

    const auto weaponData = activeWeapon->getWeaponData();
    if (!weaponData)
        return;

    auto weaponIndex = getWeaponIndex(activeWeapon->itemDefinitionIndex());



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

void Misc::slowwalk(UserCmd* cmd) noexcept
{
    if (!miscConfig.slowwalk || !miscConfig.slowwalkKey.isDown())
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

void Misc::updateClanTag(bool tagChanged) noexcept
{
    static std::string clanTag;
    static std::string origTag;
    static bool flip = false;
    static int count = 0;

    if (tagChanged) {
        clanTag = miscConfig.clanTag;
        origTag = miscConfig.clanTag;
        count = 0;
        flip = false;
        if (!clanTag.empty() && clanTag.front() != ' ' && clanTag.back() != ' ') {
            clanTag.push_back(' ');
            origTag.push_back(' ');
        }
        return;
    }
    
    static auto lastTime = 0.0f;

    if (miscConfig.customClanTag) {
        if ((memory->globalVars->realtime - lastTime < miscConfig.tagUpdateInterval) && (!tagChanged || origTag.empty()) && miscConfig.tagType != 5)
            return;

        const auto time = std::time(nullptr);
        const auto localTime = std::localtime(&time);
        auto offset = 0;

        switch (miscConfig.tagType) {
        case 1: //normal anim
            offset = Helpers::utf8SeqLen(clanTag[0]);
            if (offset != -1 && static_cast<std::size_t>(offset) <= clanTag.length())
                std::rotate(clanTag.begin(), clanTag.begin() + offset, clanTag.end());
            break;

        case 2: //auto reverse
            offset = Helpers::utf8SeqLen(origTag[0]);
            if (origTag.length() > clanTag.length() && !flip)
                clanTag = origTag.substr(0, clanTag.length() + offset);
            else if (clanTag.length() == 0)
                flip = false;
            else if (origTag.length() <= clanTag.length() || flip) {
                flip = true;
                clanTag = origTag.substr(0, clanTag.length() - offset);
            }
            break;

        case 3: //reverse auto reverse?
            offset = Helpers::utf8SeqLen(origTag[0]);
            if (origTag.length() > clanTag.length() && !flip)
                clanTag = origTag.substr(0, clanTag.length() + offset);
            else if (clanTag.length() == 0) {
                flip = false;
                count = 0;
            }
            else if (origTag.length() <= clanTag.length() || flip) {
                flip = true;
                clanTag = origTag.substr(0 + (offset * ++count), origTag.length());
            }
            break;

        case 4: //custom animation
            clanTag = miscConfig.tagAnimationSteps[count];
            if (count < miscConfig.tagAnimationSteps.size() - 1)
                count++;
            else
                count = 0;
            break;

        case 5: //clock
            if (memory->globalVars->realtime - lastTime < 1.0f)
                return;

            char s[11];
            s[0] = '\0';
            snprintf(s, sizeof(s), "[%02d:%02d:%02d]", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
            lastTime = memory->globalVars->realtime;
            memory->setClanTag(s, s);
            return;
        }
        lastTime = memory->globalVars->realtime;
        memory->setClanTag(clanTag.c_str(), clanTag.c_str());
    }
}

void Misc::spectatorList() noexcept
{
    if (!miscConfig.spectatorList.enabled)
        return;

    GameData::Lock lock;

    const auto& observers = GameData::observers();

    if (std::ranges::none_of(observers, [](const auto& obs) { return obs.targetIsLocalPlayer; }) && !gui->isOpen())
        return;

    if (miscConfig.spectatorList.pos != ImVec2{}) {
        ImGui::SetNextWindowPos(miscConfig.spectatorList.pos);
        miscConfig.spectatorList.pos = {};
    }

    if (miscConfig.spectatorList.size != ImVec2{}) {
        ImGui::SetNextWindowSize(ImClamp(miscConfig.spectatorList.size, {}, ImGui::GetIO().DisplaySize));
        miscConfig.spectatorList.size = {};
    }

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
    if (!gui->isOpen())
        windowFlags |= ImGuiWindowFlags_NoInputs;
    if (miscConfig.spectatorList.noTitleBar)
        windowFlags |= ImGuiWindowFlags_NoTitleBar;

    if (!gui->isOpen())
        ImGui::PushStyleColor(ImGuiCol_TitleBg, ImGui::GetColorU32(ImGuiCol_TitleBgActive));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
    ImGui::Begin("Spectator list", nullptr, windowFlags);
    ImGui::PopStyleVar();

    if (!gui->isOpen())
        ImGui::PopStyleColor();

    for (const auto& observer : observers) {
        if (!observer.targetIsLocalPlayer)
            continue;

        if (const auto it = std::ranges::find(GameData::players(), observer.playerHandle, &PlayerData::handle); it != GameData::players().cend()) {
            if (const auto texture = it->getAvatarTexture()) {
                const auto textSize = ImGui::CalcTextSize(it->name.c_str());
                ImGui::Image(texture, ImVec2(textSize.y, textSize.y), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 0.3f));
                ImGui::SameLine();
                ImGui::TextWrapped("%s", it->name.c_str());
            }
        }
    }

    ImGui::End();
}

void Misc::damageList(GameEvent* event) noexcept
{
    if (!miscConfig.damageList.enabled)
        return;

    static std::mutex mtx;
    std::scoped_lock _{ mtx };

    static std::unordered_map<int, int> damageCount;

    if (event) {
        switch (fnv::hashRuntime(event->getName())) {
        case fnv::hash("round_start"):
            damageCount.clear();
            break;
        case fnv::hash("player_hurt"):
            if (const auto localPlayerId = localPlayer ? localPlayer->getUserId() : 0; localPlayerId && event->getInt("attacker") == localPlayerId && event->getInt("userid") != localPlayerId)
                if (const auto player = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid"))); player)
                    damageCount[player->handle()] += event->getInt("dmg_health");
            break;
        case fnv::hash("player_death"):
            if (const auto player = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid"))); player)
                damageCount.erase(player->handle());
            break;
        }
    }
    else {
        if ((damageCount.empty() || !interfaces->engine->isInGame()) && !gui->isOpen())
            return;

        if (miscConfig.damageList.pos != ImVec2{}) {
            ImGui::SetNextWindowPos(miscConfig.damageList.pos);
            miscConfig.damageList.pos = {};
        }

        if (miscConfig.damageList.size != ImVec2{}) {
            ImGui::SetNextWindowSize(ImClamp(miscConfig.damageList.size, {}, ImGui::GetIO().DisplaySize));
            miscConfig.damageList.size = {};
        }

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
        if (!gui->isOpen())
            windowFlags |= ImGuiWindowFlags_NoInputs;
        if (miscConfig.damageList.noTitleBar)
            windowFlags |= ImGuiWindowFlags_NoTitleBar;

        if (!gui->isOpen())
            ImGui::PushStyleColor(ImGuiCol_TitleBg, ImGui::GetColorU32(ImGuiCol_TitleBgActive));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
        ImGui::Begin("Damage list", nullptr, windowFlags);
        ImGui::PopStyleVar();

        if (!gui->isOpen())
            ImGui::PopStyleColor();

        std::vector<std::pair<int, int>> damageList(damageCount.cbegin(), damageCount.cend());
        std::ranges::sort(damageList, std::ranges::greater{}, &std::pair<int, int>::second);
        GameData::Lock lock;

        for (std::size_t rowsCount = 0; const auto & [handle, damage] : damageList) {
            if (rowsCount >= miscConfig.damageList.maxRows)
                break;

            if (damage == 0)
                continue;

            if (const auto playerData = GameData::playerByHandle(handle)) {
                if (playerData->health == 0)
                    continue;

                if (const auto texture = playerData->getAvatarTexture()) {
                    const auto textSize = ImGui::CalcTextSize(playerData->name.c_str());
                    ImGui::Image(texture, ImVec2(textSize.y, textSize.y), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 0.3f));
                    ImGui::SameLine();
                    ImGui::TextWrapped("%s: ", playerData->name.c_str());
                    ImGui::SameLine(0.f, 1.f);
                    if (!rowsCount)
                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                    ImGui::TextWrapped("Dmg: %d | Health: %d", damage, playerData->health);
                    if (!rowsCount)
                        ImGui::PopStyleColor();
                    ++rowsCount;
                }
            }
        }
        ImGui::End();
    }
}

static void drawCrosshair(ImDrawList* drawList, const ImVec2& pos, ImU32 color) noexcept
{
    drawList->AddCircleFilled(pos, miscConfig.recoilCrosshair.thickness, color, 360);
    drawList->AddCircle(pos, miscConfig.recoilCrosshair.thickness, IM_COL32_BLACK, 360, 0.5);
}

void Misc::noscopeCrosshair() noexcept
{
    static auto nozoom_crosshair = interfaces->cvar->findVar("weapon_debug_spread_show");

    GameData::Lock lock;
    if (const auto& local = GameData::local(); !miscConfig.noscopeCrosshair || !local.exists || !local.alive || !local.noScope)
        nozoom_crosshair->setValue(0);
    else
        nozoom_crosshair->setValue(3);
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
    if (!miscConfig.recoilCrosshair.asColorToggle().enabled)
        return;

    GameData::Lock lock;
    const auto& localPlayerData = GameData::local();

    if (!localPlayerData.exists || !localPlayerData.alive)
        return;

    if (!localPlayerData.shooting)
        return;

    if (ImVec2 pos; Helpers::worldToScreenPixelAligned(localPlayerData.aimPunch, pos))
        drawCrosshair(drawList, pos, Helpers::calculateColor(miscConfig.recoilCrosshair.asColorToggle().asColor4()));
}

// ImGui::ShadeVertsLinearColorGradientKeepAlpha() modified to do interpolation in HSV
static void shadeVertsHSVColorGradientKeepAlpha(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1)
{
    ImVec2 gradient_extent = gradient_p1 - gradient_p0;
    float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
    ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
    ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;

    ImVec4 col0HSV = ImGui::ColorConvertU32ToFloat4(col0);
    ImVec4 col1HSV = ImGui::ColorConvertU32ToFloat4(col1);
    ImGui::ColorConvertRGBtoHSV(col0HSV.x, col0HSV.y, col0HSV.z, col0HSV.x, col0HSV.y, col0HSV.z);
    ImGui::ColorConvertRGBtoHSV(col1HSV.x, col1HSV.y, col1HSV.z, col1HSV.x, col1HSV.y, col1HSV.z);
    ImVec4 colDelta = col1HSV - col0HSV;

    for (ImDrawVert* vert = vert_start; vert < vert_end; vert++)
    {
        float d = ImDot(vert->pos - gradient_p0, gradient_extent);
        float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);

        float h = col0HSV.x + colDelta.x * t;
        float s = col0HSV.y + colDelta.y * t;
        float v = col0HSV.z + colDelta.z * t;

        ImVec4 rgb;
        ImGui::ColorConvertHSVtoRGB(h, s, v, rgb.x, rgb.y, rgb.z);
        vert->col = (ImGui::ColorConvertFloat4ToU32(rgb) & ~IM_COL32_A_MASK) | (vert->col & IM_COL32_A_MASK);
    }
}

void Misc::watermark(ImDrawList* drawList) noexcept
{
    std::vector<std::reference_wrapper<const PlayerData>> playersOrdered{ GameData::players().begin(), GameData::players().end() };
    std::ranges::sort(playersOrdered, [](const PlayerData& a, const PlayerData& b) {
        // enemies first
        if (a.enemy != b.enemy)
            return a.enemy && !b.enemy;

        return a.handle < b.handle;
        });

    if (!miscConfig.watermark.enabled)
        return;

    for (const PlayerData& player : playersOrdered) {

        //NAME
        const char* name = interfaces->engine->getSteamAPIContext()->steamFriends->getPersonaName();

        //FPS
        static auto fps = 1.0f;
        fps = 0.9f * fps + 0.1f * memory->globalVars->absoluteFrameTime;

        //PING
        const auto ping = GameData::getNetOutgoingLatency();

        //TICKRATE
        const auto tick = 1.f / memory->globalVars->intervalPerTick;

        //TIME
        std::time_t t = std::time(nullptr);
        std::ostringstream time;
        time << std::put_time(std::localtime(&t), ("%H:%M:%S"));

        std::ostringstream format;
        format << "Osiris"
            << " | " << name
            << " | " << (fps != 0.0f ? static_cast<int>(1 / fps) : 0) << " fps";

        if (interfaces->engine->isClientLocalToActiveServer()) {
            format << " | local " << tick << " tick";
        }
        else if (interfaces->engine->isInGame()) {
            auto* pInfo = interfaces->engine->getNetworkChannel();
            if (pInfo) {
                if ((*memory->gameRules)->isValveDS())
                    format << " | official DS " << pInfo->getAddress();
                else
                    format << " | online " << pInfo->getAddress();

                format << " | " << ping << " ms " << tick << " tick";
            }
        }
        else if (interfaces->engine->isConnected())
            format << " | loading";

        format << " | " << time.str().c_str();

        const auto textSize = ImGui::CalcTextSize(format.str().c_str());
        const auto displaySize = ImGui::GetIO().DisplaySize;

        ImRect window{
            displaySize.x - textSize.x - 9.f,
            1.f,
            displaySize.x - 1.f,
            textSize.y + 9.f
        };

        drawList->AddRectFilled(window.Min, window.Max, ImGui::GetColorU32(ImGuiCol_WindowBg), 4);
        const int vertStartIdx = drawList->VtxBuffer.Size;
        drawList->AddRect(window.Min, window.Max, ImGui::GetColorU32(ImGuiCol_TitleBgActive), 4);
        const int vertEndIdx = drawList->VtxBuffer.Size;

        float r, g, b;
        std::tie(r, g, b) = rainbowColor(3.f);
        shadeVertsHSVColorGradientKeepAlpha(drawList, vertStartIdx, vertEndIdx, window.GetTL(), window.GetBR(), ImColor(r, g, b, 1.f), ImGui::GetColorU32(ImGuiCol_TitleBgActive));

        ImVec2 textPos{
            window.GetCenter().x - (textSize.x / 2),
            window.GetCenter().y - (textSize.y / 2)
        };
        drawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), format.str().c_str());
    }
}

void Misc::prepareRevolver(UserCmd* cmd) noexcept
{
    constexpr auto timeToTicks = [](float time) {  return static_cast<int>(0.5f + time / memory->globalVars->intervalPerTick); };
    float revolverPrepareTime{ miscConfig.revolverPrepareTime };

    static float readyTime;
    if (miscConfig.prepareRevolver && localPlayer && (!miscConfig.prepareRevolverKey.isSet() || miscConfig.prepareRevolverKey.isDown())) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && activeWeapon->itemDefinitionIndex() == WeaponId::Revolver) {
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
    if (!miscConfig.fastPlant)
        return;

    if (static auto plantAnywhere = interfaces->cvar->findVar("mp_plant_c4_anywhere"); plantAnywhere->getInt())
        return;

    if (!localPlayer || !localPlayer->isAlive() || (localPlayer->inBombZone() && localPlayer->flags() & 1))
        return;

    if (const auto activeWeapon = localPlayer->getActiveWeapon(); !activeWeapon || activeWeapon->getClientClass()->classId != ClassId::C4)
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

void Misc::fastStop(UserCmd* cmd) noexcept
{
    if (!miscConfig.fastStop)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (localPlayer->moveType() == MoveType::NOCLIP || localPlayer->moveType() == MoveType::LADDER || !(localPlayer->flags() & 1) || cmd->buttons & UserCmd::IN_JUMP)
        return;

    if (cmd->buttons & (UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT | UserCmd::IN_FORWARD | UserCmd::IN_BACK))
        return;
    
    const auto velocity = localPlayer->velocity();
    const auto speed = velocity.length2D();
    if (speed < 15.0f)
        return;
    
    Vector direction = velocity.toAngle();
    direction.y = cmd->viewangles.y - direction.y;

    const auto negatedDirection = Vector::fromAngle(direction) * -speed;
    cmd->forwardmove = negatedDirection.x;
    cmd->sidemove = negatedDirection.y;
}

void Misc::drawBombTimer() noexcept
{
    if (!miscConfig.bombTimer.enabled)
        return;

    GameData::Lock lock;
    
    const auto& plantedC4 = GameData::plantedC4();
    if (plantedC4.blowTime == 0.0f && !gui->isOpen())
        return;

    if (!gui->isOpen()) {
        ImGui::SetNextWindowBgAlpha(0.3f);
    }

    static float windowWidth = 200.0f;
    ImGui::SetNextWindowPos({ (ImGui::GetIO().DisplaySize.x - 200.0f) / 2.0f, 60.0f }, ImGuiCond_Once);
    ImGui::SetNextWindowSize({ windowWidth, 0 }, ImGuiCond_Once);

    if (!gui->isOpen())
        ImGui::SetNextWindowSize({ windowWidth, 0 });

    ImGui::SetNextWindowSizeConstraints({ 0, -1 }, { FLT_MAX, -1 });
    ImGui::Begin("Bomb Timer", nullptr, ImGuiWindowFlags_NoTitleBar | (gui->isOpen() ? 0 : ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration));

    std::ostringstream ss; ss << "Bomb on " << (!plantedC4.bombsite ? 'A' : 'B') << " : " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(plantedC4.blowTime - memory->globalVars->currenttime, 0.0f) << " s";

    ImGui::textUnformattedCentered(ss.str().c_str());

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, Helpers::calculateColor(miscConfig.bombTimer.asColor3()));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
    ImGui::progressBarFullWidth((plantedC4.blowTime - memory->globalVars->currenttime) / plantedC4.timerLength, 5.0f);

    if (plantedC4.defuserHandle != -1) {
        const bool canDefuse = plantedC4.blowTime >= plantedC4.defuseCountDown;

        if (plantedC4.defuserHandle == GameData::local().handle) {
            if (canDefuse) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                ImGui::textUnformattedCentered("You can defuse!");
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                ImGui::textUnformattedCentered("You can not defuse!");
            }
            ImGui::PopStyleColor();
        } else if (const auto defusingPlayer = GameData::playerByHandle(plantedC4.defuserHandle)) {
            std::ostringstream ss; ss << defusingPlayer->name << " is defusing: " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(plantedC4.defuseCountDown - memory->globalVars->currenttime, 0.0f) << " s";

            ImGui::textUnformattedCentered(ss.str().c_str());

            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, canDefuse ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255));
            ImGui::progressBarFullWidth((plantedC4.defuseCountDown - memory->globalVars->currenttime) / plantedC4.defuseLength, 5.0f);
            ImGui::PopStyleColor();
        }
    }

    windowWidth = ImGui::GetCurrentWindow()->SizeFull.x;

    ImGui::PopStyleColor(2);
    ImGui::End();
}

void Misc::stealNames() noexcept
{
    if (!miscConfig.nameStealer)
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

        if (playerInfo.fakeplayer || std::ranges::find(stolenIds, playerInfo.userId) != stolenIds.cend())
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
    blur->setValue(miscConfig.disablePanoramablur);
}

void Misc::quickReload(UserCmd* cmd) noexcept
{
    if (miscConfig.quickReload) {
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

    if (static auto nextChangeTime = 0.0f; nextChangeTime <= memory->globalVars->realtime) {
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

    if (miscConfig.bunnyHop && !(localPlayer->flags() & 1) && localPlayer->moveType() != MoveType::LADDER && !wasLastTimeOnGround)
    {
        cmd->buttons &= ~UserCmd::IN_JUMP;
        if (miscConfig.dzBunnyHop && !(localPlayer->flags() & 1) && !wasLastTimeOnGround)
            cmd->buttons &= ~UserCmd::IN_DUCK;
    }

    wasLastTimeOnGround = localPlayer->flags() & 1;
}

void Misc::fakeBan(bool set) noexcept
{
    static bool shouldSet = false;

    if (set)
        shouldSet = set;

    if (shouldSet && interfaces->engine->isInGame() && localPlayer->isAlive())
    {
        interfaces->engine->clientCmdUnrestricted(("playerchatwheel . \"Cheer! \xe2\x80\xa8" + std::string{ static_cast<char>(miscConfig.banColor + 1) } + miscConfig.banText + "\"").c_str());
        shouldSet = false;
    }
}

void Misc::nadePredict() noexcept
{
    static auto nadeVar{ interfaces->cvar->findVar("cl_grenadepreview") };

    nadeVar->onChangeCallbacks.size = 0;
    nadeVar->setValue(miscConfig.nadePredict);
}

void Misc::quickHealthshot(UserCmd* cmd) noexcept
{
    if (!localPlayer)
        return;

    static bool inProgress{ false };

    if (!miscConfig.quickHealthshotKey.isSet() || miscConfig.quickHealthshotKey.isDown())
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
    if (miscConfig.fixTabletSignal && localPlayer) {
        if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && activeWeapon->getClientClass()->classId == ClassId::Tablet)
            activeWeapon->tabletReceptionIsBlocked() = false;
    }
}

void Misc::killMessage(GameEvent& event) noexcept
{
    if (!miscConfig.killMessage)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    std::string cmd = "say \"";
    cmd += miscConfig.killMessageString;
    cmd += '"';
    interfaces->engine->clientCmdUnrestricted(cmd.c_str());
}

void Misc::fixMovement(UserCmd* cmd, float yaw) noexcept
{
    if (miscConfig.fixMovement) {
        float oldYaw = yaw + (yaw < 0.0f ? 360.0f : 0.0f);
        float newYaw = cmd->viewangles.y + (cmd->viewangles.y < 0.0f ? 360.0f : 0.0f);
        float yawDelta = newYaw < oldYaw ? fabsf(newYaw - oldYaw) : 360.0f - fabsf(newYaw - oldYaw);
        yawDelta = 360.0f - yawDelta;

        const float forwardmove = cmd->forwardmove;
        const float sidemove = cmd->sidemove;
        cmd->forwardmove = std::cos(Helpers::deg2rad(yawDelta)) * forwardmove + std::cos(Helpers::deg2rad(yawDelta + 90.0f)) * sidemove;
        cmd->sidemove = std::sin(Helpers::deg2rad(yawDelta)) * forwardmove + std::sin(Helpers::deg2rad(yawDelta + 90.0f)) * sidemove;
    }
}

void Misc::antiAfkKick(UserCmd* cmd) noexcept
{
    if (miscConfig.antiAfkKick && cmd->commandNumber % 2)
        cmd->buttons |= 1 << 27;
}

void Misc::fixAnimationLOD(FrameStage stage) noexcept
{
#ifdef _WIN32
    if (miscConfig.fixAnimationLOD && stage == FrameStage::RENDER_START) {
        if (!localPlayer)
            return;

        for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
            Entity* entity = interfaces->entityList->getEntity(i);
            if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive()) continue;
            *reinterpret_cast<int*>(entity + 0xA28) = 0;
            *reinterpret_cast<int*>(entity + 0xA30) = memory->globalVars->framecount;
        }
    }
#endif
}

void Misc::autoPistol(UserCmd* cmd) noexcept
{
    if (miscConfig.autoPistol && localPlayer) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && activeWeapon->isPistol() && activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime()) {
            if (activeWeapon->itemDefinitionIndex() == WeaponId::Revolver)
                cmd->buttons &= ~UserCmd::IN_ATTACK2;
            else
                cmd->buttons &= ~UserCmd::IN_ATTACK;
        }
    }
}

void Misc::chokePackets(bool& sendPacket) noexcept
{
    if (!miscConfig.chokedPacketsKey.isSet() || miscConfig.chokedPacketsKey.isDown())
        sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= miscConfig.chokedPackets;
}

void Misc::autoReload(UserCmd* cmd) noexcept
{
    if (miscConfig.autoReload && localPlayer) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && getWeaponIndex(activeWeapon->itemDefinitionIndex()) && !activeWeapon->clip())
            cmd->buttons &= ~(UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2);
    }
}

void Misc::revealRanks(UserCmd* cmd) noexcept
{
    if (miscConfig.revealRanks && cmd->buttons & UserCmd::IN_SCORE)
        interfaces->client->dispatchUserMessage(50, 0, 0, nullptr);
}

static float NormalizeYaw(float offset) {
    if (offset > 180.f) {
        offset = (offset - 180.f) + -180.f;
    }
    if (offset < -180) {
        offset = (offset + 180.f) + 180.f;
    }
    return offset;
}

void Misc::RageStrafe(UserCmd* cmd) noexcept { /* Pasterino Cause I just aint got time to bother*/
    if ((localPlayer->flags() & ONGROUND))
        return;

    float speed = localPlayer->velocity().length2D();
    Vector velocity = localPlayer->velocity();
    float yawVelocity = radiansToDegrees(atan2(velocity.y, velocity.x));
    float velocityDelta = NormalizeYaw(cmd->viewangles.y - yawVelocity);
    static float sideSpeed = interfaces->cvar->findVar("cl_sidespeed")->getFloat();



    if (fabsf(cmd->mousedx > 2)) {

        cmd->sidemove = (cmd->mousedx < 0.f) ? -sideSpeed : sideSpeed;
        return;
    }

    if (cmd->buttons & UserCmd::IN_BACK)
        cmd->viewangles.y -= 180.f;
    else if (cmd->buttons & UserCmd::IN_MOVELEFT)
        cmd->viewangles.y -= 90.f;
    else if (cmd->buttons & UserCmd::IN_MOVERIGHT)
        cmd->viewangles.y += 90.f;

    if (!speed > 0.5f || speed == NAN || speed == INFINITE) {

        cmd->forwardmove = 450.f;
        return;
    }

    cmd->forwardmove = std::clamp(5850.f / speed, -450.f, 450.f);

    if ((cmd->forwardmove < -450.f || cmd->forwardmove > 450.f))
        cmd->forwardmove = 0.f;

    cmd->sidemove = (velocityDelta > 0.0f) ? -sideSpeed : sideSpeed;
    cmd->viewangles.y = NormalizeYaw(cmd->viewangles.y - velocityDelta);
}

void Misc::autoStrafe(UserCmd* cmd) noexcept
{
    if (!localPlayer || miscConfig.rageStrafe)
        return;

    if (localPlayer && miscConfig.autoStrafe && !(localPlayer->flags() & 1) && localPlayer->moveType() != MoveType::NOCLIP) {
        if (cmd->mousedx < 0)
            cmd->sidemove = -450.0f;
        else if (cmd->mousedx > 0)
            cmd->sidemove = 450.0f;
    }
}

void Misc::removeCrouchCooldown(UserCmd* cmd) noexcept
{
    if (miscConfig.fastDuck)
        cmd->buttons |= UserCmd::IN_BULLRUSH;
}

void Misc::moonwalk(UserCmd* cmd) noexcept
{
    if (miscConfig.moonwalk && localPlayer && localPlayer->moveType() != MoveType::LADDER)
        cmd->buttons ^= UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT;
}

void Misc::playHitSound(GameEvent& event) noexcept
{
    if (!miscConfig.hitSound)
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

    if (static_cast<std::size_t>(miscConfig.hitSound - 1) < hitSounds.size())
        interfaces->engine->clientCmdUnrestricted(hitSounds[miscConfig.hitSound - 1]);
    else if (miscConfig.hitSound == 5)
        interfaces->engine->clientCmdUnrestricted(("play " + miscConfig.customHitSound).c_str());
}

void Misc::killSound(GameEvent& event) noexcept
{
    if (!miscConfig.killSound)
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

    if (static_cast<std::size_t>(miscConfig.killSound - 1) < killSounds.size())
        interfaces->engine->clientCmdUnrestricted(killSounds[miscConfig.killSound - 1]);
    else if (miscConfig.killSound == 5)
        interfaces->engine->clientCmdUnrestricted(("play " + miscConfig.customKillSound).c_str());
}

void Misc::purchaseList(GameEvent* event) noexcept
{
    static std::mutex mtx;
    std::scoped_lock _{ mtx };

    struct PlayerPurchases {
        int totalCost;
        std::unordered_map<std::string, int> items;
    };

    static std::unordered_map<int, PlayerPurchases> playerPurchases;
    static std::unordered_map<std::string, int> purchaseTotal;
    static int totalCost;

    static auto freezeEnd = 0.0f;

    if (event) {
        switch (fnv::hashRuntime(event->getName())) {
        case fnv::hash("item_purchase"): {
            if (const auto player = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid"))); player && localPlayer && localPlayer->isOtherEnemy(player)) {
                if (const auto definition = memory->itemSystem()->getItemSchema()->getItemDefinitionByName(event->getString("weapon"))) {
                    auto& purchase = playerPurchases[player->handle()];
                    if (const auto weaponInfo = memory->weaponSystem->getWeaponInfo(definition->getWeaponId())) {
                        purchase.totalCost += weaponInfo->price;
                        totalCost += weaponInfo->price;
                    }
                    const std::string weapon = interfaces->localize->findAsUTF8(definition->getItemBaseName());
                    ++purchaseTotal[weapon];
                    ++purchase.items[weapon];
                }
            }
            break;
        }
        case fnv::hash("round_start"):
            freezeEnd = 0.0f;
            playerPurchases.clear();
            purchaseTotal.clear();
            totalCost = 0;
            break;
        case fnv::hash("round_freeze_end"):
            freezeEnd = memory->globalVars->realtime;
            break;
        }
    } else {
        if (!miscConfig.purchaseList.enabled)
            return;

        if (static const auto mp_buytime = interfaces->cvar->findVar("mp_buytime"); (!interfaces->engine->isInGame() || freezeEnd != 0.0f && memory->globalVars->realtime > freezeEnd + (!miscConfig.purchaseList.onlyDuringFreezeTime ? mp_buytime->getFloat() : 0.0f) || playerPurchases.empty() || purchaseTotal.empty()) && !gui->isOpen())
            return;

        if (miscConfig.purchaseList.pos != ImVec2{}) {
            ImGui::SetNextWindowPos(miscConfig.purchaseList.pos);
            miscConfig.purchaseList.pos = {};
        }

        if (miscConfig.purchaseList.size != ImVec2{}) {
            ImGui::SetNextWindowSize(ImClamp(miscConfig.purchaseList.size, {}, ImGui::GetIO().DisplaySize));
            miscConfig.purchaseList.size = {};
        }

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
        if (!gui->isOpen())
            windowFlags |= ImGuiWindowFlags_NoInputs;
        if (miscConfig.purchaseList.noTitleBar)
            windowFlags |= ImGuiWindowFlags_NoTitleBar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
        ImGui::Begin("Purchases", nullptr, windowFlags);
        ImGui::PopStyleVar();

        if (miscConfig.purchaseList.mode == miscConfig.PurchaseList::Details) {
            GameData::Lock lock;

            for (const auto& [handle, purchases] : playerPurchases) {
                std::string s;
                s.reserve(std::accumulate(purchases.items.begin(), purchases.items.end(), 0, [](int length, const auto& p) { return length + p.first.length() + 2; }));
                for (const auto& purchasedItem : purchases.items) {
                    if (purchasedItem.second > 1)
                        s += std::to_string(purchasedItem.second) + "x ";
                    s += purchasedItem.first + ", ";
                }

                if (s.length() >= 2)
                    s.erase(s.length() - 2);

                if (const auto player = GameData::playerByHandle(handle)) {
                    if (miscConfig.purchaseList.showPrices)
                        ImGui::TextWrapped("%s $%d: %s", player->name.c_str(), purchases.totalCost, s.c_str());
                    else
                        ImGui::TextWrapped("%s: %s", player->name.c_str(), s.c_str());
                }
            }
        } else if (miscConfig.purchaseList.mode == miscConfig.PurchaseList::Summary) {
            for (const auto& purchase : purchaseTotal)
                ImGui::TextWrapped("%d x %s", purchase.second, purchase.first.c_str());

            if (miscConfig.purchaseList.showPrices && totalCost > 0) {
                ImGui::Separator();
                ImGui::TextWrapped("Total: $%d", totalCost);
            }
        }
        ImGui::End();
    }
}

void Misc::oppositeHandKnife(FrameStage stage) noexcept
{
    if (!miscConfig.oppositeHandKnife)
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
    if (!miscConfig.reportbot.enabled)
        return;

    if (!localPlayer)
        return;

    static auto lastReportTime = 0.0f;

    if (lastReportTime + miscConfig.reportbot.delay > memory->globalVars->realtime)
        return;

    if (reportbotRound >= miscConfig.reportbot.rounds)
        return;

    for (int i = 1; i <= interfaces->engine->getMaxClients(); ++i) {
        const auto entity = interfaces->entityList->getEntity(i);

        if (!entity || entity == localPlayer.get())
            continue;

        if (miscConfig.reportbot.target != 2 && (localPlayer->isOtherEnemy(entity) ? miscConfig.reportbot.target != 0 : miscConfig.reportbot.target != 1))
            continue;

        PlayerInfo playerInfo;
        if (!interfaces->engine->getPlayerInfo(i, playerInfo))
            continue;

        if (playerInfo.fakeplayer || std::ranges::find(reportedPlayers, playerInfo.xuid) != reportedPlayers.cend())
            continue;

        std::string report;

        if (miscConfig.reportbot.textAbuse)
            report += "textabuse,";
        if (miscConfig.reportbot.griefing)
            report += "grief,";
        if (miscConfig.reportbot.wallhack)
            report += "wallhack,";
        if (miscConfig.reportbot.aimbot)
            report += "aimbot,";
        if (miscConfig.reportbot.other)
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
    if (!miscConfig.preserveKillfeed.enabled)
        return;

    static auto nextUpdate = 0.0f;

    if (roundStart) {
        nextUpdate = memory->globalVars->realtime + 10.0f;
        return;
    }

    if (nextUpdate > memory->globalVars->realtime)
        return;

    nextUpdate = memory->globalVars->realtime + 2.0f;

    const auto deathNotice = std::uintptr_t(memory->findHudElement(memory->hud, "CCSGO_HudDeathNotice"));
    if (!deathNotice)
        return;

    const auto deathNoticePanel = (*(UIPanel**)(*reinterpret_cast<std::uintptr_t*>(deathNotice WIN32_LINUX(-20 + 88, -32 + 128)) + sizeof(std::uintptr_t)));

    const auto childPanelCount = deathNoticePanel->getChildCount();

    for (int i = 0; i < childPanelCount; ++i) {
        const auto child = deathNoticePanel->getChild(i);
        if (!child)
            continue;

        if (child->hasClass("DeathNotice_Killer") && (!miscConfig.preserveKillfeed.onlyHeadshots || child->hasClass("DeathNoticeHeadShot")))
            child->setAttributeFloat("SpawnTime", memory->globalVars->currenttime);
    }
}

void Misc::voteRevealer(GameEvent& event) noexcept
{
    if (!miscConfig.revealVotes)
        return;

    const auto entity = interfaces->entityList->getEntity(event.getInt("entityid"));
    if (!entity || !entity->isPlayer())
        return;
    
    const auto votedYes = event.getInt("vote_option") == 0;
    const auto isLocal = localPlayer && entity == localPlayer.get();
    const char color = votedYes ? '\x06' : '\x07';

    memory->clientMode->getHudChat()->printf(0, " \x0C\u2022Osiris\u2022 %c%s\x01 voted %c%s\x01", isLocal ? '\x01' : color, isLocal ? "You" : entity->getPlayerName().c_str(), color, votedYes ? "Yes" : "No");
}

void Misc::onVoteStart(const void* data, int size) noexcept
{
    if (!miscConfig.revealVotes)
        return;

    constexpr auto voteName = [](int index) {
        switch (index) {
        case 0: return "Kick";
        case 1: return "Change Level";
        case 6: return "Surrender";
        case 13: return "Start TimeOut";
        default: return "";
        }
    };

    const auto reader = ProtobufReader{ static_cast<const std::uint8_t*>(data), size };
    const auto entityIndex = reader.readInt32(2);

    const auto entity = interfaces->entityList->getEntity(entityIndex);
    if (!entity || !entity->isPlayer())
        return;

    const auto isLocal = localPlayer && entity == localPlayer.get();

    const auto voteType = reader.readInt32(3);
    memory->clientMode->getHudChat()->printf(0, " \x0C\u2022Osiris\u2022 %c%s\x01 call vote (\x06%s\x01)", isLocal ? '\x01' : '\x06', isLocal ? "You" : entity->getPlayerName().c_str(), voteName(voteType));
}

void Misc::onVotePass() noexcept
{
    if (miscConfig.revealVotes)
        memory->clientMode->getHudChat()->printf(0, " \x0C\u2022Osiris\u2022\x01 Vote\x06 PASSED");
}

void Misc::onVoteFailed() noexcept
{
    if (miscConfig.revealVotes)
        memory->clientMode->getHudChat()->printf(0, " \x0C\u2022Osiris\u2022\x01 Vote\x07 FAILED");
}

void Misc::autoDisconnect(GameEvent& event) noexcept
{
    if (!miscConfig.autoDisconnect)
        return;

    if (miscConfig.autoDisconnect)
        interfaces->engine->clientCmdUnrestricted("disconnect");
}

void Misc::drawOffscreenEnemies(ImDrawList* drawList) noexcept
{
    if (!miscConfig.offscreenEnemies.enabled)
        return;

    const auto yaw = Helpers::deg2rad(interfaces->engine->getViewAngles().y);

    GameData::Lock lock;
    for (auto& player : GameData::players()) {
        if ((player.dormant && player.fadingAlpha() == 0.0f) || !player.alive || !player.enemy || player.inViewFrustum)
            continue;

        const auto positionDiff = GameData::local().origin - player.origin;

        auto x = std::cos(yaw) * positionDiff.y - std::sin(yaw) * positionDiff.x;
        auto y = std::cos(yaw) * positionDiff.x + std::sin(yaw) * positionDiff.y;
        if (const auto len = std::sqrt(x * x + y * y); len != 0.0f) {
            x /= len;
            y /= len;
        }

        constexpr auto avatarRadius = 13.0f;
        constexpr auto triangleSize = 10.0f;

        const auto pos = ImGui::GetIO().DisplaySize / 2 + ImVec2{ x, y } * 200;
        const auto trianglePos = pos + ImVec2{ x, y } * (avatarRadius + (miscConfig.offscreenEnemies.healthBar.enabled ? 5 : 3));

        Helpers::setAlphaFactor(player.fadingAlpha());
        const auto white = Helpers::calculateColor(255, 255, 255, 255);
        const auto background = Helpers::calculateColor(0, 0, 0, 80);
        const auto color = Helpers::calculateColor(miscConfig.offscreenEnemies.asColor4());
        const auto healthBarColor = miscConfig.offscreenEnemies.healthBar.type == HealthBar::HealthBased ? Helpers::healthColor(std::clamp(player.health / 100.0f, 0.0f, 1.0f)) : Helpers::calculateColor(miscConfig.offscreenEnemies.healthBar.asColor4());
        Helpers::setAlphaFactor(1.0f);

        const ImVec2 trianglePoints[]{
            trianglePos + ImVec2{  0.4f * y, -0.4f * x } * triangleSize,
            trianglePos + ImVec2{  1.0f * x,  1.0f * y } * triangleSize,
            trianglePos + ImVec2{ -0.4f * y,  0.4f * x } * triangleSize
        };

        drawList->AddConvexPolyFilled(trianglePoints, 3, color);
        drawList->AddCircleFilled(pos, avatarRadius + 1, white & IM_COL32_A_MASK, 40);

        const auto texture = player.getAvatarTexture();

        const bool pushTextureId = drawList->_TextureIdStack.empty() || texture != drawList->_TextureIdStack.back();
        if (pushTextureId)
            drawList->PushTextureID(texture);

        const int vertStartIdx = drawList->VtxBuffer.Size;
        drawList->AddCircleFilled(pos, avatarRadius, white, 40);
        const int vertEndIdx = drawList->VtxBuffer.Size;
        ImGui::ShadeVertsLinearUV(drawList, vertStartIdx, vertEndIdx, pos - ImVec2{ avatarRadius, avatarRadius }, pos + ImVec2{ avatarRadius, avatarRadius }, { 0, 0 }, { 1, 1 }, true);

        if (pushTextureId)
            drawList->PopTextureID();

        if (miscConfig.offscreenEnemies.healthBar.enabled) {
            const auto radius = avatarRadius + 2;
            const auto healthFraction = std::clamp(player.health / 100.0f, 0.0f, 1.0f);

            drawList->AddCircle(pos, radius, background, 40, 3.0f);

            const int vertStartIdx = drawList->VtxBuffer.Size;
            if (healthFraction == 1.0f) { // sometimes PathArcTo is missing one top pixel when drawing a full circle, so draw it with AddCircle
                drawList->AddCircle(pos, radius, healthBarColor, 40, 2.0f);
            } else {
                constexpr float pi = std::numbers::pi_v<float>;
                drawList->PathArcTo(pos, radius - 0.5f, pi / 2 - pi * healthFraction, pi / 2 + pi * healthFraction, 40);
                drawList->PathStroke(healthBarColor, false, 2.0f);
            }
            const int vertEndIdx = drawList->VtxBuffer.Size;

            if (miscConfig.offscreenEnemies.healthBar.type == HealthBar::Gradient)
                shadeVertsHSVColorGradientKeepAlpha(drawList, vertStartIdx, vertEndIdx, pos - ImVec2{ 0.0f, radius }, pos + ImVec2{ 0.0f, radius }, IM_COL32(0, 255, 0, 255), IM_COL32(255, 0, 0, 255));
        }
    }
}

void Misc::autoAccept(const char* soundEntry) noexcept
{
    if (!miscConfig.autoAccept)
        return;

    if (std::strcmp(soundEntry, "UIPanorama.popup_accept_match_beep"))
        return;

    if (const auto idx = memory->registeredPanoramaEvents->find(memory->makePanoramaSymbol("MatchAssistedAccept")); idx != -1) {
        if (const auto eventPtr = memory->registeredPanoramaEvents->memory[idx].value.makeEvent(nullptr))
            interfaces->panoramaUIEngine->accessUIEngine()->dispatchEvent(eventPtr);
    }

#ifdef _WIN32
    auto window = FindWindowW(L"Valve001", NULL);
    FLASHWINFO flash{ sizeof(FLASHWINFO), window, FLASHW_TRAY | FLASHW_TIMERNOFG, 0, 0 };
    FlashWindowEx(&flash);
    ShowWindow(window, SW_RESTORE);
#endif
}

void Misc::fixMouseDelta(UserCmd* cmd) noexcept
{
    if (!cmd)
        return;

    static Vector delta_viewangles{ };
    Vector delta = cmd->viewangles - delta_viewangles;

    delta.x = std::clamp(delta.x, -89.0f, 89.0f);
    delta.y = std::clamp(delta.y, -180.0f, 180.0f);
    delta.z = 0.0f;
    static ConVar* sensitivity;
    if (!sensitivity)
        sensitivity = interfaces->cvar->findVar("sensitivity");;
    if (delta.x != 0.f) {
        static ConVar* m_pitch;

        if (!m_pitch)
            m_pitch = interfaces->cvar->findVar("m_pitch");

        int final_dy = static_cast<int>((delta.x / m_pitch->getFloat()) / sensitivity->getFloat());
        if (final_dy <= 32767) {
            if (final_dy >= -32768) {
                if (final_dy >= 1 || final_dy < 0) {
                    if (final_dy <= -1 || final_dy > 0)
                        final_dy = final_dy;
                    else
                        final_dy = -1;
                }
                else {
                    final_dy = 1;
                }
            }
            else {
                final_dy = 32768;
            }
        }
        else {
            final_dy = 32767;
        }

        cmd->mousedy = static_cast<short>(final_dy);
    }

    if (delta.y != 0.f) {
        static ConVar* m_yaw;

        if (!m_yaw)
            m_yaw = interfaces->cvar->findVar("m_yaw");

        int final_dx = static_cast<int>((delta.y / m_yaw->getFloat()) / sensitivity->getFloat());
        if (final_dx <= 32767) {
            if (final_dx >= -32768) {
                if (final_dx >= 1 || final_dx < 0) {
                    if (final_dx <= -1 || final_dx > 0)
                        final_dx = final_dx;
                    else
                        final_dx = -1;
                }
                else {
                    final_dx = 1;
                }
            }
            else {
                final_dx = 32768;
            }
        }
        else {
            final_dx = 32767;
        }

        cmd->mousedx = static_cast<short>(final_dx);
    }

    delta_viewangles = cmd->viewangles;
}

void Misc::updateEventListeners(bool forceRemove) noexcept
{
    class PurchaseEventListener : public GameEventListener {
    public:
        void fireGameEvent(GameEvent* event) override { purchaseList(event); }
    };

    static PurchaseEventListener listener;
    static bool listenerRegistered = false;

    if (miscConfig.purchaseList.enabled && !listenerRegistered) {
        interfaces->gameEventManager->addListener(&listener, "item_purchase");
        listenerRegistered = true;
    } else if ((!miscConfig.purchaseList.enabled || forceRemove) && listenerRegistered) {
        interfaces->gameEventManager->removeListener(&listener);
        listenerRegistered = false;
    }
}

void Misc::blockbot(UserCmd* cmd) noexcept
{
    if (!miscConfig.blockbot || !miscConfig.blockbotKey.isDown())
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto mt = localPlayer->moveType(); mt == MoveType::LADDER || mt == MoveType::NOCLIP)
        return;

    float bestDistance = 200.0f;
    int plyIndex = -1;

    for (int i = 1; i < interfaces->engine->getMaxClients(); i++)
    {
        Entity* ply = interfaces->entityList->getEntity(i);

        if (!ply)
            continue;

        if (!ply->isAlive() || ply->isDormant() || ply == localPlayer.get())
            continue;

        float distance = localPlayer->origin().distTo(ply->origin());

        if (distance < bestDistance)
        {
            bestDistance = distance;
            plyIndex = i;
        }
    }

    if (plyIndex == -1)
        return;

    Entity* target = interfaces->entityList->getEntity(plyIndex);

    if (!target)
        return;

    if (localPlayer->origin().z - target->origin().z > 20)
    {
        Vector vecForward = target->origin() - localPlayer->origin();

        cmd->forwardmove = ((sin(Helpers::deg2rad(cmd->viewangles.y)) * vecForward.y) + (cos(Helpers::deg2rad(cmd->viewangles.y)) * vecForward.x)) * 450.0f;
        cmd->sidemove = ((cos(Helpers::deg2rad(cmd->viewangles.y)) * -vecForward.y) + (sin(Helpers::deg2rad(cmd->viewangles.y)) * vecForward.x)) * 450.0f;
    }
    else {
        Vector angles = Helpers::calculateRelativeAngle(localPlayer->origin(), target->origin());

        angles.y = angles.y - localPlayer->eyeAngles().y;
        angles.normalize();

        if (angles.y < 0.0f)
            cmd->sidemove = 450.0f;
        else if (angles.y > 0.0f)
            cmd->sidemove = -450.0f;
    }
}

void Misc::runLog(GameEvent* event) noexcept {
    if (!miscConfig.eventLog.enabled)
        return;

    std::string printedText = " ";
    auto eventName = fnv::hashRuntime(event->getName());

    const auto userid = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid")));

    if (!userid || !localPlayer)
        return;

    printedText += userid->getPlayerName() + " ";

    if (eventName == fnv::hash("player_death") || eventName == fnv::hash("player_hurt"))
    {
        const auto attacker = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("attacker")));

        if (!attacker)
            return;

        if (miscConfig.eventLog.localOnly && attacker != localPlayer.get() && userid != localPlayer.get())
            return;

        if (eventName == fnv::hash("player_hurt"))
            printedText += "was hurt by " + attacker->getPlayerName() + " for " + std::to_string(event->getInt("dmg_health")) + " health.";
        else
            printedText += "was killed by " + attacker->getPlayerName() + ".";
    }
    else
    {
        if (miscConfig.eventLog.localOnly && userid != localPlayer.get())
            return;
        switch (eventName) {
        case fnv::hash("bomb_planted"):
            printedText += "planted the bomb.";
            break;
        case fnv::hash("bomb_defused"):
            printedText += "defused the bomb.";
            break;
        case fnv::hash("bomb_beginplant"):
            printedText += "has started planting the bomb.";
            break;
        case fnv::hash("bomb_begindefuse"):
            printedText += "has started defusing the bomb.";
            break;
        }
    }

    if (miscConfig.eventLog.chat)
        memory->clientMode->getHudChat()->printf(0, std::string(" \x06 [Osiris]\x01" + printedText).c_str());

    if (miscConfig.eventLog.console)
        interfaces->engine->clientCmdUnrestricted(std::string("echo [Osiris]" + printedText).c_str());
}

void Misc::updateInput() noexcept
{

}

static bool windowOpen = false;

void Misc::menuBarItem() noexcept
{
    if (ImGui::MenuItem("Misc")) {
        windowOpen = true;
        ImGui::SetWindowFocus("Misc");
        ImGui::SetWindowPos("Misc", { 100.0f, 100.0f });
    }
}

void Misc::tabItem() noexcept
{
    if (ImGui::BeginTabItem("Misc")) {
        drawGUI(true);
        ImGui::EndTabItem();
    }
}

void Misc::drawGUI(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!windowOpen)
            return;
        ImGui::SetNextWindowSize({ 580.0f, 0.0f });
        ImGui::Begin("Misc", &windowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 230.0f);
    ImGui::hotkey("Menu Key", miscConfig.menuKey);
    ImGui::Checkbox("Anti AFK kick", &miscConfig.antiAfkKick);
    ImGui::Checkbox("Auto strafe", &miscConfig.autoStrafe);
    if (miscConfig.autoStrafe) {
        ImGui::SameLine();
        ImGui::Checkbox("Rage strafe", &miscConfig.rageStrafe);
    }
    ImGui::Checkbox("Bunny hop", &miscConfig.bunnyHop);
    if (miscConfig.bunnyHop) {
        ImGui::SameLine();
        ImGui::Checkbox("Bunny hop +squat", &miscConfig.dzBunnyHop);
    };
    ImGui::Checkbox("Fast duck", &miscConfig.fastDuck);
    ImGui::Checkbox("Moonwalk", &miscConfig.moonwalk);
    ImGui::Checkbox("Edge Jump", &miscConfig.edgejump);
    ImGui::SameLine();
    ImGui::PushID("Edge Jump Key");
    ImGui::hotkey("", miscConfig.edgejumpkey);
    ImGui::PopID();
    ImGui::Checkbox("Fastshift", &miscConfig.fastshift);
    if (miscConfig.fastshift) {
        ImGui::SameLine();
        ImGui::PushID("Fastshift Key");
        ImGui::hotkey("", miscConfig.fastshiftKey);
        ImGui::InputInt("Fastshift Speed", &miscConfig.fastshiftspeed, 1, 500);
    }
    ImGui::PopID();
    ImGui::Checkbox("Slowwalk", &miscConfig.slowwalk);
    ImGui::SameLine();
    ImGui::PushID("Slowwalk Key");
    ImGui::hotkey("", miscConfig.slowwalkKey);
    ImGui::PopID();
    ImGui::Checkbox("Block Bot", &miscConfig.blockbot);
    ImGui::SameLine();
    ImGui::PushID("Block Bot Key");
    ImGui::hotkey("", miscConfig.blockbotKey);
    ImGui::PopID();
    ImGui::Checkbox("Noscope crosshair", &miscConfig.noscopeCrosshair);
    ImGuiCustom::colorPicker("Recoil crosshair", miscConfig.recoilCrosshair);
    ImGui::Checkbox("Auto pistol", &miscConfig.autoPistol);
    ImGui::Checkbox("Auto reload", &miscConfig.autoReload);
    ImGui::Checkbox("Auto accept", &miscConfig.autoAccept);
    ImGui::Checkbox("Radar hack", &miscConfig.radarHack);
    ImGui::Checkbox("Reveal ranks", &miscConfig.revealRanks);
    ImGui::Checkbox("Reveal money", &miscConfig.revealMoney);
    ImGui::Checkbox("Reveal suspect", &miscConfig.revealSuspect);
    ImGui::Checkbox("Reveal votes", &miscConfig.revealVotes);

    ImGui::Checkbox("Spectator list", &miscConfig.spectatorList.enabled);
    ImGui::SameLine();

    ImGui::PushID("Spectator list");
    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::Checkbox("No Title Bar", &miscConfig.spectatorList.noTitleBar);
        ImGui::EndPopup();
    }
    ImGui::PopID();

    ImGui::Checkbox("Damage list", &miscConfig.damageList.enabled);
    ImGui::SameLine();

    ImGui::PushID("Damage list");
    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::Checkbox("No Title Bar", &miscConfig.damageList.noTitleBar);
        miscConfig.damageList.maxRows = std::clamp(miscConfig.damageList.maxRows, 1, 64);
        ImGui::PushItemWidth(100.0f);
        ImGui::InputInt("Maximum rows", &miscConfig.damageList.maxRows);
        ImGui::PopItemWidth();
        ImGui::EndPopup();
    }
    ImGui::PopID();

    ImGui::Checkbox("Watermark", &miscConfig.watermark.enabled);
    ImGuiCustom::colorPicker("Offscreen Enemies", miscConfig.offscreenEnemies.asColor4(), &miscConfig.offscreenEnemies.enabled);
    ImGui::SameLine();
    ImGui::PushID("Offscreen Enemies");
    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::Checkbox("Health Bar", &miscConfig.offscreenEnemies.healthBar.enabled);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(95.0f);
        ImGui::Combo("Type", &miscConfig.offscreenEnemies.healthBar.type, "Gradient\0Solid\0Health-based\0");
        if (miscConfig.offscreenEnemies.healthBar.type == HealthBar::Solid) {
            ImGui::SameLine();
            ImGuiCustom::colorPicker("", miscConfig.offscreenEnemies.healthBar.asColor4());
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
    ImGui::Checkbox("Fix animation LOD", &miscConfig.fixAnimationLOD);
    ImGui::Checkbox("Fix bone matrix", &miscConfig.fixBoneMatrix);
    ImGui::Checkbox("Fix movement", &miscConfig.fixMovement);
    ImGui::Checkbox("Disable model occlusion", &miscConfig.disableModelOcclusion);
    ImGui::SliderFloat("Aspect Ratio", &miscConfig.aspectratio, 0.0f, 5.0f, "%.2f");
    ImGui::NextColumn();
    ImGui::Checkbox("Disable HUD blur", &miscConfig.disablePanoramablur);
    ImGui::Checkbox("Custom clantag", &miscConfig.customClanTag);
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(0);

    if (ImGui::InputText("", miscConfig.clanTag, sizeof(miscConfig.clanTag)))
        Misc::updateClanTag(true);
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID("Clantag options");
    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::SameLine();
        ImGui::PushItemWidth(120.0f);
        if (ImGui::Combo("Animation", &miscConfig.tagType, "Static\0Animated\0Auto reverse\0Reverse auto reverse\0Custom\0Clock\0"))
            Misc::updateClanTag(true);
        if (miscConfig.tagType != 0 && miscConfig.tagType != 5) //dont ask for update rate if it is static or clock
            ImGui::SliderFloat("Update rate", &miscConfig.tagUpdateInterval, 0.5f, 3.f, "%.1f");
        if (miscConfig.tagType == 4) {
            for (auto i = 0; i < miscConfig.tagAnimationSteps.size(); i++) {
                ImGui::PushID(i + 100);
                ImGui::InputText("", &miscConfig.tagAnimationSteps[i]);
                ImGui::PopID();
            }

            if (ImGui::Button("+")) {
                miscConfig.tagAnimationSteps.push_back("");
                Misc::updateClanTag(true);
            }

            ImGui::SameLine();

            if (ImGui::Button("-") && miscConfig.tagAnimationSteps.size() > 2) {
                miscConfig.tagAnimationSteps.pop_back();
                Misc::updateClanTag(true);
            }
        }
        ImGui::PopItemWidth();
        ImGui::EndPopup();
    }
    ImGui::PopID();
    ImGui::Checkbox("Kill message", &miscConfig.killMessage);
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(1);
    ImGui::InputText("", &miscConfig.killMessageString);
    ImGui::PopID();
    ImGui::Checkbox("Name stealer", &miscConfig.nameStealer);
    ImGui::PushID(3);
    ImGui::SetNextItemWidth(100.0f);
    ImGui::Combo("", &miscConfig.banColor, "White\0Red\0Purple\0Green\0Light green\0Turquoise\0Light red\0Gray\0Yellow\0Gray 2\0Light blue\0Gray/Purple\0Blue\0Pink\0Dark orange\0Orange\0");
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(4);
    ImGui::InputText("", &miscConfig.banText);
    ImGui::PopID();
    ImGui::SameLine();
    if (ImGui::Button("Setup fake ban"))
        Misc::fakeBan(true);
    ImGui::Checkbox("Fast plant", &miscConfig.fastPlant);
    ImGui::Checkbox("Fast Stop", &miscConfig.fastStop);
    ImGuiCustom::colorPicker("Bomb timer", miscConfig.bombTimer);
    ImGui::Checkbox("Quick reload", &miscConfig.quickReload);
    ImGui::Checkbox("Prepare revolver", &miscConfig.prepareRevolver);
    ImGui::SameLine();
    ImGui::PushID("Prepare revolver Key");
    ImGui::hotkey("", miscConfig.prepareRevolverKey);
    if (miscConfig.prepareRevolver) {
        ImGui::SameLine();
        ImGui::SliderFloat("Prepare revolver time", &miscConfig.revolverPrepareTime, 0.0f, 255.0f, "%.9f");
    }
    ImGui::PopID();
    ImGui::Combo("Hit Sound", &miscConfig.hitSound, "None\0Metal\0Gamesense\0Bell\0Glass\0Custom\0");
    if (miscConfig.hitSound == 5) {
        ImGui::InputText("Hit Sound filename", &miscConfig.customHitSound);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("audio file must be put in csgo/sound/ directory");
    }
    ImGui::PushID(5);
    ImGui::Combo("Kill Sound", &miscConfig.killSound, "None\0Metal\0Gamesense\0Bell\0Glass\0Custom\0");
    if (miscConfig.killSound == 5) {
        ImGui::InputText("Kill Sound filename", &miscConfig.customKillSound);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("audio file must be put in csgo/sound/ directory");
    }
    ImGui::PopID();
    ImGui::SetNextItemWidth(90.0f);
    ImGui::InputInt("Choked packets", &miscConfig.chokedPackets, 1, 5);
    miscConfig.chokedPackets = std::clamp(miscConfig.chokedPackets, 0, 128);
    ImGui::SameLine();
    ImGui::PushID("Choked packets Key");
    ImGui::hotkey("", miscConfig.chokedPacketsKey);
    ImGui::PopID();
    ImGui::Checkbox("Grenade Prediction", &miscConfig.nadePredict);
    ImGui::Checkbox("Fix tablet signal", &miscConfig.fixTabletSignal);
    ImGui::Checkbox("Quick Healthshot", &miscConfig.quickHealthshot);
    if (miscConfig.quickHealthshot) {
        ImGui::SameLine();
        ImGui::hotkey("", miscConfig.quickHealthshotKey);
    }
    ImGui::SetNextItemWidth(120.0f);
    ImGui::SliderFloat("Max angle delta", &miscConfig.maxAngleDelta, 0.0f, 255.0f, "%.2f");
    ImGui::Checkbox("Opposite Hand Knife", &miscConfig.oppositeHandKnife);
    ImGui::Checkbox("Auto disconnect", &miscConfig.autoDisconnect);
    ImGui::Checkbox("Preserve Killfeed", &miscConfig.preserveKillfeed.enabled);
    ImGui::SameLine();

    ImGui::PushID("Preserve Killfeed");
    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::Checkbox("Only Headshots", &miscConfig.preserveKillfeed.onlyHeadshots);
        ImGui::EndPopup();
    }
    ImGui::PopID();

    ImGui::Checkbox("Purchase List", &miscConfig.purchaseList.enabled);
    ImGui::SameLine();

    ImGui::PushID("Purchase List");
    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::SetNextItemWidth(75.0f);
        ImGui::Combo("Mode", &miscConfig.purchaseList.mode, "Details\0Summary\0");
        ImGui::Checkbox("Only During Freeze Time", &miscConfig.purchaseList.onlyDuringFreezeTime);
        ImGui::Checkbox("Show Prices", &miscConfig.purchaseList.showPrices);
        ImGui::Checkbox("No Title Bar", &miscConfig.purchaseList.noTitleBar);
        ImGui::EndPopup();
    }
    ImGui::PopID();

    ImGui::Checkbox("Reportbot", &miscConfig.reportbot.enabled);
    ImGui::SameLine();
    ImGui::PushID("Reportbot");

    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::PushItemWidth(80.0f);
        ImGui::Combo("Target", &miscConfig.reportbot.target, "Enemies\0Allies\0All\0");
        ImGui::InputInt("Delay (s)", &miscConfig.reportbot.delay);
        miscConfig.reportbot.delay = (std::max)(miscConfig.reportbot.delay, 1);
        ImGui::InputInt("Rounds", &miscConfig.reportbot.rounds);
        miscConfig.reportbot.rounds = (std::max)(miscConfig.reportbot.rounds, 1);
        ImGui::PopItemWidth();
        ImGui::Checkbox("Abusive Communications", &miscConfig.reportbot.textAbuse);
        ImGui::Checkbox("Griefing", &miscConfig.reportbot.griefing);
        ImGui::Checkbox("Wall Hacking", &miscConfig.reportbot.wallhack);
        ImGui::Checkbox("Aim Hacking", &miscConfig.reportbot.aimbot);
        ImGui::Checkbox("Other Hacking", &miscConfig.reportbot.other);
        if (ImGui::Button("Reset"))
            Misc::resetReportbot();
        ImGui::EndPopup();
    }
    ImGui::PopID();

    ImGui::Checkbox("Event Log", &miscConfig.eventLog.enabled);
    ImGui::SameLine();
    ImGui::PushID("Event Log");

    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::PushItemWidth(80.0f);

        if (ImGui::Button("Types"))
            ImGui::OpenPopup("Type");

        if (ImGui::BeginPopup("Type")) {
            ImGui::PushItemWidth(80.0f);

            ImGui::Checkbox("Chat", &miscConfig.eventLog.chat);
            ImGui::Checkbox("Console", &miscConfig.eventLog.console);
            ImGui::EndPopup();
        }

        ImGui::Checkbox("Local Only", &miscConfig.eventLog.localOnly);
        ImGui::Checkbox("Death Logs", &miscConfig.eventLog.deathLog);
        ImGui::Checkbox("Damage Logs", &miscConfig.eventLog.damageLog);
        ImGui::Checkbox("Bomb Logs", &miscConfig.eventLog.bombLog);
        ImGui::EndPopup();
    }
    ImGui::PopID();

    if (ImGui::Button("Unhook"))
        hooks->uninstall();

    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

static void from_json(const json& j, ImVec2& v)
{
    read(j, "X", v.x);
    read(j, "Y", v.y);
}

static void from_json(const json& j, MiscConfig::PurchaseList& pl)
{
    read(j, "Enabled", pl.enabled);
    read(j, "Only During Freeze Time", pl.onlyDuringFreezeTime);
    read(j, "Show Prices", pl.showPrices);
    read(j, "No Title Bar", pl.noTitleBar);
    read(j, "Mode", pl.mode);
    read<value_t::object>(j, "Pos", pl.pos);
    read<value_t::object>(j, "Size", pl.size);
}

static void from_json(const json& j, OffscreenEnemies& o)
{
    from_json(j, static_cast<ColorToggle&>(o));

    read<value_t::object>(j, "Health Bar", o.healthBar);
}

static void from_json(const json& j, MiscConfig::SpectatorList& sl)
{
    read(j, "Enabled", sl.enabled);
    read(j, "No Title Bar", sl.noTitleBar);
    read<value_t::object>(j, "Pos", sl.pos);
    read<value_t::object>(j, "Size", sl.size);
}

static void from_json(const json& j, MiscConfig::DamageList& dl)
{
    read(j, "Enabled", dl.enabled);
    read(j, "No Title Bar", dl.noTitleBar);
    read<value_t::object>(j, "Pos", dl.pos);
    read<value_t::object>(j, "Size", dl.size);
    read(j, "Max rows", dl.maxRows);
}

static void from_json(const json& j, MiscConfig::Watermark& o)
{
    read(j, "Enabled", o.enabled);
}

static void from_json(const json& j, PreserveKillfeed& o)
{
    read(j, "Enabled", o.enabled);
    read(j, "Only Headshots", o.onlyHeadshots);
}

static void from_json(const json& j, MiscConfig& m)
{
    read(j, "Menu key", m.menuKey);
    read(j, "Anti AFK kick", m.antiAfkKick);
    read(j, "Auto strafe", m.autoStrafe);
    read(j, "Rage strafe", m.rageStrafe);
    read(j, "Bunny hop", m.bunnyHop);
    read(j, "Bunny hop with a squat", m.dzBunnyHop);
    read(j, "Custom clan tag", m.customClanTag);
    read(j, "Clan tag", m.clanTag, sizeof(m.clanTag));
    read<value_t::object>(j, "Clan tag animation steps", m.tagAnimationSteps);
    read(j, "Fast duck", m.fastDuck);
    read(j, "Moonwalk", m.moonwalk);
    read(j, "Edge Jump", m.edgejump);
    read(j, "Edge Jump Key", m.edgejumpkey);
    read(j, "Fastshift", m.fastshift);
    read(j, "Fastshift key", m.fastshiftKey);
    read(j, "Fastshift speed", m.fastshiftspeed);
    read(j, "Slowwalk", m.slowwalk);
    read(j, "Slowwalk key", m.slowwalkKey);
    read(j, "Block Bot", m.blockbot);
    read(j, "Block Bot Key", m.blockbotKey);
    read(j, "Noscope crosshair", m.noscopeCrosshair);
    read<value_t::object>(j, "Recoil crosshair", m.recoilCrosshair);
    read(j, "Auto pistol", m.autoPistol);
    read(j, "Auto reload", m.autoReload);
    read(j, "Auto accept", m.autoAccept);
    read(j, "Radar hack", m.radarHack);
    read(j, "Reveal ranks", m.revealRanks);
    read(j, "Reveal money", m.revealMoney);
    read(j, "Reveal suspect", m.revealSuspect);
    read(j, "Reveal votes", m.revealVotes);
    read<value_t::object>(j, "Spectator list", m.spectatorList);
    read<value_t::object>(j, "Damage list", m.damageList);
    read<value_t::object>(j, "Watermark", m.watermark);
    read<value_t::object>(j, "Offscreen Enemies", m.offscreenEnemies);
    read(j, "Fix animation LOD", m.fixAnimationLOD);
    read(j, "Fix bone matrix", m.fixBoneMatrix);
    read(j, "Fix movement", m.fixMovement);
    read(j, "Disable model occlusion", m.disableModelOcclusion);
    read(j, "Aspect Ratio", m.aspectratio);
    read(j, "Kill message", m.killMessage);
    read<value_t::string>(j, "Kill message string", m.killMessageString);
    read(j, "Name stealer", m.nameStealer);
    read(j, "Disable HUD blur", m.disablePanoramablur);
    read(j, "Ban color", m.banColor);
    read<value_t::string>(j, "Ban text", m.banText);
    read(j, "Fast plant", m.fastPlant);
    read(j, "Fast Stop", m.fastStop);
    read<value_t::object>(j, "Bomb timer", m.bombTimer);
    read(j, "Quick reload", m.quickReload);
    read(j, "Prepare revolver", m.prepareRevolver);
    read(j, "Prepare revolver key", m.prepareRevolverKey);
    read(j, "Prepare revolver time", m.revolverPrepareTime);
    read(j, "Hit sound", m.hitSound);
    read(j, "Choked packets", m.chokedPackets);
    read(j, "Choked packets key", m.chokedPacketsKey);
    read(j, "Quick healthshot key", m.quickHealthshotKey);
    read(j, "Grenade predict", m.nadePredict);
    read(j, "Fix tablet signal", m.fixTabletSignal);
    read(j, "Quick Healthshot", m.quickHealthshot);
    read(j, "Max angle delta", m.maxAngleDelta);
    read<value_t::string>(j, "Custom Hit Sound", m.customHitSound);
    read(j, "Kill sound", m.killSound);
    read<value_t::string>(j, "Custom Kill Sound", m.customKillSound);
    read<value_t::object>(j, "Purchase List", m.purchaseList);
    read<value_t::object>(j, "Reportbot", m.reportbot);
    read(j, "Opposite Hand Knife", m.oppositeHandKnife);
    read<value_t::object>(j, "Preserve Killfeed", m.preserveKillfeed);
    read<value_t::object>(j, "Event Log", m.eventLog);
    read(j, "Auto disconnect", m.autoDisconnect);

}

static void from_json(const json& j, MiscConfig::Reportbot& r)
{
    read(j, "Enabled", r.enabled);
    read(j, "Target", r.target);
    read(j, "Delay", r.delay);
    read(j, "Rounds", r.rounds);
    read(j, "Abusive Communications", r.textAbuse);
    read(j, "Griefing", r.griefing);
    read(j, "Wall Hacking", r.wallhack);
    read(j, "Aim Hacking", r.aimbot);
    read(j, "Other Hacking", r.other);
}

static void from_json(const json& j, MiscConfig::EventLog& l) {
    read(j, "Enabled", l.enabled);
    read(j, "Local Only", l.localOnly);
    read(j, "Chat", l.chat);
    read(j, "Console", l.console);
    read(j, "Damage Log", l.damageLog);
    read(j, "Death Log", l.deathLog);
    read(j, "Bomb Log", l.bombLog);
}

static void to_json(json& j, const MiscConfig::EventLog& o, const MiscConfig::EventLog& dummy = {}) {
    WRITE("Enabled", enabled);
    WRITE("Local Only", localOnly);
    WRITE("Chat", chat);
    WRITE("Console", console);
    WRITE("Damage Log", damageLog);
    WRITE("Death Log", deathLog);
    WRITE("Bomb Log", bombLog);
}

static void to_json(json& j, const MiscConfig::Reportbot& o, const MiscConfig::Reportbot& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Target", target);
    WRITE("Delay", delay);
    WRITE("Rounds", rounds);
    WRITE("Abusive Communications", textAbuse);
    WRITE("Griefing", griefing);
    WRITE("Wall Hacking", wallhack);
    WRITE("Aim Hacking", aimbot);
    WRITE("Other Hacking", other);
}

static void to_json(json& j, const MiscConfig::PurchaseList& o, const MiscConfig::PurchaseList& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Only During Freeze Time", onlyDuringFreezeTime);
    WRITE("Show Prices", showPrices);
    WRITE("No Title Bar", noTitleBar);
    WRITE("Mode", mode);

    //if (const auto window = ImGui::FindWindowByName("Purchase list")) {
    //    j["Pos"] = window->Pos;
    //    j["Size"] = window->SizeFull;
    //}
}

static void to_json(json& j, const ImVec2& o, const ImVec2& dummy = {})
{
    WRITE("X", x);
    WRITE("Y", y);
}

static void to_json(json& j, const OffscreenEnemies& o, const OffscreenEnemies& dummy = {})
{
    to_json(j, static_cast<const ColorToggle&>(o), dummy);

    WRITE("Health Bar", healthBar);
}

static void to_json(json& j, const MiscConfig::SpectatorList& o, const MiscConfig::SpectatorList& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("No Title Bar", noTitleBar);

    if (const auto window = ImGui::FindWindowByName("Spectator list")) {
        j["Pos"] = window->Pos;
        j["Size"] = window->SizeFull;
    }
}

static void to_json(json& j, const MiscConfig::DamageList& o, const MiscConfig::DamageList& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("No Title Bar", noTitleBar);
    WRITE("Max rows", maxRows);

    if (const auto window = ImGui::FindWindowByName("Damage list")) {
        j["Pos"] = window->Pos;
        j["Size"] = window->SizeFull;
    }
}

static void to_json(json& j, const MiscConfig::Watermark& o, const MiscConfig::Watermark& dummy = {})
{
    WRITE("Enabled", enabled);
}

static void to_json(json& j, const PreserveKillfeed& o, const PreserveKillfeed& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Only Headshots", onlyHeadshots);
}

static void to_json(json& j, const MiscConfig& o)
{
    const MiscConfig dummy;

    WRITE("Menu key", menuKey);
    WRITE("Anti AFK kick", antiAfkKick);
    WRITE("Auto strafe", autoStrafe);
    WRITE("Rage strafe", rageStrafe);
    WRITE("Bunny hop", bunnyHop);
    WRITE("Bunny hop with a squat", dzBunnyHop);
    WRITE("Custom clan tag", customClanTag);

    if (o.clanTag[0])
        j["Clan tag"] = o.clanTag;

    WRITE("Clan tag type", tagType);
    WRITE("Clan tag update interval", tagUpdateInterval);
    WRITE("Clan tag animation steps", tagAnimationSteps);
    WRITE("Fast duck", fastDuck);
    WRITE("Moonwalk", moonwalk);
    WRITE("Edge Jump", edgejump);
    WRITE("Edge Jump Key", edgejumpkey);
    WRITE("Fastshift", fastshift);
    WRITE("Fastshift key", fastshiftKey);
    WRITE("Fastshift speed", fastshiftspeed);
    WRITE("Slowwalk", slowwalk);
    WRITE("Slowwalk key", slowwalkKey);
    WRITE("Block Bot", blockbot);
    WRITE("Block Bot Key", blockbotKey);
    WRITE("Noscope crosshair", noscopeCrosshair);
    WRITE("Recoil crosshair", recoilCrosshair);
    WRITE("Auto pistol", autoPistol);
    WRITE("Auto reload", autoReload);
    WRITE("Auto accept", autoAccept);
    WRITE("Radar hack", radarHack);
    WRITE("Reveal ranks", revealRanks);
    WRITE("Reveal money", revealMoney);
    WRITE("Reveal suspect", revealSuspect);
    WRITE("Reveal votes", revealVotes);
    WRITE("Spectator list", spectatorList);
    WRITE("Damage list", damageList);
    WRITE("Watermark", watermark);
    WRITE("Offscreen Enemies", offscreenEnemies);
    WRITE("Fix animation LOD", fixAnimationLOD);
    WRITE("Fix bone matrix", fixBoneMatrix);
    WRITE("Fix movement", fixMovement);
    WRITE("Disable model occlusion", disableModelOcclusion);
    WRITE("Aspect Ratio", aspectratio);
    WRITE("Kill message", killMessage);
    WRITE("Kill message string", killMessageString);
    WRITE("Name stealer", nameStealer);
    WRITE("Disable HUD blur", disablePanoramablur);
    WRITE("Ban color", banColor);
    WRITE("Ban text", banText);
    WRITE("Fast plant", fastPlant);
    WRITE("Fast Stop", fastStop);
    WRITE("Bomb timer", bombTimer);
    WRITE("Quick reload", quickReload);
    WRITE("Prepare revolver", prepareRevolver);
    WRITE("Prepare revolver key", prepareRevolverKey);
    WRITE("Prepare revolver time", revolverPrepareTime);
    WRITE("Hit sound", hitSound);
    WRITE("Choked packets", chokedPackets);
    WRITE("Choked packets key", chokedPacketsKey);
    WRITE("Quick healthshot key", quickHealthshotKey);
    WRITE("Grenade predict", nadePredict);
    WRITE("Fix tablet signal", fixTabletSignal);
    WRITE("Quick Healthshot", quickHealthshot);
    WRITE("Max angle delta", maxAngleDelta);
    WRITE("Custom Hit Sound", customHitSound);
    WRITE("Kill sound", killSound);
    WRITE("Custom Kill Sound", customKillSound);
    WRITE("Purchase List", purchaseList);
    WRITE("Reportbot", reportbot);
    WRITE("Opposite Hand Knife", oppositeHandKnife);
    WRITE("Preserve Killfeed", preserveKillfeed);
    WRITE("Auto disconnect", autoDisconnect);
    WRITE("Event Log", eventLog);
}

json Misc::toJson() noexcept
{
    json j;
    to_json(j, miscConfig);
    return j;
}

void Misc::fromJson(const json& j) noexcept
{
    from_json(j, miscConfig);
}

void Misc::resetConfig() noexcept
{
    miscConfig = {};
}
