#pragma once

#include <cstdint>

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
};

inline GlobalContext globalContext;
