#pragma once

#include <array>

#include <CS2/Classes/Color.h>

namespace cs2
{

constexpr Color kColorWhite{255, 255, 255};

constexpr Color kColorPlayerYellow{248, 246, 45};
constexpr Color kColorPlayerPurple{192, 54, 153};
constexpr Color kColorPlayerGreen{29, 162, 132};
constexpr Color kColorPlayerBlue{136, 206, 245};
constexpr Color kColorPlayerOrange{255, 155, 37};

constexpr auto kPlayerColors = std::to_array<Color>({
    kColorPlayerBlue,
    kColorPlayerGreen,
    kColorPlayerYellow,
    kColorPlayerOrange,
    kColorPlayerPurple
});

}
