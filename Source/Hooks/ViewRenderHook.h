#pragma once

#include <CS2/Classes/CViewRender.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtSwapper.h>

extern "C" void ViewRenderHook_onRenderStart_asm(cs2::CViewRender* thisptr) noexcept;

class ViewRenderHook : public RefCountedHook<ViewRenderHook> {
public:
    ViewRenderHook(const ClientPatterns& clientPatterns, const VmtLengthCalculator& vmtLengthCalculator) noexcept
        : viewRender{clientPatterns.viewRender()}
        , vmtLengthCalculator{vmtLengthCalculator}
    {
    }

    [[nodiscard]] cs2::CViewRender::OnRenderStart* getOriginalOnRenderStart() const noexcept
    {
        return originalOnRenderStart;
    }

private:
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

    friend class RefCountedHook;

    cs2::CViewRender** viewRender;
    VmtLengthCalculator vmtLengthCalculator;
    VmtSwapper hook;
    cs2::CViewRender::OnRenderStart* originalOnRenderStart{ nullptr };
};
