#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Common/Visibility.h>
#include <Features/Hud/PostRoundTimer/PostRoundTimer.h>

#include <Mocks/MockConfig.h>
#include <Mocks/MockGameRules.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/PostRoundTimer/MockPostRoundTimerPanel.h>
#include <Mocks/HudMocks/MockHud.h>
#include <Mocks/MockPanel.h>
#include <Mocks/MockPanoramaUiEngine.h>

class PostRoundTimerTest : public testing::Test {
protected:
    PostRoundTimerTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
    }

    void enabled(bool b)
    {
        mockConfig.expectGetVariable<PostRoundTimerEnabled>(b);
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockPostRoundTimerPanel> mockPostRoundTimerPanel;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockGameRules> mockGameRules;
    testing::StrictMock<MockPanoramaUiEngine> mockPanoramaUiEngine;
    
    FeaturesStates featuresStates{};

    auto& state() noexcept
    {
        return featuresStates.hudFeaturesStates.postRoundTimerState;
    }

    PostRoundTimer<MockHookContext> postRoundTimer{mockHookContext};
};

TEST_F(PostRoundTimerTest, IsHiddenIfShouldNotRun) {
    enabled(false);
    EXPECT_EQ(postRoundTimer.update(), Visibility::Hidden);
}

TEST_F(PostRoundTimerTest, HidesTimerPanelOnDisable) {
    EXPECT_CALL(mockHookContext, makePostRoundTimerPanel()).WillOnce(testing::ReturnRef(mockPostRoundTimerPanel));
    EXPECT_CALL(mockPostRoundTimerPanel, hide());

    postRoundTimer.onDisable();
}

class PostRoundTimerOnUnloadTest : public PostRoundTimerTest, public testing::WithParamInterface<cs2::PanelHandle> {
};

TEST_P(PostRoundTimerOnUnloadTest, PanelIsDeletedOnUnload) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    state().countdownContainerPanelHandle = GetParam();

    EXPECT_CALL(mockHookContext, makePanoramaUiEngine()).WillOnce(testing::ReturnRef(mockPanoramaUiEngine));
    EXPECT_CALL(mockPanoramaUiEngine, deletePanelByHandle(GetParam()));

    postRoundTimer.onUnload();
}

INSTANTIATE_TEST_SUITE_P(, PostRoundTimerOnUnloadTest, testing::Values(
    cs2::PanelHandle{},
    cs2::PanelHandle{.panelIndex = 123, .serialNumber = 456},
    cs2::PanelHandle{.panelIndex = 1234, .serialNumber = 2048}));

struct PostRoundTimerUpdateTestParam {
    bool hasScheduledRoundRestart{};
    Optional<bool> isGameRoundTimeVisible{};
    Visibility expectedVisibility{};
};

class PostRoundTimerUpdateTest : public PostRoundTimerTest, public testing::WithParamInterface<PostRoundTimerUpdateTestParam> {
protected:
    testing::StrictMock<MockGameRules> mockGameRules;
    testing::StrictMock<MockHud> mockHud;
    testing::StrictMock<MockPanel> mockTimerTextPanel;
};

TEST_P(PostRoundTimerUpdateTest, Update) {
    enabled(true);

    EXPECT_CALL(mockHookContext, gameRules()).Times(testing::AtMost(1)).WillRepeatedly(testing::ReturnRef(mockGameRules));
    EXPECT_CALL(mockGameRules, hasScheduledRoundRestart()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(GetParam().hasScheduledRoundRestart));

    EXPECT_CALL(mockHookContext, hud()).Times(testing::AtMost(1)).WillRepeatedly(testing::ReturnRef(mockHud));
    EXPECT_CALL(mockHud, timerTextPanel()).Times(testing::AtMost(1)).WillRepeatedly(testing::ReturnRef(mockTimerTextPanel));
    EXPECT_CALL(mockTimerTextPanel, isVisible()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(GetParam().isGameRoundTimeVisible));

    EXPECT_CALL(mockHookContext, makePostRoundTimerPanel()).WillOnce(testing::ReturnRef(mockPostRoundTimerPanel));

    if (GetParam().expectedVisibility == Visibility::Visible)
        EXPECT_CALL(mockPostRoundTimerPanel, showAndUpdate());

    if (GetParam().expectedVisibility == Visibility::Hidden)
        EXPECT_CALL(mockPostRoundTimerPanel, hide());

    EXPECT_EQ(postRoundTimer.update(), GetParam().expectedVisibility);
}

INSTANTIATE_TEST_SUITE_P(, PostRoundTimerUpdateTest, testing::ValuesIn(
    std::to_array<PostRoundTimerUpdateTestParam>({
        {.hasScheduledRoundRestart = false, .isGameRoundTimeVisible = false, .expectedVisibility = Visibility::Hidden},
        {.hasScheduledRoundRestart = false, .isGameRoundTimeVisible = true, .expectedVisibility = Visibility::Hidden},
        {.hasScheduledRoundRestart = false, .isGameRoundTimeVisible = std::nullopt, .expectedVisibility = Visibility::Hidden},
        {.hasScheduledRoundRestart = true, .isGameRoundTimeVisible = false, .expectedVisibility = Visibility::Visible},
        {.hasScheduledRoundRestart = true, .isGameRoundTimeVisible = true, .expectedVisibility = Visibility::Hidden},
        {.hasScheduledRoundRestart = true, .isGameRoundTimeVisible = std::nullopt, .expectedVisibility = Visibility::Visible}
    })
));
