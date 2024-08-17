#pragma once

#include <gmock/gmock.h>

struct MockBombStatusPanel;
struct MockBombTimer;
struct MockPostRoundTimer;

struct MockBombStatusPanelManagerContext {
    MOCK_METHOD(MockBombStatusPanel&, bombStatusPanel, ());
    MOCK_METHOD(MockPostRoundTimer&, postRoundTimer, ());
    MOCK_METHOD(MockBombTimer&, bombTimer, ());
};
