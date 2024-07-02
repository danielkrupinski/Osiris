#pragma once

#include "KillfeedPreserverState.h"

template <typename Dependencies>
struct KillfeedPreserverContext {
    [[nodiscard]] decltype(auto) deathNotices() const noexcept
    {
        return _dependencies.hud().deathNotices();
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return _state;
    }
    
    KillfeedPreserverState& _state;
    Dependencies& _dependencies;
};
