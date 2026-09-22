#pragma once

#include <cstdint>
#include <utility>

#include <CS2/Panorama/CUIPanel.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>
#include <Utils/StringBuilder.h>
#include <Utils/StringParser.h>

template <typename HookContext>
class DeathNotice {
public:
    DeathNotice(HookContext& hookContext, cs2::CUIPanel* uiPanel) noexcept
        : hookContext{hookContext}
        , uiPanel{uiPanel}
    {
    }

    [[nodiscard]] bool isLocalPlayerKiller() const
    {
        return panel().hasClass(panoramaSymbols().deathNoticeKillerSymbol);
    }

    [[nodiscard]] auto wasSpawnedThisRound() const
    {
        return hookContext.gameRules().roundStartTime().lessEqual(getSpawnTime());
    }

    [[nodiscard]] float getSpawnTime() const
    {
        float spawnTime = 0.0f;
        if (const auto spawnTimeString = panel().getAttributeString(panoramaSymbols().spawnTimeSymbol, ""))
            StringParser{spawnTimeString}.parseFloat(spawnTime);
        return spawnTime;
    }

    void markAsJustSpawned() const
    {
        if (const auto curtime = hookContext.globalVars().curtime(); curtime.hasValue())
            setSpawnTime(curtime.value());
    }

    void setSpawnTime(float spawnTime) const
    {
        panel().setAttributeString(panoramaSymbols().spawnTimeSymbol,
            StringBuilderStorage<20>{}.builder().put(static_cast<std::uint64_t>(spawnTime), '.', '0').cstring());
    }

private:
    [[nodiscard]] decltype(auto) panel() const
    {
        return hookContext.template make<PanoramaUiPanel>(uiPanel);
    }

    [[nodiscard]] decltype(auto) panoramaSymbols() const
    {
        return hookContext.panoramaSymbols();
    }

    HookContext& hookContext;
    cs2::CUIPanel* uiPanel;
};
