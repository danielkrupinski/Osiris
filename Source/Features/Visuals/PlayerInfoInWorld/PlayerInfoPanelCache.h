#pragma once

template <typename HookContext>
class PlayerInfoPanelCache {
public:
    explicit PlayerInfoPanelCache(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void allocateNewEntry() const noexcept
    {
        state().cache.pushBack({});
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
