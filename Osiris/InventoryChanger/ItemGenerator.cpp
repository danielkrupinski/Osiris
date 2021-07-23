#include <cmath>
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
        } else if (Helpers::random(0, 9) == 0) {
            dynamicData.statTrak = 0;
        }

        dynamicDataIdx = Inventory::emplaceDynamicData(std::move(dynamicData));
    }

    return std::make_pair(unlockedItemIdx, dynamicDataIdx);
}

using StaticData::TournamentMap;

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

};

constexpr auto tournaments = std::to_array<Tournament>({
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
