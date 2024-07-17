#pragma once

#include <gmock/gmock.h>

struct MockBombTimerPanel {
    MOCK_METHOD(void, showAndUpdate, ());
    MOCK_METHOD(void, hide, ());
};
