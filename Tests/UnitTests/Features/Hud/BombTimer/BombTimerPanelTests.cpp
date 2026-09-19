#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <CS2/Constants/IconURLs.h>
#include <Features/Hud/BombTimer/BombTimerPanel.h>
#include <Mocks/BombTimerMocks/MockBombTimerPanelFactory.h>
#include <Mocks/MockPanel.h>
#include <Mocks/MockPlantedC4.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockPanoramaUiEngine.h>
#include <Mocks/MockClientPanel.h>
#include <Mocks/MockImagePanel.h>
#include <Mocks/HudMocks/MockHud.h>
#include <Mocks/MockLabelPanel.h>
#include <Mocks/MockPanelHandle.h>

class BombTimerPanelTest : public testing::Test {
protected:
    testing::StrictMock<MockPanel> mockBombTimerContainerPanel;
    testing::StrictMock<MockPanel> mockBombsiteIconPanel;
    testing::StrictMock<MockPanel> mockBombTimerTextPanel;
    testing::StrictMock<MockPlantedC4> mockPlantedC4;
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockPanoramaUiEngine> mockPanoramaUiEngine;
    testing::StrictMock<MockPanoramaUiPanelChildPanels> mockChildPanels;
    testing::StrictMock<MockPanelHandle> mockPanelHandle;
    testing::StrictMock<MockClientPanel> mockBombsiteIconClientPanel;
    testing::StrictMock<MockImagePanel> mockBombsiteIconImagePanel;
    testing::StrictMock<MockClientPanel> mockBombTimerTextClientPanel;
    testing::StrictMock<MockLabelPanel> mockBombTimerTextLabelPanel;

    FeaturesStates featuresStates{};

    auto& state() noexcept
    {
        return featuresStates.hudFeaturesStates.bombTimerState;
    }

    BombTimerPanel<MockHookContext&> bombTimerPanel{mockHookContext};
};

class BombTimerPanelHideTest : public BombTimerPanelTest, public testing::WithParamInterface<cs2::PanelHandle> {
};

TEST_P(BombTimerPanelHideTest, HidesContainerPanel) {
    state().bombTimerContainerPanelHandle = GetParam();

    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, makePanoramaUiEngine()).WillOnce(testing::ReturnRef(mockPanoramaUiEngine));
    EXPECT_CALL(mockPanoramaUiEngine, getPanelFromHandle(GetParam())).WillOnce(testing::ReturnRef(mockBombTimerContainerPanel));
    EXPECT_CALL(mockBombTimerContainerPanel, hide());

    bombTimerPanel.hide();
}

INSTANTIATE_TEST_SUITE_P(, BombTimerPanelHideTest, testing::Values(
    cs2::PanelHandle{},
    cs2::PanelHandle{.panelIndex = 123, .serialNumber = 456},
    cs2::PanelHandle{.panelIndex = 1234, .serialNumber = 2048}));

TEST_F(BombTimerPanelTest, BombTimerPanelIsCreatedCorrectly) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillRepeatedly(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, makePanelHandle(testing::_)).WillOnce(testing::ReturnRef(mockPanelHandle));
    EXPECT_CALL(mockPanelHandle, getOrInit(testing::_)).WillOnce(testing::WithArg<0>(
        [this](auto&& f) -> decltype(auto) {
            testing::StrictMock<MockHud> mockHud;
            testing::StrictMock<MockBombTimerPanelFactory> mockBombTimerPanelFactory;
            testing::StrictMock<MockPanel> mockScoreAndTimeAndBombPanel;
            EXPECT_CALL(mockHookContext, makeBombTimerPanelFactory()).WillOnce(testing::ReturnRef(mockBombTimerPanelFactory));
            EXPECT_CALL(mockHookContext, hud()).WillOnce(testing::ReturnRef(mockHud));
            EXPECT_CALL(mockHud, scoreAndTimeAndBomb()).WillOnce(testing::ReturnRef(mockScoreAndTimeAndBombPanel));

            {
                testing::InSequence s;
                EXPECT_CALL(mockBombTimerPanelFactory, createContainerPanel(testing::Ref(mockScoreAndTimeAndBombPanel))).WillOnce(testing::ReturnRef(mockBombTimerContainerPanel));
                EXPECT_CALL(mockBombTimerPanelFactory, createBombsiteIconPanel(testing::Ref(mockBombTimerContainerPanel))).WillOnce(testing::ReturnRef(mockBombsiteIconPanel));
                EXPECT_CALL(mockBombTimerPanelFactory, createTimerTextPanel(testing::Ref(mockBombTimerContainerPanel))).WillOnce(testing::ReturnRef(mockBombTimerTextPanel));
            }

            auto& container = f();
            EXPECT_THAT(container, testing::Ref(mockBombTimerContainerPanel));
            return container;
        }
    ));
    
    EXPECT_CALL(mockBombTimerContainerPanel, show());

    EXPECT_CALL(mockBombTimerContainerPanel, children()).WillRepeatedly(testing::ReturnRef(mockChildPanels));
    EXPECT_CALL(mockChildPanels, operatorSubscript(0)).WillOnce(testing::ReturnRef(mockBombsiteIconPanel));
    EXPECT_CALL(mockChildPanels, operatorSubscript(1)).WillOnce(testing::ReturnRef(mockBombTimerTextPanel));

    EXPECT_CALL(mockPlantedC4, bombsiteIndex()).WillOnce(testing::Return(cs2::BombsiteIndex::BombsiteA));
    EXPECT_CALL(mockBombsiteIconPanel, clientPanel()).WillOnce(testing::ReturnRef(mockBombsiteIconClientPanel));
    EXPECT_CALL(mockBombsiteIconClientPanel, asImagePanel()).WillOnce(testing::ReturnRef(mockBombsiteIconImagePanel));
    EXPECT_CALL(mockBombsiteIconImagePanel, setImageSvg(cs2::kBombSiteAIconUrl));

    EXPECT_CALL(mockPlantedC4, getTimeToExplosion()).WillOnce(testing::Return(15.5f));
    EXPECT_CALL(mockBombTimerTextPanel, clientPanel()).WillOnce(testing::ReturnRef(mockBombTimerTextClientPanel));
    EXPECT_CALL(mockBombTimerTextClientPanel, asLabel()).WillOnce(testing::ReturnRef(mockBombTimerTextLabelPanel));
    EXPECT_CALL(mockBombTimerTextLabelPanel, setText(testing::StrEq("15.5")));

    bombTimerPanel.showAndUpdate(mockPlantedC4);
}

