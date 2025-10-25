#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/BombTimer/BombTimer.h>
#include <Mocks/BombTimerMocks/MockBombTimerCondition.h>
#include <Mocks/BombTimerMocks/MockBombTimerContext.h>
#include <Mocks/BombTimerMocks/MockBombTimerPanel.h>
#include <Mocks/MockHookContext.h>

class BombTimerTest : public testing::Test {
protected:
    void shouldRun(bool b)
    {
        EXPECT_CALL(mockBombTimerCondition, shouldRun()).WillOnce(testing::Return(b));
    }

    void shouldShowBombTimer(bool b)
    {
        EXPECT_CALL(mockBombTimerCondition, shouldShowBombTimer()).WillOnce(testing::Return(b));
    }

    testing::StrictMock<MockBombTimerContext> mockBombTimerContext;
    testing::StrictMock<MockBombTimerCondition> mockBombTimerCondition;
    testing::StrictMock<MockBombTimerPanel> mockBombTimerPanel;

    BombTimer<MockHookContext, MockBombTimerContext&> bombTimer{mockBombTimerContext};
};

TEST_F(BombTimerTest, DoesNotRunIfShouldNotRun) {
    shouldRun(false);

    EXPECT_CALL(mockBombTimerContext, bombTimerCondition()).WillOnce(testing::ReturnRef(mockBombTimerCondition));
    EXPECT_CALL(mockBombTimerCondition, shouldShowBombTimer()).Times(0);
    EXPECT_CALL(mockBombTimerContext, bombTimerPanel()).Times(0);

    EXPECT_EQ(bombTimer.update(), Visibility::Hidden);
}

TEST_F(BombTimerTest, ShowsTimerWhenBombTimerShouldBeShown) {
    shouldRun(true);
    shouldShowBombTimer(true);

    EXPECT_CALL(mockBombTimerContext, bombTimerCondition()).WillOnce(testing::ReturnRef(mockBombTimerCondition));
    EXPECT_CALL(mockBombTimerContext, bombTimerPanel()).WillOnce(testing::ReturnRef(mockBombTimerPanel));
    EXPECT_CALL(mockBombTimerPanel, showAndUpdate());
    
    EXPECT_EQ(bombTimer.update(), Visibility::Visible);
}

TEST_F(BombTimerTest, HidesTimerWhenBombTimerShouldNotBeShown) {
    shouldRun(true);
    shouldShowBombTimer(false);

    EXPECT_CALL(mockBombTimerContext, bombTimerCondition()).WillOnce(testing::ReturnRef(mockBombTimerCondition));
    EXPECT_CALL(mockBombTimerContext, bombTimerPanel()).WillOnce(testing::ReturnRef(mockBombTimerPanel));
    EXPECT_CALL(mockBombTimerPanel, hide());
    
    EXPECT_EQ(bombTimer.update(), Visibility::Hidden);
}

TEST_F(BombTimerTest, ForceHidesPanelIfShouldRun) {
    shouldRun(true);

    EXPECT_CALL(mockBombTimerContext, bombTimerCondition()).WillOnce(testing::ReturnRef(mockBombTimerCondition));
    EXPECT_CALL(mockBombTimerContext, bombTimerPanel()).WillOnce(testing::ReturnRef(mockBombTimerPanel));
    EXPECT_CALL(mockBombTimerPanel, hide());
    
    bombTimer.forceHide();
}

TEST_F(BombTimerTest, DoesNotForceHidePanelIfShouldNotRun) {
    shouldRun(false);
    EXPECT_CALL(mockBombTimerContext, bombTimerCondition()).WillOnce(testing::ReturnRef(mockBombTimerCondition));
    bombTimer.forceHide();
}

TEST_F(BombTimerTest, OnDisableHidesBombTimerPanel) {
    EXPECT_CALL(mockBombTimerContext, bombTimerPanel()).WillOnce(testing::ReturnRef(mockBombTimerPanel));
    EXPECT_CALL(mockBombTimerPanel, hide());
    bombTimer.onDisable();
}
