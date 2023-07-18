#pragma once

#include "../JsonForward.h"
#include "../Memory.h"
#include <InventoryChanger/InventoryChanger.h>
#include <Platform/Macros/IsPlatform.h>
#include <Interfaces/ClientInterfaces.h>
#include <Interfaces/OtherInterfaces.h>
#include <RetSpoof/FunctionInvoker.h>
#include <CSGO/Functions.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/PatternFinder.h>
#include <BytePatterns/ClientPatternFinder.h>
#include <BytePatterns/EnginePatternFinder.h>

namespace csgo { enum class FrameStage; }
namespace csgo { enum class UserMessageType; }

class GameEvent;
struct ImDrawList;
class EngineInterfaces;
class Glow;
class Visuals;
struct DemoPlaybackParameters;

class Misc {
public:
    Misc(const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const OtherInterfaces& otherInterfaces, const Memory& memory, const ClientPatternFinder& clientPatternFinder, const EnginePatternFinder& enginePatternFinder);

    bool isRadarHackOn() noexcept;
    bool isMenuKeyPressed() noexcept;
    float maxAngleDelta() noexcept;
    float aspectRatio() noexcept;

    void edgejump(csgo::UserCmd* cmd) noexcept;
    void slowwalk(csgo::UserCmd* cmd) noexcept;
    void updateClanTag(bool = false) noexcept;
    void spectatorList() noexcept;
    void noscopeCrosshair(ImDrawList* drawlist) noexcept;
    void recoilCrosshair(ImDrawList* drawList) noexcept;
    void watermark() noexcept;
    void prepareRevolver(csgo::UserCmd*) noexcept;
    void fastPlant(csgo::UserCmd*) noexcept;
    void fastStop(csgo::UserCmd*) noexcept;
    void drawBombTimer() noexcept;
    void stealNames() noexcept;
    void disablePanoramablur() noexcept;
    void quickReload(csgo::UserCmd*) noexcept;
    bool changeName(bool, const char*, float) noexcept;
    void bunnyHop(csgo::UserCmd*) noexcept;
    void fakeBan(bool = false) noexcept;
    void nadePredict() noexcept;
    void fixTabletSignal() noexcept;
    void killMessage(const csgo::GameEvent& event) noexcept;
    void fixMovement(csgo::UserCmd* cmd, float yaw) noexcept;
    void antiAfkKick(csgo::UserCmd* cmd) noexcept;
    void fixAnimationLOD(csgo::FrameStage stage) noexcept;
    void autoPistol(csgo::UserCmd* cmd) noexcept;
    void autoReload(csgo::UserCmd* cmd) noexcept;
    void revealRanks(csgo::UserCmd* cmd) noexcept;
    void autoStrafe(csgo::UserCmd* cmd) noexcept;
    void removeCrouchCooldown(csgo::UserCmd* cmd) noexcept;
    void moonwalk(csgo::UserCmd* cmd) noexcept;
    void playHitSound(const csgo::GameEvent& event) noexcept;
    void killSound(const csgo::GameEvent& event) noexcept;
    void purchaseList(const csgo::GameEvent* event = nullptr) noexcept;
    void oppositeHandKnife(csgo::FrameStage stage) noexcept;
    void runReportbot() noexcept;
    void resetReportbot() noexcept;
    void preserveKillfeed(bool roundStart = false) noexcept;
    void voteRevealer(const csgo::GameEvent& event) noexcept;
    void drawOffscreenEnemies(ImDrawList* drawList) noexcept;
    void autoAccept(const char* soundEntry) noexcept;

    bool isPlayingDemoHook(ReturnAddress returnAddress, std::uintptr_t frameAddress) const;
    const csgo::DemoPlaybackParameters* getDemoPlaybackParametersHook(ReturnAddress returnAddress, const csgo::DemoPlaybackParameters& demoPlaybackParameters) const;
    std::optional<std::pair<csgo::Vector, csgo::Vector>> listLeavesInBoxHook(ReturnAddress returnAddress, std::uintptr_t frameAddress) const;
    void dispatchUserMessageHook(csgo::UserMessageType type, int size, const void* data);

    void updateEventListeners(bool forceRemove = false) noexcept;
    void updateInput() noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem(Visuals& visuals, inventory_changer::InventoryChanger& inventoryChanger, Glow& glow) noexcept;
    void drawGUI(Visuals& visuals, inventory_changer::InventoryChanger& inventoryChanger, Glow& glow, bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;

    bool unhook = false;

private:
    void onVoteStart(const void* data, int size) noexcept;
    void onVotePass() noexcept;
    void onVoteFailed() noexcept;

    ClientInterfaces clientInterfaces;
    EngineInterfaces engineInterfaces;
    OtherInterfaces interfaces;
    const Memory& memory;

    ReturnAddress demoOrHLTV;
    std::uintptr_t money;
    ReturnAddress insertIntoTree;
    ReturnAddress demoFileEndReached;
    FunctionInvoker<csgo::SendClanTag> setClanTag;
    FunctionInvoker<csgo::SubmitReport> submitReport;
};
