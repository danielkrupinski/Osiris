#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/DefusingAlert/DefusingAlertPanel.h>
#include <Mocks/DefusingAlertMocks/MockDefusingAlertContext.h>
#include <Mocks/DefusingAlertMocks/MockDefusingCountdownTextPanel.h>
#include <Mocks/MockPanel.h>
#include <Mocks/MockPlantedC4.h>
#include <Utils/Optional.h>

class DefusingAlertPanelTest : public testing::Test {
protected:
    testing::StrictMock<MockDefusingAlertContext> mockDefusingAlertContext;
    testing::StrictMock<MockPanel> mockDefusingAlertContainerPanel;
    testing::StrictMock<MockDefusingCountdownTextPanel> mockDefusingCountdownTextPanel;
    testing::StrictMock<MockPlantedC4> mockPlantedC4;

    DefusingAlertPanel<MockDefusingAlertContext&> defusingAlertPanel{mockDefusingAlertContext};
};

TEST_F(DefusingAlertPanelTest, HidesDefusingAlertContainerPanel) {
    EXPECT_CALL(mockDefusingAlertContext, defusingAlertContainerPanel()).WillOnce(testing::ReturnRef(mockDefusingAlertContainerPanel));
    EXPECT_CALL(mockDefusingAlertContainerPanel, hide());

    defusingAlertPanel.hide();
}

struct DefusingAlertPanelTestParam {
    Optional<float> timeToDefuseEnd{3.34f};
    Optional<bool> canBeDefused{true};
};

class DefusingAlertPanelTestWithParam : public DefusingAlertPanelTest, public testing::WithParamInterface<DefusingAlertPanelTestParam> {
};

TEST_P(DefusingAlertPanelTestWithParam, ShowsDefusingAlertContainerPanelAndUpdatesCountdownTextPanel) {
    EXPECT_CALL(mockDefusingAlertContext, defusingAlertContainerPanel()).WillOnce(testing::ReturnRef(mockDefusingAlertContainerPanel));
    EXPECT_CALL(mockDefusingAlertContainerPanel, show());
    
    EXPECT_CALL(mockDefusingAlertContext, defusingCountdownTextPanel()).WillOnce(testing::ReturnRef(mockDefusingCountdownTextPanel));
    EXPECT_CALL(mockDefusingCountdownTextPanel, setTimeToDefuseEnd(GetParam().timeToDefuseEnd)).WillOnce(testing::ReturnRef(mockDefusingCountdownTextPanel));
    EXPECT_CALL(mockDefusingCountdownTextPanel, setCanBeDefused(GetParam().canBeDefused)).WillOnce(testing::ReturnRef(mockDefusingCountdownTextPanel));

    EXPECT_CALL(mockDefusingAlertContext, c4BeingDefused()).WillOnce(testing::ReturnRef(mockPlantedC4));
    EXPECT_CALL(mockPlantedC4, getTimeToDefuseEnd()).WillOnce(testing::Return(GetParam().timeToDefuseEnd));
    EXPECT_CALL(mockPlantedC4, canBeDefused()).WillOnce(testing::Return(GetParam().canBeDefused));

    defusingAlertPanel.showAndUpdate();
}

INSTANTIATE_TEST_SUITE_P(, DefusingAlertPanelTestWithParam, testing::Values(
    DefusingAlertPanelTestParam{.timeToDefuseEnd = 0.0f},
    DefusingAlertPanelTestParam{.timeToDefuseEnd = 3.45f},
    DefusingAlertPanelTestParam{.timeToDefuseEnd = 10.0f},
    DefusingAlertPanelTestParam{.timeToDefuseEnd = std::nullopt},
    DefusingAlertPanelTestParam{.canBeDefused = false},
    DefusingAlertPanelTestParam{.canBeDefused = true},
    DefusingAlertPanelTestParam{.canBeDefused = std::nullopt},
    DefusingAlertPanelTestParam{.timeToDefuseEnd = std::nullopt, .canBeDefused = std::nullopt}
));
