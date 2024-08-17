#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Hud/BombStatus/BombStatusPanelManager.h>

#include <Mocks/BombTimerMocks/MockBombTimer.h>
#include <Mocks/HudMocks/MockBombStatusPanel.h>
#include <Mocks/HudMocks/MockBombStatusPanelManagerContext.h>
#include <Mocks/PostRoundTimer/MockPostRoundTimer.h>

class BombStatusPanelManagerTest : public testing::Test {
protected:
    BombStatusPanelManagerTest()
    {
        EXPECT_CALL(mockContext, postRoundTimer()).WillOnce(testing::ReturnRef(mockPostRoundTimer));
        EXPECT_CALL(mockContext, bombTimer()).WillOnce(testing::ReturnRef(mockBombTimer));
        EXPECT_CALL(mockContext, bombStatusPanel()).WillOnce(testing::ReturnRef(mockBombStatusPanel));
    }

    testing::StrictMock<MockBombStatusPanelManagerContext> mockContext;
    testing::StrictMock<MockPostRoundTimer> mockPostRoundTimer;
    testing::StrictMock<MockBombTimer> mockBombTimer;
    testing::StrictMock<MockBombStatusPanel> mockBombStatusPanel;

    BombStatusPanelManager<MockBombStatusPanelManagerContext&> bombStatusPanelManager{mockContext};
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
