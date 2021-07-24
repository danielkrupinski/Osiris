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

[[nodiscard]] static std::array<StickerConfig, 5> generateSouvenirStickers(std::uint32_t tournamentID, TournamentMap map, TournamentStage stage, TournamentTeam team1, TournamentTeam team2) noexcept;

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
            dynamicData.stickers = generateSouvenirStickers(caseData.souvenirPackageTournamentID, caseData.tournamentMap, dynamicData.tournamentStage, dynamicData.tournamentTeam1, dynamicData.tournamentTeam2);
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
    std::vector<int> mvpPlayersIDs;
};

struct Tournament {
    std::uint32_t tournamentID;
    const std::vector<Match>& matches;
};

static const std::vector<Match> dreamHack2013Matches{
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
};

static const std::vector<Match> emsOneKatowice2014Matches{
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
    { TournamentMap::Mirage, TournamentStage::Semifinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::VirtusPro, {} },
    { TournamentMap::Inferno, TournamentStage::Semifinal, TournamentTeam::NinjasInPyjamas, TournamentTeam::VirtusPro, {} },
};

constexpr auto tournaments = std::to_array<Tournament>({
    { 1, dreamHack2013Matches },
    { 3, emsOneKatowice2014Matches }
});

static_assert(std::ranges::is_sorted(tournaments, {}, & Tournament::tournamentID));

[[nodiscard]] static const std::vector<Match>* getTournamentMatches(std::uint32_t tournamentID) noexcept
{
    if (const auto it = std::ranges::lower_bound(tournaments, tournamentID, {}, &Tournament::tournamentID); it != tournaments.end() && it->tournamentID == tournamentID)
        return &it->matches;
    assert(false && "Missing tournament match data!");
    return nullptr;
}

static auto operator<=>(TournamentMap a, TournamentMap b) noexcept
{
    return static_cast<std::underlying_type_t<TournamentMap>>(a) <=> static_cast<std::underlying_type_t<TournamentMap>>(b);
}

[[nodiscard]] static std::span<const Match> filterMatchesToMap(const std::vector<Match>& matches, TournamentMap map) noexcept
{
    if (map == TournamentMap::None)
        return matches;

    if (const auto [begin, end] = std::ranges::equal_range(matches, map, {}, &Match::map); begin != end)
        return { begin, end };

    assert(false && "Couldn't find a match played on a map of a souvenir package!");
    return {};
}

[[nodiscard]] static auto generateSouvenirPackageData(const StaticData::Case& caseData) noexcept
{
    assert(caseData.isSouvenirPackage());
    DynamicSouvenirPackageData dynamicData;

    if (const auto matches = getTournamentMatches(caseData.souvenirPackageTournamentID)) {
        if (const auto matchesOnMap = filterMatchesToMap(*matches, caseData.tournamentMap); !matchesOnMap.empty()) {
            const auto& randomMatch = matchesOnMap[Helpers::random(0, matchesOnMap.size() - 1)];
            dynamicData.tournamentStage = randomMatch.stage;
            dynamicData.tournamentTeam1 = randomMatch.team1;
            dynamicData.tournamentTeam2 = randomMatch.team2;
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
    if (!matches)
        return nullptr;

    const auto matchesOnMap = filterMatchesToMap(*matches, map);
    if (matchesOnMap.empty())
        return nullptr;

    const auto match = std::ranges::find_if(matchesOnMap, [stage, team1, team2](const auto& match) { return match.stage == stage && match.team1 == team1 && match.team2 == team2; });
    return (match != matchesOnMap.end() ? &*match : nullptr);
}

[[nodiscard]] static std::array<StickerConfig, 5> generateSouvenirStickers(std::uint32_t tournamentID, TournamentMap map, TournamentStage stage, TournamentTeam team1, TournamentTeam team2) noexcept
{
    std::array<StickerConfig, 5> stickers;

    stickers[0].stickerID = StaticData::findSouvenirTournamentSticker(tournamentID);

    if (tournamentID != 1) {
        if (team1 != TournamentTeam{})
            stickers[1].stickerID = StaticData::findTournamentGoldSticker(tournamentID, team1, 0);

        if (team2 != TournamentTeam{})
            stickers[2].stickerID = StaticData::findTournamentGoldSticker(tournamentID, team2, 0); 
    }

    std::mt19937 gen{ std::random_device{}() };
    std::shuffle(stickers.begin(), stickers.begin() + 4, gen);

    return stickers;
}
