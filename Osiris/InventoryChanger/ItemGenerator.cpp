#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <random>
#include <span>
#include <vector>

#include "../Helpers.h"
#include "Inventory.h"
#include "ItemGenerator.h"

#include "../SDK/ItemSchema.h"
#include "TournamentMatches.h"
#include "GameItems/Lookup.h"

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

[[nodiscard]] static std::array<StickerConfig, 5> generateSouvenirStickers(WeaponId weaponID, std::uint32_t tournamentID, TournamentMap map, TournamentStage stage, TournamentTeam team1, TournamentTeam team2, ProPlayer player) noexcept;

template <typename Integral, std::size_t N>
[[nodiscard]] constexpr auto normalizedFloatsToIntegers(const std::array<float, N>& floats) noexcept
{
    std::array<Integral, N> integers;
    for (std::size_t i = 0; i < N; ++i)
        integers[i] = static_cast<Integral>(floats[i] * (std::numeric_limits<Integral>::max)());
    return integers;
}

class DropRate {
public:
    static constexpr auto MaxDistinctRarities = 6; // in Cobblestone souvenir package, gray to red
    using T = std::uint16_t;

    constexpr DropRate(StaticData::EconRarities rarities, const std::array<float, MaxDistinctRarities>& chances)
        : rarities{ rarities }, chances{ normalizedFloatsToIntegers<T>(chances) } {}

    [[nodiscard]] constexpr EconRarity mapToRarity(T number) const
    {
        for (std::uint8_t i = 0; i < chances.size(); ++i) {
            if (number < chances[i])
                return rarities.getNthRarity(i);
            number -= chances[i];
        }
        return rarities.getNthRarity(0);
    }

    StaticData::EconRarities rarities;
    std::array<T, MaxDistinctRarities> chances{};
};

constexpr auto dropRates = std::to_array<DropRate>({
    { { EconRarity::Blue, EconRarity::Purple, EconRarity::Pink, EconRarity::Red }, { 0.7992f, 0.1598f, 0.032f, 0.0064f } },
    { { EconRarity::Blue, EconRarity::Purple, EconRarity::Pink, EconRarity::Red, EconRarity::Gold }, { 0.7992f, 0.1598f, 0.032f, 0.0064f, 0.0026f } },
    { { EconRarity::Gray, EconRarity::LightBlue, EconRarity::Blue }, { 0.7992f, 0.1598f, 0.041f } }, // Inferno souvenir package
    { { EconRarity::Blue, EconRarity::Purple }, { 0.80f, 0.20f } }, // Stockholm 2021 Team Patch Packs, Sticker capsules: Pinups, Sugarface, Poorly Drawn, Recoil
    { { EconRarity::LightBlue, EconRarity::Blue, EconRarity::Purple }, { 0.7992f, 0.1598f, 0.041f } }, // Cache souvenir package
    { { EconRarity::Gray, EconRarity::LightBlue, EconRarity::Blue, EconRarity::Purple }, { 0.80f, 0.16f, 0.03f, 0.01f } }, // Souvenir package: Nuke, Mirage, Train, Vertigo
    { { EconRarity::Blue, EconRarity::Pink }, { 0.95f, 0.05f } }, // Team Roles sticker capsule
    { { EconRarity::Purple, EconRarity::Pink }, { 0.80f, 0.20f } }, // Tournament Team (holo / foil) sticker capsules
    { { EconRarity::Blue, EconRarity::Purple, EconRarity::Pink }, { 0.7992f, 0.1598f, 0.041f } }, // Regular sticker capsules
    { { EconRarity::Gray, EconRarity::LightBlue, EconRarity::Blue, EconRarity::Purple, EconRarity::Pink }, { 0.80f, 0.16f, 0.032f, 0.0064f, 0.0016f } }, // DreamHack2013 Collections, Souvenir package: Nuke 2018, Inferno 2018, Overpass, Dust 2
    { { EconRarity::Blue, EconRarity::Purple, EconRarity::Red }, { 0.8013f, 0.1923f, 0.0064f } }, // Autograph sticker capsules
    { { EconRarity::Gray, EconRarity::LightBlue, EconRarity::Blue, EconRarity::Purple, EconRarity::Pink, EconRarity::Red }, { 0.80f, 0.16f, 0.032f, 0.0064f, 0.0012f, 0.0004f } }, // Souvenir package: Cobblestone, Ancient, Vertigo 2021, Mirage 2021, Dust 2 2021
});

