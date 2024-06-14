#pragma once

#include <CS2/Classes/C_CSGameRules.h>
#include <CS2/Constants/PanelIDs.h>
#include <FeatureHelpers/FeatureToggle.h>
#include <Helpers/PanoramaPanelPointer.h>
#include <Utils/StringParser.h>
#include <GameClasses/GameRules.h>

#include "States/KillfeedPreserverState.h"

struct KillfeedPreserveToggle : FeatureToggle<KillfeedPreserveToggle> {
    explicit KillfeedPreserveToggle(KillfeedPreserverState& state) noexcept
        : state{state}
    {
    }

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.enabled;
    }

    KillfeedPreserverState& state;
};

class KillfeedPreserver {
public:
    KillfeedPreserver(KillfeedPreserverState& state, HookDependencies& hookDependencies, HudOptional hud) noexcept
        : state{state}
        , hookDependencies{hookDependencies}
        , hud{hud}
    {
    }

    void run() noexcept
    {
        if (!state.enabled)
            return;

        const auto gameRules = hookDependencies.gameDependencies().gameRulesDeps.gameRules;
        if (!gameRules || !*gameRules)
            return;

        const auto globalVars = hookDependencies.globalVars();
        if (!globalVars)
            return;

        const auto curtime = globalVars->curtime();
        if (!curtime)
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
                panel.setAttributeString(state.spawnTimeSymbol, StringBuilderStorage<20>{}.builder().put(static_cast<std::uint64_t>(*curtime), '.', '0').cstring());
            }
        }
    }

private:
    [[nodiscard]] PanoramaUiPanel getDeathNoticesPanel() noexcept
    {
        if (const auto deathNoticesPanel = state.deathNoticesPointer.get())
            return deathNoticesPanel;

        if (hud) {
            if (const auto hudDeathNotice = hud->findChildInLayoutFile(cs2::HudDeathNotice))
            state.deathNoticesPointer = hudDeathNotice.findChildInLayoutFile(cs2::VisibleNotices);
        }

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

    KillfeedPreserverState& state;
    HookDependencies& hookDependencies;
    HudOptional hud;
};
