#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/BombTimer/BombTimer.h>
#include <Mocks/BombTimerMocks/MockBombTimerPanel.h>
#include <Mocks/BombTimerMocks/MockBombTimerPanelFactory.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockPanel.h>
#include <Mocks/HudMocks/MockHud.h>
#include <Mocks/MockPlantedC4.h>
#include <Mocks/MockPanoramaUiEngine.h>
#include <Mocks/MockClientPanel.h>
#include <Mocks/MockLabelPanel.h>
#include <Mocks/MockImagePanel.h>

class BombTimerTest : public testing::Test {
protected:
    BombTimerTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
    }

    void bombTimerEnabled(bool b)
    {
        mockConfig.expectGetVariable<BombTimerEnabled>(b);
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockHud> mockHud;
    testing::StrictMock<MockBombTimerPanel> mockBombTimerPanel;

    BombTimer<MockHookContext> bombTimer{mockHookContext};
};

TEST_F(BombTimerTest, DoesNotRunIfShouldNotRun) {
    bombTimerEnabled(false);
    EXPECT_EQ(bombTimer.update(), Visibility::Hidden);
}

TEST_F(BombTimerTest, ForceHidesPanelIfShouldRun) {
    bombTimerEnabled(true);

    EXPECT_CALL(mockHookContext, makeBombTimerPanel()).WillOnce(testing::ReturnRef(mockBombTimerPanel));
    EXPECT_CALL(mockBombTimerPanel, hide());
    
    bombTimer.forceHide();
}

TEST_F(BombTimerTest, DoesNotForceHidePanelIfShouldNotRun) {
    bombTimerEnabled(false);
    bombTimer.forceHide();
}

TEST_F(BombTimerTest, OnDisableHidesBombTimerPanel) {
    EXPECT_CALL(mockHookContext, makeBombTimerPanel()).WillOnce(testing::ReturnRef(mockBombTimerPanel));
    EXPECT_CALL(mockBombTimerPanel, hide());
    bombTimer.onDisable();
}

struct BombTimerUpdateTestParam {
    Optional<bool> bombPlantedPanelVisible{};
    bool hasPlantedC4{};
    Optional<bool> isBombTicking{};
    Optional<float> timeToExplosion{};
    Visibility expectedBombTimerVisibility{};
};

class BombTimerUpdateTestWithParam : public BombTimerTest, public testing::WithParamInterface<BombTimerUpdateTestParam> {
protected:
    testing::StrictMock<MockPanel> mockBombPlantedPanel;
};

TEST_P(BombTimerUpdateTestWithParam, Update) {
    bombTimerEnabled(true);
    testing::StrictMock<MockPlantedC4> mockPlantedC4;

    EXPECT_CALL(mockHookContext, hud()).Times(testing::AtMost(1)).WillRepeatedly(testing::ReturnRef(mockHud));
    EXPECT_CALL(mockHud, bombPlantedPanel()).Times(testing::AtMost(1)).WillRepeatedly(testing::ReturnRef(mockBombPlantedPanel));
    EXPECT_CALL(mockBombPlantedPanel, isVisible()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(GetParam().bombPlantedPanelVisible));
    EXPECT_CALL(mockHookContext, makeBombTimerPanel()).WillOnce(testing::ReturnRef(mockBombTimerPanel));
    EXPECT_CALL(mockHookContext, plantedC4()).WillOnce(testing::ReturnRef(mockPlantedC4));

    EXPECT_CALL(mockPlantedC4, operatorBool()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(GetParam().hasPlantedC4));
    EXPECT_CALL(mockPlantedC4, isTicking()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(GetParam().isBombTicking));
    EXPECT_CALL(mockPlantedC4, getTimeToExplosion()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(GetParam().timeToExplosion));

    if (GetParam().expectedBombTimerVisibility == Visibility::Visible)
        EXPECT_CALL(mockBombTimerPanel, showAndUpdate(testing::Ref(mockPlantedC4)));

    if (GetParam().expectedBombTimerVisibility == Visibility::Hidden)
        EXPECT_CALL(mockBombTimerPanel, hide());
    
    EXPECT_EQ(bombTimer.update(), GetParam().expectedBombTimerVisibility);
}

INSTANTIATE_TEST_SUITE_P(, BombTimerUpdateTestWithParam, testing::ValuesIn(
    std::to_array<BombTimerUpdateTestParam>({
        {
            .bombPlantedPanelVisible = true,
            .hasPlantedC4 = true,
            .isBombTicking = true,
            .timeToExplosion = 5.5f,
            .expectedBombTimerVisibility = Visibility::Visible
        },

        {
            .bombPlantedPanelVisible = std::nullopt,
            .hasPlantedC4 = true,
            .isBombTicking = true,
            .timeToExplosion = 5.5f,
            .expectedBombTimerVisibility = Visibility::Visible
        },
        {
            .bombPlantedPanelVisible = true,
            .hasPlantedC4 = true,
            .isBombTicking = true,
            .timeToExplosion = 5.5f,
            .expectedBombTimerVisibility = Visibility::Visible
        },
        {
            .bombPlantedPanelVisible = false,
            .hasPlantedC4 = true,
            .isBombTicking = true,
            .timeToExplosion = 5.5f,
            .expectedBombTimerVisibility = Visibility::Hidden
        },

        {
            .bombPlantedPanelVisible = true,
            .hasPlantedC4 = false,
            .isBombTicking = true,
            .timeToExplosion = 5.5f,
            .expectedBombTimerVisibility = Visibility::Hidden
        },

        {
            .bombPlantedPanelVisible = true,
            .hasPlantedC4 = true,
            .isBombTicking = false,
            .timeToExplosion = 5.5f,
            .expectedBombTimerVisibility = Visibility::Hidden
        },
        {
            .bombPlantedPanelVisible = true,
            .hasPlantedC4 = true,
            .isBombTicking = std::nullopt,
            .timeToExplosion = 5.5f,
            .expectedBombTimerVisibility = Visibility::Visible
        },

        {
            .bombPlantedPanelVisible = true,
            .hasPlantedC4 = true,
            .isBombTicking = true,
            .timeToExplosion = 0.0f,
            .expectedBombTimerVisibility = Visibility::Hidden
        },
        {
            .bombPlantedPanelVisible = true,
            .hasPlantedC4 = true,
            .isBombTicking = true,
            .timeToExplosion = -0.1f,
            .expectedBombTimerVisibility = Visibility::Hidden
        },
        {
            .bombPlantedPanelVisible = true,
            .hasPlantedC4 = true,
            .isBombTicking = true,
            .timeToExplosion = std::nullopt,
            .expectedBombTimerVisibility = Visibility::Hidden
        }
    })
));
