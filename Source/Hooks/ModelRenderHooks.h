#pragma once

#include <Endpoints.h>
#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo
{
    class matrix3x4;
    struct ModelRenderInfo;
    struct ModelRenderPOD;
}

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

    static void FASTCALL_CONV drawModelExecute(FASTCALL_THIS(csgo::ModelRenderPOD* thisptr), void* ctx, void* state, const csgo::ModelRenderInfo& info, csgo::matrix3x4* customBoneToWorld) noexcept;

private:
    HookType hookImpl;

    csgo::DrawModelExecute originalDrawModelExecute;
};
