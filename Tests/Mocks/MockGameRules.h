#pragma once

#include <gmock/gmock.h>
#include <Utils/Optional.h>

struct MockGameRules {
    MOCK_METHOD(bool, hasScheduledRoundRestart, ());
    MOCK_METHOD(Optional<bool>, isRoundOver, ());
    MOCK_METHOD(Optional<float>, roundEndTime, ());
};
