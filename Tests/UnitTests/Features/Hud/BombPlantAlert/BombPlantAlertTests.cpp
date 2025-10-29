#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/BombPlantAlert/BombPlantAlert.h>
#include <Mocks/Features/Hud/BombPlantAlert/MockBombPlantAlertPanelFactory.h>
#include <Mocks/MockBaseWeapon.h>
#include <Mocks/MockClientPanel.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockC4.h>
#include <Mocks/MockGameRules.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockLabelPanel.h>
#include <Mocks/MockPanel.h>
#include <Mocks/MockPanelHandle.h>
#include <Mocks/MockPanoramaUiEngine.h>
#include <Mocks/MockPlayerPawn.h>

class BombPlantAlertTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockPlayerPawn> mockPlayerPawn;
    testing::StrictMock<MockBaseWeapon> mockBaseWeapon;
    testing::StrictMock<MockC4> mockC4;
    testing::StrictMock<MockGameRules> mockGameRules;
    testing::StrictMock<MockPanelHandle> mockPanelHandle;
    testing::StrictMock<MockPanel> mockContainerPanel;
    testing::StrictMock<MockPanel> mockBombsiteAIconPanel;
    testing::StrictMock<MockPanel> mockBombsiteBIconPanel;
    testing::StrictMock<MockPanel> mockTimerPanel;
    testing::StrictMock<MockClientPanel> mockClientPanel;
    testing::StrictMock<MockLabelPanel> mockLabelPanel;
    testing::StrictMock<MockPanoramaUiEngine> mockPanoramaUiEngine;
    FeaturesStates featuresStates{};
    BombPlantAlert<MockHookContext> bombPlantAlert{mockHookContext};
};

TEST_F(BombPlantAlertTest, HideDoesNothingWhenFeatureNotEnabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<BombPlantAlertEnabled>(false);
    bombPlantAlert.hide();
}

class BombPlantAlertPanelHandleTest
    : public BombPlantAlertTest,
      public testing::WithParamInterface<cs2::PanelHandle> {
};

TEST_P(BombPlantAlertPanelHandleTest, HideMethodHidesPanelWhenFeatureEnabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, makePanoramaUiEngine()).WillOnce(testing::ReturnRef(mockPanoramaUiEngine));
    EXPECT_CALL(mockPanoramaUiEngine, getPanelFromHandle(GetParam())).WillOnce(testing::ReturnRef(mockContainerPanel));
    EXPECT_CALL(mockContainerPanel, setVisible(false));

    mockConfig.expectGetVariable<BombPlantAlertEnabled>(true);
    featuresStates.hudFeaturesStates.bombPlantAlertState.containerPanelHandle = GetParam();
    bombPlantAlert.hide();
}

TEST_P(BombPlantAlertPanelHandleTest, PanelIsHiddenOnFeatureDisable) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, makePanoramaUiEngine()).WillOnce(testing::ReturnRef(mockPanoramaUiEngine));
    EXPECT_CALL(mockPanoramaUiEngine, getPanelFromHandle(GetParam())).WillOnce(testing::ReturnRef(mockContainerPanel));    
    EXPECT_CALL(mockContainerPanel, setVisible(false));

    featuresStates.hudFeaturesStates.bombPlantAlertState.containerPanelHandle = GetParam();
    bombPlantAlert.onDisable();
}

TEST_P(BombPlantAlertPanelHandleTest, PanelIsDeletedOnUnload) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, makePanoramaUiEngine()).WillOnce(testing::ReturnRef(mockPanoramaUiEngine));
    EXPECT_CALL(mockPanoramaUiEngine, deletePanelByHandle(GetParam()));
    featuresStates.hudFeaturesStates.bombPlantAlertState.containerPanelHandle = GetParam();
    bombPlantAlert.onUnload();
}

INSTANTIATE_TEST_SUITE_P(, BombPlantAlertPanelHandleTest, testing::Values(
    cs2::PanelHandle{},
    cs2::PanelHandle{.panelIndex = 123, .serialNumber = 999}
));