struct BombTimerPanelTestParam {
    Optional<cs2::BombsiteIndex> bombsiteIndex{cs2::BombsiteIndex::BombsiteA};
    const char* expectedBombsiteIconUrl{cs2::kBombSiteAIconUrl};
    float timeToExplosion{12.0f};
    const char* expectedTimeToExplosion{"12.0"};
};

class BombTimerPanelTestWithParam : public BombTimerPanelTest, public testing::WithParamInterface<BombTimerPanelTestParam> {
};

TEST_P(BombTimerPanelTestWithParam, ShowsContainerPanelAndSetsBombsiteIconAndTimeToExplosion) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillRepeatedly(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, makePanelHandle(testing::_)).WillOnce(testing::ReturnRef(mockPanelHandle));
    EXPECT_CALL(mockPanelHandle, getOrInit(testing::_)).WillOnce(testing::ReturnRef(mockBombTimerContainerPanel));
    
    EXPECT_CALL(mockBombTimerContainerPanel, show());

    EXPECT_CALL(mockBombTimerContainerPanel, children()).WillRepeatedly(testing::ReturnRef(mockChildPanels));
    EXPECT_CALL(mockChildPanels, operatorSubscript(0)).WillOnce(testing::ReturnRef(mockBombsiteIconPanel));
    EXPECT_CALL(mockChildPanels, operatorSubscript(1)).WillOnce(testing::ReturnRef(mockBombTimerTextPanel));

    EXPECT_CALL(mockPlantedC4, bombsiteIndex()).WillOnce(testing::Return(GetParam().bombsiteIndex));
    EXPECT_CALL(mockBombsiteIconPanel, clientPanel()).WillOnce(testing::ReturnRef(mockBombsiteIconClientPanel));
    EXPECT_CALL(mockBombsiteIconClientPanel, asImagePanel()).WillOnce(testing::ReturnRef(mockBombsiteIconImagePanel));
    if (GetParam().expectedBombsiteIconUrl != nullptr)
        EXPECT_CALL(mockBombsiteIconImagePanel, setImageSvg(testing::Matcher<const char*>(testing::StrEq(GetParam().expectedBombsiteIconUrl))));

    EXPECT_CALL(mockPlantedC4, getTimeToExplosion()).WillOnce(testing::Return(GetParam().timeToExplosion));
    EXPECT_CALL(mockBombTimerTextPanel, clientPanel()).WillOnce(testing::ReturnRef(mockBombTimerTextClientPanel));
    EXPECT_CALL(mockBombTimerTextClientPanel, asLabel()).WillOnce(testing::ReturnRef(mockBombTimerTextLabelPanel));
    EXPECT_CALL(mockBombTimerTextLabelPanel, setText(testing::StrEq(GetParam().expectedTimeToExplosion)));

    bombTimerPanel.showAndUpdate(mockPlantedC4);
}

INSTANTIATE_TEST_SUITE_P(, BombTimerPanelTestWithParam, testing::Values(
    BombTimerPanelTestParam{.bombsiteIndex{cs2::BombsiteIndex::BombsiteA}, .expectedBombsiteIconUrl = cs2::kBombSiteAIconUrl},
    BombTimerPanelTestParam{.bombsiteIndex{cs2::BombsiteIndex::BombsiteB}, .expectedBombsiteIconUrl = cs2::kBombSiteBIconUrl},
    BombTimerPanelTestParam{.bombsiteIndex{std::nullopt}, .expectedBombsiteIconUrl = nullptr},

    BombTimerPanelTestParam{.timeToExplosion = 999.9f, .expectedTimeToExplosion = "999.9"},
    BombTimerPanelTestParam{.timeToExplosion = 40.0f, .expectedTimeToExplosion = "40.0"},
    BombTimerPanelTestParam{.timeToExplosion = 11.0f, .expectedTimeToExplosion = "11.0"},
    BombTimerPanelTestParam{.timeToExplosion = 10.9999f, .expectedTimeToExplosion = "10.9"},
    BombTimerPanelTestParam{.timeToExplosion = 0.2f, .expectedTimeToExplosion = "0.2"},
    BombTimerPanelTestParam{.timeToExplosion = 0.19999f, .expectedTimeToExplosion = "0.1"},
    BombTimerPanelTestParam{.timeToExplosion = 0.0f, .expectedTimeToExplosion = "0.0"}
));
