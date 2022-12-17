#pragma once

#include <bit>
#include <cstddef>

#include <CSGO/ItemSchema.h>

namespace inventory_changer
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

}