struct BombPlantAlertNotVisibleTestParam {
    bool featureEnabled{};
    bool playerIsHoldingBomb{};
    Optional<bool> bombIsBeingPlanted{};
    Optional<float> timeToBombArmingEnd{};
};

class BombPlantAlertNotVisibleTest
    : public BombPlantAlertTest,
      public testing::WithParamInterface<BombPlantAlertNotVisibleTestParam> {
};

TEST_P(BombPlantAlertNotVisibleTest, BombPlantAlertIsNotShown) {
    mockConfig.expectGetVariable<BombPlantAlertEnabled>(GetParam().featureEnabled);

    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockPlayerPawn, getActiveWeapon()).WillRepeatedly(testing::ReturnRef(mockBaseWeapon));
    EXPECT_CALL(mockBaseWeapon, castToC4()).WillRepeatedly(testing::ReturnRef(mockC4));
    EXPECT_CALL(mockC4, operatorBool()).WillRepeatedly(testing::Return(GetParam().playerIsHoldingBomb));
    EXPECT_CALL(mockC4, isBeingPlanted()).WillRepeatedly(testing::Return(GetParam().bombIsBeingPlanted));
    EXPECT_CALL(mockC4, timeToArmingEnd()).WillRepeatedly(testing::Return(GetParam().timeToBombArmingEnd));

    EXPECT_EQ(bombPlantAlert.show(mockPlayerPawn), Visibility::Hidden);
}

INSTANTIATE_TEST_SUITE_P(, BombPlantAlertNotVisibleTest, testing::ValuesIn(
    std::to_array<BombPlantAlertNotVisibleTestParam>({
        {.featureEnabled = false, .playerIsHoldingBomb = true, .bombIsBeingPlanted{true}, .timeToBombArmingEnd{1.5f}},
        {.featureEnabled = true, .playerIsHoldingBomb = false, .bombIsBeingPlanted{true}, .timeToBombArmingEnd{1.5f}},
        {.featureEnabled = true, .playerIsHoldingBomb = true, .bombIsBeingPlanted{std::nullopt}, .timeToBombArmingEnd{1.5f}},
        {.featureEnabled = true, .playerIsHoldingBomb = true, .bombIsBeingPlanted{false}, .timeToBombArmingEnd{1.5f}},
        {.featureEnabled = true, .playerIsHoldingBomb = true, .bombIsBeingPlanted{true}, .timeToBombArmingEnd{0.0f}},
        {.featureEnabled = true, .playerIsHoldingBomb = true, .bombIsBeingPlanted{true}, .timeToBombArmingEnd{-0.1f}},
        {.featureEnabled = true, .playerIsHoldingBomb = true, .bombIsBeingPlanted{true}, .timeToBombArmingEnd{std::nullopt}}
    })
));

struct BombPlantAlertBombsiteIconTestParam {
    Optional<cs2::BombsiteIndex> bombsiteIndex{};
    bool expectBombsiteAIconVisible{};
    bool expectBombsiteBIconVisible{};
};

class BombPlantAlertBombsiteIconTest
    : public BombPlantAlertTest,
      public testing::WithParamInterface<BombPlantAlertBombsiteIconTestParam> {
};

