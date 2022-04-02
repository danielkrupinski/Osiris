#pragma once

#include <array>
#include <cassert>
#include <optional>
#include <string>
#include <vector>

#include "StaticData.h"

#include "GameItems/Item.h"
#include "Inventory/Item.h"

enum class Team;

namespace inventory
{
    struct Item;
    struct Skin;
    struct Glove;
    struct Agent;
    struct Music;
    struct SouvenirPackage;
    struct ServiceMedal;
    struct TournamentCoin;
    struct Graffiti;

    class ItemIDMap {
    public:
        [[nodiscard]] std::vector<Item_v2>::iterator get(std::uint64_t itemID)
        {
            return std::vector<Item_v2>::iterator{};
        }

    private:
        [[maybe_unused]] std::unordered_map<std::uint64_t, std::vector<Item_v2>::iterator> itemIDs;
    };
}

namespace Inventory
{
    constexpr auto InvalidDynamicDataIdx = static_cast<std::size_t>(-1);
    constexpr auto BASE_ITEMID = 1152921504606746975;

    void deleteItemNow(std::uint64_t itemID) noexcept;
    void runFrame() noexcept;
    inventory::Item* getItem(std::uint64_t itemID) noexcept;
    std::uint64_t recreateItem(std::uint64_t itemID) noexcept;
    std::size_t getItemIndex(std::uint64_t itemID) noexcept;

    inventory::Skin& dynamicSkinData(const inventory::Item& item) noexcept;
    inventory::Glove& dynamicGloveData(const inventory::Item& item) noexcept;
    inventory::Agent& dynamicAgentData(const inventory::Item& item) noexcept;
    inventory::Music& dynamicMusicData(const inventory::Item& item) noexcept;
    inventory::SouvenirPackage& dynamicSouvenirPackageData(const inventory::Item& item) noexcept;
    inventory::ServiceMedal& dynamicServiceMedalData(const inventory::Item& item) noexcept;
    inventory::TournamentCoin& dynamicTournamentCoinData(const inventory::Item& item) noexcept;
    inventory::Graffiti& dynamicGraffitiData(const inventory::Item& item) noexcept;

    std::size_t emplaceDynamicData(inventory::Skin&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::Glove&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::Agent&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::Music&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::SouvenirPackage&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::ServiceMedal&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::TournamentCoin&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::Graffiti&& data) noexcept;
}
