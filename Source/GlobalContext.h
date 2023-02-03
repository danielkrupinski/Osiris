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

    bool createMoveHook(csgo::ClientMode* thisptr, float inputSampleTime, csgo::UserCmd* cmd);
    void doPostScreenEffectsHook(csgo::ClientMode* thisptr, void* param);
    float getViewModelFovHook(csgo::ClientMode* thisptr);
    void drawModelExecuteHook(csgo::ModelRenderPOD* thisptr, void* ctx, void* state, const csgo::ModelRenderInfo& info, csgo::matrix3x4* customBoneToWorld);
    int svCheatsGetIntHook(csgo::ConVarPOD* thisptr, ReturnAddress returnAddress);
    void frameStageNotifyHook(csgo::ClientPOD* thisptr, csgo::FrameStage stage);
    int emitSoundHook(csgo::EngineSoundPOD* thisptr, void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const csgo::Vector& origin, const csgo::Vector& direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams);
    bool shouldDrawFogHook(csgo::ClientMode* thisptr, ReturnAddress returnAddress);
    bool shouldDrawViewModelHook(csgo::ClientMode* thisptr);
    void lockCursorHook(csgo::SurfacePOD* thisptr);
    void setDrawColorHook(csgo::SurfacePOD* thisptr, int r, int g, int b, int a, ReturnAddress returnAddress);
    void overrideViewHook(csgo::ClientMode* thisptr, csgo::ViewSetup* setup);
    int dispatchSoundHook(csgo::SoundInfo& soundInfo);
    void render2dEffectsPreHudHook(csgo::ViewRender* thisptr, void* viewSetup);
    const csgo::DemoPlaybackParameters* getDemoPlaybackParametersHook(csgo::EnginePOD* thisptr, ReturnAddress returnAddress);
    bool dispatchUserMessageHook(csgo::ClientPOD* thisptr, csgo::UserMessageType type, int passthroughFlags, int size, const void* data);
    bool isPlayingDemoHook(csgo::EnginePOD* thisptr, ReturnAddress returnAddress, std::uintptr_t frameAddress);
    void updateColorCorrectionWeightsHook(csgo::ClientMode* thisptr);
    float getScreenAspectRatioHook(csgo::EnginePOD* thisptr, int width, int height);
    void renderSmokeOverlayHook(csgo::ViewRender* thisptr, bool update);
    double getArgAsNumberHook(csgo::PanoramaMarshallHelperPOD* thisptr, void* params, int index, ReturnAddress returnAddress);
    const char* getArgAsStringHook(csgo::PanoramaMarshallHelperPOD* thisptr, void* params, int index, ReturnAddress returnAddress);
    void setResultIntHook(csgo::PanoramaMarshallHelperPOD* thisptr, void* params, int result, ReturnAddress returnAddress);
    unsigned getNumArgsHook(csgo::PanoramaMarshallHelperPOD* thisptr, void* params, ReturnAddress returnAddress);
    void updateInventoryEquippedStateHook(csgo::InventoryManagerPOD* thisptr, std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap);
    void soUpdatedHook(csgo::CSPlayerInventoryPOD* thisptr, csgo::SOID owner, csgo::SharedObjectPOD* object, int event);
    int listLeavesInBoxHook(void* thisptr, const csgo::Vector& mins, const csgo::Vector& maxs, unsigned short* list, int listMax, ReturnAddress returnAddress, std::uintptr_t frameAddress);

#if IS_WIN32()
    void* allocKeyValuesMemoryHook(csgo::KeyValuesSystemPOD* thisptr, int size, ReturnAddress returnAddress);

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

    std::optional<EngineInterfacesPODs> engineInterfacesPODs; // TODO: make private

    std::optional<Features> features;

    [[nodiscard]] EngineInterfaces getEngineInterfaces() const noexcept
    {
        return EngineInterfaces{ retSpoofGadgets->engine, *engineInterfacesPODs };
    }

    [[nodiscard]] OtherInterfaces getOtherInterfaces() const noexcept
    {
        return OtherInterfaces{ retSpoofGadgets->client, *interfaces };
    }

private:
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
};
