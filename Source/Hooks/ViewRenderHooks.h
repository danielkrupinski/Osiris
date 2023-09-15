#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct ViewRender; }

class ViewRenderHooks : public RefCountedHook<ViewRenderHooks> {
public:
    explicit ViewRenderHooks(const VmtLengthCalculator& vmtLengthCalculator, csgo::ViewRender* viewRender)
        : hookImpl{ vmtLengthCalculator }, viewRender{ viewRender }
    {
    }

    [[nodiscard]] auto getOriginalRender2dEffectsPreHud() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalRender2dEffectsPreHud };
    }

    [[nodiscard]] auto getOriginalRenderSmokeOverlay() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalRenderSmokeOverlay };
    }
    
    static void FASTCALL_CONV render2dEffectsPreHud(FASTCALL_THIS(csgo::ViewRender* thisptr), void* viewSetup) noexcept;
    static void FASTCALL_CONV renderSmokeOverlay(FASTCALL_THIS(csgo::ViewRender* thisptr), bool update) noexcept;

private:
    void install()
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(viewRender));
        originalRender2dEffectsPreHud = hookImpl.hook(WIN32_LINUX(39, 40), &render2dEffectsPreHud);
        originalRenderSmokeOverlay = hookImpl.hook(WIN32_LINUX(41, 42), &renderSmokeOverlay);
    }

    void uninstall()
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(viewRender));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(viewRender));
    }

    friend RefCountedHook;

    HookType hookImpl;
    csgo::ViewRender* viewRender;

    void (THISCALL_CONV* originalRender2dEffectsPreHud)(csgo::ViewRender* thisptr, void* viewSetup);
    void (THISCALL_CONV* originalRenderSmokeOverlay)(csgo::ViewRender* thisptr, bool preViewModel);
};
