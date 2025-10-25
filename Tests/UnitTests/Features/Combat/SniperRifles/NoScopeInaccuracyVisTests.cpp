#include <gtest/gtest.h>
#include <Features/Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVis.h>
#include <Mocks/HudMocks/MockHud.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockCrosshair.h>
#include <Mocks/MockPanel.h>
#include <Mocks/MockClientPanel.h>
#include <Mocks/MockCvarSystem.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockPanelFactory.h>
#include <Mocks/MockPanelHandle.h>
#include <Mocks/MockPanoramaUiEngine.h>
#include <Mocks/MockPlayerPawn.h>
#include <Mocks/MockViewToProjectionMatrix.h>

class NoScopeInaccuracyVisTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockPanelHandle> mockPanelHandle;
    testing::StrictMock<MockPanel> mockPanel;
    testing::StrictMock<MockCrosshair> mockCrosshair;
    testing::StrictMock<MockPlayerPawn> mockLocalPlayerPawn;
    testing::StrictMock<MockViewToProjectionMatrix> mockViewToProjectionMatrix;
    testing::StrictMock<MockPanoramaUiEngine> mockPanoramaUiEngine;
    testing::StrictMock<MockHud> mockHud;
    testing::StrictMock<MockPanelFactory> mockPanelFactory;
    testing::StrictMock<MockClientPanel> mockClientPanel;
    testing::StrictMock<MockPanel> mockHudReticle;
    FeaturesStates featuresStates{};
    NoScopeInaccuracyVis<MockHookContext> noScopeInaccuracyVis{mockHookContext};
};

TEST_F(NoScopeInaccuracyVisTest, UpdateDoesNothingWhenDisabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<no_scope_inaccuracy_vis_vars::Enabled>(false);
    noScopeInaccuracyVis.update();
}

TEST_F(NoScopeInaccuracyVisTest, PanelIsHiddenOnDisable) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, makePanelHandle(testing::Ref(featuresStates.noScopeInaccuracyVisState.panelHandle))).WillOnce(testing::ReturnRef(mockPanelHandle));
    EXPECT_CALL(mockPanelHandle, get()).WillOnce(testing::ReturnRef(mockPanel));
    EXPECT_CALL(mockPanel, hide());
    noScopeInaccuracyVis.onDisable();
}

TEST_F(NoScopeInaccuracyVisTest, PanelIsCreatedWhenDoesNotExist) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<no_scope_inaccuracy_vis_vars::Enabled>(true);
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, makePanelHandle(featuresStates.noScopeInaccuracyVisState.panelHandle)).WillOnce(testing::ReturnRef(mockPanelHandle));
    EXPECT_CALL(mockHookContext, activeLocalPlayerPawn()).WillOnce(testing::ReturnRef(mockLocalPlayerPawn));
    EXPECT_CALL(mockPanelHandle, getOrInit(testing::_)).WillOnce(testing::WithArg<0>(testing::Invoke([this](auto&& panelFactoryFunctor) -> decltype(auto) {
        EXPECT_CALL(mockHookContext, hud()).WillOnce(testing::ReturnRef(mockHud));
        EXPECT_CALL(mockHud, getHudReticle()).WillOnce(testing::ReturnRef(mockHudReticle));
        EXPECT_CALL(mockHookContext, panelFactory()).WillOnce(testing::ReturnRef(mockPanelFactory));
        EXPECT_CALL(mockPanelFactory, createPanel(testing::Ref(mockHudReticle))).WillOnce(testing::ReturnRef(mockClientPanel));
        EXPECT_CALL(mockClientPanel, uiPanel()).WillOnce(testing::ReturnRef(mockPanel));
        EXPECT_CALL(mockPanel, setWidth(cs2::CUILength::heightPercentage(100)));
        EXPECT_CALL(mockPanel, setBorderRadius(cs2::CUILength::percent(50)));
        EXPECT_CALL(mockPanel, setAlign(testing::_)).WillOnce(testing::WithArg<0>(testing::Invoke([](const auto& panelAlignmentParams) {
            EXPECT_EQ(panelAlignmentParams.horizontalAlignment, cs2::k_EHorizontalAlignmentCenter);
            EXPECT_EQ(panelAlignmentParams.verticalAlignment, cs2::k_EVerticalAlignmentCenter);
        })));
        return panelFactoryFunctor();
    })));
    EXPECT_CALL(mockLocalPlayerPawn, operatorBool()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockLocalPlayerPawn, isScoped()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockLocalPlayerPawn, isUsingSniperRifle()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockHookContext, makeViewToProjectionMatrix()).WillOnce(testing::ReturnRef(mockViewToProjectionMatrix));
    EXPECT_CALL(mockViewToProjectionMatrix, transformY(testing::_)).WillOnce(testing::Return(0.1f));
    EXPECT_CALL(mockHookContext, localPlayerBulletInaccuracy()).WillOnce(testing::Return(0.1f));
    EXPECT_CALL(mockHookContext, makeCrosshair()).WillOnce(testing::ReturnRef(mockCrosshair));
    EXPECT_CALL(mockCrosshair, getColor()).WillOnce(testing::Return(cs2::Color{255, 255, 255}));
    EXPECT_CALL(mockPanel, setVisible(true));
    EXPECT_CALL(mockPanel, setHeight(testing::_));
    EXPECT_CALL(mockPanel, setBorder(no_scope_inaccuracy_vis_params::kBorderWidth, testing::_));
    EXPECT_CALL(mockPanel, setBackgroundColor(testing::_));
    noScopeInaccuracyVis.update();
}

