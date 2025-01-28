#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/PostRoundTimer/PostRoundTimerPanelFactory.h>

#include <Mocks/MockClientPanel.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockPanel.h>
#include <Mocks/MockPanelFactory.h>

class PostRoundTimerPanelFactoryTest : public testing::Test {
protected:
    PostRoundTimerPanelFactoryTest()
    {
        EXPECT_CALL(mockHookContext, panelFactory()).WillOnce(testing::ReturnRef(mockPanelFactory));
        EXPECT_CALL(mockClientPanel, uiPanel()).WillOnce(testing::ReturnRef(mockUiPanel));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockPanelFactory> mockPanelFactory;
    testing::StrictMock<MockClientPanel> mockClientPanel;
    testing::StrictMock<MockPanel> mockParentPanel;
    testing::StrictMock<MockPanel> mockUiPanel;

    PostRoundTimerPanelFactory<MockHookContext&> postRoundTimerPanelFactory{mockHookContext};
};

TEST_F(PostRoundTimerPanelFactoryTest, ContainerPanelIsCorrectlyCreatedAndStyled) {
    EXPECT_CALL(mockPanelFactory, createPanel(testing::Ref(mockParentPanel))).WillOnce(testing::ReturnRef(mockClientPanel));
    
    using namespace post_round_timer_panel_params::container_panel_params;
    EXPECT_CALL(mockUiPanel, setWidth(kWidth));
    EXPECT_CALL(mockUiPanel, setHeight(kHeight));

    EXPECT_THAT(postRoundTimerPanelFactory.createCountdownContainerPanel(mockParentPanel), testing::Ref(mockUiPanel));
}

TEST_F(PostRoundTimerPanelFactoryTest, TextPanelIsCorrectlyCreatedAndStyled) {
    EXPECT_CALL(mockPanelFactory, createLabelPanel(testing::Ref(mockParentPanel))).WillOnce(testing::ReturnRef(mockClientPanel));

    using namespace post_round_timer_panel_params::text_panel_params;
    EXPECT_CALL(mockUiPanel, setWidth(kWidth));
    EXPECT_CALL(mockUiPanel, setMixBlendMode(kMixBlendMode));
    EXPECT_CALL(mockUiPanel, setFont(testing::Ref(kFont)));
    EXPECT_CALL(mockUiPanel, setTextAlign(kTextAlign));

    EXPECT_THAT(postRoundTimerPanelFactory.createCountdownTextPanel(mockParentPanel), testing::Ref(mockUiPanel));
}
