#pragma once

#include <cstdint>

#include <CS2/Classes/ClientModeCSNormal.h>
#include <MemoryPatterns/PatternTypes/ClientPatternTypes.h>
#include <Vmt/VmtLengthCalculator.h>
#include <Vmt/VmtSwapper.h>

float ClientModeHook_getViewmodelFov(cs2::ClientModeCSNormal* thisptr) noexcept;

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

    void hookGetViewmodelFov() const noexcept
    {
        const auto vmt = clientModeInstanceVmtPointer();
        if (vmt && hook().install(hookContext.hooks().clientVmtLengthCalculator, *vmt)) {
            hookContext.hooks().originalGetViewmodelFov = hook().hook(WIN64_LINUX(27, 28), &ClientModeHook_getViewmodelFov);
        }
    }

    void restoreGetViewmodelFov() const noexcept
    {
        if (!hook().wasEverInstalled())
            return;

        if (const auto vmt = clientModeInstanceVmtPointer())
            hook().uninstall(*vmt);
    }

private:
    [[nodiscard]] std::uintptr_t** clientModeInstanceVmtPointer() const
    {
        if (const auto clientMode = hookContext.patternSearchResults().template get<PointerToClientMode>())
            return reinterpret_cast<std::uintptr_t**>(clientMode);
        return nullptr;
    }

    [[nodiscard]] auto& hook() const
    {
        return hookContext.hooks().clientModeVmtSwapper;
    }

    HookContext& hookContext;
};
