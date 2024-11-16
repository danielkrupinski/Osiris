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

constexpr Color kColorPlayerYellowSaturated{255, 251, 0};
constexpr Color kColorPlayerPurpleSaturated{255, 0, 183};
constexpr Color kColorPlayerGreenSaturated{0, 255, 196};
constexpr Color kColorPlayerBlueSaturated{0, 166, 255};
constexpr Color kColorPlayerOrangeSaturated{255, 136, 0};

constexpr auto kPlayerColorsSaturated = std::to_array<Color>({
    kColorPlayerBlueSaturated,
    kColorPlayerGreenSaturated,
    kColorPlayerYellowSaturated,
    kColorPlayerOrangeSaturated,
    kColorPlayerPurpleSaturated
});

constexpr Color kColorPlayerYellowHalfSaturated{255, 253, 128};
constexpr Color kColorPlayerPurpleHalfSaturated{255, 128, 219};
constexpr Color kColorPlayerGreenHalfSaturated{128, 255, 255};
constexpr Color kColorPlayerBlueHalfSaturated{128, 210, 255};
constexpr Color kColorPlayerOrangeHalfSaturated{255, 195, 128};

constexpr auto kPlayerColorsHalfSaturated = std::to_array<Color>({
    kColorPlayerBlueHalfSaturated,
    kColorPlayerGreenHalfSaturated,
    kColorPlayerYellowHalfSaturated,
    kColorPlayerOrangeHalfSaturated,
    kColorPlayerPurpleHalfSaturated
});

}
