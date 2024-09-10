#pragma once

#include "KillfeedPreserverState.h"

template <typename Context>
struct KillfeedPreserverContext {
    KillfeedPreserverContext(KillfeedPreserverState& state, Context& context) noexcept
        : _state{state}, _context{context}
    {
    }

    [[nodiscard]] decltype(auto) deathNotices() const noexcept
    {
        return _context.hud().deathNotices();
    }

    [[nodiscard]] auto preserveDeathNotice() const noexcept
    {
        return [](auto&& deathNotice)
        {
            if (deathNotice.isLocalPlayerKiller() && deathNotice.wasSpawnedThisRound().valueOr(false))
                deathNotice.markAsJustSpawned();
        };
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return _state;
    }
    
private:
    KillfeedPreserverState& _state;
    Context& _context;
};
