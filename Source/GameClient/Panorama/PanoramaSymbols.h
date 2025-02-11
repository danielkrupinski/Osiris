#pragma once

#include <CS2/Panorama/CPanoramaSymbol.h>
#include <GameClient/Panorama/PanoramaUiEngine.h>

struct PanoramaSymbols {
    template <typename HookContext>
    PanoramaSymbols(HookContext& hookContext) noexcept
        : deathNoticeKillerSymbol{hookContext.template make<PanoramaUiEngine>().makeSymbol(0, "DeathNotice_Killer")}
        , spawnTimeSymbol{hookContext.template make<PanoramaUiEngine>().makeSymbol(0, "SpawnTime")}
    {
    }

    cs2::CPanoramaSymbol deathNoticeKillerSymbol;
    cs2::CPanoramaSymbol spawnTimeSymbol;
};
