#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/DefusingAlert/DefusingAlertCondition.h>
#include <Features/Hud/DefusingAlert/DefusingAlertState.h>
#include <Mocks/DefusingAlertMocks/MockDefusingAlertContext.h>

class DefusingAlertConditionTest : public testing::Test {
protected:
    void hasC4BeingDefused(bool b)
    {
        EXPECT_CALL(mockDefusingAlertContext, hasC4BeingDefused()).WillOnce(testing::Return(b));
    }

    testing::StrictMock<MockDefusingAlertContext> mockDefusingAlertContext;
    DefusingAlertCondition<MockDefusingAlertContext&> defusingAlertCondition{mockDefusingAlertContext};
    DefusingAlertState defusingAlertState;
};

TEST_F(DefusingAlertConditionTest, ShouldRunIfEnabled) {
    defusingAlertState.enabled = true;
    EXPECT_CALL(mockDefusingAlertContext, state()).WillOnce(testing::ReturnRef(defusingAlertState));

    EXPECT_EQ(defusingAlertCondition.shouldRun(), true);
}

TEST_F(DefusingAlertConditionTest, ShouldNotRunIfNotEnabled) {
    defusingAlertState.enabled = false;
    EXPECT_CALL(mockDefusingAlertContext, state()).WillOnce(testing::ReturnRef(defusingAlertState));

    EXPECT_EQ(defusingAlertCondition.shouldRun(), false);
}

TEST_F(DefusingAlertConditionTest, ShouldShowDefusingAlertIfHasC4BeingDefused) {
    hasC4BeingDefused(true);
    EXPECT_EQ(defusingAlertCondition.shouldShowDefuseAlert(), true);
}

TEST_F(DefusingAlertConditionTest, ShouldNotShowDefusingAlertIfDoesNotHaveC4BeingDefused) {
    hasC4BeingDefused(false);
    EXPECT_EQ(defusingAlertCondition.shouldShowDefuseAlert(), false);
}
