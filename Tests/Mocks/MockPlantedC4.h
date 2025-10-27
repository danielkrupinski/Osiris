#pragma once

#include <optional>

#include <gmock/gmock.h>
#include <CS2/Constants/BombsiteIndex.h>
#include <Utils/Optional.h>

struct MockBaseEntity;

struct MockPlantedC4 {
    MOCK_METHOD(Optional<cs2::BombsiteIndex>, bombsiteIndex, ());
    MOCK_METHOD(Optional<float>, getTimeToExplosion, ());
    MOCK_METHOD(Optional<float>, getTimeToDefuseEnd, ());
    MOCK_METHOD(Optional<bool>, canBeDefused, ());
    MOCK_METHOD(Optional<bool>, isTicking, ());
    MOCK_METHOD(MockBaseEntity&, baseEntity, ());
};