struct RareSpecialItem {
    WeaponId weaponID;
    int paintKit;
};

constexpr auto operationRiptideKnives = std::to_array<RareSpecialItem>({
    { WeaponId::Bowie, 1104 }, // Lore
    { WeaponId::Bowie, 568 }, // Gamma Doppler Emerald
    { WeaponId::Bowie, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Bowie, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Bowie, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Bowie, 572 }, // Gamma Doppler Phase 4
    { WeaponId::Bowie, 1114 }, // Autotronic
    { WeaponId::Bowie, 1109 }, // Black Laminate
    { WeaponId::Bowie, 581 }, // Freehand
    { WeaponId::Bowie, 579 }, // Bright Water

    { WeaponId::Butterfly, 1115 }, // Autotronic
    { WeaponId::Butterfly, 579 }, // Bright Water
    { WeaponId::Butterfly, 1105 }, // Lore
    { WeaponId::Butterfly, 1110 }, // Black Laminate
    { WeaponId::Butterfly, 581 }, // Freehand
    { WeaponId::Butterfly, 568 }, // Gamma Doppler Emerald
    { WeaponId::Butterfly, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Butterfly, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Butterfly, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Butterfly, 572 }, // Gamma Doppler Phase 4

    { WeaponId::Falchion, 568 }, // Gamma Doppler Emerald
    { WeaponId::Falchion, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Falchion, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Falchion, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Falchion, 572 }, // Gamma Doppler Phase 4
    { WeaponId::Falchion, 1106 }, // Lore
    { WeaponId::Falchion, 1116 }, // Autotronic
    { WeaponId::Falchion, 1111 }, // Black Laminate
    { WeaponId::Falchion, 579 }, // Bright Water
    { WeaponId::Falchion, 581 }, // Freehand

    { WeaponId::Huntsman, 1107 }, // Lore
    { WeaponId::Huntsman, 568 }, // Gamma Doppler Emerald
    { WeaponId::Huntsman, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Huntsman, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Huntsman, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Huntsman, 572 }, // Gamma Doppler Phase 4
    { WeaponId::Huntsman, 1112 }, // Black Laminate
    { WeaponId::Huntsman, 1117 }, // Autotronic
    { WeaponId::Huntsman, 581 }, // Freehand
    { WeaponId::Huntsman, 579 }, // Bright Water

    { WeaponId::Daggers, 568 }, // Gamma Doppler Emerald
    { WeaponId::Daggers, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Daggers, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Daggers, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Daggers, 572 }, // Gamma Doppler Phase 4
    { WeaponId::Daggers, 1108 }, // Lore
    { WeaponId::Daggers, 1118 }, // Autotronic
    { WeaponId::Daggers, 1113 }, // Black Laminate
    { WeaponId::Daggers, 581 }, // Freehand
    { WeaponId::Daggers, 579 }, // Bright Water
});

struct CrateRareSpecialItems {
    WeaponId crateWeaponID;
    std::span<const RareSpecialItem> items;
};

constexpr auto crateRareSpecialItems = std::to_array<CrateRareSpecialItems>({
    { WeaponId::OperationRiptideCase, operationRiptideKnives },
    { WeaponId::DreamsAndNightmaresCase, operationRiptideKnives }
});

[[nodiscard]] constexpr std::span<const RareSpecialItem> getRareSpecialItems(WeaponId crateWeaponID)
{
    if (const auto it = std::ranges::find(crateRareSpecialItems, crateWeaponID, &CrateRareSpecialItems::crateWeaponID); it != crateRareSpecialItems.end())
        return it->items;
    return {};
}

