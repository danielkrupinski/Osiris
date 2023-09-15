#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo
{
    struct matrix3x4;
    struct ModelRenderInfo;
    struct ModelRenderPOD;
}

class ModelRenderHooks : public RefCountedHook<ModelRenderHooks> {
public:
    explicit ModelRenderHooks(const VmtLengthCalculator& vmtLengthCalculator, csgo::ModelRenderPOD* modelRender)
        : hookImpl{ vmtLengthCalculator }, modelRender{ modelRender }
    {
    }

    [[nodiscard]] auto getOriginalDrawModelExecute() const
    {
        return FunctionInvoker{ retSpoofGadgets->engine, originalDrawModelExecute };
    }

    static void FASTCALL_CONV drawModelExecute(FASTCALL_THIS(csgo::ModelRenderPOD* thisptr), void* ctx, void* state, const csgo::ModelRenderInfo& info, csgo::matrix3x4* customBoneToWorld) noexcept;

private:
    void install()
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(modelRender));
        originalDrawModelExecute = hookImpl.hook(21, &drawModelExecute);
    }

    void uninstall()
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(modelRender));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(modelRender));
    }

    friend RefCountedHook;

    HookType hookImpl;
    csgo::ModelRenderPOD* modelRender;

    csgo::DrawModelExecute originalDrawModelExecute;
};
