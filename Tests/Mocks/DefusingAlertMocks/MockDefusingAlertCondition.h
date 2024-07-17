#pragma once

#include <gmock/gmock.h>

struct MockDefusingAlertCondition {
    MOCK_METHOD(bool, shouldRun, ());
    MOCK_METHOD(bool, shouldShowDefuseAlert, ());
};
