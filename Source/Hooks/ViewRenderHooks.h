#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct ViewRender; }

class ViewRenderHooks {
public:
    explicit ViewRenderHooks(const VmtLengthCalculator& vmtLengthCalculator)
        : hookImpl{ vmtLengthCalculator }
    {
    }

    void install(csgo::ViewRender* viewRender)
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(viewRender));
        originalRender2dEffectsPreHud = reinterpret_cast<decltype(originalRender2dEffectsPreHud)>(hookImpl.hook(WIN32_LINUX(39, 40), std::uintptr_t(&render2dEffectsPreHud)));
        originalRenderSmokeOverlay = reinterpret_cast<decltype(originalRenderSmokeOverlay)>(hookImpl.hook(WIN32_LINUX(41, 42), std::uintptr_t(&renderSmokeOverlay)));
    }

    void uninstall(csgo::ViewRender* viewRender)
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(viewRender));
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
    HookType hookImpl;

    void (THISCALL_CONV* originalRender2dEffectsPreHud)(csgo::ViewRender* thisptr, void* viewSetup);
    void (THISCALL_CONV* originalRenderSmokeOverlay)(csgo::ViewRender* thisptr, bool preViewModel);
};
