#pragma once

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "../SDK/ItemSchema.h"
#include "../SDK/WeaponId.h"

#include "GameItems/Enums.h"

enum TournamentTeam : std::uint8_t;
enum class EconRarity : std::uint8_t;

namespace inventory_changer::game_items
{
    struct Item;
    class Lookup;
    class CrateLootLookup;
}

namespace StaticData
{
    std::wstring_view getWeaponNameUpper(WeaponId weaponID) noexcept;
    std::string_view getWeaponName(WeaponId weaponID) noexcept;

    [[nodiscard]] const inventory_changer::game_items::Lookup& lookup() noexcept;
    [[nodiscard]] const inventory_changer::game_items::CrateLootLookup& crateLoot() noexcept;
}
