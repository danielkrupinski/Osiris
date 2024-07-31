#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/DefusingAlert/DefusingAlert.h>
#include <Mocks/DefusingAlertMocks/MockDefusingAlertCondition.h>
#include <Mocks/DefusingAlertMocks/MockDefusingAlertContext.h>
#include <Mocks/DefusingAlertMocks/MockDefusingAlertPanel.h>

class DefusingAlertTest : public testing::Test {
protected:
    DefusingAlertTest()
    {
        EXPECT_CALL(mockDefusingAlertContext, defusingAlertCondition()).WillOnce(testing::ReturnRef(mockDefusingAlertCondition));
    }

    void shouldRun(bool b)
    {
        EXPECT_CALL(mockDefusingAlertCondition, shouldRun()).WillOnce(testing::Return(b));
    }

    void shouldShowDefuseAlert(bool b)
    {
        EXPECT_CALL(mockDefusingAlertCondition, shouldShowDefuseAlert()).WillOnce(testing::Return(b));
    }

    testing::StrictMock<MockDefusingAlertContext> mockDefusingAlertContext;
    testing::StrictMock<MockDefusingAlertCondition> mockDefusingAlertCondition;
    testing::StrictMock<MockDefusingAlertPanel> mockDefusingAlertPanel;

    DefusingAlert<MockDefusingAlertContext&> defusingAlert{mockDefusingAlertContext};
};

TEST_F(DefusingAlertTest, DoesNotRunIfShouldNotRun) {
    shouldRun(false);
    
    EXPECT_CALL(mockDefusingAlertCondition, shouldShowDefuseAlert()).Times(0);
    EXPECT_CALL(mockDefusingAlertContext, defusingAlertPanel()).Times(0);

    defusingAlert.run();
}

TEST_F(DefusingAlertTest, ShowsAndUpdatesDefusingAlertPanelWhenDefusingAlertShouldBeShown) {
    shouldRun(true);
    shouldShowDefuseAlert(true);

    EXPECT_CALL(mockDefusingAlertContext, defusingAlertPanel()).WillOnce(testing::ReturnRef(mockDefusingAlertPanel));
    EXPECT_CALL(mockDefusingAlertPanel, showAndUpdate());

    defusingAlert.run();
}

TEST_F(DefusingAlertTest, HidesDefusingAlertPanelWhenDefusingAlertShouldNotBeShown) {
    shouldRun(true);
    shouldShowDefuseAlert(false);

    EXPECT_CALL(mockDefusingAlertContext, defusingAlertPanel()).WillOnce(testing::ReturnRef(mockDefusingAlertPanel));
    EXPECT_CALL(mockDefusingAlertPanel, hide());

    defusingAlert.run();
}
