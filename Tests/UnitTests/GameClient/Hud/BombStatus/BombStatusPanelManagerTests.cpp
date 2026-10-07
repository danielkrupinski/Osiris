#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <GameClient/Hud/BombStatus/BombStatusPanelManager.h>

#include <Mocks/BombTimerMocks/MockBombTimer.h>
#include <Mocks/HudMocks/MockBombStatusPanel.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/PostRoundTimer/MockPostRoundTimer.h>

class BombStatusPanelManagerTest : public testing::Test {
protected:
    BombStatusPanelManagerTest()
    {
        EXPECT_CALL(mockHookContext, makePostRoundTimer()).WillOnce(testing::ReturnRef(mockPostRoundTimer));
        EXPECT_CALL(mockHookContext, makeBombTimer()).WillOnce(testing::ReturnRef(mockBombTimer));
        EXPECT_CALL(mockHookContext, makeBombStatusPanel()).WillOnce(testing::ReturnRef(mockBombStatusPanel));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockPostRoundTimer> mockPostRoundTimer;
    testing::StrictMock<MockBombTimer> mockBombTimer;
    testing::StrictMock<MockBombStatusPanel> mockBombStatusPanel;

    BombStatusPanelManager<MockHookContext> bombStatusPanelManager{mockHookContext};
};

TEST_F(BombStatusPanelManagerTest, BombStatusPanelAndBombTimerAreHiddenWhenPostRoundTimerIsVisible) {
    EXPECT_CALL(mockPostRoundTimer, update()).WillOnce(testing::Return(Visibility::Visible));
    EXPECT_CALL(mockBombTimer, forceHide());
    EXPECT_CALL(mockBombStatusPanel, hide());

    bombStatusPanelManager.run();
}

TEST_F(BombStatusPanelManagerTest, BombStatusPanelIsHiddenWhenPostRoundTimerIsNotVisibleAndBombTimerIsVisible) {
    EXPECT_CALL(mockPostRoundTimer, update()).WillOnce(testing::Return(Visibility::Hidden));
    EXPECT_CALL(mockBombTimer, update()).WillOnce(testing::Return(Visibility::Visible));
    EXPECT_CALL(mockBombStatusPanel, hide());

    bombStatusPanelManager.run();
}

TEST_F(BombStatusPanelManagerTest, BombStatusPanelIsRestoredWhenBothPostRoundTimerAndBombTimerAreNotVisible) {
    EXPECT_CALL(mockPostRoundTimer, update()).WillOnce(testing::Return(Visibility::Hidden));
    EXPECT_CALL(mockBombTimer, update()).WillOnce(testing::Return(Visibility::Hidden));
    EXPECT_CALL(mockBombStatusPanel, restore());

    bombStatusPanelManager.run();
}
