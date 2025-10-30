#pragma once

#include <gmock/gmock.h>
#include <CS2/Constants/BombsiteIndex.h>
#include <Utils/Optional.h>

struct MockC4 {
    MOCK_METHOD(bool, operatorBool, ());
    MOCK_METHOD(Optional<bool>, isBeingPlanted, ());
    MOCK_METHOD(Optional<float>, timeToArmingEnd, ());
    MOCK_METHOD(Optional<float>, armingEndTime, ());
    MOCK_METHOD(Optional<cs2::BombsiteIndex>, nearestBombsiteIndex, ());

    [[nodiscard]] operator bool()
    {
        return operatorBool();
    }
};
