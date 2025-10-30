#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <CS2/Constants/IconURLs.h>
#include <Features/Hud/BombPlantAlert/BombPlantAlertPanelFactory.h>
#include <Matchers/Matchers.h>
#include <Mocks/MockClientPanel.h>
#include <Mocks/MockImagePanel.h>
#include <Mocks/MockPanel.h>
#include <Mocks/MockPanoramaUiEngine.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockPanelFactory.h>

class BombPlantAlertPanelFactoryTest : public testing::Test {
protected:
    testing::StrictMock<MockPanelFactory> mockPanelFactory;
    testing::StrictMock<MockPanoramaUiEngine> mockPanoramaUiEngine;
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockPanel> mockParentPanel;
    testing::StrictMock<MockPanel> mockPanel;
    testing::StrictMock<MockImagePanel> mockImagePanel;
    testing::StrictMock<MockClientPanel> mockClientPanel;
    BombPlantAlertPanelFactory<MockHookContext> bombPlantAlertPanelFactory{mockHookContext};
};

TEST_F(BombPlantAlertPanelFactoryTest, ContainerPanelIsSetUpCorrectly) {
    EXPECT_CALL(mockHookContext, panelFactory()).WillOnce(testing::ReturnRef(mockPanelFactory));
    EXPECT_CALL(mockPanelFactory, createPanel(testing::Ref(mockParentPanel), testing::_)).WillOnce(testing::ReturnRef(mockClientPanel));
    EXPECT_CALL(mockClientPanel, uiPanel()).WillOnce(testing::ReturnRef(mockPanel));
    namespace panel_params = bomb_plant_alert_panel_params::container_panel_params;
    EXPECT_CALL(mockPanel, setFlowChildren(panel_params::kChildrenFlow));
    EXPECT_CALL(mockPanel, setWidth(panel_params::kWidth));
    EXPECT_CALL(mockPanel, setHeight(panel_params::kHeight));
    EXPECT_CALL(mockPanel, setPosition(panel_params::kPositionX, panel_params::kPositionY));
    EXPECT_CALL(mockPanel, setBorderRadius(panel_params::kBorderRadius));
    EXPECT_CALL(mockHookContext, makePanoramaUiEngine()).WillOnce(testing::ReturnRef(mockPanoramaUiEngine));
    EXPECT_CALL(mockPanoramaUiEngine, runScript(testing::Ref(mockParentPanel), testing::_));

    bombPlantAlertPanelFactory.createContainerPanel(mockParentPanel);
}

TEST_F(BombPlantAlertPanelFactoryTest, TimerPanelIsSetUpCorrectly) {
    EXPECT_CALL(mockHookContext, panelFactory()).WillOnce(testing::ReturnRef(mockPanelFactory));
    EXPECT_CALL(mockPanelFactory, createLabelPanel(testing::Ref(mockParentPanel))).WillOnce(testing::ReturnRef(mockClientPanel));
    EXPECT_CALL(mockClientPanel, uiPanel()).WillOnce(testing::ReturnRef(mockPanel));
    namespace panel_params = bomb_plant_alert_panel_params::timer_panel_params;
    EXPECT_CALL(mockPanel, setWidth(panel_params::kWidth));
    EXPECT_CALL(mockPanel, setFont(PanelFontParamsEq(panel_params::kFont)));
    EXPECT_CALL(mockPanel, setMixBlendMode(panel_params::kMixBlendMode));
    EXPECT_CALL(mockPanel, setAlign(PanelAlignmentParamsEq(panel_params::kAlignment)));
    EXPECT_CALL(mockPanel, setMargin(PanelMarginParamsEq(panel_params::kMargin)));
    EXPECT_CALL(mockPanel, setTextAlign(panel_params::kTextAlign));

    bombPlantAlertPanelFactory.createTimerPanel(mockParentPanel);
}

class BombPlantAlertPanelFactoryBombsiteIconTest
    : public BombPlantAlertPanelFactoryTest,
      public testing::WithParamInterface<const char*> {
};

TEST_P(BombPlantAlertPanelFactoryBombsiteIconTest, BombsiteIconPanelIsSetUpCorrectly) {
    EXPECT_CALL(mockHookContext, panelFactory()).WillOnce(testing::ReturnRef(mockPanelFactory));
    EXPECT_CALL(mockPanelFactory, createImagePanel(testing::Ref(mockParentPanel))).WillOnce(testing::ReturnRef(mockImagePanel));
    EXPECT_CALL(mockImagePanel, uiPanel()).WillOnce(testing::ReturnRef(mockPanel));
    namespace panel_params = bomb_plant_alert_panel_params::bombsite_icon_panel_params;
    EXPECT_CALL(mockImagePanel, setImageSvg(testing::StrEq(GetParam()), panel_params::kTextureHeight));
    EXPECT_CALL(mockPanel, setAlign(PanelAlignmentParamsEq(panel_params::kAlignment)));
    EXPECT_CALL(mockPanel, setMargin(PanelMarginParamsEq(panel_params::kMargin)));

    bombPlantAlertPanelFactory.createBombsiteIconPanel(mockParentPanel, GetParam());
}

INSTANTIATE_TEST_SUITE_P(, BombPlantAlertPanelFactoryBombsiteIconTest, testing::Values(
    cs2::kBombsiteAPlantingIconUrl,
    cs2::kBombsiteBPlantingIconUrl
));
