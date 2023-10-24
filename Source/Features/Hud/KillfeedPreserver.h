#pragma once

#include <CS2/Classes/C_CSGameRules.h>
#include <CS2/Constants/PanelIDs.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <Helpers/PanoramaPanelPointer.h>
#include <Utils/StringParser.h>
#include <GameClasses/GameRules.h>

class KillfeedPreserver : public TogglableFeature<KillfeedPreserver> {
public:
    explicit KillfeedPreserver(HudProvider hudProvider) noexcept
        : hudProvider{ hudProvider }
    {
    }

    void run() noexcept
    {
        if (!isEnabled())
            return;

        if (!gameRules || !*gameRules)
            return;

        const auto roundStartTime = GameRules{(*gameRules)}.getRoundStartTime();

        initSymbols();

        const auto deathNotices = getDeathNotices();
        if (!deathNotices)
            return;

        for (int i = 0; i < deathNotices->size; ++i) {
            const PanoramaUiPanel panel{ deathNotices->memory[i] };
            if (!panel.hasClass(deathNoticeKillerSymbol))
                continue;

            const auto spawnTimeString = panel.getAttributeString(spawnTimeSymbol, "");
            if (!spawnTimeString)
                continue;

            float spawnTime = 0.0f;
            StringParser{ spawnTimeString }.parseFloat(spawnTime);

            if (spawnTime > roundStartTime) {
                panel.setAttributeString(spawnTimeSymbol, StringBuilderStorage<20>{}.builder().put(static_cast<std::uint64_t>((*globalVars)->curtime), '.', '0').cstring());
            }
        }
    }


private:
    [[nodiscard]] PanoramaUiPanel getDeathNoticesPanel() noexcept
    {
        if (const auto deathNoticesPanel = deathNoticesPointer.get())
            return deathNoticesPanel;

        if (const auto hudDeathNotice = hudProvider.findChildInLayoutFile(cs2::HudDeathNotice))
            deathNoticesPointer = hudDeathNotice.findChildInLayoutFile(cs2::VisibleNotices);

        return deathNoticesPointer.get();
    }

    [[nodiscard]] cs2::CUIPanel::childrenVector* getDeathNotices() noexcept
    {
        if (const auto visibleDeathNoticesPanel = getDeathNoticesPanel())
            return visibleDeathNoticesPanel.children();
        return nullptr;
    }

    void initSymbols() noexcept
    { 
        if (deathNoticeKillerSymbol == -1)
            deathNoticeKillerSymbol = PanoramaUiEngine::makeSymbol(0, "DeathNotice_Killer");

        if (spawnTimeSymbol == -1)
            spawnTimeSymbol = PanoramaUiEngine::makeSymbol(0, "SpawnTime");
    }

    friend TogglableFeature;

    HudProvider hudProvider;
    cs2::GlobalVars** globalVars{ ClientPatterns::globalVars() };
    cs2::C_CSGameRules** gameRules{ ClientPatterns::gameRules() };

    cs2::CPanoramaSymbol deathNoticeKillerSymbol{ -1 };
    cs2::CPanoramaSymbol spawnTimeSymbol{ -1 };
    PanoramaPanelPointer deathNoticesPointer;
};
