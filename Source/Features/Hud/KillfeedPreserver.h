#pragma once

#include <CS2/Classes/C_CSGameRules.h>
#include <CS2/Constants/PanelIDs.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <Helpers/PanoramaPanelPointer.h>
#include <Utils/StringParser.h>
#include <GameClasses/GameRules.h>

#include "States/KillfeedPreserverState.h"

class KillfeedPreserver : public TogglableFeature<KillfeedPreserver> {
public:
    KillfeedPreserver(KillfeedPreserverState& state, HookDependencies& hookDependencies, HudProvider hudProvider, cs2::C_CSGameRules** gameRules) noexcept
        : TogglableFeature{state.enabled}
        , state{state}
        , hookDependencies{hookDependencies}
        , hudProvider{hudProvider}
        , gameRules{gameRules}
    {
    }

    void run() noexcept
    {
        if (!isEnabled())
            return;

        if (!gameRules || !*gameRules)
            return;

        if (!hookDependencies.requestDependency<CurTime>())
            return;

        const auto roundStartTime = GameRules{(*gameRules)}.getRoundStartTime();

        initSymbols();

        const auto deathNotices = getDeathNotices();
        if (!deathNotices)
            return;

        for (int i = 0; i < deathNotices->size; ++i) {
            const PanoramaUiPanel panel{ deathNotices->memory[i] };
            if (!panel.hasClass(state.deathNoticeKillerSymbol))
                continue;

            const auto spawnTimeString = panel.getAttributeString(state.spawnTimeSymbol, "");
            if (!spawnTimeString)
                continue;

            float spawnTime = 0.0f;
            StringParser{ spawnTimeString }.parseFloat(spawnTime);

            if (spawnTime > roundStartTime) {
                panel.setAttributeString(state.spawnTimeSymbol, StringBuilderStorage<20>{}.builder().put(static_cast<std::uint64_t>(hookDependencies.getDependency<CurTime>()), '.', '0').cstring());
            }
        }
    }


private:
    [[nodiscard]] PanoramaUiPanel getDeathNoticesPanel() noexcept
    {
        if (const auto deathNoticesPanel = state.deathNoticesPointer.get())
            return deathNoticesPanel;

        if (const auto hudDeathNotice = hudProvider.findChildInLayoutFile(cs2::HudDeathNotice))
            state.deathNoticesPointer = hudDeathNotice.findChildInLayoutFile(cs2::VisibleNotices);

        return state.deathNoticesPointer.get();
    }

    [[nodiscard]] cs2::CUIPanel::childrenVector* getDeathNotices() noexcept
    {
        if (const auto visibleDeathNoticesPanel = getDeathNoticesPanel())
            return visibleDeathNoticesPanel.children();
        return nullptr;
    }

    void initSymbols() noexcept
    { 
        if (state.deathNoticeKillerSymbol == -1)
            state.deathNoticeKillerSymbol = PanoramaUiEngine::makeSymbol(0, "DeathNotice_Killer");

        if (state.spawnTimeSymbol == -1)
            state.spawnTimeSymbol = PanoramaUiEngine::makeSymbol(0, "SpawnTime");
    }

    friend TogglableFeature;

    KillfeedPreserverState& state;
    HookDependencies& hookDependencies;
    HudProvider hudProvider;
    cs2::C_CSGameRules** gameRules;
};
