#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/BombTimer/BombTimer.h>
#include <Mocks/BombTimerMocks/MockBombTimerContext.h>
#include <Mocks/BombTimerMocks/MockBombTimerPanel.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockPanel.h>

class BombTimerTest : public testing::Test {
protected:
    BombTimerTest()
    {
        EXPECT_CALL(mockBombTimerContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
    }

    void bombTimerEnabled(bool b)
    {
        mockConfig.expectGetVariable<BombTimerEnabled>(b);
    }

    testing::StrictMock<MockBombTimerContext> mockBombTimerContext;
    testing::StrictMock<MockBombTimerPanel> mockBombTimerPanel;
    testing::StrictMock<MockConfig> mockConfig;

    BombTimer<MockHookContext, MockBombTimerContext&> bombTimer{mockBombTimerContext};
};

TEST_F(BombTimerTest, DoesNotRunIfShouldNotRun) {
    bombTimerEnabled(false);
    EXPECT_EQ(bombTimer.update(), Visibility::Hidden);
}

TEST_F(BombTimerTest, ForceHidesPanelIfShouldRun) {
    bombTimerEnabled(true);

    EXPECT_CALL(mockBombTimerContext, bombTimerPanel()).WillOnce(testing::ReturnRef(mockBombTimerPanel));
    EXPECT_CALL(mockBombTimerPanel, hide());
    
    bombTimer.forceHide();
}

TEST_F(BombTimerTest, DoesNotForceHidePanelIfShouldNotRun) {
    bombTimerEnabled(false);
    bombTimer.forceHide();
}

TEST_F(BombTimerTest, OnDisableHidesBombTimerPanel) {
    EXPECT_CALL(mockBombTimerContext, bombTimerPanel()).WillOnce(testing::ReturnRef(mockBombTimerPanel));
    EXPECT_CALL(mockBombTimerPanel, hide());
    bombTimer.onDisable();
}

struct BombTimerUpdateTestParam {
    Optional<bool> bombPlantedPanelVisible{};
    bool hasTickingC4{};
    Visibility expectedBombTimerVisibility{};
};

class BombTimerUpdateTestWithParam : public BombTimerTest, public testing::WithParamInterface<BombTimerUpdateTestParam> {
protected:
    testing::StrictMock<MockPanel> mockBombPlantedPanel;
};

TEST_P(BombTimerUpdateTestWithParam, Update) {
    const auto& p = GetParam();
    bombTimerEnabled(true);

    EXPECT_CALL(mockBombTimerContext, bombPlantedPanel()).Times(testing::AtMost(1)).WillRepeatedly(testing::ReturnRef(mockBombPlantedPanel));
    EXPECT_CALL(mockBombPlantedPanel, isVisible()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(p.bombPlantedPanelVisible));
    EXPECT_CALL(mockBombTimerContext, hasTickingC4()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(p.hasTickingC4));

    EXPECT_CALL(mockBombTimerContext, bombTimerPanel()).WillOnce(testing::ReturnRef(mockBombTimerPanel));

    if (p.expectedBombTimerVisibility == Visibility::Visible)
        EXPECT_CALL(mockBombTimerPanel, showAndUpdate());

    if (p.expectedBombTimerVisibility == Visibility::Hidden)
        EXPECT_CALL(mockBombTimerPanel, hide());
    
    EXPECT_EQ(bombTimer.update(), p.expectedBombTimerVisibility);
}

INSTANTIATE_TEST_SUITE_P(, BombTimerUpdateTestWithParam, testing::ValuesIn(
    std::to_array<BombTimerUpdateTestParam>({
        {.bombPlantedPanelVisible = std::nullopt, .hasTickingC4 = true, .expectedBombTimerVisibility = Visibility::Visible},
        {.bombPlantedPanelVisible = true, .hasTickingC4 = true, .expectedBombTimerVisibility = Visibility::Visible},
        {.bombPlantedPanelVisible = false, .hasTickingC4 = true, .expectedBombTimerVisibility = Visibility::Hidden},
        {.bombPlantedPanelVisible = std::nullopt, .hasTickingC4 = false, .expectedBombTimerVisibility = Visibility::Hidden},
        {.bombPlantedPanelVisible = true, .hasTickingC4 = false, .expectedBombTimerVisibility = Visibility::Hidden},
        {.bombPlantedPanelVisible = false, .hasTickingC4 = false, .expectedBombTimerVisibility = Visibility::Hidden}
    })
));
