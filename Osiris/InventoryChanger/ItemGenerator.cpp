#include <algorithm>
#include <cmath>
#include <random>
#include <span>
#include <vector>

#include "../Helpers.h"
#include "Inventory.h"
#include "ItemGenerator.h"

#include "../SDK/ItemSchema.h"

static float generateWear() noexcept
{
    float wear;
    if (const auto condition = Helpers::random(1, 10000); condition <= 1471)
        wear = Helpers::random(0.0f, 0.07f);
    else if (condition <= 3939)
        wear = Helpers::random(0.07f, 0.15f);
    else if (condition <= 8257)
        wear = Helpers::random(0.15f, 0.38f);
    else if (condition <= 9049)
        wear = Helpers::random(0.38f, 0.45f);
    else
        wear = Helpers::random(0.45f, 1.0f);
    return wear;
}

using StaticData::TournamentMap;

[[nodiscard]] static std::array<StickerConfig, 5> generateSouvenirStickers(std::uint32_t tournamentID, TournamentMap map, TournamentStage stage, TournamentTeam team1, TournamentTeam team2, ProPlayer player) noexcept;

std::pair<std::size_t, std::size_t> ItemGenerator::generateItemFromContainer(const InventoryItem& caseItem) noexcept
{
    assert(caseItem.isCase());

    const auto& caseData = StaticData::cases()[caseItem.get().dataIndex];
    assert(caseData.hasLoot());

    const auto unlockedItemIdx = StaticData::caseLoot()[Helpers::random(static_cast<int>(caseData.lootBeginIdx), static_cast<int>(caseData.lootEndIdx - 1))];
    std::size_t dynamicDataIdx = Inventory::INVALID_DYNAMIC_DATA_IDX;

    if (const auto& item = StaticData::gameItems()[unlockedItemIdx]; caseData.willProduceStatTrak && item.isMusic()) {
        DynamicMusicData dynamicData;
        dynamicData.statTrak = 0;
        dynamicDataIdx = Inventory::emplaceDynamicData(std::move(dynamicData));
    } else if (item.isSkin()) {
        DynamicSkinData dynamicData;
        const auto& staticData = StaticData::paintKits()[item.dataIndex];
        dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, generateWear());
        dynamicData.seed = Helpers::random(1, 1000);

        if (caseData.isSouvenirPackage()) {
            dynamicData.tournamentID = caseData.souvenirPackageTournamentID;
            const auto& souvenir = Inventory::dynamicSouvenirPackageData(caseItem.getDynamicDataIndex());
            dynamicData.tournamentStage = souvenir.tournamentStage;
            dynamicData.tournamentTeam1 = souvenir.tournamentTeam1;
            dynamicData.tournamentTeam2 = souvenir.tournamentTeam2;
            dynamicData.proPlayer = souvenir.proPlayer;
            dynamicData.stickers = generateSouvenirStickers(caseData.souvenirPackageTournamentID, caseData.tournamentMap, dynamicData.tournamentStage, dynamicData.tournamentTeam1, dynamicData.tournamentTeam2, dynamicData.proPlayer);
        } else if (Helpers::random(0, 9) == 0) {
            dynamicData.statTrak = 0;
        }

        dynamicDataIdx = Inventory::emplaceDynamicData(std::move(dynamicData));
    }

    return std::make_pair(unlockedItemIdx, dynamicDataIdx);
}

struct Match {
    TournamentMap map;
    TournamentStage stage;
    TournamentTeam team1;
    TournamentTeam team2;
    std::array<ProPlayer, 10> mvpPlayers;

    [[nodiscard]] bool hasMVPs() const noexcept { return std::ranges::find(mvpPlayers, ProPlayer{}) != mvpPlayers.begin(); }
    [[nodiscard]] ProPlayer getRandomMVP() const noexcept
    {
        if (!hasMVPs())
            return ProPlayer{};
        return mvpPlayers[Helpers::random(0, std::distance(mvpPlayers.begin(), std::ranges::find(mvpPlayers, ProPlayer{})) - 1)];
    }
};

struct Tournament {
    std::uint32_t tournamentID;
    std::span<const Match> matches;
};

