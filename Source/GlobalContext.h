#pragma once

#include <cstdint>

#include <Platform/IsPlatform.h>

#if IS_WIN32()
#include <d3d9.h>
#include <Windows.h>
#else
#include <SDL2/SDL.h>
#endif
#include "Config.h"
#include "EventListener.h"

#include "Hacks/Aimbot.h"
#include "Hacks/Visuals.h"
#include "Interfaces/ClientInterfaces.h"
#include "Interfaces/EngineInterfaces.h"
#include "SDK/CSPlayerInventory.h"

struct DemoPlaybackParameters;
class matrix3x4;
struct ModelRenderInfo;
struct recvProxyData;
struct SoundInfo;
struct UserCmd;
struct ViewSetup;

namespace csgo
{
    enum class FrameStage;
    enum class UserMessageType;
}

class GlobalContext {
public:
    GlobalContext();

    bool createMoveHook(float inputSampleTime, UserCmd* cmd);
    void doPostScreenEffectsHook(void* param);
    float getViewModelFovHook();
    void drawModelExecuteHook(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld);
    bool svCheatsGetBoolHook(void* _this, std::uintptr_t returnAddress);
    void frameStageNotifyHook(csgo::FrameStage stage);
    int emitSoundHook(void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const Vector& origin, const Vector& direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams);
    bool shouldDrawFogHook(std::uintptr_t returnAddress);
    bool shouldDrawViewModelHook();
    void lockCursorHook();
    void setDrawColorHook(int r, int g, int b, int a, std::uintptr_t returnAddress);
    void overrideViewHook(ViewSetup* setup);
    int dispatchSoundHook(SoundInfo& soundInfo);
    void render2dEffectsPreHudHook(void* viewSetup);
    const DemoPlaybackParameters* getDemoPlaybackParametersHook(std::uintptr_t returnAddress);
    bool dispatchUserMessageHook(csgo::UserMessageType type, int passthroughFlags, int size, const void* data);
    bool isPlayingDemoHook(std::uintptr_t returnAddress, std::uintptr_t frameAddress);
    void updateColorCorrectionWeightsHook();
    float getScreenAspectRatioHook(int width, int height);
    void renderSmokeOverlayHook(bool update);
    double getArgAsNumberHook(void* params, int index, std::uintptr_t returnAddress);
    const char* getArgAsStringHook(void* params, int index, std::uintptr_t returnAddress);
    void setResultIntHook(void* params, int result, std::uintptr_t returnAddress);
    unsigned getNumArgsHook(void* params, std::uintptr_t returnAddress);
    void updateInventoryEquippedStateHook(std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap);
    void soUpdatedHook(SOID owner, csgo::pod::SharedObject* object, int event);
    int listLeavesInBoxHook(const Vector& mins, const Vector& maxs, unsigned short* list, int listMax, std::uintptr_t returnAddress, std::uintptr_t frameAddress);

#if IS_WIN32()
    void* allocKeyValuesMemoryHook(int size, std::uintptr_t returnAddress);

    LRESULT wndProcHook(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
    HRESULT presentHook(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion);
    HRESULT resetHook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params);

#else
    int pollEventHook(SDL_Event* event);
    void swapWindowHook(SDL_Window* window);
#endif

    void viewModelSequenceNetvarHook(recvProxyData& data, void* outStruct, void* arg3);

    void fireGameEventCallback(csgo::pod::GameEvent* eventPointer);

    std::optional<EventListener> gameEventListener;

    std::optional<EngineInterfacesPODs> engineInterfacesPODs; // TODO: make private

    std::optional<Visuals> visuals;

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
    std::optional<Aimbot> aimbot;
};

inline std::optional<GlobalContext> globalContext;
