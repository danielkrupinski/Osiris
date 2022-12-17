#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <random>

#include <InventoryChanger/Inventory/Structs.h>
#include <CSGO/Constants/PaintkitConditionChances.h>
#include <CSGO/Constants/PaintkitWear.h>

#include "ItemGenerator.h"
#include "TournamentMatches.h"

namespace csgo { enum class StickerId : int; }

namespace inventory_changer::item_generator
{

template <typename RandomEngine>
class AttributeGenerator {
public:
    explicit AttributeGenerator(RandomEngine& randomEngine) : randomEngine{ randomEngine } {}

    [[nodiscard]] float generatePaintKitWear() const
    {
        using namespace csgo::paintkit_wear;
        static constexpr auto wearRanges = std::to_array<float>({ factoryNew, minimalWear, fieldTested, wellWorn, battleScarred, maxWear });

        using namespace csgo::paintkit_condition_chances;
        static constexpr auto conditionChances = std::to_array<float>({ factoryNewChance, minimalWearChance, fieldTestedChance, wellWornChance, battleScarredChance });

        return randomEngine(std::piecewise_constant_distribution<float>{ wearRanges.begin(), wearRanges.end(), conditionChances.begin() });
    }

    [[nodiscard]] float generateFactoryNewPaintKitWear() const
    {
        using namespace csgo::paintkit_wear;
        return randomEngine(std::uniform_real_distribution<float>{ factoryNew, minimalWear });
    }

    [[nodiscard]] int generatePaintKitSeed() const
    {
        return randomEngine(std::uniform_int_distribution<>{ 1, 1000 });
    }

    [[nodiscard]] std::uint32_t generateTimestamp(std::uint32_t minTimestamp, std::uint32_t maxTimestamp) const
    {
        assert(maxTimestamp >= minTimestamp);
        return randomEngine(std::uniform_int_distribution<std::uint32_t>{ minTimestamp, maxTimestamp });
    }

    [[nodiscard]] inventory::SouvenirPackage generateSouvenirPackage(csgo::Tournament tournament, TournamentMap map) const
    {
        return std::visit([this](const auto& matches) {
            inventory::SouvenirPackage souvenirPackage;

            if (matches.empty())
                return souvenirPackage;

            const auto& randomMatch = matches[randomEngine(std::uniform_int_distribution<std::size_t>{ 0, matches.size() - 1 })];
            souvenirPackage.tournamentStage = randomMatch.stage;
            souvenirPackage.tournamentTeam1 = randomMatch.team1;
            souvenirPackage.tournamentTeam2 = randomMatch.team2;

            if constexpr (std::is_same_v<decltype(randomMatch), const MatchWithMVPs&>) {
                if (const auto numberOfMVPs = countMVPs(randomMatch); numberOfMVPs > 0)
                    souvenirPackage.proPlayer = randomMatch.mvpPlayers[randomEngine(std::uniform_int_distribution<std::size_t>{ 0, numberOfMVPs - 1 })];
            }

            return souvenirPackage;
        }, getTournamentMatchesOnMap(tournament, map));
    }

    void shuffleStickers(std::uint8_t numberOfSlots, inventory::SkinStickers& stickers) const
    {
        assert(numberOfSlots <= stickers.size());
        std::shuffle(stickers.begin(), stickers.begin() + numberOfSlots, randomEngine);
    }

    [[nodiscard]] bool generateStatTrak() const
    {
        return randomEngine(std::uniform_int_distribution<>{ 0, 9 }) == 0;
    }

    [[nodiscard]] csgo::StickerId randomStickerId(csgo::StickerId min, csgo::StickerId max) const
    {
        assert(max >= min);
        return static_cast<csgo::StickerId>(randomEngine(std::uniform_int_distribution<>{ static_cast<int>(min), static_cast<int>(max) }));
    }

private:
    RandomEngine& randomEngine;
};

}
