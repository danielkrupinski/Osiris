#pragma once

#include <gmock/gmock.h>

struct BombTimerState;
struct MockBombSiteIconPanel;
struct MockBombTimerCondition;
struct MockBombTimerPanel;
struct MockBombTimerTextPanel;
struct MockPanel;
struct MockPlantedC4;

struct MockBombTimerContext {
    MOCK_METHOD(MockBombTimerCondition&, bombTimerCondition, ());
    MOCK_METHOD(MockBombTimerPanel&, bombTimerPanel, ());
    MOCK_METHOD(MockPanel&, bombPlantedPanel, ());
    MOCK_METHOD(MockPanel&, bombTimerContainerPanel, ());
    MOCK_METHOD(bool, hasTickingC4, ());
    MOCK_METHOD(BombTimerState&, state, ());
    MOCK_METHOD(MockPlantedC4&, tickingC4, ());
    MOCK_METHOD(MockBombTimerTextPanel&, bombTimerTextPanel, ());
    MOCK_METHOD(MockBombSiteIconPanel&, bombSiteIconPanel, ());
};