TEST_P(BombPlantAlertBombsiteIconTest, CorrectBombIconIsShown) {
    mockConfig.expectGetVariable<BombPlantAlertEnabled>(true);

    auto& state = featuresStates.hudFeaturesStates.bombPlantAlertState;
    state.containerPanelHandle = cs2::PanelHandle{.panelIndex = 123, .serialNumber = 500};
    state.bombsiteAIconPanelHandle = cs2::PanelHandle{.panelIndex = 124, .serialNumber = 501};
    state.bombsiteBIconPanelHandle = cs2::PanelHandle{.panelIndex = 125, .serialNumber = 502};
    state.timerPanelHandle = cs2::PanelHandle{.panelIndex = 126, .serialNumber = 503};
    EXPECT_CALL(mockHookContext, featuresStates()).WillRepeatedly(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockHookContext, gameRules()).WillOnce(testing::ReturnRef(mockGameRules));
    EXPECT_CALL(mockHookContext, makePanelHandle(testing::Ref(state.containerPanelHandle))).WillOnce(testing::ReturnRef(mockPanelHandle));
    EXPECT_CALL(mockHookContext, makePanoramaUiEngine()).WillRepeatedly(testing::ReturnRef(mockPanoramaUiEngine));
    EXPECT_CALL(mockPanoramaUiEngine, getPanelFromHandle(state.bombsiteAIconPanelHandle)).WillOnce(testing::ReturnRef(mockBombsiteAIconPanel));
    EXPECT_CALL(mockPanoramaUiEngine, getPanelFromHandle(state.bombsiteBIconPanelHandle)).WillOnce(testing::ReturnRef(mockBombsiteBIconPanel));
    EXPECT_CALL(mockPanoramaUiEngine, getPanelFromHandle(state.timerPanelHandle)).WillOnce(testing::ReturnRef(mockTimerPanel));
    EXPECT_CALL(mockPanelHandle, getOrInit(testing::_)).WillOnce(testing::ReturnRef(mockContainerPanel));
    EXPECT_CALL(mockGameRules, isRoundOver()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockGameRules, roundEndTime()).WillOnce(testing::Return(123.0f));
    EXPECT_CALL(mockPlayerPawn, getActiveWeapon()).WillOnce(testing::ReturnRef(mockBaseWeapon));
    EXPECT_CALL(mockBaseWeapon, castToC4()).WillOnce(testing::ReturnRef(mockC4));
    EXPECT_CALL(mockC4, operatorBool()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockC4, isBeingPlanted()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockC4, timeToArmingEnd()).WillRepeatedly(testing::Return(0.5f));
    EXPECT_CALL(mockC4, armingEndTime()).WillOnce(testing::Return(122.0f));
    EXPECT_CALL(mockC4, nearestBombsiteIndex()).WillOnce(testing::Return(GetParam().bombsiteIndex));
    EXPECT_CALL(mockContainerPanel, setVisible(true));
    EXPECT_CALL(mockContainerPanel, setBackgroundColor(bomb_plant_alert_params::kPositiveBackgroundColor));
    EXPECT_CALL(mockBombsiteAIconPanel, setVisible(GetParam().expectBombsiteAIconVisible));
    EXPECT_CALL(mockBombsiteBIconPanel, setVisible(GetParam().expectBombsiteBIconVisible));
    EXPECT_CALL(mockTimerPanel, setColor(bomb_plant_alert_params::kPositiveTimerColor));
    EXPECT_CALL(mockTimerPanel, clientPanel()).WillOnce(testing::ReturnRef(mockClientPanel));
    EXPECT_CALL(mockClientPanel, asLabel()).WillOnce(testing::ReturnRef(mockLabelPanel));
    EXPECT_CALL(mockLabelPanel, setText(testing::StrEq("0.5")));

    EXPECT_EQ(bombPlantAlert.show(mockPlayerPawn), Visibility::Visible);
}

INSTANTIATE_TEST_SUITE_P(, BombPlantAlertBombsiteIconTest, testing::ValuesIn(
    std::to_array<BombPlantAlertBombsiteIconTestParam>({
        {.bombsiteIndex{std::nullopt}, .expectBombsiteAIconVisible = false, .expectBombsiteBIconVisible = false},
        {.bombsiteIndex{cs2::BombsiteIndex::BombsiteA}, .expectBombsiteAIconVisible = true, .expectBombsiteBIconVisible = false},
        {.bombsiteIndex{cs2::BombsiteIndex::BombsiteB}, .expectBombsiteAIconVisible = false, .expectBombsiteBIconVisible = true}
    })
));

struct BombPlantAlertColorTestParam {
    Optional<float> armingEndTime{};
    Optional<float> roundEndTime{};
    Optional<bool> isRoundOver{};
    cs2::Color expectedBackgroundColor;
    cs2::Color expectedTimerTextColor;
};

class BombPlantAlertColorTest
    : public BombPlantAlertTest,
      public testing::WithParamInterface<BombPlantAlertColorTestParam> {
};

