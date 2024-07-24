#pragma once

#include <Utils/StringParser.h>

#include "DeathNoticeContext.h"

template <typename Context>
class DeathNotice {
public:
    explicit DeathNotice(Context context) noexcept
        : context{context}
    {
    }

    template <typename HookContext>
    DeathNotice(HookContext& hookContext, cs2::CUIPanel* panel) noexcept
        : context{hookContext, panel}
    {
    }

    [[nodiscard]] bool isLocalPlayerKiller() const noexcept
    {
        return context.panel().hasClass(PanoramaSymbols::instance().deathNoticeKillerSymbol);
    }

    [[nodiscard]] bool wasSpawnedThisRound() const noexcept
    {
        const auto roundStartTime = context.gameRules().roundStartTime();
        return roundStartTime && getSpawnTime() >= *roundStartTime;
    }

    [[nodiscard]] float getSpawnTime() const noexcept
    {
        float spawnTime = 0.0f;
        if (const auto spawnTimeString = context.panel().getAttributeString(PanoramaSymbols::instance().spawnTimeSymbol, ""))
            StringParser{spawnTimeString}.parseFloat(spawnTime);
        return spawnTime;
    }

    void markAsJustSpawned() const noexcept
    {
        if (const auto curtime = context.globalVars().curtime())
            setSpawnTime(*curtime);
    }

    void setSpawnTime(float spawnTime) const noexcept
    {
        context.panel().setAttributeString(PanoramaSymbols::instance().spawnTimeSymbol,
            StringBuilderStorage<20>{}.builder().put(static_cast<std::uint64_t>(spawnTime), '.', '0').cstring());
    }

private:
    Context context;
};

template <typename HookContext>
DeathNotice(HookContext&, cs2::CUIPanel*) -> DeathNotice<DeathNoticeContext<HookContext>>;
