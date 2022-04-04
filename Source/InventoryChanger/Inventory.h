#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <list>
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
        [[nodiscard]] std::optional<std::list<Item_v2>::const_iterator> get(std::uint64_t itemID) const
        {
            const auto it = itemIDs.find(itemID);
            return it != itemIDs.end() ? std::make_optional(it->second) : std::nullopt;
        }

        [[nodiscard]] std::optional<std::uint64_t> getItemID(std::list<Item_v2>::const_iterator iterator) const
        {
            if (const auto it = std::ranges::find(itemIDs, iterator, &decltype(itemIDs)::value_type::second); it != itemIDs.end())
                return it->first;
            return {};
        }

        void add(std::uint64_t itemID, std::list<Item_v2>::const_iterator iterator)
        {
            itemIDs.emplace(itemID, iterator);
        }

        std::optional<std::uint64_t> remove(std::list<Item_v2>::const_iterator iterator)
        {
            if (const auto it = std::ranges::find(itemIDs, iterator, &decltype(itemIDs)::value_type::second); it != itemIDs.end()) {
                const auto itemID = it->first;
                itemIDs.erase(it);
                return itemID;
            }
            return {};
        }

        [[nodiscard]] static ItemIDMap instance()
        {
            static ItemIDMap map;
            return map;
        }

    private:
        std::unordered_map<std::uint64_t, std::list<Item_v2>::const_iterator> itemIDs;
    };
}

namespace Inventory
{
    constexpr auto InvalidDynamicDataIdx = static_cast<std::size_t>(-1);
    constexpr auto BASE_ITEMID = 1152921504606746975;

    void runFrame() noexcept;
    inventory::Item* getItem(std::uint64_t itemID) noexcept;

    inventory::Skin& dynamicSkinData(const inventory::Item& item) noexcept;
    inventory::Glove& dynamicGloveData(const inventory::Item& item) noexcept;
    inventory::Agent& dynamicAgentData(const inventory::Item& item) noexcept;
    inventory::Music& dynamicMusicData(const inventory::Item& item) noexcept;
    inventory::SouvenirPackage& dynamicSouvenirPackageData(const inventory::Item& item) noexcept;
    inventory::ServiceMedal& dynamicServiceMedalData(const inventory::Item& item) noexcept;
    inventory::TournamentCoin& dynamicTournamentCoinData(const inventory::Item& item) noexcept;
    inventory::Graffiti& dynamicGraffitiData(const inventory::Item& item) noexcept;
}