constexpr auto dreamHack2013Matches = std::to_array<Match>({
    // Group A
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::NatusVincere, TournamentTeam::Fnatic, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::ClanMystik, TournamentTeam::LGBEsports, {} },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::LGBEsports, TournamentTeam::NatusVincere, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::ClanMystik, TournamentTeam::Fnatic, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::ClanMystik,TournamentTeam::LGBEsports, {} },

    // Group B
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::NinjasInPyjamas, TournamentTeam::RecursiveEsports, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::UniversalSoldiers, TournamentTeam::IBUYPOWER, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::RecursiveEsports, TournamentTeam::IBUYPOWER, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::NinjasInPyjamas, TournamentTeam::UniversalSoldiers, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::RecursiveEsports, TournamentTeam::UniversalSoldiers, {} },

    // Group C
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::VeryGames, TournamentTeam::Xapso, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::NFaculty, TournamentTeam::ComplexityGaming, {} },
    { TournamentMap::Train, TournamentStage::GroupStage, TournamentTeam::NFaculty, TournamentTeam::Xapso, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::ComplexityGaming, TournamentTeam::VeryGames, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::NFaculty, TournamentTeam::VeryGames, {} },

    // Group D
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::AstanaDragons, TournamentTeam::_ReasonGaming, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::CopenhagenWolves, TournamentTeam::SKGaming, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::SKGaming, TournamentTeam::_ReasonGaming, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::AstanaDragons, TournamentTeam::CopenhagenWolves, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::AstanaDragons, TournamentTeam::_ReasonGaming, {} },

    // Quarterfinals
    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::LGBEsports, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Train, TournamentStage::Quarterfinal, TournamentTeam::LGBEsports, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Mirage, TournamentStage::Quarterfinal, TournamentTeam::LGBEsports, TournamentTeam::NinjasInPyjamas, {} },

    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::CopenhagenWolves, TournamentTeam::VeryGames, {} },
    { TournamentMap::Inferno, TournamentStage::Quarterfinal, TournamentTeam::CopenhagenWolves, TournamentTeam::VeryGames, {} },
    { TournamentMap::Mirage, TournamentStage::Quarterfinal, TournamentTeam::CopenhagenWolves, TournamentTeam::VeryGames, {} },

    { TournamentMap::Inferno, TournamentStage::Quarterfinal, TournamentTeam::Fnatic, TournamentTeam::RecursiveEsports, {} },
    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::Fnatic, TournamentTeam::RecursiveEsports, {} },
    { TournamentMap::Train, TournamentStage::Quarterfinal, TournamentTeam::Fnatic, TournamentTeam::RecursiveEsports, {} },

    { TournamentMap::Nuke, TournamentStage::Quarterfinal, TournamentTeam::AstanaDragons, TournamentTeam::ComplexityGaming, {} },
    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::AstanaDragons, TournamentTeam::ComplexityGaming, {} },
    { TournamentMap::Inferno, TournamentStage::Quarterfinal, TournamentTeam::AstanaDragons, TournamentTeam::ComplexityGaming, {} },

    // Semifinals
    { TournamentMap::Dust2, TournamentStage::Semifinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::VeryGames, {} },
    { TournamentMap::Inferno, TournamentStage::Semifinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::VeryGames, {} },
    { TournamentMap::Nuke, TournamentStage::Semifinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::VeryGames, {} },

    { TournamentMap::Train, TournamentStage::Semifinal, TournamentTeam::Fnatic, TournamentTeam::ComplexityGaming, {} },
    { TournamentMap::Mirage, TournamentStage::Semifinal, TournamentTeam::Fnatic, TournamentTeam::ComplexityGaming, {} },

    // Grand Final
    { TournamentMap::Dust2, TournamentStage::GrandFinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::Fnatic, {} },
    { TournamentMap::Inferno, TournamentStage::GrandFinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::Fnatic, {} },
    { TournamentMap::Train, TournamentStage::GrandFinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::Fnatic, {} },
});

