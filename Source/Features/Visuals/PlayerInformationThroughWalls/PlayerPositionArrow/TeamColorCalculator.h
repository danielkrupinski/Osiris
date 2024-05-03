#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <FeatureHelpers/TeamNumber.h>

struct TeamColorCalculator {
    [[nodiscard]] cs2::Color getTeamColor() const noexcept
    {
        switch (teamNumber) {
            using enum TeamNumber;
            case TT: return cs2::kColorTeamTT;
            case CT: return cs2::kColorTeamCT;
            default: return cs2::kColorWhite;
        }
    }

    TeamNumber teamNumber;
};
