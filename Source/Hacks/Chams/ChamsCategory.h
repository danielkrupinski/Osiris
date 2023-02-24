#pragma once

#include <string_view>

enum class ChamsCategory {
    Allies = 0,
    Enemies,
    Planting,
    Defusing,
    LocalPlayer,
    Weapons,
    Hands,
    Backtrack,
    Sleeves
};

[[nodiscard]] constexpr std::string_view toString(ChamsCategory category) noexcept
{
    switch (category) {
    using enum ChamsCategory;
    case Allies: return "Allies";
    case Enemies: return "Enemies";
    case Planting: return "Planting";
    case Defusing: return "Defusing";
    case LocalPlayer: return "LocalPlayer";
    case Weapons: return "Weapons";
    case Hands: return "Hands";
    case Backtrack: return "Backtrack";
    case Sleeves: return "Sleeves";
    default: return "";
    }
}