class NoScopeInaccuracyVisOnUnloadTest
    : public NoScopeInaccuracyVisTest,
      public testing::WithParamInterface<cs2::PanelHandle> {
};

TEST_P(NoScopeInaccuracyVisOnUnloadTest, PanelIsDeletedOnUnload) {
    EXPECT_CALL(mockHookContext, makePanoramaUiEngine()).WillOnce(testing::ReturnRef(mockPanoramaUiEngine));
    auto& panelHandle = featuresStates.noScopeInaccuracyVisState.panelHandle;
    panelHandle = GetParam();
    EXPECT_CALL(mockPanoramaUiEngine, deletePanelByHandle(panelHandle));
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    noScopeInaccuracyVis.onUnload();
}

INSTANTIATE_TEST_SUITE_P(, NoScopeInaccuracyVisOnUnloadTest, testing::Values(
    cs2::PanelHandle{.panelIndex = 123, .serialNumber = 1024},
    cs2::PanelHandle{}
));

struct NoScopeInaccuracyVisNotVisibleTestParam {
    bool hasActiveLocalPlayerPawn{};
    bool usingSniperRifle{};
    Optional<bool> scoped{};
};

class NoScopeInaccuracyVisNotVisibleTest
    : public NoScopeInaccuracyVisTest,
      public testing::WithParamInterface<NoScopeInaccuracyVisNotVisibleTestParam> {
};

TEST_P(NoScopeInaccuracyVisNotVisibleTest, PanelIsNotUpdatedWhenNotVisible) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<no_scope_inaccuracy_vis_vars::Enabled>(true);
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, makePanelHandle(testing::Ref(featuresStates.noScopeInaccuracyVisState.panelHandle))).WillOnce(testing::ReturnRef(mockPanelHandle));
    EXPECT_CALL(mockHookContext, activeLocalPlayerPawn()).WillOnce(testing::ReturnRef(mockLocalPlayerPawn));
    EXPECT_CALL(mockPanelHandle, getOrInit(testing::_)).WillOnce(testing::ReturnRef(mockPanel));
    EXPECT_CALL(mockLocalPlayerPawn, operatorBool()).WillRepeatedly(testing::Return(GetParam().hasActiveLocalPlayerPawn));
    EXPECT_CALL(mockLocalPlayerPawn, isScoped()).WillRepeatedly(testing::Return(GetParam().scoped));
    EXPECT_CALL(mockLocalPlayerPawn, isUsingSniperRifle()).WillRepeatedly(testing::Return(GetParam().usingSniperRifle));
    EXPECT_CALL(mockPanel, setVisible(false));
    noScopeInaccuracyVis.update();
}

