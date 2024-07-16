#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Fakes/FakeDefusingAlertState.h>
#include <Features/Hud/DefusingAlert/DefusingAlertToggle.h>
#include <Mocks/MockDefusingAlertContext.h>
#include <Mocks/MockDefusingAlertPanel.h>

class DefusingAlertToggleTest : public testing::Test {
protected:
    DefusingAlertToggleTest()
    {
        EXPECT_CALL(mockDefusingAlertContext, state()).WillOnce(testing::ReturnRef(state));
    }

    testing::StrictMock<MockDefusingAlertContext> mockDefusingAlertContext;
    testing::StrictMock<MockDefusingAlertPanel> mockDefusingAlertPanel;

    DefusingAlertToggle<MockDefusingAlertContext&> defusingAlertToggle{mockDefusingAlertContext};
    FakeDefusingAlertState state;
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
