#pragma once

#include <array>

#include <CS2/Classes/Color.h>

namespace cs2
{

constexpr Color kColorBlack{0, 0, 0};
constexpr Color kColorWhite{255, 255, 255};
constexpr Color kColorGreen{0, 180, 0};
constexpr Color kColorLightGreen{144, 238, 144};
constexpr Color kColorRed{255, 0, 0};
constexpr Color kColorYellow{255, 255, 0};
constexpr Color kColorSilver{192, 192, 192};

constexpr Color kColorTeamTT{234, 190, 84};
constexpr Color kColorTeamCT{150, 200, 250};

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

constexpr Color kColorDefuseKit{119, 221, 255};

}
