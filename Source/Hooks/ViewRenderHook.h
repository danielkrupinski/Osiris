#pragma once

#include <CS2/Classes/CViewRender.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtSwapper.h>

class ViewRenderHook : public RefCountedHook<ViewRenderHook> {
public:
    explicit ViewRenderHook(const VmtLengthCalculator& vmtLengthCalculator) noexcept
        : vmtLengthCalculator{ vmtLengthCalculator }
    {
    }

    [[nodiscard]] cs2::CViewRender::OnRenderStart* getOriginalOnRenderStart() const noexcept
    {
        return originalOnRenderStart;
    }

private:
    static void onRenderStart(cs2::CViewRender* thisptr) noexcept;

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
            originalOnRenderStart = hook.hook(4, &onRenderStart);
        }
    }

    friend class RefCountedHook;

    cs2::CViewRender** viewRender{ ClientPatterns::viewRender() };
    VmtLengthCalculator vmtLengthCalculator;
    VmtSwapper hook;
    cs2::CViewRender::OnRenderStart* originalOnRenderStart{ nullptr };
};
