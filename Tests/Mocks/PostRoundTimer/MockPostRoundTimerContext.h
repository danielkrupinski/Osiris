#pragma once

#include <gmock/gmock.h>

struct MockGameRules;
struct MockPostRoundTimerCondition;
struct MockPostRoundTimerPanel;
struct PostRoundTimerState;

struct MockPostRoundTimerContext {
    MOCK_METHOD(MockPostRoundTimerCondition&, condition, ());
    MOCK_METHOD(MockPostRoundTimerPanel&, postRoundTimerPanel, ());
    MOCK_METHOD(MockGameRules&, gameRules, ());
    MOCK_METHOD(PostRoundTimerState&, state, ());
    MOCK_METHOD(bool, isGameRoundTimeVisible, ());
};
