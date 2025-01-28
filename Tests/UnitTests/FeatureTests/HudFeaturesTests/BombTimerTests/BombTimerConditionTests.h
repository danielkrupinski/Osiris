#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/BombTimer/BombTimerCondition.h>
#include <Features/Hud/BombTimer/BombTimerState.h>
#include <Mocks/BombTimerMocks/MockBombTimerContext.h>
#include <Mocks/MockPanel.h>
#include <Utils/Optional.h>

class BombTimerConditionTest : public testing::Test {
protected:
    testing::StrictMock<MockBombTimerContext> mockBombTimerContext;
    BombTimerCondition<MockBombTimerContext&> bombTimerCondition{mockBombTimerContext};
};

TEST_F(BombTimerConditionTest, ShouldRunIfEnabled) {
    BombTimerState bombTimerState;
    bombTimerState.enabled = true;
    EXPECT_CALL(mockBombTimerContext, state()).WillOnce(testing::ReturnRef(bombTimerState));

    EXPECT_EQ(bombTimerCondition.shouldRun(), true);
}

TEST_F(BombTimerConditionTest, ShouldNotRunIfNotEnabled) {
    BombTimerState bombTimerState;
    bombTimerState.enabled = false;
    EXPECT_CALL(mockBombTimerContext, state()).WillOnce(testing::ReturnRef(bombTimerState));

    EXPECT_EQ(bombTimerCondition.shouldRun(), false);
}

struct BombTimerConditionTestParam {
    Optional<bool> bombPlantedPanelVisible{};
    bool hasTickingC4{};
    bool expectedShouldShow{};
};

class BombTimerConditionTestWithParam : public BombTimerConditionTest, public testing::WithParamInterface<BombTimerConditionTestParam> {
protected:
    testing::StrictMock<MockPanel> mockBombPlantedPanel;
};

TEST_P(BombTimerConditionTestWithParam, ShouldShowBombTimer) {
    EXPECT_CALL(mockBombTimerContext, bombPlantedPanel()).Times(testing::AtMost(1)).WillRepeatedly(testing::ReturnRef(mockBombPlantedPanel));

    EXPECT_CALL(mockBombTimerContext, hasTickingC4()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(GetParam().hasTickingC4));
    EXPECT_CALL(mockBombPlantedPanel, isVisible()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(GetParam().bombPlantedPanelVisible));

    EXPECT_EQ(bombTimerCondition.shouldShowBombTimer(), GetParam().expectedShouldShow);
}

INSTANTIATE_TEST_SUITE_P(, BombTimerConditionTestWithParam, testing::Values(
    BombTimerConditionTestParam{.bombPlantedPanelVisible = std::nullopt, .hasTickingC4 = true, .expectedShouldShow = true},
    BombTimerConditionTestParam{.bombPlantedPanelVisible = true, .hasTickingC4 = true, .expectedShouldShow = true},
    BombTimerConditionTestParam{.bombPlantedPanelVisible = false, .hasTickingC4 = true, .expectedShouldShow = false},
    BombTimerConditionTestParam{.bombPlantedPanelVisible = std::nullopt, .hasTickingC4 = false, .expectedShouldShow = false},
    BombTimerConditionTestParam{.bombPlantedPanelVisible = true, .hasTickingC4 = false, .expectedShouldShow = false},
    BombTimerConditionTestParam{.bombPlantedPanelVisible = false, .hasTickingC4 = false, .expectedShouldShow = false}
));