[[nodiscard]] static EconRarity getRandomRarity(StaticData::EconRarities rarities)
{
    if (const auto rate = std::ranges::find(dropRates, rarities, &DropRate::rarities); rate != dropRates.end()) {
        const auto rolledNumber = Helpers::random((std::numeric_limits<DropRate::T>::min)(), (std::numeric_limits<DropRate::T>::max)());
        return rate->mapToRarity(rolledNumber);
    }
    return EconRarity::Default;
}

[[nodiscard]] const game_items::Item& getRandomItemIndexFromContainer(WeaponId weaponID, const StaticData::Case& container) noexcept
{
    assert(container.hasLoot());

    const auto rareSpecialItems = getRareSpecialItems(weaponID);
    auto rarities = container.rarities;

    if (!rareSpecialItems.empty())
        rarities.set(EconRarity::Gold);

    if (const auto rarity = getRandomRarity(rarities); rarity != EconRarity::Default) {
        if (rarity == EconRarity::Gold) {
            const auto& randomRareSpecialItem = rareSpecialItems[Helpers::random<std::size_t>(0u, rareSpecialItems.size() - 1u)];
            if (const auto item = StaticData::lookup().findItem(randomRareSpecialItem.weaponID, randomRareSpecialItem.paintKit); item.has_value())
                return *item;
        } else {
            const auto loot = StaticData::getCrateLootOfRarity(container, rarity);
            return loot[Helpers::random<std::size_t>(0u, loot.size() - 1u)];
        }
    }

    std::span<const std::reference_wrapper<const game_items::Item>> loot = StaticData::getCrateLoot(container);
    assert(!loot.empty());
    return loot[Helpers::random<std::size_t>(0u, loot.size() - 1u)];
}

std::pair<const game_items::Item&, std::size_t> ItemGenerator::generateItemFromContainer(const InventoryItem& caseItem) noexcept
{
    assert(caseItem.isCase());

    const auto& caseData = StaticData::getCase(caseItem.get());
    assert(caseData.hasLoot());

    const auto& unlockedItem = getRandomItemIndexFromContainer(caseItem.get().getWeaponID(), caseData);
    std::size_t dynamicDataIdx = Inventory::InvalidDynamicDataIdx;

    if (caseData.willProduceStatTrak && unlockedItem.isMusic()) {
        DynamicMusicData dynamicData;
        dynamicData.statTrak = 0;
        dynamicDataIdx = Inventory::emplaceDynamicData(std::move(dynamicData));
    } else if (unlockedItem.isSkin()) {
        DynamicSkinData dynamicData;
        const auto& staticData = StaticData::lookup().getStorage().getPaintKit(unlockedItem);
        dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, generateWear());
        dynamicData.seed = Helpers::random(1, 1000);

        if (StaticData::isSouvenirPackage(caseItem.get())) {
            dynamicData.tournamentID = caseData.souvenirPackageTournamentID;
            const auto& souvenir = Inventory::dynamicSouvenirPackageData(caseItem.getDynamicDataIndex());
            dynamicData.tournamentStage = souvenir.tournamentStage;
            dynamicData.tournamentTeam1 = souvenir.tournamentTeam1;
            dynamicData.tournamentTeam2 = souvenir.tournamentTeam2;
            dynamicData.proPlayer = souvenir.proPlayer;
            dynamicData.stickers = generateSouvenirStickers(unlockedItem.getWeaponID(), caseData.souvenirPackageTournamentID, caseData.tournamentMap, dynamicData.tournamentStage, dynamicData.tournamentTeam1, dynamicData.tournamentTeam2, dynamicData.proPlayer);
        } else if (Helpers::random(0, 9) == 0) {
            dynamicData.statTrak = 0;
        }

        dynamicDataIdx = Inventory::emplaceDynamicData(std::move(dynamicData));
    }

    return std::make_pair(std::cref(unlockedItem), dynamicDataIdx);
}

