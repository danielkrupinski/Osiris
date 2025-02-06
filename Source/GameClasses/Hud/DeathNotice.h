#pragma once

#include <cstdint>
#include <utility>

#include <Utils/StringBuilder.h>
#include <Utils/StringParser.h>

#include "DeathNoticeContext.h"

template <typename HookContext, typename Context = DeathNoticeContext<HookContext>>
class DeathNotice {
public:
    template <typename... Args>
    explicit DeathNotice(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] bool isLocalPlayerKiller() const noexcept
    {
        return context.panel().hasClass(context.panoramaSymbols().deathNoticeKillerSymbol);
    }

    [[nodiscard]] auto wasSpawnedThisRound() const noexcept
    {
        return context.gameRules().roundStartTime().lessEqual(getSpawnTime());
    }

    [[nodiscard]] float getSpawnTime() const noexcept
    {
        float spawnTime = 0.0f;
        if (const auto spawnTimeString = context.panel().getAttributeString(context.panoramaSymbols().spawnTimeSymbol, ""))
            StringParser{spawnTimeString}.parseFloat(spawnTime);
        return spawnTime;
    }

    void markAsJustSpawned() const noexcept
    {
        if (const auto curtime = context.globalVars().curtime(); curtime.hasValue())
            setSpawnTime(curtime.value());
    }

    void setSpawnTime(float spawnTime) const noexcept
    {
        context.panel().setAttributeString(context.panoramaSymbols().spawnTimeSymbol,
            StringBuilderStorage<20>{}.builder().put(static_cast<std::uint64_t>(spawnTime), '.', '0').cstring());
    }

private:
    Context context;
};
