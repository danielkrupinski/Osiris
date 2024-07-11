#pragma once

#include <gmock/gmock.h>

struct MockPlantedC4 {
    MOCK_METHOD(const char*, getBombSiteIconUrl, ());
    MOCK_METHOD(float, getTimeToExplosion, ());
};