constexpr auto emsOneKatowice2014Matches = std::to_array<Match>({
    // Group A
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::Titan, TournamentTeam::Mousesports, {} },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::VirtusPro, TournamentTeam::HellRaisers, {} },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::Titan, TournamentTeam::VirtusPro, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::Mousesports, TournamentTeam::HellRaisers, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::Titan, TournamentTeam::HellRaisers, {} },

    // Group B
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::NinjasInPyjamas, TournamentTeam::_3DMax, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::TeamLDLC, TournamentTeam::VoxEminor, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::NinjasInPyjamas, TournamentTeam::TeamLDLC, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::_3DMax, TournamentTeam::VoxEminor, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::TeamLDLC, TournamentTeam::_3DMax, {} },

    // Group C
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::Fnatic, TournamentTeam::ReasonGaming, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::TeamDignitas, TournamentTeam::IBUYPOWER, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::ReasonGaming, TournamentTeam::TeamDignitas, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::Fnatic, TournamentTeam::IBUYPOWER, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::Fnatic, TournamentTeam::ReasonGaming, {} },

    // Group D
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::ComplexityGaming, TournamentTeam::ClanMystik, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::LGBEsports, TournamentTeam::NatusVincere, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::LGBEsports, TournamentTeam::ComplexityGaming, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::NatusVincere, TournamentTeam::ClanMystik, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::ComplexityGaming, TournamentTeam::ClanMystik, {} },

    // Quarterfinals
    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::ComplexityGaming, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Nuke, TournamentStage::Quarterfinal, TournamentTeam::ComplexityGaming, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Train, TournamentStage::Quarterfinal, TournamentTeam::ComplexityGaming, TournamentTeam::NinjasInPyjamas, {} },

    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::TeamDignitas, TournamentTeam::HellRaisers, {} },
    { TournamentMap::Mirage, TournamentStage::Quarterfinal, TournamentTeam::TeamDignitas, TournamentTeam::HellRaisers, {} },

    { TournamentMap::Mirage, TournamentStage::Quarterfinal, TournamentTeam::VirtusPro, TournamentTeam::TeamLDLC, {} },
    { TournamentMap::Inferno, TournamentStage::Quarterfinal, TournamentTeam::VirtusPro, TournamentTeam::TeamLDLC, {} },

    { TournamentMap::Inferno, TournamentStage::Quarterfinal, TournamentTeam::Fnatic, TournamentTeam::LGBEsports, {} },
    { TournamentMap::Mirage, TournamentStage::Quarterfinal, TournamentTeam::Fnatic, TournamentTeam::LGBEsports, {} },
    { TournamentMap::Train, TournamentStage::Quarterfinal, TournamentTeam::Fnatic, TournamentTeam::LGBEsports, {} },

    // Semifinals
    { TournamentMap::Inferno, TournamentStage::Semifinal, TournamentTeam::TeamDignitas, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Nuke, TournamentStage::Semifinal, TournamentTeam::TeamDignitas, TournamentTeam::NinjasInPyjamas, {} },

    { TournamentMap::Inferno, TournamentStage::Semifinal, TournamentTeam::LGBEsports, TournamentTeam::VirtusPro, {} },
    { TournamentMap::Mirage, TournamentStage::Semifinal, TournamentTeam::LGBEsports, TournamentTeam::VirtusPro, {} },
    { TournamentMap::Train, TournamentStage::Semifinal, TournamentTeam::LGBEsports, TournamentTeam::VirtusPro, {} },

    // Grand Final
    { TournamentMap::Mirage, TournamentStage::GrandFinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::VirtusPro, {} },
    { TournamentMap::Inferno, TournamentStage::GrandFinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::VirtusPro, {} },
});

// Starting with ESL One Cologne 2014 souvenir packages are bound to a certain map. Matches of those tournaments must be sorted by map.

