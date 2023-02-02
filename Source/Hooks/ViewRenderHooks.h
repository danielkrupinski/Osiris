#pragma once

#include <Endpoints.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo { struct ViewRender; }

void STDCALL_CONV render2dEffectsPreHud(LINUX_ARGS(void* thisptr, ) void* viewSetup) noexcept;
void STDCALL_CONV renderSmokeOverlay(LINUX_ARGS(void* thisptr, ) bool update) noexcept;

template <typename HookImpl>
class ViewRenderHooks {
public:
    void install(csgo::ViewRender* viewRender)
    {
        hookImpl.init(viewRender);
        originalRender2dEffectsPreHud = reinterpret_cast<decltype(originalRender2dEffectsPreHud)>(hookImpl.hookAt(WIN32_LINUX(39, 40), &render2dEffectsPreHud));
        originalRenderSmokeOverlay = reinterpret_cast<decltype(originalRenderSmokeOverlay)>(hookImpl.hookAt(WIN32_LINUX(41, 42), &renderSmokeOverlay));
    }

    void uninstall()
    {
        hookImpl.restore();
    }

    [[nodiscard]] auto getOriginalRender2dEffectsPreHud() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalRender2dEffectsPreHud };
    }

    [[nodiscard]] auto getOriginalRenderSmokeOverlay() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalRenderSmokeOverlay };
    }

private:
    HookImpl hookImpl;

    void (THISCALL_CONV* originalRender2dEffectsPreHud)(csgo::ViewRender* thisptr, void* viewSetup);
    void (THISCALL_CONV* originalRenderSmokeOverlay)(csgo::ViewRender* thisptr, bool preViewModel);
};
