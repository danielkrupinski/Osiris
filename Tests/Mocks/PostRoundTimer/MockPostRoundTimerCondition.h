#pragma once

#include <gmock/gmock.h>

struct MockPostRoundTimerCondition {
    MOCK_METHOD(bool, shouldRun, ());
    MOCK_METHOD(bool, shouldShowPostRoundTimer, ());
};
