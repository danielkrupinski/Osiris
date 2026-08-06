#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/DefusingAlert/DefusingAlert.h>
#include <Mocks/DefusingAlertMocks/MockDefusingAlertContext.h>
#include <Mocks/DefusingAlertMocks/MockDefusingAlertPanel.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>

class DefusingAlertTest : public testing::Test {
protected:
    DefusingAlertTest()
    {
        EXPECT_CALL(mockDefusingAlertContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
    }

    void shouldRun(bool b)
    {
        mockConfig.expectGetVariable<DefusingAlertEnabled>(b);
    }

    void shouldShowDefuseAlert(bool b)
    {
        EXPECT_CALL(mockDefusingAlertContext, hasC4BeingDefused()).WillOnce(testing::Return(b));
    }

    testing::StrictMock<MockDefusingAlertContext> mockDefusingAlertContext;
    testing::StrictMock<MockDefusingAlertPanel> mockDefusingAlertPanel;
    testing::StrictMock<MockConfig> mockConfig;

    DefusingAlert<MockHookContext, MockDefusingAlertContext&> defusingAlert{mockDefusingAlertContext};
};

TEST_F(DefusingAlertTest, DoesNotRunIfShouldNotRun) {
    shouldRun(false);

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

TEST_F(DefusingAlertTest, OnDisableHidesDefusingAlertPanel) {
    EXPECT_CALL(mockDefusingAlertContext, defusingAlertPanel()).WillOnce(testing::ReturnRef(mockDefusingAlertPanel));
    EXPECT_CALL(mockDefusingAlertPanel, hide());

    defusingAlert.onDisable();
}
