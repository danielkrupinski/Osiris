#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <random>

#include <InventoryChanger/Inventory/Structs.h>

#include "ItemGenerator.h"
#include "TournamentMatches.h"
#include "Utils.h"

namespace inventory_changer::item_generator
{

template <typename RandomEngine>
class AttributeGenerator {
public:
    explicit AttributeGenerator(RandomEngine& randomEngine) : randomEngine{ randomEngine } {}

    [[nodiscard]] PaintKitCondition generatePaintKitCondition() const
    {
        if (const auto condition = randomEngine.random(1, 10'000); condition <= 1471)
            return FactoryNew;
        else if (condition <= 3939)
            return MinimalWear;
        else if (condition <= 8257)
            return FieldTested;
        else if (condition <= 9049)
            return WellWorn;
        return BattleScarred;
    }

    [[nodiscard]] float generatePaintKitWear(PaintKitCondition condition) const
    {
        static constexpr auto wearRanges = std::to_array<float>({ 0.0f, 0.07f, 0.15f, 0.38f, 0.45f, 1.0f });
        return randomEngine.random(wearRanges[condition - 1], wearRanges[condition]);
    }

    [[nodiscard]] int generatePaintKitSeed() const
    {
        return randomEngine.random(1, 1000);
    }

    [[nodiscard]] std::uint32_t generateServiceMedalIssueDate(std::uint16_t year) const
    {
        return getRandomDateTimestampOfYear(year);
    }

    [[nodiscard]] inventory::SouvenirPackage generateSouvenirPackage(std::uint32_t tournamentID, TournamentMap map) const
    {
        return std::visit([this](const auto& matches) {
            inventory::SouvenirPackage souvenirPackage;

            if (matches.empty())
                return souvenirPackage;

            const auto& randomMatch = matches[randomEngine.random(std::size_t{ 0 }, matches.size() - 1)];
            souvenirPackage.tournamentStage = randomMatch.stage;
            souvenirPackage.tournamentTeam1 = randomMatch.team1;
            souvenirPackage.tournamentTeam2 = randomMatch.team2;

            if constexpr (std::is_same_v<decltype(randomMatch), const MatchWithMVPs&>) {
                if (const auto numberOfMVPs = countMVPs(randomMatch); numberOfMVPs > 0)
                    souvenirPackage.proPlayer = randomMatch.mvpPlayers[randomEngine.random(std::size_t{ 0 }, numberOfMVPs - 1)];
            }

            return souvenirPackage;
        }, getTournamentMatchesOnMap(tournamentID, map));
    }

    void shuffleStickers(std::uint8_t numberOfSlots, inventory::SkinStickers& stickers) const
    {
        assert(numberOfSlots <= stickers.size());
        std::shuffle(stickers.begin(), stickers.begin() + numberOfSlots, randomEngine);
    }

    [[nodiscard]] bool generateStatTrak() const
    {
        return randomEngine.random(0, 9) == 0;
    }

private:
    [[nodiscard]] static std::pair<std::time_t, std::time_t> clampTimespanToNow(std::time_t min, std::time_t max) noexcept
    {
        const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        return std::make_pair((std::min)(min, now), (std::min)(max, now));
    }

    [[nodiscard]] std::uint32_t getRandomDateTimestampOfYear(std::uint16_t year) const noexcept
    {
        const auto [min, max] = clampTimespanToNow(getStartOfYearTimestamp(year), getEndOfYearTimestamp(year));
        return static_cast<std::uint32_t>(randomEngine.random(min, max));
    }

    RandomEngine& randomEngine;
};

}
