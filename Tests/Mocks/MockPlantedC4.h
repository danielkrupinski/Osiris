#pragma once

#include <optional>

#include <gmock/gmock.h>

struct MockPlantedC4 {
    MOCK_METHOD(const char*, getBombSiteIconUrl, ());
    MOCK_METHOD(float, getTimeToExplosion, ());
    MOCK_METHOD(std::optional<float>, getTimeToDefuseEnd, ());
    MOCK_METHOD(std::optional<bool>, canBeDefused, ());
};
