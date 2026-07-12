#pragma once

#include "PlayerInfoPanelCacheState.h"

template <typename HookContext>
class PlayerInfoPanelCache {
public:
    explicit PlayerInfoPanelCache(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void allocateNewEntry() const noexcept
    {
        const auto newEntryIndex = state().cache.getSize();
        if (newEntryIndex >= PlayerInfoPanelCacheState::kMaxEntries || !state().cache.pushBack({}))
            return;
        state().cache.back().playerHealthText = state().playerHealthText[newEntryIndex].data();
        state().cache.back().activeWeaponAmmoText = state().activeWeaponAmmoText[newEntryIndex].data();
    }

    [[nodiscard]] auto& nextEntry() const noexcept
    {
        return state().cache[perHookState().nextEntryIndex++];
    }

    void clear() const noexcept
    {
        state().cache.clear();
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.playerInfoPanelCacheState();
    }

    [[nodiscard]] auto& perHookState() const noexcept
    {
        return hookContext.playerInfoPanelCachePerHookState();
    }

    HookContext& hookContext;
};
