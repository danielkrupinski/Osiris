#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <CS2/Constants/IconURLs.h>
#include <Features/Hud/BombTimer/BombTimerPanel.h>
#include <Mocks/BombTimerMocks/MockBombSiteIconPanel.h>
#include <Mocks/BombTimerMocks/MockBombTimerContext.h>
#include <Mocks/BombTimerMocks/MockBombTimerTextPanel.h>
#include <Mocks/MockPanel.h>
#include <Mocks/MockPlantedC4.h>

class BombTimerPanelTest : public testing::Test {
protected:
    testing::StrictMock<MockBombTimerContext> mockBombTimerContext;
    testing::StrictMock<MockPanel> mockBombTimerContainerPanel;
    testing::StrictMock<MockBombSiteIconPanel> mockBombSiteIconPanel;
    testing::StrictMock<MockBombTimerTextPanel> mockBombTimerTextPanel;
    testing::StrictMock<MockPlantedC4> mockPlantedC4;

    BombTimerPanel<MockBombTimerContext&> bombTimerPanel{mockBombTimerContext};
};

TEST_F(BombTimerPanelTest, HidesContainerPanel) {
    EXPECT_CALL(mockBombTimerContext, bombTimerContainerPanel()).WillOnce(testing::ReturnRef(mockBombTimerContainerPanel));
    EXPECT_CALL(mockBombTimerContainerPanel, hide());

    bombTimerPanel.hide();
}

struct BombTimerPanelTestParam {
    const char* bombSiteIconUrl{cs2::kBombSiteAIconUrl};
    float timeToExplosion{12.0f};
};

class BombTimerPanelTestWithParam : public BombTimerPanelTest, public testing::WithParamInterface<BombTimerPanelTestParam> {
};

TEST_P(BombTimerPanelTestWithParam, ShowsContainerPanelAndSetsBombSiteIconAndTimeToExplosion) {
    EXPECT_CALL(mockBombTimerContext, bombTimerContainerPanel()).WillOnce(testing::ReturnRef(mockBombTimerContainerPanel));
    EXPECT_CALL(mockBombTimerContext, tickingC4()).WillOnce(testing::ReturnRef(mockPlantedC4));
    EXPECT_CALL(mockBombTimerContext, bombSiteIconPanel()).WillOnce(testing::ReturnRef(mockBombSiteIconPanel));
    EXPECT_CALL(mockBombTimerContext, bombTimerTextPanel()).WillOnce(testing::ReturnRef(mockBombTimerTextPanel));

    EXPECT_CALL(mockBombTimerContainerPanel, show());

    EXPECT_CALL(mockPlantedC4, getBombSiteIconUrl()).WillOnce(testing::Return(GetParam().bombSiteIconUrl));
    EXPECT_CALL(mockBombSiteIconPanel, setIcon(GetParam().bombSiteIconUrl));

    EXPECT_CALL(mockPlantedC4, getTimeToExplosion()).WillOnce(testing::Return(GetParam().timeToExplosion));
    EXPECT_CALL(mockBombTimerTextPanel, setTimeToExplosion(GetParam().timeToExplosion));

    bombTimerPanel.showAndUpdate();
}

INSTANTIATE_TEST_SUITE_P(, BombTimerPanelTestWithParam, testing::Values(
    BombTimerPanelTestParam{.bombSiteIconUrl = cs2::kBombSiteAIconUrl},
    BombTimerPanelTestParam{.bombSiteIconUrl = cs2::kBombSiteBIconUrl},
    BombTimerPanelTestParam{.bombSiteIconUrl = nullptr},

    BombTimerPanelTestParam{.timeToExplosion = 40.0f},
    BombTimerPanelTestParam{.timeToExplosion = 15.234f}
));
