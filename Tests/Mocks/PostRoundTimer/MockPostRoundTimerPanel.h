#pragma once

#include <gmock/gmock.h>

struct MockPostRoundTimerPanel {
    MOCK_METHOD(void, showAndUpdate, ());
    MOCK_METHOD(void, hide, ());
};
