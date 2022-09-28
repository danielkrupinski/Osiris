#pragma once

#include <cstdint>

#ifdef _WIN32
#include <Windows.h>
#else
#include <SDL2/SDL.h>
#endif

#include "EventListener.h"

class matrix3x4;
struct ModelRenderInfo;
struct UserCmd;

namespace csgo { enum class FrameStage; }

class GlobalContext {
public:
    bool createMoveHook(float inputSampleTime, UserCmd* cmd);
    void doPostScreenEffectsHook(void* param);
    float getViewModelFovHook();
    void drawModelExecuteHook(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld);
    bool svCheatsGetBoolHook(void* _this, std::uintptr_t returnAddress);
    void frameStageNotifyHook(csgo::FrameStage stage);

#ifdef _WIN32
    LRESULT wndProcHook(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
#else
    int pollEventHook(SDL_Event* event);
#endif

    std::optional<EventListener> gameEventListener;

private:
    enum class State {
        NotInitialized,
        Initializing,
        Initialized
    };

    State state = State::NotInitialized;
};

inline std::optional<GlobalContext> globalContext;