constexpr auto eslOneCologne2014Matches = std::to_array<Match>({
    { TournamentMap::Cache, TournamentStage::GroupStage, TournamentTeam::CopenhagenWolves, TournamentTeam::LondonConspiracy, {} },
    { TournamentMap::Cache, TournamentStage::GroupStage, TournamentTeam::VirtusPro, TournamentTeam::IBUYPOWER, {} },
    { TournamentMap::Cache, TournamentStage::GrandFinal, TournamentTeam::Fnatic, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Cobblestone, TournamentStage::GroupStage, TournamentTeam::NinjasInPyjamas, TournamentTeam::EpsilonEsports, {} },
    { TournamentMap::Cobblestone, TournamentStage::GroupStage, TournamentTeam::IBUYPOWER, TournamentTeam::Fnatic, {} },
    { TournamentMap::Cobblestone, TournamentStage::GroupStage, TournamentTeam::IBUYPOWER, TournamentTeam::DATTeam, {} },
    { TournamentMap::Cobblestone, TournamentStage::Quarterfinal, TournamentTeam::Cloud9, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Cobblestone, TournamentStage::Semifinal, TournamentTeam::TeamLDLC, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Cobblestone, TournamentStage::GrandFinal, TournamentTeam::Fnatic, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::TeamWolf, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::NatusVincere, TournamentTeam::CopenhagenWolves, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::TeamDignitas, TournamentTeam::VoxEminor, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::Cloud9, TournamentTeam::Titan, {} },
    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::NatusVincere, TournamentTeam::Fnatic, {} },
    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::EpsilonEsports, TournamentTeam::TeamDignitas, {} },
    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::VirtusPro, TournamentTeam::TeamLDLC, {} },
    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::Cloud9, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Dust2, TournamentStage::Semifinal,  TournamentTeam::TeamDignitas, TournamentTeam::Fnatic, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::EpsilonEsports, TournamentTeam::HellRaisers, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::NatusVincere, TournamentTeam::CopenhagenWolves, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::TeamLDLC, TournamentTeam::NatusVincere, {} },
    { TournamentMap::Inferno, TournamentStage::Quarterfinal, TournamentTeam::NatusVincere, TournamentTeam::Fnatic, {} },
    { TournamentMap::Inferno, TournamentStage::Quarterfinal, TournamentTeam::EpsilonEsports, TournamentTeam::TeamDignitas, {} },
    { TournamentMap::Inferno, TournamentStage::Semifinal, TournamentTeam::TeamLDLC, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Inferno, TournamentStage::GrandFinal, TournamentTeam::Fnatic, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::HellRaisers, TournamentTeam::TeamWolf, {} },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::TeamDignitas, TournamentTeam::Cloud9, {} },
    { TournamentMap::Mirage, TournamentStage::Quarterfinal, TournamentTeam::VirtusPro, TournamentTeam::TeamLDLC, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::TeamLDLC, TournamentTeam::LondonConspiracy, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::Titan, TournamentTeam::VoxEminor, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::Titan, TournamentTeam::TeamDignitas, {} },
    { TournamentMap::Nuke, TournamentStage::Semifinal, TournamentTeam::TeamLDLC, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Nuke, TournamentStage::Quarterfinal, TournamentTeam::Cloud9, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Nuke, TournamentStage::Quarterfinal, TournamentTeam::NatusVincere, TournamentTeam::Fnatic, {} },
    { TournamentMap::Overpass, TournamentStage::GroupStage, TournamentTeam::NinjasInPyjamas, TournamentTeam::HellRaisers, {} },
    { TournamentMap::Overpass, TournamentStage::GroupStage, TournamentTeam::VirtusPro, TournamentTeam::DATTeam, {} },
    { TournamentMap::Overpass, TournamentStage::GroupStage, TournamentTeam::VirtusPro, TournamentTeam::Fnatic, {} },
    { TournamentMap::Overpass, TournamentStage::Semifinal, TournamentTeam::TeamDignitas, TournamentTeam::Fnatic, {} },
});
static_assert(std::ranges::is_sorted(eslOneCologne2014Matches, {}, &Match::map));

constexpr auto dreamHack2014Matches = std::to_array<Match>({
    { TournamentMap::Cache, TournamentStage::GroupStage, TournamentTeam::BravadoGaming, TournamentTeam::Cloud9, {} },
    { TournamentMap::Cache, TournamentStage::GroupStage, TournamentTeam::NinjasInPyjamas, TournamentTeam::ESCGaming, {} },
    { TournamentMap::Cache, TournamentStage::Quarterfinal, TournamentTeam::VirtusPro, TournamentTeam::PENTASports, {} },
    { TournamentMap::Cache, TournamentStage::Quarterfinal, TournamentTeam::TeamLDLC, TournamentTeam::Fnatic, {} },
    { TournamentMap::Cache, TournamentStage::Semifinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::VirtusPro, {} },
    { TournamentMap::Cobblestone, TournamentStage::GroupStage, TournamentTeam::TeamLDLC, TournamentTeam::ESCGaming, {} },
    { TournamentMap::Cobblestone, TournamentStage::Quarterfinal, TournamentTeam::NatusVincere, TournamentTeam::TeamDignitas, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::Fnatic, TournamentTeam::Cloud9, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::PENTASports, TournamentTeam::CopenhagenWolves, {} },
    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::HellRaisers, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::TeamLDLC, TournamentTeam::Fnatic, {} },
    { TournamentMap::Dust2, TournamentStage::Semifinal, TournamentTeam::NatusVincere, TournamentTeam::TeamLDLC, {} },
    { TournamentMap::Dust2, TournamentStage::GrandFinal, TournamentTeam::TeamLDLC, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::Cloud9, TournamentTeam::HellRaisers, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::IBUYPOWER, TournamentTeam::PENTASports, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::VirtusPro, TournamentTeam::MyXMG, {} },
    { TournamentMap::Inferno, TournamentStage::Quarterfinal, TournamentTeam::HellRaisers, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Inferno, TournamentStage::Semifinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::VirtusPro, {} },
    { TournamentMap::Inferno, TournamentStage::Semifinal, TournamentTeam::NatusVincere, TournamentTeam::TeamLDLC, {} },
    { TournamentMap::Inferno, TournamentStage::GrandFinal, TournamentTeam::TeamLDLC, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::Fnatic, TournamentTeam::BravadoGaming, {} },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::Fnatic, TournamentTeam::HellRaisers, {} },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::ESCGaming, TournamentTeam::PlanetkeyDynamics, {} },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::NatusVincere, TournamentTeam::Flipsid3Tactics, {} },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::MyXMG, TournamentTeam::Flipsid3Tactics, {} },
    { TournamentMap::Mirage, TournamentStage::Quarterfinal, TournamentTeam::VirtusPro, TournamentTeam::PENTASports, {} },
    { TournamentMap::Mirage, TournamentStage::Quarterfinal, TournamentTeam::NatusVincere, TournamentTeam::TeamDignitas, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::TeamDignitas, TournamentTeam::PENTASports, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::IBUYPOWER, TournamentTeam::CopenhagenWolves, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::TeamDignitas, TournamentTeam::IBUYPOWER, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::VirtusPro, TournamentTeam::NatusVincere, {} },
    { TournamentMap::Nuke, TournamentStage::Semifinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::VirtusPro, {} },
    { TournamentMap::Overpass, TournamentStage::GroupStage, TournamentTeam::NinjasInPyjamas, TournamentTeam::PlanetkeyDynamics, {} },
    { TournamentMap::Overpass, TournamentStage::GroupStage, TournamentTeam::NinjasInPyjamas, TournamentTeam::TeamLDLC, {} },
    { TournamentMap::Overpass, TournamentStage::GroupStage, TournamentTeam::Flipsid3Tactics, TournamentTeam::NatusVincere, {} },
    { TournamentMap::Overpass, TournamentStage::Quarterfinal, TournamentTeam::TeamLDLC, TournamentTeam::Fnatic, {} },
    { TournamentMap::Overpass, TournamentStage::GrandFinal, TournamentTeam::TeamLDLC, TournamentTeam::NinjasInPyjamas, {} },
});
static_assert(std::ranges::is_sorted(dreamHack2014Matches, {}, &Match::map));