TEST_P(BombPlantAlertColorTest, CorrectColorsAreUsed) {
    mockConfig.expectGetVariable<BombPlantAlertEnabled>(true);

    auto& state = featuresStates.hudFeaturesStates.bombPlantAlertState;
    state.containerPanelHandle = cs2::PanelHandle{.panelIndex = 123, .serialNumber = 500};
    state.bombsiteAIconPanelHandle = cs2::PanelHandle{.panelIndex = 124, .serialNumber = 501};
    state.bombsiteBIconPanelHandle = cs2::PanelHandle{.panelIndex = 125, .serialNumber = 502};
    state.timerPanelHandle = cs2::PanelHandle{.panelIndex = 126, .serialNumber = 503};
    EXPECT_CALL(mockHookContext, featuresStates()).WillRepeatedly(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockHookContext, gameRules()).WillOnce(testing::ReturnRef(mockGameRules));
    EXPECT_CALL(mockHookContext, makePanelHandle(testing::Ref(state.containerPanelHandle))).WillOnce(testing::ReturnRef(mockPanelHandle));
    EXPECT_CALL(mockHookContext, makePanoramaUiEngine()).WillRepeatedly(testing::ReturnRef(mockPanoramaUiEngine));
    EXPECT_CALL(mockPanoramaUiEngine, getPanelFromHandle(state.bombsiteAIconPanelHandle)).WillOnce(testing::ReturnRef(mockBombsiteAIconPanel));
    EXPECT_CALL(mockPanoramaUiEngine, getPanelFromHandle(state.bombsiteBIconPanelHandle)).WillOnce(testing::ReturnRef(mockBombsiteBIconPanel));
    EXPECT_CALL(mockPanoramaUiEngine, getPanelFromHandle(state.timerPanelHandle)).WillOnce(testing::ReturnRef(mockTimerPanel));
    EXPECT_CALL(mockPanelHandle, getOrInit(testing::_)).WillOnce(testing::ReturnRef(mockContainerPanel));
    EXPECT_CALL(mockGameRules, isRoundOver()).WillOnce(testing::Return(GetParam().isRoundOver));
    EXPECT_CALL(mockGameRules, roundEndTime()).WillRepeatedly(testing::Return(GetParam().roundEndTime));
    EXPECT_CALL(mockPlayerPawn, getActiveWeapon()).WillOnce(testing::ReturnRef(mockBaseWeapon));
    EXPECT_CALL(mockBaseWeapon, castToC4()).WillOnce(testing::ReturnRef(mockC4));
    EXPECT_CALL(mockC4, operatorBool()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockC4, isBeingPlanted()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockC4, timeToArmingEnd()).WillRepeatedly(testing::Return(0.5f));
    EXPECT_CALL(mockC4, armingEndTime()).WillRepeatedly(testing::Return(GetParam().armingEndTime));
    EXPECT_CALL(mockC4, nearestBombsiteIndex()).WillOnce(testing::Return(cs2::BombsiteIndex::BombsiteA));
    EXPECT_CALL(mockContainerPanel, setVisible(true));
    EXPECT_CALL(mockContainerPanel, setBackgroundColor(GetParam().expectedBackgroundColor));
    EXPECT_CALL(mockBombsiteAIconPanel, setVisible(true));
    EXPECT_CALL(mockBombsiteBIconPanel, setVisible(false));
    EXPECT_CALL(mockTimerPanel, setColor(GetParam().expectedTimerTextColor));
    EXPECT_CALL(mockTimerPanel, clientPanel()).WillOnce(testing::ReturnRef(mockClientPanel));
    EXPECT_CALL(mockClientPanel, asLabel()).WillOnce(testing::ReturnRef(mockLabelPanel));
    EXPECT_CALL(mockLabelPanel, setText(testing::StrEq("0.5")));

    EXPECT_EQ(bombPlantAlert.show(mockPlayerPawn), Visibility::Visible);
}

