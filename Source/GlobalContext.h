#pragma once

#include <cstdint>

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32()
#include <d3d9.h>
#include <Windows.h>
#elif IS_LINUX()
#include <SDL2/SDL.h>
#endif
#include "Config.h"
#include "EventListener.h"

#include "Hacks/Aimbot.h"
#include "Hacks/Backtrack.h"
#include "Hacks/Glow.h"
#include "Hacks/Visuals.h"
#include "Interfaces/ClientInterfaces.h"
#include "Interfaces/EngineInterfaces.h"
#include "CSGO/CSPlayerInventory.h"
#include "Utils/ReturnAddress.h"
#include "InventoryChanger/InventoryChanger.h"
#include "Hacks/Features.h"

namespace csgo
{
    enum class FrameStage;
    enum class UserMessageType;

    struct ConVarPOD;
    struct DemoPlaybackParameters;
    struct matrix3x4;
    struct ModelRenderInfo;
    struct recvProxyData;
    struct SoundInfo;
    struct UserCmd;
    struct ViewSetup;
}

enum class GlobalContextState {
    NotInitialized,
    Initializing,
    Initialized
};

template <typename PlatformApi>
class GlobalContext {
public:
    GlobalContext(PlatformApi platformApi);

#if IS_LINUX()
    int pollEventHook(SDL_Event* event);
    void swapWindowHook(SDL_Window* window);
#endif

    PlatformApi platformApi;

    std::optional<EventListener> gameEventListener;
    std::optional<EngineInterfacesPODs> engineInterfacesPODs;
    std::optional<Features> features;

    [[nodiscard]] EngineInterfaces getEngineInterfaces() const noexcept
    {
        return EngineInterfaces{ retSpoofGadgets->engine, *engineInterfacesPODs };
    }

    [[nodiscard]] OtherInterfaces getOtherInterfaces() const noexcept
    {
        return OtherInterfaces{ retSpoofGadgets->client, *interfaces };
    }

    void renderFrame();

    GlobalContextState state = GlobalContextState::NotInitialized;

    std::optional<Config> config;
    std::optional<ClientInterfacesPODs> clientInterfaces;
    std::optional<const OtherInterfacesPODs> interfaces;
    std::optional<Helpers::RandomGenerator> randomGenerator;
    std::optional<const Memory> memory;

    void initialize();
};
