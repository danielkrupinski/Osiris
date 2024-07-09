#pragma once

#include <gmock/gmock.h>

struct MockBombTimerCondition;
struct MockBombTimerPanel;
struct MockGameBombStatusPanel;
struct MockPanel;

struct MockBombTimerContext {
    MOCK_METHOD(MockBombTimerCondition&, bombTimerCondition, ());
    MOCK_METHOD(MockBombTimerPanel&, bombTimerPanel, ());
    MOCK_METHOD(MockGameBombStatusPanel&, gameBombStatusPanel, ());
    MOCK_METHOD(MockPanel&, bombStatusPanel, ());
    MOCK_METHOD(MockPanel&, invisiblePanel, ());
    MOCK_METHOD(MockPanel&, scoreAndTimeAndBombPanel, ());
};
