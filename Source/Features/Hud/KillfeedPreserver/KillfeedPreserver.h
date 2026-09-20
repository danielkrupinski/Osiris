#pragma once

#include <utility>

#include <HookContext/HookContextMacros.h>
#include "KillfeedPreserverConfigVariables.h"

template <typename HookContext>
class KillfeedPreserver {
public:
    explicit KillfeedPreserver(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void run() const
    {
        if (GET_CONFIG_VAR(KillfeedPreserverEnabled))
            deathNotices().forEach(preserveDeathNotice());
    }

private:
    [[nodiscard]] decltype(auto) deathNotices() const
    {
        return hookContext.hud().deathNotices();
    }

    [[nodiscard]] auto preserveDeathNotice() const
    {
        return [](auto&& deathNotice)
        {
            if (deathNotice.isLocalPlayerKiller() && deathNotice.wasSpawnedThisRound().valueOr(false))
                deathNotice.markAsJustSpawned();
        };
    }

    HookContext& hookContext;
};
