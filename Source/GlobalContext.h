#pragma once

#include <cstdint>

#ifdef _WIN32
#include <Windows.h>
#else
#include <SDL2/SDL.h>
#endif
#include "Config.h"
#include "EventListener.h"

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

#ifdef _WIN32
    LRESULT wndProcHook(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
    HRESULT presentHook(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion);
#else
    int pollEventHook(SDL_Event* event);
    void swapWindowHook(SDL_Window* window);
#endif

    void viewModelSequenceNetvarHook(recvProxyData& data, void* outStruct, void* arg3);

    void fireGameEventCallback(GameEvent* event);

    std::optional<EventListener> gameEventListener;

    std::optional<EngineInterfaces> engineInterfaces; // TODO: make private

private:
    void renderFrame();

    enum class State {
        NotInitialized,
        Initializing,
        Initialized
    };

    State state = State::NotInitialized;

    std::optional<Config> config;
    std::optional<ClientInterfaces> clientInterfaces;

};

inline std::optional<GlobalContext> globalContext;
