#pragma once

#include <array>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/PostRoundTimer/PostRoundTimerContext.h>

#include <Mocks/HudMocks/MockHud.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockPanel.h>
#include <Utils/Optional.h>

class PostRoundTimerContextTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    PostRoundTimerContext<MockHookContext&> postRoundTimerContext{mockHookContext};
};

TEST_F(PostRoundTimerContextTest, ReturnsGameRulesFromHookContext) {
    testing::StrictMock<MockGameRules> mockGameRules;
    EXPECT_CALL(mockHookContext, gameRules()).WillOnce(testing::ReturnRef(mockGameRules));

    EXPECT_THAT(postRoundTimerContext.gameRules(), testing::Ref(mockGameRules));
}

struct PostRoundTimerContextGameRoundTimeVisibleTestParam {
    Optional<bool> isVisible{};
    bool expectedIsVisible{};
};

class PostRoundTimerContextGameRoundTimeVisibleTest : public PostRoundTimerContextTest, public testing::WithParamInterface<PostRoundTimerContextGameRoundTimeVisibleTestParam> {
};

TEST_P(PostRoundTimerContextGameRoundTimeVisibleTest, IsGameRoundTimeVisible) {
    testing::StrictMock<MockHud> mockHud;
    testing::StrictMock<MockPanel> mockGameRoundTimePanel;
    
    EXPECT_CALL(mockHookContext, hud()).WillOnce(testing::ReturnRef(mockHud));
    EXPECT_CALL(mockHud, timerTextPanel()).WillOnce(testing::ReturnRef(mockGameRoundTimePanel));
    EXPECT_CALL(mockGameRoundTimePanel, isVisible()).WillOnce(testing::Return(GetParam().isVisible));

    EXPECT_EQ(postRoundTimerContext.isGameRoundTimeVisible(), GetParam().expectedIsVisible);
}

INSTANTIATE_TEST_SUITE_P(, PostRoundTimerContextGameRoundTimeVisibleTest, testing::ValuesIn(
    std::to_array<PostRoundTimerContextGameRoundTimeVisibleTestParam>({
        {.isVisible = std::nullopt, .expectedIsVisible = false},
        {.isVisible = false, .expectedIsVisible = false},
        {.isVisible = true, .expectedIsVisible = true}
    })
));
