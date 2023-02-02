#pragma once

#include <Endpoints.h>
#include <Platform/Macros/CallingConventions.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo
{
    class matrix3x4;
    struct ModelRenderInfo;
    struct ModelRenderPOD;
}

void STDCALL_CONV drawModelExecute(LINUX_ARGS(void* thisptr, ) void* ctx, void* state, const csgo::ModelRenderInfo& info, csgo::matrix3x4* customBoneToWorld) noexcept;

template <typename HookImpl>
class ModelRenderHooks {
public:
    void install(csgo::ModelRenderPOD* modelRender)
    {
        hookImpl.init(modelRender);
        originalDrawModelExecute = reinterpret_cast<decltype(originalDrawModelExecute)>(hookImpl.hookAt(21, &drawModelExecute));
    }

    void uninstall()
    {
        hookImpl.restore();
    }

    [[nodiscard]] auto getOriginalDrawModelExecute() const
    {
        return FunctionInvoker{ retSpoofGadgets->engine, originalDrawModelExecute };
    }

private:
    HookImpl hookImpl;

    void (THISCALL_CONV* originalDrawModelExecute)(csgo::ModelRenderPOD* thisptr, void* ctx, void* state, const csgo::ModelRenderInfo* info, csgo::matrix3x4* customBoneToWorld);
};
