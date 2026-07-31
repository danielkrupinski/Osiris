#pragma once

#include <cstddef>
#include <cstdint>

#include <CS2/Classes/ClientModeCSNormal.h>
#include <MemoryPatterns/PatternTypes/ClientPatternTypes.h>
#include <Utils/GenericFunctionPointer.h>
#include <Vmt/VmtLengthCalculator.h>
#include <Vmt/VmtSwapper.h>

float ClientModeHook_getViewmodelFov(cs2::ClientModeCSNormal* thisptr) noexcept;
void ClientModeHook_overrideView(cs2::ClientModeCSNormal* thisptr, cs2::CViewSetup* setup) noexcept;

template <typename HookContext>
class ClientModeHooks {
public:
    explicit ClientModeHooks(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] cs2::ClientModeCSNormal::GetViewmodelFov* originalGetViewmodelFov() const noexcept
    {
        return hookContext.hooks().originalGetViewmodelFov;
    }

    [[nodiscard]] cs2::ClientModeCSNormal::OverrideView* originalOverrideView() const noexcept
    {
        return hookContext.hooks().originalOverrideView;
    }

    void hookGetViewmodelFov() const noexcept
    {
        if (!ensureVmt())
            return;
        const auto original = (cs2::ClientModeCSNormal::GetViewmodelFov*)hook().hook(kGetViewmodelFovIndex, &ClientModeHook_getViewmodelFov);
        if (!hookContext.hooks().originalGetViewmodelFov)
            hookContext.hooks().originalGetViewmodelFov = original;
        hookContext.hooks().getViewmodelFovHookInstalled = true;
    }

    void hookOverrideView() const noexcept
    {
        if (!ensureVmt())
            return;
        const auto original = (cs2::ClientModeCSNormal::OverrideView*)hook().hook(kOverrideViewIndex, (GenericFunctionPointer)&ClientModeHook_overrideView);
        if (!hookContext.hooks().originalOverrideView)
            hookContext.hooks().originalOverrideView = original;
        hookContext.hooks().overrideViewHookInstalled = true;
    }

    void restoreGetViewmodelFov() const noexcept
    {
        if (!hook().wasEverInstalled())
            return;
        if (hookContext.hooks().originalGetViewmodelFov && hookContext.hooks().getViewmodelFovHookInstalled) {
            static_cast<void>(hook().hook(kGetViewmodelFovIndex, hookContext.hooks().originalGetViewmodelFov));
            hookContext.hooks().getViewmodelFovHookInstalled = false;
        }
        uninstallIfUnused();
    }

    void restoreOverrideView() const noexcept
    {
        if (!hook().wasEverInstalled())
            return;
        if (hookContext.hooks().originalOverrideView && hookContext.hooks().overrideViewHookInstalled) {
            static_cast<void>(hook().hook(kOverrideViewIndex, hookContext.hooks().originalOverrideView));
            hookContext.hooks().overrideViewHookInstalled = false;
        }
        uninstallIfUnused();
    }

    void restoreAll() const noexcept
    {
        if (!hook().wasEverInstalled())
            return;
        if (const auto vmt = clientModeInstanceVmtPointer())
            hook().uninstall(*vmt);
        hookContext.hooks().getViewmodelFovHookInstalled = false;
        hookContext.hooks().overrideViewHookInstalled = false;
    }

private:
    [[nodiscard]] bool ensureVmt() const noexcept
    {
        const auto vmt = clientModeInstanceVmtPointer();
        if (!vmt)
            return false;
        if (!hook().wasEverInstalled()) {
            if (!hook().install(hookContext.hooks().clientVmtLengthCalculator, *vmt))
                return false;
        } else if (!hook().isInstalled(*vmt)) {
            hook().install(hookContext.hooks().clientVmtLengthCalculator, *vmt);
        }
        return true;
    }

    void uninstallIfUnused() const noexcept
    {
        if (hookContext.hooks().getViewmodelFovHookInstalled || hookContext.hooks().overrideViewHookInstalled)
            return;
        if (const auto vmt = clientModeInstanceVmtPointer(); vmt && hook().isInstalled(*vmt))
            hook().uninstall(*vmt);
    }

    [[nodiscard]] std::uintptr_t** clientModeInstanceVmtPointer() const
    {
        if (const auto clientMode = hookContext.patternSearchResults().template get<PointerToClientMode>()) {
            if (auto* const directVmt = reinterpret_cast<std::uintptr_t**>(clientMode); isKnownOrUsableVmtPointer(directVmt))
                return directVmt;

            if (auto* const indirectClientMode = *reinterpret_cast<cs2::ClientModeCSNormal** const*>(clientMode)) {
                if (auto* const indirectVmt = reinterpret_cast<std::uintptr_t**>(indirectClientMode); isKnownOrUsableVmtPointer(indirectVmt))
                    return indirectVmt;
            }
        }
        return nullptr;
    }

    [[nodiscard]] bool isKnownOrUsableVmtPointer(std::uintptr_t** vmt) const noexcept
    {
        if (!vmt || !*vmt)
            return false;
        if (hook().wasEverInstalled() && hook().isInstalled(*vmt))
            return true;
        return static_cast<std::size_t>(hookContext.hooks().clientVmtLengthCalculator(*vmt)) > kMaxHookIndex;
    }

    [[nodiscard]] auto& hook() const
    {
        return hookContext.hooks().clientModeVmtSwapper;
    }

    static constexpr auto kOverrideViewIndex{15};
    static constexpr auto kGetViewmodelFovIndex{27};
    static constexpr auto kMaxHookIndex{kGetViewmodelFovIndex};

    HookContext& hookContext;
};
