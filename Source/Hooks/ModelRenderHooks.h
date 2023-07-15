#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo
{
    struct matrix3x4;
    struct ModelRenderInfo;
    struct ModelRenderPOD;
}

class ModelRenderHooks {
public:
    explicit ModelRenderHooks(const VmtLengthCalculator& vmtLengthCalculator)
        : hookImpl{ vmtLengthCalculator }
    {
    }

    void install(csgo::ModelRenderPOD* modelRender)
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(modelRender));
        originalDrawModelExecute = reinterpret_cast<decltype(originalDrawModelExecute)>(hookImpl.hook(21, std::uintptr_t(&drawModelExecute)));
    }

    void uninstall(csgo::ModelRenderPOD* modelRender)
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(modelRender));
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