[[nodiscard]] static std::span<const Match> getTournamentMatches(std::uint32_t tournamentID) noexcept
{
    if (const auto it = std::ranges::lower_bound(tournaments, tournamentID, {}, &Tournament::tournamentID); it != tournaments.end() && it->tournamentID == tournamentID)
        return it->matches;
    return {};
}

constexpr auto operator<=>(TournamentMap a, TournamentMap b) noexcept
{
    return static_cast<std::underlying_type_t<TournamentMap>>(a) <=> static_cast<std::underlying_type_t<TournamentMap>>(b);
}

[[nodiscard]] static std::span<const Match> filterMatchesToMap(std::span<const Match> matches, TournamentMap map) noexcept
{
    if (map == TournamentMap::None)
        return matches;

    assert(std::ranges::is_sorted(matches, {}, &Match::map));

    struct Comp {
        bool operator()(TournamentMap map, const Match& match) const noexcept { return map < match.map; }
        bool operator()(const Match& match, TournamentMap map) const noexcept { return match.map < map; }
    };

    // not using std::ranges::equal_range() here because clang 12 on linux doesn't support it yet
    if (const auto [begin, end] = std::equal_range(matches.begin(), matches.end(), map, Comp{}); begin != end)
        return { begin, end };

    return {};
}

[[nodiscard]] static auto generateSouvenirPackageData(const StaticData::Case& caseData) noexcept
{
    DynamicSouvenirPackageData dynamicData;

    if (const auto matches = getTournamentMatches(caseData.souvenirPackageTournamentID); !matches.empty()) {
        if (const auto matchesOnMap = filterMatchesToMap(matches, caseData.tournamentMap); !matchesOnMap.empty()) {
            const auto& randomMatch = matchesOnMap[Helpers::random(static_cast<std::size_t>(0), matchesOnMap.size() - 1)];
            dynamicData.tournamentStage = randomMatch.stage;
            dynamicData.tournamentTeam1 = randomMatch.team1;
            dynamicData.tournamentTeam2 = randomMatch.team2;
            dynamicData.proPlayer = randomMatch.getRandomMVP();
        }
    }

    return dynamicData;
}

[[nodiscard]] std::time_t tmToUTCTimestamp(std::tm& tm) noexcept
{
#ifdef _WIN32
    return _mkgmtime(&tm);
#else
    return timegm(&tm);
#endif
}

[[nodiscard]] std::time_t getStartOfYearTimestamp(std::uint16_t year) noexcept
{
    assert(year >= 1900);
    std::tm tm{};
    tm.tm_mday = 1;
    tm.tm_year = year - 1900;
    return tmToUTCTimestamp(tm);
}

[[nodiscard]] std::time_t getEndOfYearTimestamp(std::uint16_t year) noexcept
{
    assert(year >= 1900);
    std::tm tm{};
    tm.tm_sec = 59;
    tm.tm_min = 59;
    tm.tm_hour = 23;
    tm.tm_mday = 31;
    tm.tm_mon = 12 - 1;
    tm.tm_year = year - 1900;
    return tmToUTCTimestamp(tm);
}

[[nodiscard]] static std::pair<std::time_t, std::time_t> clampTimespanToNow(std::time_t min, std::time_t max) noexcept
{
    const auto now = std::time(nullptr);
    return std::make_pair((std::min)(min, now), (std::min)(max, now));
}

[[nodiscard]] static std::uint32_t getRandomDateTimestampOfYear(std::uint16_t year) noexcept
{
    const auto [min, max] = clampTimespanToNow(getStartOfYearTimestamp(year), getEndOfYearTimestamp(year));
    return static_cast<std::uint32_t>(Helpers::random(min, max));
}

