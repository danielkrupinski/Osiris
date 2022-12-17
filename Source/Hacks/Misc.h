#pragma once

#include "../JsonForward.h"
#include "../Memory.h"
#include <InventoryChanger/InventoryChanger.h>
#include <Platform/Macros/IsPlatform.h>
#include <Interfaces/ClientInterfaces.h>
#include <Interfaces/OtherInterfaces.h>
#include <RetSpoof/FunctionInvoker.h>
#include <CSGO/Functions.h>

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
    Misc(const ClientInterfaces& clientInterfaces, const OtherInterfaces& otherInterfaces, const Memory& memory, const helpers::PatternFinder& clientPatternFinder, const helpers::PatternFinder& enginePatternFinder)
        : clientInterfaces{ clientInterfaces }, interfaces{ otherInterfaces }, memory{ memory },
#if IS_WIN32()
        setClanTag{ retSpoofGadgets->engine, enginePatternFinder("\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15").get() },
        submitReport{ retSpoofGadgets->client, clientPatternFinder("\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x28\x8B\x4D\x08").get() }
#elif IS_LINUX()
        setClanTag{ retSpoofGadgets->engine, enginePatternFinder("\xE8????\xE9????\x66\x0F\x1F\x44??\x48\x8B\x7D\xB0").add(1).relativeToAbsolute().get() },
        submitReport{ retSpoofGadgets->client, clientPatternFinder("\x55\x48\x89\xF7\x48\x89\xE5\x41\x57\x41\x56\x41\x55\x41\x54\x53\x48\x89\xD3\x48\x83\xEC\x58").get() }
#endif
    {
#if IS_WIN32()
        demoOrHLTV = ReturnAddress{ clientPatternFinder("\x84\xC0\x75\x09\x38\x05").get() };
        money = clientPatternFinder("\x84\xC0\x75\x0C\x5B").get();
        insertIntoTree = ReturnAddress{ clientPatternFinder("\x56\x52\xFF\x50\x18").add(5).get() };
        demoFileEndReached = ReturnAddress{ clientPatternFinder("\x8B\xC8\x85\xC9\x74\x1F\x80\x79\x10").get() };
#elif IS_LINUX()
        demoOrHLTV = ReturnAddress{ clientPatternFinder("\x0F\xB6\x10\x89\xD0").add(-16).get() };
        money = clientPatternFinder("\x84\xC0\x75\x9E\xB8????\xEB\xB9").get();
        insertIntoTree = ReturnAddress{ clientPatternFinder("\x74\x24\x4C\x8B\x10").add(31).get() };
        demoFileEndReached = ReturnAddress{ clientPatternFinder("\x48\x85\xC0\x0F\x84????\x80\x78\x10?\x74\x7F").get() };
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
    void prepareRevolver(const csgo::Engine& engine, csgo::UserCmd*) noexcept;
    void fastPlant(const csgo::EngineTrace& engineTrace, csgo::UserCmd*) noexcept;
    void fastStop(csgo::UserCmd*) noexcept;
    void drawBombTimer() noexcept;
    void stealNames(const csgo::Engine& engine) noexcept;
    void disablePanoramablur() noexcept;
    void quickReload(csgo::UserCmd*) noexcept;
    bool changeName(const csgo::Engine& engine, bool, const char*, float) noexcept;
    void bunnyHop(csgo::UserCmd*) noexcept;
    void fakeBan(const csgo::Engine& engine, bool = false) noexcept;
    void nadePredict() noexcept;
    void fixTabletSignal() noexcept;
    void killMessage(const csgo::Engine& engine, const csgo::GameEvent& event) noexcept;
    void fixMovement(csgo::UserCmd* cmd, float yaw) noexcept;
    void antiAfkKick(csgo::UserCmd* cmd) noexcept;
    void fixAnimationLOD(const csgo::Engine& engine, csgo::FrameStage stage) noexcept;
    void autoPistol(csgo::UserCmd* cmd) noexcept;
    void chokePackets(const csgo::Engine& engine, bool& sendPacket) noexcept;
    void autoReload(csgo::UserCmd* cmd) noexcept;
    void revealRanks(csgo::UserCmd* cmd) noexcept;
    void autoStrafe(csgo::UserCmd* cmd) noexcept;
    void removeCrouchCooldown(csgo::UserCmd* cmd) noexcept;
    void moonwalk(csgo::UserCmd* cmd) noexcept;
    void playHitSound(const csgo::Engine& engine, const csgo::GameEvent& event) noexcept;
    void killSound(const csgo::Engine& engine, const csgo::GameEvent& event) noexcept;
    void purchaseList(const csgo::Engine& engine, const csgo::GameEvent* event = nullptr) noexcept;
    void oppositeHandKnife(csgo::FrameStage stage) noexcept;
    void runReportbot(const csgo::Engine& engine) noexcept;
    void resetReportbot() noexcept;
    void preserveKillfeed(bool roundStart = false) noexcept;
    void voteRevealer(const csgo::GameEvent& event) noexcept;
    void drawOffscreenEnemies(const csgo::Engine& engine, ImDrawList* drawList) noexcept;
    void autoAccept(const char* soundEntry) noexcept;

    bool isPlayingDemoHook(ReturnAddress returnAddress, std::uintptr_t frameAddress) const;
    const csgo::DemoPlaybackParameters* getDemoPlaybackParametersHook(ReturnAddress returnAddress, const csgo::DemoPlaybackParameters& demoPlaybackParameters) const;
    std::optional<std::pair<csgo::Vector, csgo::Vector>> listLeavesInBoxHook(ReturnAddress returnAddress, std::uintptr_t frameAddress) const;
    void dispatchUserMessageHook(csgo::UserMessageType type, int size, const void* data);

    void updateEventListeners(const EngineInterfaces& engineInterfaces, bool forceRemove = false) noexcept;
    void updateInput() noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem(Visuals& visuals, inventory_changer::InventoryChanger& inventoryChanger, Glow& glow, const EngineInterfaces& engineInterfaces) noexcept;
    void drawGUI(Visuals& visuals, inventory_changer::InventoryChanger& inventoryChanger, Glow& glow, const EngineInterfaces& engineInterfaces, bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;

private:
    void onVoteStart(const void* data, int size) noexcept;
    void onVotePass() noexcept;
    void onVoteFailed() noexcept;

    ClientInterfaces clientInterfaces;
    OtherInterfaces interfaces;
    const Memory& memory;

    ReturnAddress demoOrHLTV;
    std::uintptr_t money;
    ReturnAddress insertIntoTree;
    ReturnAddress demoFileEndReached;
    FunctionInvoker<csgo::SendClanTag> setClanTag;
    FunctionInvoker<csgo::SubmitReport> submitReport;
};
