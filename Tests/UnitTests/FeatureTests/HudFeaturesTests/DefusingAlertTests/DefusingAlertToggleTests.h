#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/DefusingAlert/DefusingAlertState.h>
#include <Features/Hud/DefusingAlert/DefusingAlertToggle.h>
#include <Mocks/DefusingAlertMocks/MockDefusingAlertContext.h>
#include <Mocks/DefusingAlertMocks/MockDefusingAlertPanel.h>

class DefusingAlertToggleTest : public testing::Test {
protected:
    DefusingAlertToggleTest()
    {
        EXPECT_CALL(mockDefusingAlertContext, state()).WillOnce(testing::ReturnRef(state));
    }

    testing::StrictMock<MockDefusingAlertContext> mockDefusingAlertContext;
    testing::StrictMock<MockDefusingAlertPanel> mockDefusingAlertPanel;

    DefusingAlertToggle<MockDefusingAlertContext&> defusingAlertToggle{mockDefusingAlertContext};
    DefusingAlertState state;
};

TEST_F(DefusingAlertToggleTest, EnablingSetsEnabledVariableToTrue) {
    state.enabled = false;
    defusingAlertToggle.enable();
    EXPECT_EQ(state.enabled, true);
}

TEST_F(DefusingAlertToggleTest, EnablingDoesNothingIfAlreadyEnabled) {
    state.enabled = true;
    defusingAlertToggle.enable();
    EXPECT_EQ(state.enabled, true);
}

TEST_F(DefusingAlertToggleTest, DisablingDoesNothingIfAlreadyDisabled) {
    state.enabled = false;
    defusingAlertToggle.disable();
    EXPECT_EQ(state.enabled, false);
}

TEST_F(DefusingAlertToggleTest, DisablingHidesDefusingAlertPanel) {
    state.enabled = true;

    EXPECT_CALL(mockDefusingAlertContext, defusingAlertPanel()).WillOnce(testing::ReturnRef(mockDefusingAlertPanel));
    EXPECT_CALL(mockDefusingAlertPanel, hide());

    defusingAlertToggle.disable();

    EXPECT_EQ(state.enabled, false);
}
