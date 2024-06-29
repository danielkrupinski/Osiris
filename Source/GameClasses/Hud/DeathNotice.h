#pragma once

#include <Utils/StringParser.h>

template <typename Panel, typename Dependencies>
class DeathNotice {
public:
    [[nodiscard]] bool isLocalPlayerKiller() const noexcept
    {
        return panel.hasClass(PanoramaSymbols::instance().deathNoticeKillerSymbol);
    }

    [[nodiscard]] bool wasSpawnedThisRound() const noexcept
    {
        const auto roundStartTime = dependencies.gameRules().roundStartTime();
        return roundStartTime && getSpawnTime() >= *roundStartTime;
    }

    [[nodiscard]] float getSpawnTime() const noexcept
    {
        float spawnTime = 0.0f;
        if (const auto spawnTimeString = panel.getAttributeString(PanoramaSymbols::instance().spawnTimeSymbol, ""))
            StringParser{spawnTimeString}.parseFloat(spawnTime);
        return spawnTime;
    }

    void markAsJustSpawned() const noexcept
    {
        if (const auto curtime = dependencies.globalVars().curtime())
            setSpawnTime(*curtime);
    }

    void setSpawnTime(float spawnTime) const noexcept
    {
        panel.setAttributeString(PanoramaSymbols::instance().spawnTimeSymbol,
            StringBuilderStorage<20>{}.builder().put(static_cast<std::uint64_t>(spawnTime), '.', '0').cstring());
    }

    Panel panel;
    Dependencies dependencies;
};
