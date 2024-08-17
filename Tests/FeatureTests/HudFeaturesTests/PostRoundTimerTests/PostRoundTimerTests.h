#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Common/Visibility.h>
#include <Features/Hud/PostRoundTimer/PostRoundTimer.h>

#include <Mocks/PostRoundTimer/MockPostRoundTimerCondition.h>
#include <Mocks/PostRoundTimer/MockPostRoundTimerContext.h>
#include <Mocks/PostRoundTimer/MockPostRoundTimerPanel.h>

class PostRoundTimerTest : public testing::Test {
protected:
    PostRoundTimerTest()
    {
        EXPECT_CALL(mockContext, condition()).WillOnce(testing::ReturnRef(mockCondition));
    }

    void shouldRun(bool b)
    {
        EXPECT_CALL(mockCondition, shouldRun()).WillOnce(testing::Return(b));
    }

    void shouldShowPostRoundTimer(bool b)
    {
        EXPECT_CALL(mockCondition, shouldShowPostRoundTimer()).WillOnce(testing::Return(b));
    }

    testing::StrictMock<MockPostRoundTimerContext> mockContext;
    testing::StrictMock<MockPostRoundTimerCondition> mockCondition;
    testing::StrictMock<MockPostRoundTimerPanel> mockPostRoundTimerPanel;

    PostRoundTimer<MockPostRoundTimerContext&> postRoundTimer{mockContext};
};

TEST_F(PostRoundTimerTest, IsHiddenIfShouldNotRun) {
    shouldRun(false);
    EXPECT_EQ(postRoundTimer.update(), Visibility::Hidden);
}

TEST_F(PostRoundTimerTest, IsVisibleIfShouldRunAndShouldShowPostRoundTimer) {
    shouldRun(true);
    shouldShowPostRoundTimer(true);
    EXPECT_CALL(mockContext, postRoundTimerPanel()).WillOnce(testing::ReturnRef(mockPostRoundTimerPanel));
    EXPECT_CALL(mockPostRoundTimerPanel, showAndUpdate());

    EXPECT_EQ(postRoundTimer.update(), Visibility::Visible);
}

TEST_F(PostRoundTimerTest, IsHiddenIfShouldRunButShouldNotShowPostRoundTimer) {
    shouldRun(true);
    shouldShowPostRoundTimer(false);
    EXPECT_CALL(mockContext, postRoundTimerPanel()).WillOnce(testing::ReturnRef(mockPostRoundTimerPanel));
    EXPECT_CALL(mockPostRoundTimerPanel, hide());

    EXPECT_EQ(postRoundTimer.update(), Visibility::Hidden);
}
