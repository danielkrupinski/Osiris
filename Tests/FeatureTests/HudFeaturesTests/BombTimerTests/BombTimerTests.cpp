#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/BombTimer/BombTimer.h>
#include <Mocks/MockBombTimerCondition.h>
#include <Mocks/MockBombTimerContext.h>
#include <Mocks/MockBombTimerPanel.h>
#include <Mocks/MockGameBombStatusPanel.h>

class BombTimerTest : public testing::Test {
protected:
    BombTimerTest()
    {
        EXPECT_CALL(mockBombTimerContext, bombTimerCondition()).WillOnce(testing::ReturnRef(mockBombTimerCondition));
    }

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
    testing::StrictMock<MockGameBombStatusPanel> mockGameStatusPanel;
    testing::StrictMock<MockBombTimerPanel> mockBombTimerPanel;

    BombTimer<MockBombTimerContext&> bombTimer{mockBombTimerContext};
};

TEST_F(BombTimerTest, DoesNotRunIfShouldNotRun) {
    shouldRun(false);

    EXPECT_CALL(mockBombTimerCondition, shouldShowBombTimer()).Times(0);
    EXPECT_CALL(mockBombTimerContext, gameBombStatusPanel()).Times(0);
    EXPECT_CALL(mockBombTimerContext, bombTimerPanel()).Times(0);

    bombTimer.run();
}

TEST_F(BombTimerTest, HidesGameBombStatusAndShowsTimerWhenBombTimerShouldBeShown) {
    shouldRun(true);
    shouldShowBombTimer(true);
    
    EXPECT_CALL(mockBombTimerContext, gameBombStatusPanel()).WillOnce(testing::ReturnRef(mockGameStatusPanel));
    EXPECT_CALL(mockGameStatusPanel, hide());

    EXPECT_CALL(mockBombTimerContext, bombTimerPanel()).WillOnce(testing::ReturnRef(mockBombTimerPanel));
    EXPECT_CALL(mockBombTimerPanel, showAndUpdate());
    
    bombTimer.run();
}

TEST_F(BombTimerTest, RestoresGameBombStatusAndHideTimerWhenBombTimerShouldNotBeShown) {
    shouldRun(true);
    shouldShowBombTimer(false);
    
    EXPECT_CALL(mockBombTimerContext, gameBombStatusPanel()).WillOnce(testing::ReturnRef(mockGameStatusPanel));
    EXPECT_CALL(mockGameStatusPanel, restore());

    EXPECT_CALL(mockBombTimerContext, bombTimerPanel()).WillOnce(testing::ReturnRef(mockBombTimerPanel));
    EXPECT_CALL(mockBombTimerPanel, hide());
    
    bombTimer.run();
}
