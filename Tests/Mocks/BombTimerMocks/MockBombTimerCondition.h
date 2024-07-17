#pragma once

#include <gmock/gmock.h>

struct MockBombTimerCondition {
    MOCK_METHOD(bool, shouldRun, ());
    MOCK_METHOD(bool, shouldShowBombTimer, ());
};