INSTANTIATE_TEST_SUITE_P(, BombPlantAlertColorTest, testing::ValuesIn(
    std::to_array<BombPlantAlertColorTestParam>({
        {
            .armingEndTime{10.5f}, 
            .roundEndTime{10.6f},
            .isRoundOver{false},
            .expectedBackgroundColor{bomb_plant_alert_params::kPositiveBackgroundColor},
            .expectedTimerTextColor{bomb_plant_alert_params::kPositiveTimerColor}
        },
        {
            .armingEndTime{23.001f}, 
            .roundEndTime{23.0f},
            .isRoundOver{false},
            .expectedBackgroundColor{bomb_plant_alert_params::kNegativeBackgroundColor},
            .expectedTimerTextColor{bomb_plant_alert_params::kNegativeTimerColor}
        },
        {
            .armingEndTime{10.5f}, 
            .roundEndTime{10.6f},
            .isRoundOver{std::nullopt},
            .expectedBackgroundColor{bomb_plant_alert_params::kPositiveBackgroundColor},
            .expectedTimerTextColor{bomb_plant_alert_params::kPositiveTimerColor}
        },
        {
            .armingEndTime{23.001f}, 
            .roundEndTime{23.0f},
            .isRoundOver{std::nullopt},
            .expectedBackgroundColor{bomb_plant_alert_params::kNegativeBackgroundColor},
            .expectedTimerTextColor{bomb_plant_alert_params::kNegativeTimerColor}
        },
        {
            .armingEndTime{10.5f}, 
            .roundEndTime{10.6f},
            .isRoundOver{true},
            .expectedBackgroundColor{bomb_plant_alert_params::kNegativeBackgroundColor},
            .expectedTimerTextColor{bomb_plant_alert_params::kNegativeTimerColor}
        },
        {
            .armingEndTime{23.001f}, 
            .roundEndTime{23.0f},
            .isRoundOver{true},
            .expectedBackgroundColor{bomb_plant_alert_params::kNegativeBackgroundColor},
            .expectedTimerTextColor{bomb_plant_alert_params::kNegativeTimerColor}
        },
        {
            .armingEndTime{std::nullopt}, 
            .roundEndTime{10.6f},
            .isRoundOver{false},
            .expectedBackgroundColor{bomb_plant_alert_params::kFallbackBackgroundColor},
            .expectedTimerTextColor{bomb_plant_alert_params::kFallbackTimerColor}
        },
        {
            .armingEndTime{10.0f}, 
            .roundEndTime{std::nullopt},
            .isRoundOver{false},
            .expectedBackgroundColor{bomb_plant_alert_params::kFallbackBackgroundColor},
            .expectedTimerTextColor{bomb_plant_alert_params::kFallbackTimerColor}
        },
        {
            .armingEndTime{std::nullopt}, 
            .roundEndTime{std::nullopt},
            .isRoundOver{false},
            .expectedBackgroundColor{bomb_plant_alert_params::kFallbackBackgroundColor},
            .expectedTimerTextColor{bomb_plant_alert_params::kFallbackTimerColor}
        },
        {
            .armingEndTime{std::nullopt}, 
            .roundEndTime{std::nullopt},
            .isRoundOver{std::nullopt},
            .expectedBackgroundColor{bomb_plant_alert_params::kFallbackBackgroundColor},
            .expectedTimerTextColor{bomb_plant_alert_params::kFallbackTimerColor}
        },
        {
            .armingEndTime{std::nullopt}, 
            .roundEndTime{std::nullopt},
            .isRoundOver{true},
            .expectedBackgroundColor{bomb_plant_alert_params::kNegativeBackgroundColor},
            .expectedTimerTextColor{bomb_plant_alert_params::kNegativeTimerColor}
        }
    })
));

struct BombPlantAlertTimerTextTestParam {
    float timeToArmingEnd{};
    const char* expectedTimerText{};
};

class BombPlantAlertTimerTextTest
    : public BombPlantAlertTest,
      public testing::WithParamInterface<BombPlantAlertTimerTextTestParam> {
};

