#pragma once

#include <array>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/PostRoundTimer/PostRoundTimerCondition.h>
#include <Features/Hud/PostRoundTimer/PostRoundTimerState.h>

#include <Mocks/MockGameRules.h>
#include <Mocks/PostRoundTimer/MockPostRoundTimerContext.h>

class PostRoundTimerConditionTest : public testing::Test {
protected:
    testing::StrictMock<MockPostRoundTimerContext> mockContext;
    PostRoundTimerCondition<MockPostRoundTimerContext&> postRoundTimerCondition{mockContext};
};

class PostRoundTimerConditionShouldRunTest : public PostRoundTimerConditionTest {
protected:
    PostRoundTimerConditionShouldRunTest()
    {
        EXPECT_CALL(mockContext, state()).WillOnce(testing::ReturnRef(state));
    }

    PostRoundTimerState state;
};

TEST_F(PostRoundTimerConditionShouldRunTest, ShouldRunIfEnabled) {
    state.enabled = true;
    EXPECT_TRUE(postRoundTimerCondition.shouldRun());
}

TEST_F(PostRoundTimerConditionShouldRunTest, ShouldNotRunIfNotEnabled) {
    state.enabled = false;
    EXPECT_FALSE(postRoundTimerCondition.shouldRun());
}

struct PostRoundTimerConditionShouldShowTestParam {
    bool hasScheduledRoundRestart{};
    bool isGameRoundTimeVisible{};
    bool expectedShouldShow{};
};

class PostRoundTimerConditionShouldShowTest : public PostRoundTimerConditionTest, public testing::WithParamInterface<PostRoundTimerConditionShouldShowTestParam> {
protected:
    testing::StrictMock<MockGameRules> mockGameRules;
};

TEST_P(PostRoundTimerConditionShouldShowTest, ShouldShowPostRoundTimer) {
    EXPECT_CALL(mockContext, isGameRoundTimeVisible()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(GetParam().isGameRoundTimeVisible));
    EXPECT_CALL(mockContext, gameRules()).Times(testing::AtMost(1)).WillRepeatedly(testing::ReturnRef(mockGameRules));
    EXPECT_CALL(mockGameRules, hasScheduledRoundRestart()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(GetParam().hasScheduledRoundRestart));

    EXPECT_EQ(postRoundTimerCondition.shouldShowPostRoundTimer(), GetParam().expectedShouldShow);
}

INSTANTIATE_TEST_SUITE_P(, PostRoundTimerConditionShouldShowTest, testing::ValuesIn(
    std::to_array<PostRoundTimerConditionShouldShowTestParam>({
        {.hasScheduledRoundRestart = false, .isGameRoundTimeVisible = false, .expectedShouldShow = false},
        {.hasScheduledRoundRestart = false, .isGameRoundTimeVisible = true, .expectedShouldShow = false},
        {.hasScheduledRoundRestart = true, .isGameRoundTimeVisible = false, .expectedShouldShow = true},
        {.hasScheduledRoundRestart = true, .isGameRoundTimeVisible = true, .expectedShouldShow = false}
    })
));
