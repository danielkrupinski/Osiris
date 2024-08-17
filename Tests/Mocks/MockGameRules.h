#pragma once

#include <gmock/gmock.h>

struct MockGameRules {
    MOCK_METHOD(bool, hasScheduledRoundRestart, ());
};