INSTANTIATE_TEST_SUITE_P(, NoScopeInaccuracyVisNotVisibleTest, testing::ValuesIn(
    std::to_array<NoScopeInaccuracyVisNotVisibleTestParam>({
        {.hasActiveLocalPlayerPawn = false, .usingSniperRifle = true, .scoped{false}},
        {.hasActiveLocalPlayerPawn = true, .usingSniperRifle = false, .scoped{false}},
        {.hasActiveLocalPlayerPawn = true, .usingSniperRifle = true, .scoped{true}},
        {.hasActiveLocalPlayerPawn = true, .usingSniperRifle = true, .scoped{std::nullopt}}
    })
));

struct NoScopeInaccuracyVisColorTestParam {
    Optional<cs2::Color> crosshairColor;
    cs2::Color expectedBorderColor;
    cs2::Color expectedBackgroundColor;
};

class NoScopeInaccuracyVisColorTest
    : public NoScopeInaccuracyVisTest,
      public testing::WithParamInterface<NoScopeInaccuracyVisColorTestParam> {
};

TEST_P(NoScopeInaccuracyVisColorTest, CorrectPanelColorsAreSet) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<no_scope_inaccuracy_vis_vars::Enabled>(true);
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, makePanelHandle(featuresStates.noScopeInaccuracyVisState.panelHandle)).WillOnce(testing::ReturnRef(mockPanelHandle));
    EXPECT_CALL(mockHookContext, activeLocalPlayerPawn()).WillOnce(testing::ReturnRef(mockLocalPlayerPawn));
    EXPECT_CALL(mockPanelHandle, getOrInit(testing::_)).WillOnce(testing::ReturnRef(mockPanel));
    EXPECT_CALL(mockLocalPlayerPawn, operatorBool()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockLocalPlayerPawn, isScoped()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockLocalPlayerPawn, isUsingSniperRifle()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockHookContext, makeViewToProjectionMatrix()).WillOnce(testing::ReturnRef(mockViewToProjectionMatrix));
    EXPECT_CALL(mockViewToProjectionMatrix, transformY(testing::_)).WillOnce(testing::Return(0.2f));
    EXPECT_CALL(mockHookContext, localPlayerBulletInaccuracy()).WillOnce(testing::Return(0.1f));
    EXPECT_CALL(mockHookContext, makeCrosshair()).WillOnce(testing::ReturnRef(mockCrosshair));
    EXPECT_CALL(mockCrosshair, getColor()).WillOnce(testing::Return(GetParam().crosshairColor));
    EXPECT_CALL(mockPanel, setVisible(true));
    EXPECT_CALL(mockPanel, setHeight(testing::_));
    EXPECT_CALL(mockPanel, setBorder(no_scope_inaccuracy_vis_params::kBorderWidth, GetParam().expectedBorderColor));
    EXPECT_CALL(mockPanel, setBackgroundColor(GetParam().expectedBackgroundColor));
    noScopeInaccuracyVis.update();
}

static_assert(no_scope_inaccuracy_vis_params::kBorderAlpha == 255, "Update the test below");
static_assert(no_scope_inaccuracy_vis_params::kBackgroundAlpha == 30, "Update the test below");
static_assert(no_scope_inaccuracy_vis_params::kFallbackColor == cs2::Color{255, 255, 255}, "Update the test below");

INSTANTIATE_TEST_SUITE_P(, NoScopeInaccuracyVisColorTest, testing::ValuesIn(
    std::to_array<NoScopeInaccuracyVisColorTestParam>({
        {
            .crosshairColor{std::nullopt},
            .expectedBorderColor{255, 255, 255, 255},
            .expectedBackgroundColor{255, 255, 255, 30}
        },
        {
            .crosshairColor{cs2::Color{120, 50, 210}},
            .expectedBorderColor{120, 50, 210, 255},
            .expectedBackgroundColor{120, 50, 210, 30}
        },
        {
            .crosshairColor{cs2::Color{100, 50, 220, 10}},
            .expectedBorderColor{100, 50, 220, 255},
            .expectedBackgroundColor{100, 50, 220, 30}
        }
    })
));