std::size_t ItemGenerator::createDefaultDynamicData(const game_items::Item& item) noexcept
{
    std::size_t index = Inventory::InvalidDynamicDataIdx;

    if (item.isSkin()) {
        const auto& staticData = StaticData::lookup().getStorage().getPaintKit(item);
        DynamicSkinData dynamicData;
        dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, Helpers::random(0.0f, 0.07f));
        dynamicData.seed = Helpers::random(1, 1000);

        if (Helpers::isMP5LabRats(item.getWeaponID(), StaticData::lookup().getStorage().getPaintKit(item).id))
            dynamicData.stickers[3].stickerID = 28;

        index = Inventory::emplaceDynamicData(std::move(dynamicData));
    } else if (item.isGloves()) {
        const auto& staticData = StaticData::lookup().getStorage().getPaintKit(item);
        DynamicGloveData dynamicData;
        dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, Helpers::random(0.0f, 0.07f));
        dynamicData.seed = Helpers::random(1, 1000);
        index = Inventory::emplaceDynamicData(std::move(dynamicData));
    } else if (item.isAgent()) {
        index = Inventory::emplaceDynamicData(DynamicAgentData{});
    } else if (item.isMusic()) {
        index = Inventory::emplaceDynamicData(DynamicMusicData{});
    } else if (item.isCase()) {
        if (const auto& staticData = StaticData::getCase(item); StaticData::isSouvenirPackage(item))
            index = Inventory::emplaceDynamicData(generateSouvenirPackageData(staticData));
    } else if (item.isServiceMedal()) {
        DynamicServiceMedalData dynamicData;
        dynamicData.issueDateTimestamp = getRandomDateTimestampOfYear(StaticData::lookup().getStorage().getServiceMedalYear(item));
        index = Inventory::emplaceDynamicData(std::move(dynamicData));
    } else if (item.isTournamentCoin()) {
        index = Inventory::emplaceDynamicData(DynamicTournamentCoinData{});
    }

    return index;
}

[[nodiscard]] static const Match* findTournamentMatch(std::uint32_t tournamentID, TournamentMap map, TournamentStage stage, TournamentTeam team1, TournamentTeam team2) noexcept
{
    const auto matchesOnMap = filterMatchesToMap(getTournamentMatches(tournamentID), map);
    if (matchesOnMap.empty())
        return nullptr;

    const auto match = std::ranges::find_if(matchesOnMap, [stage, team1, team2](const auto& match) { return match.stage == stage && match.team1 == team1 && match.team2 == team2; });
    return (match != matchesOnMap.end() ? &*match : nullptr);
}

[[nodiscard]] static std::uint8_t getNumberOfSupportedStickerSlots(WeaponId weaponID) noexcept
{
    if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(weaponID))
        return static_cast<std::uint8_t>(std::clamp(def->getNumberOfSupportedStickerSlots(), 0, 5));
    return 0;
}

[[nodiscard]] static std::array<StickerConfig, 5> generateSouvenirStickers(WeaponId weaponID, std::uint32_t tournamentID, TournamentMap map, TournamentStage stage, TournamentTeam team1, TournamentTeam team2, ProPlayer player) noexcept
{
    std::array<StickerConfig, 5> stickers;

    stickers[0].stickerID = StaticData::lookup().findTournamentEventStickerID(tournamentID);

    if (tournamentID != 1) {
        stickers[1].stickerID = StaticData::lookup().findTournamentTeamGoldStickerID(tournamentID, team1);
        stickers[2].stickerID = StaticData::lookup().findTournamentTeamGoldStickerID(tournamentID, team2);

        if (const auto match = findTournamentMatch(tournamentID, map, stage, team1, team2); match && match->hasMVPs())
            stickers[3].stickerID = StaticData::lookup().findTournamentPlayerGoldStickerID(tournamentID, static_cast<int>(player));
        else if (tournamentID >= 18) // starting with PGL Stockholm 2021
            stickers[3].stickerID = StaticData::getTournamentMapGoldStickerID(map);
    }

    std::mt19937 gen{ std::random_device{}() };
    std::shuffle(stickers.begin(), stickers.begin() + getNumberOfSupportedStickerSlots(weaponID), gen);

    return stickers;
}
