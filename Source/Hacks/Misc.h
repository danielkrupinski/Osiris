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
    Misc(const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const OtherInterfaces& otherInterfaces, const Memory& memory, const PatternFinder& clientPatternFinder, const PatternFinder& enginePatternFinder)
        : clientInterfaces{ clientInterfaces }, engineInterfaces{ engineInterfaces }, interfaces{ otherInterfaces }, memory{ memory },
#if IS_WIN32()
        setClanTag{ retSpoofGadgets->engine, enginePatternFinder("53 56 57 8B DA 8B F9 FF 15"_pat).get() },
        submitReport{ retSpoofGadgets->client, clientPatternFinder("55 8B EC 83 E4 F8 83 EC 28 8B 4D 08"_pat).get() }
#elif IS_LINUX()
        setClanTag{ retSpoofGadgets->engine, enginePatternFinder("E8 ? ? ? ? E9 ? ? ? ? 0F 1F 44 00 00 48 8B 7D B0"_pat).add(1).relativeToAbsolute().get() },
        submitReport{ retSpoofGadgets->client, clientPatternFinder("55 48 89 F7 48 89 E5 41 57 41 56 41 55 41 54 53 48 89 D3 48 83 EC 58"_pat).get() }
#endif
    {
#if IS_WIN32()
        demoOrHLTV = ReturnAddress{ clientPatternFinder("84 C0 75 09 38 05"_pat).get() };
        money = clientPatternFinder("84 C0 75 0C 5B"_pat).get();
        insertIntoTree = ReturnAddress{ clientPatternFinder("56 52 FF 50 18"_pat).add(5).get() };
        demoFileEndReached = ReturnAddress{ clientPatternFinder("8B C8 85 C9 74 1F 80 79 10"_pat).get() };
#elif IS_LINUX()
        demoOrHLTV = clientPatternFinder("84 C0 75 0A 48 8D 05"_pat).asReturnAddress();
        money = clientPatternFinder("84 C0 75 C6 B8 ? ? ? ? EB D9"_pat).get();
        insertIntoTree = clientPatternFinder("41 FF 52 30 89 85"_pat).add(4).asReturnAddress();
        demoFileEndReached = clientPatternFinder("48 85 C0 74 76 80 78 10 00"_pat).asReturnAddress();
#endif
    }

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
