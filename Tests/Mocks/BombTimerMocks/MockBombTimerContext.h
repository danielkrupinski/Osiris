#pragma once

#include <gmock/gmock.h>

struct BombTimerState;
struct MockBombSiteIconPanel;
struct MockBombTimerCondition;
struct MockBombTimerPanel;
struct MockBombTimerTextPanel;
struct MockGameBombStatusPanel;
struct MockPanel;
struct MockPlantedC4;

struct MockBombTimerContext {
    MOCK_METHOD(MockBombTimerCondition&, bombTimerCondition, ());
    MOCK_METHOD(MockBombTimerPanel&, bombTimerPanel, ());
    MOCK_METHOD(MockGameBombStatusPanel&, gameBombStatusPanel, ());
    MOCK_METHOD(MockPanel&, bombStatusPanel, ());
    MOCK_METHOD(MockPanel&, invisiblePanel, ());
    MOCK_METHOD(MockPanel&, scoreAndTimeAndBombPanel, ());
    MOCK_METHOD(MockPanel&, bombTimerContainerPanel, ());
    MOCK_METHOD(bool, hasTickingC4, ());
    MOCK_METHOD(BombTimerState&, state, ());
    MOCK_METHOD(MockPlantedC4&, tickingC4, ());
    MOCK_METHOD(MockBombTimerTextPanel&, bombTimerTextPanel, ());
    MOCK_METHOD(MockBombSiteIconPanel&, bombSiteIconPanel, ());
};
