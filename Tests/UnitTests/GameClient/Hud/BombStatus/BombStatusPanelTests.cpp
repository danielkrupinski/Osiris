#include <gtest/gtest.h>

#include <GameClient/Hud/BombStatus/BombStatusPanel.h>
#include <GameClient/Hud/BombStatus/BombStatusPanelState.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockPanel.h>
#include <Mocks/MockPanelHandle.h>
#include <Mocks/HudMocks/MockHud.h>
#include <Mocks/MockPanelFactory.h>
#include <Mocks/MockClientPanel.h>
#include <Mocks/MockPanoramaUiEngine.h>

class BombStatusPanelTest : public testing::Test {
protected:
    BombStatusPanelTest()
    {
        EXPECT_CALL(mockHookContext, bombStatusPanelState()).WillRepeatedly(testing::ReturnRef(state));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockPanel> mockBombStatusPanel;
    testing::StrictMock<MockPanel> mockInvisiblePanel;
    testing::StrictMock<MockPanel> mockScoreAndTimeAndBombPanel;
    testing::StrictMock<MockPanelHandle> mockPanelHandle;
    testing::StrictMock<MockHud> mockHud;
    testing::StrictMock<MockPanoramaUiEngine> mockPanoramaUiEngine;

    BombStatusPanelState state{};

    BombStatusPanel<MockHookContext> bombStatusPanel{mockHookContext};
};

class BombStatusPanelHandleTest : public BombStatusPanelTest, public testing::WithParamInterface<cs2::PanelHandle> {
};

TEST_P(BombStatusPanelHandleTest, HidesBombStatusPanelBySettingItsParentToInvisiblePanelIfPreviouslyVisible) {
    state.visibility = Visibility::Visible;
    state.invisiblePanelHandle = GetParam();

    EXPECT_CALL(mockHookContext, makePanelHandle(state.invisiblePanelHandle)).WillOnce(testing::ReturnRef(mockPanelHandle));
    EXPECT_CALL(mockPanelHandle, getOrInit(testing::_)).WillOnce(testing::ReturnRef(mockInvisiblePanel));

    EXPECT_CALL(mockHookContext, hud()).WillOnce(testing::ReturnRef(mockHud));
    EXPECT_CALL(mockHud, bombStatus()).WillOnce(testing::ReturnRef(mockBombStatusPanel));
    EXPECT_CALL(mockBombStatusPanel, setParent(testing::Ref(mockInvisiblePanel)));

    bombStatusPanel.hide();
    EXPECT_EQ(state.visibility, Visibility::Hidden);
}

TEST_P(BombStatusPanelHandleTest, InvisiblePanelIsDeletedOnUnload) {
    state.visibility = Visibility::Visible;
    state.invisiblePanelHandle = GetParam();

    EXPECT_CALL(mockHookContext, makePanoramaUiEngine()).WillOnce(testing::ReturnRef(mockPanoramaUiEngine));
    EXPECT_CALL(mockPanoramaUiEngine, deletePanelByHandle(GetParam()));

    bombStatusPanel.onUnload();
    EXPECT_EQ(state.visibility, Visibility::Visible);
}

TEST_P(BombStatusPanelHandleTest, BombStatusPanelIsRestoredBeforeInvisiblePanelIsDeletedOnUnload) {
    state.visibility = Visibility::Hidden;
    state.invisiblePanelHandle = GetParam();

    testing::Sequence s;

    EXPECT_CALL(mockHookContext, hud()).WillRepeatedly(testing::ReturnRef(mockHud));
    EXPECT_CALL(mockHud, scoreAndTimeAndBomb()).WillOnce(testing::ReturnRef(mockScoreAndTimeAndBombPanel));
    EXPECT_CALL(mockHud, bombStatus()).WillOnce(testing::ReturnRef(mockBombStatusPanel));
    EXPECT_CALL(mockBombStatusPanel, setParent(testing::Ref(mockScoreAndTimeAndBombPanel))).InSequence(s);

    EXPECT_CALL(mockHookContext, makePanoramaUiEngine()).WillOnce(testing::ReturnRef(mockPanoramaUiEngine));
    EXPECT_CALL(mockPanoramaUiEngine, deletePanelByHandle(GetParam())).InSequence(s);

    bombStatusPanel.onUnload();
    EXPECT_EQ(state.visibility, Visibility::Visible);
}

INSTANTIATE_TEST_SUITE_P(, BombStatusPanelHandleTest, testing::Values(
    cs2::PanelHandle{},
    cs2::PanelHandle{.panelIndex = 123, .serialNumber = 456},
    cs2::PanelHandle{.panelIndex = 1234, .serialNumber = 2048}));

TEST_F(BombStatusPanelTest, DoesNotHideAgainIfAlreadyHidden) {
    state.visibility = Visibility::Hidden;
    bombStatusPanel.hide();
    EXPECT_EQ(state.visibility, Visibility::Hidden);
}

TEST_F(BombStatusPanelTest, RestoresBombStatusPanelBySettingItsParentToTheOriginalPanelIfPreviouslyHidden) {
    state.visibility = Visibility::Hidden;

    EXPECT_CALL(mockHookContext, hud()).WillRepeatedly(testing::ReturnRef(mockHud));
    EXPECT_CALL(mockHud, scoreAndTimeAndBomb()).WillOnce(testing::ReturnRef(mockScoreAndTimeAndBombPanel));
    EXPECT_CALL(mockHud, bombStatus()).WillOnce(testing::ReturnRef(mockBombStatusPanel));
    EXPECT_CALL(mockBombStatusPanel, setParent(testing::Ref(mockScoreAndTimeAndBombPanel)));

    bombStatusPanel.restore();
    EXPECT_EQ(state.visibility, Visibility::Visible);
}

TEST_F(BombStatusPanelTest, DoesNotRestoreAgainIfAlreadyRestored) {
    state.visibility = Visibility::Visible;
    bombStatusPanel.restore();
    EXPECT_EQ(state.visibility, Visibility::Visible);
}
