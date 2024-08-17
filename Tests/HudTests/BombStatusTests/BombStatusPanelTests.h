#pragma once

#include <gtest/gtest.h>

#include <Hud/BombStatus/BombStatusPanel.h>
#include <Mocks/HudMocks/MockBombStatusPanelContext.h>
#include <Mocks/MockPanel.h>

class BombStatusPanelTest : public testing::Test {
protected:
    BombStatusPanelTest()
    {
        EXPECT_CALL(mockContext, visibility()).WillOnce(testing::ReturnRef(visibility));
    }

    testing::StrictMock<MockBombStatusPanelContext> mockContext;
    testing::StrictMock<MockPanel> mockBombStatusPanel;
    testing::StrictMock<MockPanel> mockInvisiblePanel;
    testing::StrictMock<MockPanel> mockScoreAndTimeAndBombPanel;

    BombStatusPanel<MockBombStatusPanelContext&> bombStatusPanel{mockContext};
    Visibility visibility{};
};

TEST_F(BombStatusPanelTest, HidesBombStatusPanelBySettingItsParentToInvisiblePanelIfPreviouslyVisible) {
    visibility = Visibility::Visible;

    EXPECT_CALL(mockContext, bombStatusPanel()).WillOnce(testing::ReturnRef(mockBombStatusPanel));
    EXPECT_CALL(mockContext, invisiblePanel()).WillOnce(testing::ReturnRef(mockInvisiblePanel));
    EXPECT_CALL(mockBombStatusPanel, setParent(testing::Ref(mockInvisiblePanel)));

    bombStatusPanel.hide();
    EXPECT_EQ(visibility, Visibility::Hidden);
}

TEST_F(BombStatusPanelTest, DoesNotHideAgainIfAlreadyHidden) {
    visibility = Visibility::Hidden;
    bombStatusPanel.hide();
    EXPECT_EQ(visibility, Visibility::Hidden);
}

TEST_F(BombStatusPanelTest, RestoresBombStatusPanelBySettingItsParentToTheOriginalPanelIfPreviouslyHidden) {
    visibility = Visibility::Hidden;

    EXPECT_CALL(mockContext, bombStatusPanel()).WillOnce(testing::ReturnRef(mockBombStatusPanel));
    EXPECT_CALL(mockContext, scoreAndTimeAndBombPanel()).WillOnce(testing::ReturnRef(mockScoreAndTimeAndBombPanel));
    EXPECT_CALL(mockBombStatusPanel, setParent(testing::Ref(mockScoreAndTimeAndBombPanel)));

    bombStatusPanel.restore();
    EXPECT_EQ(visibility, Visibility::Visible);
}

TEST_F(BombStatusPanelTest, DoesNotRestoreAgainIfAlreadyRestored) {
    visibility = Visibility::Visible;
    bombStatusPanel.restore();
    EXPECT_EQ(visibility, Visibility::Visible);
}
