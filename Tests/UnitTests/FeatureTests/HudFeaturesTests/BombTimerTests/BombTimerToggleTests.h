#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/BombTimer/BombTimerState.h>
#include <Features/Hud/BombTimer/BombTimerToggle.h>
#include <Mocks/BombTimerMocks/MockBombTimerContext.h>
#include <Mocks/BombTimerMocks/MockBombTimerPanel.h>

class BombTimerToggleTest : public testing::Test {
protected:
    BombTimerToggleTest()
    {
        EXPECT_CALL(mockBombTimerContext, state()).WillOnce(testing::ReturnRef(state));
    }

    testing::StrictMock<MockBombTimerContext> mockBombTimerContext;
    testing::StrictMock<MockBombTimerPanel> mockBombTimerPanel;

    BombTimerToggle<MockBombTimerContext&> bombTimerToggle{mockBombTimerContext};
    BombTimerState state;
};

TEST_F(BombTimerToggleTest, EnablingSetsEnabledVariableToTrue) {
    state.enabled = false;
    bombTimerToggle.enable();
    EXPECT_EQ(state.enabled, true);
}

TEST_F(BombTimerToggleTest, EnablingDoesNothingIfAlreadyEnabled) {
    state.enabled = true;
    bombTimerToggle.enable();
    EXPECT_EQ(state.enabled, true);
}

TEST_F(BombTimerToggleTest, DisablingDoesNothingIfAlreadyDisabled) {
    state.enabled = false;
    bombTimerToggle.disable();
    EXPECT_EQ(state.enabled, false);
}

TEST_F(BombTimerToggleTest, DisablingRestoresBombStatusPanelAndHidesBombTimerPanel) {
    state.enabled = true;

    EXPECT_CALL(mockBombTimerContext, bombTimerPanel()).WillOnce(testing::ReturnRef(mockBombTimerPanel));
    EXPECT_CALL(mockBombTimerPanel, hide());

    bombTimerToggle.disable();

    EXPECT_EQ(state.enabled, false);
}
