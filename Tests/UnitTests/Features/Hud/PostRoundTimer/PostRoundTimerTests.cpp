#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Common/Visibility.h>
#include <Features/Hud/PostRoundTimer/PostRoundTimer.h>

#include <Mocks/MockConfig.h>
#include <Mocks/MockGameRules.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/PostRoundTimer/MockPostRoundTimerContext.h>
#include <Mocks/PostRoundTimer/MockPostRoundTimerPanel.h>

class PostRoundTimerTest : public testing::Test {
protected:
    PostRoundTimerTest()
    {
        EXPECT_CALL(mockContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
    }

    void shouldRun(bool b)
    {
        mockConfig.expectGetVariable<PostRoundTimerEnabled>(b);
    }

    void shouldShowPostRoundTimer(bool b)
    {
        EXPECT_CALL(mockContext, isGameRoundTimeVisible()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(b == false));
        EXPECT_CALL(mockContext, gameRules()).Times(testing::AtMost(1)).WillRepeatedly(testing::ReturnRef(mockGameRules));
        EXPECT_CALL(mockGameRules, hasScheduledRoundRestart()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(b == true));
    }

    testing::StrictMock<MockPostRoundTimerContext> mockContext;
    testing::StrictMock<MockPostRoundTimerPanel> mockPostRoundTimerPanel;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockGameRules> mockGameRules;

    PostRoundTimer<MockHookContext, MockPostRoundTimerContext&> postRoundTimer{mockContext};
};

TEST_F(PostRoundTimerTest, IsHiddenIfShouldNotRun) {
    shouldRun(false);
    EXPECT_EQ(postRoundTimer.update(), Visibility::Hidden);
}

struct PostRoundTimerUpdateTestParam {
    bool hasScheduledRoundRestart{};
    bool isGameRoundTimeVisible{};
    Visibility expectedVisibility{};
};

class PostRoundTimerUpdateTest : public PostRoundTimerTest, public testing::WithParamInterface<PostRoundTimerUpdateTestParam> {
protected:
    testing::StrictMock<MockGameRules> mockGameRules;
};

TEST_P(PostRoundTimerUpdateTest, Update) {
    shouldRun(true);

    EXPECT_CALL(mockContext, isGameRoundTimeVisible()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(GetParam().isGameRoundTimeVisible));
    EXPECT_CALL(mockContext, gameRules()).Times(testing::AtMost(1)).WillRepeatedly(testing::ReturnRef(mockGameRules));
    EXPECT_CALL(mockGameRules, hasScheduledRoundRestart()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(GetParam().hasScheduledRoundRestart));

    EXPECT_CALL(mockContext, postRoundTimerPanel()).WillOnce(testing::ReturnRef(mockPostRoundTimerPanel));

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
        {.hasScheduledRoundRestart = true, .isGameRoundTimeVisible = false, .expectedVisibility = Visibility::Visible},
        {.hasScheduledRoundRestart = true, .isGameRoundTimeVisible = true, .expectedVisibility = Visibility::Hidden}
    })
));
