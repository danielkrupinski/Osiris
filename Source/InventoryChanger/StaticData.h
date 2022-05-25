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

namespace game_items
{
    struct Item;
    class Lookup;
    class CrateLootLookup;
}

namespace StaticData
{

    class EconRarities {
    public:
        EconRarities() = default;

        template <typename... Args>
        constexpr EconRarities(Args... args) noexcept
        {
            (set(args), ...);
        }

        constexpr void set(EconRarity rarity) noexcept
        {
            if (const auto rarityBit = static_cast<std::uint8_t>(rarity); rarityBit < std::numeric_limits<std::uint8_t>::digits)
                bits |= (std::byte{ 1 } << rarityBit);
        }

        [[nodiscard]] constexpr EconRarity getNthRarity(std::uint8_t n) const noexcept
        {
            for (std::uint8_t i = 0; i < std::numeric_limits<std::uint8_t>::digits; ++i) {
                if (std::to_integer<std::uint8_t>(bits & (std::byte{ 1 } << i)) != 0) {
                    if (n == 0)
                        return static_cast<EconRarity>(i);
                    --n;
                }
            }
            return EconRarity::Default;
        }

        [[nodiscard]] constexpr std::size_t count() const noexcept
        {
            return static_cast<std::size_t>(std::popcount(std::to_integer<std::uint8_t>(bits)));
        }

        [[nodiscard]] constexpr bool operator==(const EconRarities&) const = default;

    private:
        std::byte bits{ 0 };
    };

    [[nodiscard]] bool isSouvenirPackage(const game_items::Item& crate) noexcept;

    std::wstring_view getWeaponNameUpper(WeaponId weaponID) noexcept;
    std::string_view getWeaponName(WeaponId weaponID) noexcept;

    [[nodiscard]] const game_items::Lookup& lookup() noexcept;
    [[nodiscard]] const game_items::CrateLootLookup& crateLoot() noexcept;
}
