#pragma once

#include <cstdint>

#include <CS2/Classes/CViewRender.h>
#include <Vmt/VmtLengthCalculator.h>
#include <Vmt/VmtSwapper.h>

extern "C" void ViewRenderHook_onRenderStart_asm(cs2::CViewRender* thisptr) noexcept;

class ViewRenderHook {
public:
    ViewRenderHook(cs2::CViewRender** viewRender, const VmtLengthCalculator& vmtLengthCalculator) noexcept
        : viewRender{viewRender}
        , vmtLengthCalculator{vmtLengthCalculator}
    {
    }

    [[nodiscard]] cs2::CViewRender::OnRenderStart* getOriginalOnRenderStart() const noexcept
    {
        return originalOnRenderStart;
    }

    void uninstall() const noexcept
    {
        if (viewRender && *viewRender)
            hook.uninstall(*reinterpret_cast<std::uintptr_t**>(*viewRender));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hook.wasEverInstalled() && viewRender && *viewRender && hook.isInstalled(*reinterpret_cast<std::uintptr_t**>(*viewRender));
    }

    void install() noexcept
    {
        if (viewRender && *viewRender && hook.install(vmtLengthCalculator, *reinterpret_cast<std::uintptr_t**>(*viewRender))) {
            originalOnRenderStart = hook.hook(4, &ViewRenderHook_onRenderStart_asm);
        }
    }

    cs2::CViewRender** viewRender;
    VmtLengthCalculator vmtLengthCalculator;
    VmtSwapper hook;
    cs2::CViewRender::OnRenderStart* originalOnRenderStart{ nullptr };
};