struct NoScopeInaccuracyVisHeightTestParam {
    Optional<float> localPlayerBulletInaccuracy{};
    float expectedInaccuracyToTransform{};
    std::optional<float> transformedInaccuracy{};
    cs2::CUILength expectedHeight{};
};

class NoScopeInaccuracyVisHeightTest
    : public NoScopeInaccuracyVisTest,
      public testing::WithParamInterface<NoScopeInaccuracyVisHeightTestParam> {
};

TEST_P(NoScopeInaccuracyVisHeightTest, CorrectPanelHeightIsSet) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<no_scope_inaccuracy_vis_vars::Enabled>(true);
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_CALL(mockHookContext, makePanelHandle(featuresStates.noScopeInaccuracyVisState.panelHandle)).WillOnce(testing::ReturnRef(mockPanelHandle));
    EXPECT_CALL(mockHookContext, activeLocalPlayerPawn()).WillOnce(testing::ReturnRef(mockLocalPlayerPawn));
    EXPECT_CALL(mockPanelHandle, getOrInit(testing::_)).WillOnce(testing::ReturnRef(mockPanel));
    EXPECT_CALL(mockLocalPlayerPawn, operatorBool()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockLocalPlayerPawn, isScoped()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockLocalPlayerPawn, isUsingSniperRifle()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockHookContext, makeViewToProjectionMatrix()).WillOnce(testing::ReturnRef(mockViewToProjectionMatrix));
    EXPECT_CALL(mockViewToProjectionMatrix, transformY(GetParam().expectedInaccuracyToTransform)).WillOnce(testing::Return(GetParam().transformedInaccuracy));
    EXPECT_CALL(mockHookContext, localPlayerBulletInaccuracy()).WillOnce(testing::Return(GetParam().localPlayerBulletInaccuracy));
    EXPECT_CALL(mockHookContext, makeCrosshair()).WillOnce(testing::ReturnRef(mockCrosshair));
    EXPECT_CALL(mockCrosshair, getColor()).WillOnce(testing::Return(cs2::Color{255, 255, 255}));
    EXPECT_CALL(mockPanel, setVisible(true));
    EXPECT_CALL(mockPanel, setHeight(GetParam().expectedHeight));
    EXPECT_CALL(mockPanel, setBorder(no_scope_inaccuracy_vis_params::kBorderWidth, testing::_));
    EXPECT_CALL(mockPanel, setBackgroundColor(testing::_));
    noScopeInaccuracyVis.update();
}

INSTANTIATE_TEST_SUITE_P(, NoScopeInaccuracyVisHeightTest, testing::ValuesIn(
    std::to_array<NoScopeInaccuracyVisHeightTestParam>({
        {
            .localPlayerBulletInaccuracy{std::nullopt},
            .expectedInaccuracyToTransform = 0.0f,
            .transformedInaccuracy{std::nullopt},
            .expectedHeight{cs2::CUILength::percent(0)}
        },
        {
            .localPlayerBulletInaccuracy{0.1f},
            .expectedInaccuracyToTransform = 0.1f,
            .transformedInaccuracy{std::nullopt},
            .expectedHeight{cs2::CUILength::percent(0)}
        },
        {
            .localPlayerBulletInaccuracy{0.1f},
            .expectedInaccuracyToTransform = 0.1f,
            .transformedInaccuracy{0.133f},
            .expectedHeight{cs2::CUILength::percent(13.3f)}
        },
        {
            .localPlayerBulletInaccuracy{1.0f},
            .expectedInaccuracyToTransform = 1.0f,
            .transformedInaccuracy{1.33f},
            .expectedHeight{cs2::CUILength::percent(133.0f)}
        }
    })
));
