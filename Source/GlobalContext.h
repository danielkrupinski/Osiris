#pragma once

#include <cstdint>

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32()
#include <d3d9.h>
#include <Windows.h>
#else
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
    class matrix3x4;
    struct ModelRenderInfo;
    struct recvProxyData;
    struct SoundInfo;
    struct UserCmd;
    struct ViewSetup;
}

class GlobalContext {
public:
    GlobalContext();

#if IS_WIN32()
    LRESULT wndProcHook(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
    HRESULT presentHook(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion);
    HRESULT resetHook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params);

#else
    int pollEventHook(SDL_Event* event);
    void swapWindowHook(SDL_Window* window);
#endif

    void viewModelSequenceNetvarHook(csgo::recvProxyData* data, void* outStruct, void* arg3);
    void spottedHook(csgo::recvProxyData* data, void* outStruct, void* arg3);

    void fireGameEventCallback(csgo::GameEventPOD* eventPointer);

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

    enum class State {
        NotInitialized,
        Initializing,
        Initialized
    };

    State state = State::NotInitialized;

    std::optional<Config> config;
    std::optional<ClientInterfacesPODs> clientInterfaces;
    std::optional<const OtherInterfacesPODs> interfaces;
    std::optional<Helpers::RandomGenerator> randomGenerator;
    std::optional<const Memory> memory;

private:
    void initialize();
};
