#pragma once

#include <optional>

#include <gmock/gmock.h>
#include <Utils/Optional.h>

struct MockPlantedC4 {
    MOCK_METHOD(const char*, getBombSiteIconUrl, ());
    MOCK_METHOD(Optional<float>, getTimeToExplosion, ());
    MOCK_METHOD(Optional<float>, getTimeToDefuseEnd, ());
    MOCK_METHOD(Optional<bool>, canBeDefused, ());
};
