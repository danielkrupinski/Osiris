#pragma once

#include <gmock/gmock.h>

struct MockPlantedC4;

struct MockBombTimerPanel {
    MOCK_METHOD(void, showAndUpdate, (MockPlantedC4& tickingC4));
    MOCK_METHOD(void, hide, ());
};
