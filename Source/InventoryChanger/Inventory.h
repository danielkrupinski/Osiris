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
}

namespace Inventory
{
    constexpr auto InvalidDynamicDataIdx = static_cast<std::size_t>(-1);
    constexpr auto BASE_ITEMID = 1152921504606746975;

    void runFrame() noexcept;
    inventory::Item* getItem(std::uint64_t itemID) noexcept;
}