constexpr auto eslOneKatowice2015Matches = std::to_array<Match>({
    { TournamentMap::Cache, TournamentStage::GroupStage, TournamentTeam::TeamEnVyUs, TournamentTeam::LGBEsports, {} },
    { TournamentMap::Cache, TournamentStage::GroupStage, TournamentTeam::PENTASports, TournamentTeam::Titan, {} },
    { TournamentMap::Cache, TournamentStage::GroupStage, TournamentTeam::VoxEminor, TournamentTeam::Flipsid3Tactics, {} },
    { TournamentMap::Cache, TournamentStage::Quarterfinal, TournamentTeam::Fnatic, TournamentTeam::PENTASports, {} },
    { TournamentMap::Cache, TournamentStage::Quarterfinal, TournamentTeam::TeamEnVyUs, TournamentTeam::NatusVincere, {} },
    { TournamentMap::Cache, TournamentStage::Semifinal, TournamentTeam::TeamEnVyUs, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Cache, TournamentStage::GrandFinal, TournamentTeam::Fnatic, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Cobblestone, TournamentStage::GroupStage, TournamentTeam::NatusVincere, TournamentTeam::Fnatic, {} },
    { TournamentMap::Cobblestone, TournamentStage::GroupStage, TournamentTeam::TeamEnVyUs, TournamentTeam::Titan, {} },
    { TournamentMap::Cobblestone, TournamentStage::Quarterfinal, TournamentTeam::TeamEnVyUs, TournamentTeam::NatusVincere, {} },
    { TournamentMap::Cobblestone, TournamentStage::Semifinal, TournamentTeam::VirtusPro, TournamentTeam::Fnatic, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::PENTASports, TournamentTeam::LGBEsports, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::PENTASports, TournamentTeam::LGBEsports, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::CounterLogicGaming, TournamentTeam::KeydStars, {} },
    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::TeamEnVyUs, TournamentTeam::NatusVincere, {} },
    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::TeamSoloMidKinguin, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Dust2, TournamentStage::Semifinal, TournamentTeam::TeamEnVyUs, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Dust2, TournamentStage::GrandFinal, TournamentTeam::Fnatic, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::Fnatic, TournamentTeam::VoxEminor, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::VoxEminor, TournamentTeam::NatusVincere, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::KeydStars, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::HellRaisers, TournamentTeam::KeydStars, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::VirtusPro, TournamentTeam::Cloud9G2A, {} },
    { TournamentMap::Inferno, TournamentStage::Quarterfinal, TournamentTeam::Fnatic, TournamentTeam::PENTASports, {} },
    { TournamentMap::Inferno, TournamentStage::Quarterfinal, TournamentTeam::TeamSoloMidKinguin, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Inferno, TournamentStage::GrandFinal, TournamentTeam::Fnatic, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::NatusVincere, TournamentTeam::Flipsid3Tactics, {} },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::CounterLogicGaming, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Mirage, TournamentStage::Quarterfinal, TournamentTeam::KeydStars, TournamentTeam::VirtusPro, {} },
    { TournamentMap::Mirage, TournamentStage::Semifinal, TournamentTeam::VirtusPro, TournamentTeam::Fnatic, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::CounterLogicGaming, TournamentTeam::HellRaisers, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::Cloud9G2A, TournamentTeam::TeamSoloMidKinguin, {} },
    { TournamentMap::Nuke, TournamentStage::GroupStage, TournamentTeam::TeamSoloMidKinguin, TournamentTeam::_3DMax, {} },
    { TournamentMap::Nuke, TournamentStage::Quarterfinal, TournamentTeam::KeydStars, TournamentTeam::VirtusPro, {} },
    { TournamentMap::Nuke, TournamentStage::Quarterfinal, TournamentTeam::TeamSoloMidKinguin, TournamentTeam::NinjasInPyjamas, {} },
    { TournamentMap::Overpass, TournamentStage::GroupStage, TournamentTeam::VirtusPro, TournamentTeam::_3DMax, {} },
    { TournamentMap::Overpass, TournamentStage::GroupStage, TournamentTeam::TeamSoloMidKinguin, TournamentTeam::Cloud9G2A, {} },
    { TournamentMap::Overpass, TournamentStage::Quarterfinal, TournamentTeam::KeydStars, TournamentTeam::VirtusPro, {} },
});
static_assert(std::ranges::is_sorted(eslOneKatowice2015Matches, {}, & Match::map));

