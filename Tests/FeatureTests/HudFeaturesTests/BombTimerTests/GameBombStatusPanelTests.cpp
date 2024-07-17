#include <gtest/gtest.h>

#include <Features/Hud/BombTimer/GameBombStatusPanel.h>
#include <Mocks/BombTimerMocks/MockBombTimerContext.h>
#include <Mocks/MockPanel.h>

class GameBombStatusPanelTest : public testing::Test {
protected:
    testing::StrictMock<MockBombTimerContext> mockBombTimerContext;
    testing::StrictMock<MockPanel> mockBombStatusPanel;
    testing::StrictMock<MockPanel> mockInvisiblePanel;
    testing::StrictMock<MockPanel> mockScoreAndTimeAndBombPanel;

    GameBombStatusPanel<MockBombTimerContext&> gameBombStatusPanel{mockBombTimerContext};
};

TEST_F(GameBombStatusPanelTest, HidesBombStatusPanelBySettingItsParentToInvisiblePanel) {
    EXPECT_CALL(mockBombTimerContext, bombStatusPanel()).WillOnce(testing::ReturnRef(mockBombStatusPanel));
    EXPECT_CALL(mockBombTimerContext, invisiblePanel()).WillOnce(testing::ReturnRef(mockInvisiblePanel));
    EXPECT_CALL(mockBombStatusPanel, setParent(testing::Ref(mockInvisiblePanel)));

    gameBombStatusPanel.hide();
}

TEST_F(GameBombStatusPanelTest, RestoresBombStatusPanelBySettingItsParentToTheOriginalPanel) {
    EXPECT_CALL(mockBombTimerContext, bombStatusPanel()).WillOnce(testing::ReturnRef(mockBombStatusPanel));
    EXPECT_CALL(mockBombTimerContext, scoreAndTimeAndBombPanel()).WillOnce(testing::ReturnRef(mockScoreAndTimeAndBombPanel));
    EXPECT_CALL(mockBombStatusPanel, setParent(testing::Ref(mockScoreAndTimeAndBombPanel)));

    gameBombStatusPanel.restore();
}