TEST_P(BombPlantAlertTimerTextTest, CorrectTimerTextIsSet) {
    mockConfig.expectGetVariable<BombPlantAlertEnabled>(true);

    auto& state = featuresStates.hudFeaturesStates.bombPlantAlertState;
    state.containerPanelHandle = cs2::PanelHandle{.panelIndex = 123, .serialNumber = 500};
    state.bombsiteAIconPanelHandle = cs2::PanelHandle{.panelIndex = 124, .serialNumber = 501};
    state.bombsiteBIconPanelHandle = cs2::PanelHandle{.panelIndex = 125, .serialNumber = 502};
    state.timerPanelHandle = cs2::PanelHandle{.panelIndex = 126, .serialNumber = 503};
    EXPECT_CALL(mockHookContext, featuresStates()).WillRepeatedly(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockHookContext, gameRules()).WillOnce(testing::ReturnRef(mockGameRules));
    EXPECT_CALL(mockHookContext, makePanelHandle(testing::Ref(state.containerPanelHandle))).WillOnce(testing::ReturnRef(mockPanelHandle));
    EXPECT_CALL(mockHookContext, makePanoramaUiEngine()).WillRepeatedly(testing::ReturnRef(mockPanoramaUiEngine));
    EXPECT_CALL(mockPanoramaUiEngine, getPanelFromHandle(state.bombsiteAIconPanelHandle)).WillOnce(testing::ReturnRef(mockBombsiteAIconPanel));
    EXPECT_CALL(mockPanoramaUiEngine, getPanelFromHandle(state.bombsiteBIconPanelHandle)).WillOnce(testing::ReturnRef(mockBombsiteBIconPanel));
    EXPECT_CALL(mockPanoramaUiEngine, getPanelFromHandle(state.timerPanelHandle)).WillOnce(testing::ReturnRef(mockTimerPanel));
    EXPECT_CALL(mockPanelHandle, getOrInit(testing::_)).WillOnce(testing::ReturnRef(mockContainerPanel));
    EXPECT_CALL(mockGameRules, isRoundOver()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockGameRules, roundEndTime()).WillOnce(testing::Return(123.0f));
    EXPECT_CALL(mockPlayerPawn, getActiveWeapon()).WillOnce(testing::ReturnRef(mockBaseWeapon));
    EXPECT_CALL(mockBaseWeapon, castToC4()).WillOnce(testing::ReturnRef(mockC4));
    EXPECT_CALL(mockC4, operatorBool()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockC4, isBeingPlanted()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockC4, timeToArmingEnd()).WillRepeatedly(testing::Return(GetParam().timeToArmingEnd));
    EXPECT_CALL(mockC4, armingEndTime()).WillOnce(testing::Return(122.0f));
    EXPECT_CALL(mockC4, nearestBombsiteIndex()).WillOnce(testing::Return(cs2::BombsiteIndex::BombsiteA));
    EXPECT_CALL(mockContainerPanel, setVisible(true));
    EXPECT_CALL(mockContainerPanel, setBackgroundColor(bomb_plant_alert_params::kPositiveBackgroundColor));
    EXPECT_CALL(mockBombsiteAIconPanel, setVisible(true));
    EXPECT_CALL(mockBombsiteBIconPanel, setVisible(false));
    EXPECT_CALL(mockTimerPanel, setColor(bomb_plant_alert_params::kPositiveTimerColor));
    EXPECT_CALL(mockTimerPanel, clientPanel()).WillOnce(testing::ReturnRef(mockClientPanel));
    EXPECT_CALL(mockClientPanel, asLabel()).WillOnce(testing::ReturnRef(mockLabelPanel));
    EXPECT_CALL(mockLabelPanel, setText(testing::StrEq(GetParam().expectedTimerText)));

    EXPECT_EQ(bombPlantAlert.show(mockPlayerPawn), Visibility::Visible);
}

INSTANTIATE_TEST_SUITE_P(, BombPlantAlertTimerTextTest, testing::ValuesIn(
    std::to_array<BombPlantAlertTimerTextTestParam>({
        {.timeToArmingEnd = 0.05f, .expectedTimerText = "0.0"},
        {.timeToArmingEnd = 0.1f, .expectedTimerText = "0.1"},
        {.timeToArmingEnd = 0.19f, .expectedTimerText = "0.1"},
        {.timeToArmingEnd = 3.0f, .expectedTimerText = "3.0"},
        {.timeToArmingEnd = 5.55f, .expectedTimerText = "5.5"}
    })
));