constexpr auto eslOneCologne2015Matches = std::to_array<Match>({
    // Group A
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::NinjasInPyjamas, TournamentTeam::CounterLogicGaming, { ProPlayer::f0rest, ProPlayer::allu, ProPlayer::friberg, ProPlayer::Xizt, ProPlayer::GeT_RiGhT, ProPlayer::tarik, ProPlayer::reltuC, ProPlayer::FNS, ProPlayer::jdm64, ProPlayer::hazed } },
    { TournamentMap::Train, TournamentStage::GroupStage, TournamentTeam::TeamSoloMid, TournamentTeam::Renegades, { ProPlayer::device, ProPlayer::dupreeh, ProPlayer::karrigan, ProPlayer::Xyp9x, ProPlayer::cajunb, ProPlayer::AZR, ProPlayer::SPUNJ, ProPlayer::jks, ProPlayer::Havoc, ProPlayer::yam } },
    { TournamentMap::Cache, TournamentStage::GroupStage, TournamentTeam::NinjasInPyjamas, TournamentTeam::TeamSoloMid, { ProPlayer::f0rest, ProPlayer::GeT_RiGhT, ProPlayer::Xyp9x, ProPlayer::karrigan, ProPlayer::device, ProPlayer::dupreeh } },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::Renegades, TournamentTeam::Titan, { ProPlayer::SPUNJ, ProPlayer::AZR, ProPlayer::yam, ProPlayer::jks, ProPlayer::Maniac, ProPlayer::shox, ProPlayer::RpK, ProPlayer::SmithZz } },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::NinjasInPyjamas, TournamentTeam::Renegades, { ProPlayer::allu, ProPlayer::f0rest, ProPlayer::GeT_RiGhT, ProPlayer::Xizt, ProPlayer::jks, ProPlayer::SPUNJ } },

    // Group B
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::TeamEnVyUs, TournamentTeam::Flipsid3Tactics, {} },
    { TournamentMap::Overpass, TournamentStage::GroupStage, TournamentTeam::LuminosityGaming, TournamentTeam::TeamKinguin, {} },
    { TournamentMap::Cobblestone, TournamentStage::GroupStage, TournamentTeam::TeamEnVyUs, TournamentTeam::LuminosityGaming, {} },
    { TournamentMap::Cobblestone, TournamentStage::GroupStage, TournamentTeam::Mousesports, TournamentTeam::Flipsid3Tactics, {} },
    { TournamentMap::Cobblestone, TournamentStage::GroupStage, TournamentTeam::LuminosityGaming, TournamentTeam::Flipsid3Tactics, {} },

    // Group C
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::TeamEBettle, TournamentTeam::Fnatic, {} },
    { TournamentMap::Overpass, TournamentStage::GroupStage, TournamentTeam::NatusVincere, TournamentTeam::Titan, {} },
    { TournamentMap::Inferno, TournamentStage::GroupStage, TournamentTeam::Fnatic, TournamentTeam::NatusVincere, {} },
    { TournamentMap::Mirage, TournamentStage::GroupStage, TournamentTeam::CounterLogicGaming, TournamentTeam::TeamEBettle, {} },
    { TournamentMap::Cobblestone, TournamentStage::GroupStage, TournamentTeam::NatusVincere, TournamentTeam::CounterLogicGaming, {} },

    // Group D
    { TournamentMap::Cobblestone, TournamentStage::GroupStage, TournamentTeam::VirtusPro, TournamentTeam::TeamImmunity, {} },
    { TournamentMap::Overpass, TournamentStage::GroupStage, TournamentTeam::Mousesports, TournamentTeam::Cloud9G2A, {} },
    { TournamentMap::Cobblestone, TournamentStage::GroupStage, TournamentTeam::VirtusPro, TournamentTeam::Cloud9G2A, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::TeamImmunity, TournamentTeam::TeamKinguin, {} },
    { TournamentMap::Dust2, TournamentStage::GroupStage, TournamentTeam::TeamKinguin, TournamentTeam::Cloud9G2A, {} },

    // Quarterfinals
    { TournamentMap::Inferno, TournamentStage::Quarterfinal, TournamentTeam::NatusVincere, TournamentTeam::TeamEnVyUs, {} },
    { TournamentMap::Mirage, TournamentStage::Quarterfinal, TournamentTeam::NatusVincere, TournamentTeam::TeamEnVyUs, {} },

    { TournamentMap::Dust2, TournamentStage::Quarterfinal, TournamentTeam::TeamSoloMid, TournamentTeam::TeamKinguin, {} },
    { TournamentMap::Overpass, TournamentStage::Quarterfinal, TournamentTeam::TeamSoloMid, TournamentTeam::TeamKinguin, {} },

    { TournamentMap::Train, TournamentStage::Quarterfinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::VirtusPro, {} },
    { TournamentMap::Inferno, TournamentStage::Quarterfinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::VirtusPro, {} },

    { TournamentMap::Train, TournamentStage::Quarterfinal, TournamentTeam::Fnatic, TournamentTeam::LuminosityGaming, {} },
    { TournamentMap::Mirage, TournamentStage::Quarterfinal, TournamentTeam::Fnatic, TournamentTeam::LuminosityGaming, {} },

    // Semifinals
    { TournamentMap::Cache, TournamentStage::Semifinal, TournamentTeam::TeamSoloMid, TournamentTeam::TeamEnVyUs, {} },
    { TournamentMap::Dust2, TournamentStage::Semifinal, TournamentTeam::TeamSoloMid, TournamentTeam::TeamEnVyUs, {} },
    { TournamentMap::Inferno, TournamentStage::Semifinal, TournamentTeam::TeamSoloMid, TournamentTeam::TeamEnVyUs, {} },

    { TournamentMap::Mirage, TournamentStage::Semifinal, TournamentTeam::Fnatic, TournamentTeam::VirtusPro, {} },
    { TournamentMap::Inferno, TournamentStage::Semifinal, TournamentTeam::Fnatic, TournamentTeam::VirtusPro, {} },
    { TournamentMap::Cobblestone, TournamentStage::Semifinal, TournamentTeam::Fnatic, TournamentTeam::VirtusPro, {} },

    // Grand Final
    { TournamentMap::Dust2, TournamentStage::GrandFinal, TournamentTeam::Fnatic, TournamentTeam::TeamEnVyUs, {} },
    { TournamentMap::Cobblestone, TournamentStage::GrandFinal, TournamentTeam::Fnatic, TournamentTeam::TeamEnVyUs, {} },
});

