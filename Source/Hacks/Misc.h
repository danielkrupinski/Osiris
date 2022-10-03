#pragma once

#include "../JsonForward.h"
#include "../Interfaces.h"
#include "../Memory.h"

namespace csgo { enum class FrameStage; }

class GameEvent;
struct ImDrawList;
struct UserCmd;

namespace Misc
{
    bool shouldRevealMoney() noexcept;
    bool shouldRevealSuspect() noexcept;
    bool shouldDisableModelOcclusion() noexcept;
    bool shouldFixBoneMatrix() noexcept;
    bool isRadarHackOn() noexcept;
    bool isMenuKeyPressed() noexcept;
    float maxAngleDelta() noexcept;
    float aspectRatio() noexcept;

    void edgejump(UserCmd* cmd) noexcept;
    void slowwalk(UserCmd* cmd) noexcept;
    void updateClanTag(const Memory& memory, bool = false) noexcept;
    void spectatorList() noexcept;
    void noscopeCrosshair(const Memory& memory, ImDrawList* drawlist) noexcept;
    void recoilCrosshair(const Memory& memory, ImDrawList* drawList) noexcept;
    void watermark(const Memory& memory) noexcept;
    void prepareRevolver(Engine& engine, const Memory& memory, UserCmd*) noexcept;
    void fastPlant(EngineTrace& engineTrace, const Interfaces& interfaces, UserCmd*) noexcept;
    void fastStop(UserCmd*) noexcept;
    void drawBombTimer(const Memory& memory) noexcept;
    void stealNames(Engine& engine, const ClientInterfaces& clientInterfaces, const Interfaces& interfaces, const Memory& memory) noexcept;
    void disablePanoramablur(const Interfaces& interfaces) noexcept;
    void quickReload(const ClientInterfaces& clientInterfaces, const Interfaces& interfaces, UserCmd*) noexcept;
    bool changeName(Engine& engine, const Interfaces& interfaces, const Memory& memory, bool, const char*, float) noexcept;
    void bunnyHop(UserCmd*) noexcept;
    void fakeBan(Engine& engine, const Interfaces& interfaces, const Memory& memory, bool = false) noexcept;
    void nadePredict(const Interfaces& interfaces) noexcept;
    void fixTabletSignal() noexcept;
    void killMessage(Engine& engine, GameEvent& event) noexcept;
    void fixMovement(UserCmd* cmd, float yaw) noexcept;
    void antiAfkKick(UserCmd* cmd) noexcept;
    void fixAnimationLOD(Engine& engine, const ClientInterfaces& clientInterfaces, const Memory& memory, csgo::FrameStage stage) noexcept;
    void autoPistol(const Memory& memory, UserCmd* cmd) noexcept;
    void chokePackets(Engine& engine, bool& sendPacket) noexcept;
    void autoReload(UserCmd* cmd) noexcept;
    void revealRanks(const ClientInterfaces& clientInterfaces, UserCmd* cmd) noexcept;
    void autoStrafe(UserCmd* cmd) noexcept;
    void removeCrouchCooldown(UserCmd* cmd) noexcept;
    void moonwalk(UserCmd* cmd) noexcept;
    void playHitSound(Engine& engine, GameEvent& event) noexcept;
    void killSound(Engine& engine, GameEvent& event) noexcept;
    void purchaseList(Engine& engine, const ClientInterfaces& clientInterfaces, const Interfaces& interfaces, const Memory& memory, GameEvent* event = nullptr) noexcept;
    void oppositeHandKnife(const Interfaces& interfaces, csgo::FrameStage stage) noexcept;
    void runReportbot(Engine& engine, const ClientInterfaces& clientInterfaces, const Interfaces& interfaces, const Memory& memory) noexcept;
    void resetReportbot() noexcept;
    void preserveKillfeed(const Memory& memory, bool roundStart = false) noexcept;
    void voteRevealer(const ClientInterfaces& clientInterfaces, const Interfaces& interfaces, const Memory& memory, GameEvent& event) noexcept;
    void onVoteStart(const ClientInterfaces& clientInterfaces, const Interfaces& interfaces, const Memory& memory, const void* data, int size) noexcept;
    void onVotePass(const Memory& memory) noexcept;
    void onVoteFailed(const Memory& memory) noexcept;
    void drawOffscreenEnemies(Engine& engine, const Memory& memory, ImDrawList* drawList) noexcept;
    void autoAccept(const Interfaces& interfaces, const Memory& memory, const char* soundEntry) noexcept;

    void updateEventListeners(const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const Interfaces& interfaces, const Memory& memory, bool forceRemove = false) noexcept;
    void updateInput() noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem(Engine& engine, const Interfaces& interfaces, const Memory& memory) noexcept;
    void drawGUI(Engine& engine, const Interfaces& interfaces, const Memory& memory, bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;
}