constexpr auto tournaments = std::to_array<Tournament>({
    { 1, dreamHack2013Matches },
    { 3, emsOneKatowice2014Matches },
    { 4, eslOneCologne2014Matches },
    { 5, dreamHack2014Matches },
    { 6, eslOneKatowice2015Matches },
    { 7, eslOneCologne2015Matches }
});

static_assert(std::ranges::is_sorted(tournaments, {}, &Tournament::tournamentID));

[[nodiscard]] static std::span<const Match> getTournamentMatches(std::uint32_t tournamentID) noexcept
{
    if (const auto it = std::ranges::lower_bound(tournaments, tournamentID, {}, &Tournament::tournamentID); it != tournaments.end() && it->tournamentID == tournamentID)
        return it->matches;
    assert(false && "Missing tournament match data!");
    return {};
}

static auto operator<=>(TournamentMap a, TournamentMap b) noexcept
{
    return static_cast<std::underlying_type_t<TournamentMap>>(a) <=> static_cast<std::underlying_type_t<TournamentMap>>(b);
}

[[nodiscard]] static std::span<const Match> filterMatchesToMap(std::span<const Match> matches, TournamentMap map) noexcept
{
    if (map == TournamentMap::None)
        return matches;

    assert(std::ranges::is_sorted(matches, {}, &Match::map));

    if (const auto [begin, end] = std::ranges::equal_range(matches, map, {}, &Match::map); begin != end)
        return { begin, end };

    assert(false && "Couldn't find a match played on a map of a souvenir package!");
    return {};
}

[[nodiscard]] static auto generateSouvenirPackageData(const StaticData::Case& caseData) noexcept
{
    assert(caseData.isSouvenirPackage());
    DynamicSouvenirPackageData dynamicData;

    if (const auto matches = getTournamentMatches(caseData.souvenirPackageTournamentID); !matches.empty()) {
        if (const auto matchesOnMap = filterMatchesToMap(matches, caseData.tournamentMap); !matchesOnMap.empty()) {
            const auto& randomMatch = matchesOnMap[Helpers::random(0, matchesOnMap.size() - 1)];
            dynamicData.tournamentStage = randomMatch.stage;
            dynamicData.tournamentTeam1 = randomMatch.team1;
            dynamicData.tournamentTeam2 = randomMatch.team2;
            dynamicData.proPlayer = randomMatch.getRandomMVP();
        }
    }

    return dynamicData;
}

std::size_t ItemGenerator::createDefaultDynamicData(std::size_t gameItemIndex) noexcept
{
    std::size_t index = Inventory::INVALID_DYNAMIC_DATA_IDX;

    if (const auto& item = StaticData::gameItems()[gameItemIndex]; item.isSkin()) {
        const auto& staticData = StaticData::paintKits()[item.dataIndex];
        DynamicSkinData dynamicData;
        dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, Helpers::random(0.0f, 0.07f));
        dynamicData.seed = Helpers::random(1, 1000);
        index = Inventory::emplaceDynamicData(std::move(dynamicData));
    } else if (item.isGlove()) {
        const auto& staticData = StaticData::paintKits()[item.dataIndex];
        DynamicGloveData dynamicData;
        dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, Helpers::random(0.0f, 0.07f));
        dynamicData.seed = Helpers::random(1, 1000);
        index = Inventory::emplaceDynamicData(std::move(dynamicData));
    } else if (item.isAgent()) {
        index = Inventory::emplaceDynamicData(DynamicAgentData{});
    } else if (item.isMusic()) {
        index = Inventory::emplaceDynamicData(DynamicMusicData{});
    } else if (item.isCase()) {
        if (const auto& staticData = StaticData::cases()[item.dataIndex]; staticData.isSouvenirPackage())
            index = Inventory::emplaceDynamicData(generateSouvenirPackageData(staticData));
    }

    return index;
}

[[nodiscard]] static const Match* findTournamentMatch(std::uint32_t tournamentID, TournamentMap map, TournamentStage stage, TournamentTeam team1, TournamentTeam team2) noexcept
{
    const auto matches = getTournamentMatches(tournamentID);
    if (matches.empty())
        return nullptr;

    const auto matchesOnMap = filterMatchesToMap(matches, map);
    if (matchesOnMap.empty())
        return nullptr;

    const auto match = std::ranges::find_if(matchesOnMap, [stage, team1, team2](const auto& match) { return match.stage == stage && match.team1 == team1 && match.team2 == team2; });
    return (match != matchesOnMap.end() ? &*match : nullptr);
}

[[nodiscard]] static std::array<StickerConfig, 5> generateSouvenirStickers(std::uint32_t tournamentID, TournamentMap map, TournamentStage stage, TournamentTeam team1, TournamentTeam team2, ProPlayer player) noexcept
{
    std::array<StickerConfig, 5> stickers;

    stickers[0].stickerID = StaticData::findSouvenirTournamentSticker(tournamentID);

    if (tournamentID != 1) {
        stickers[1].stickerID = StaticData::getTournamentTeamGoldStickerID(tournamentID, team1);
        stickers[2].stickerID = StaticData::getTournamentTeamGoldStickerID(tournamentID, team2);
        if (const auto match = findTournamentMatch(tournamentID, map, stage, team1, team2); match && match->hasMVPs())
            stickers[3].stickerID = StaticData::getTournamentPlayerGoldStickerID(tournamentID, static_cast<int>(player));
    }

    std::mt19937 gen{ std::random_device{}() };
    std::shuffle(stickers.begin(), stickers.begin() + 4, gen);

    return stickers;
}
