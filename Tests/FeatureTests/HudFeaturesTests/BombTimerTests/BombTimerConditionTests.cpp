#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Fakes/FakeBombTimerState.h>
#include <Features/Hud/BombTimer/BombTimerCondition.h>
#include <Features/Hud/BombTimer/BombTimerState.h>
#include <Mocks/BombTimerMocks/MockBombTimerContext.h>

class BombTimerConditionTest : public testing::Test {
protected:
    void hasTickingC4(bool b)
    {
        EXPECT_CALL(mockBombTimerContext, hasTickingC4()).WillOnce(testing::Return(b));
    }

    testing::StrictMock<MockBombTimerContext> mockBombTimerContext;
    BombTimerCondition<MockBombTimerContext&> bombTimerCondition{mockBombTimerContext};
};

TEST_F(BombTimerConditionTest, ShouldShowBombTimerIfHasTickingC4) {
    hasTickingC4(true);
    EXPECT_EQ(bombTimerCondition.shouldShowBombTimer(), true);
}

TEST_F(BombTimerConditionTest, ShouldNotShowBombTimerIfDoesNotHaveTickingC4) {
    hasTickingC4(false);
    EXPECT_EQ(bombTimerCondition.shouldShowBombTimer(), false);
}

TEST_F(BombTimerConditionTest, ShouldRunIfEnabled) {
    FakeBombTimerState bombTimerState;
    bombTimerState.enabled = true;
    EXPECT_CALL(mockBombTimerContext, state()).WillOnce(testing::ReturnRef(bombTimerState));

    EXPECT_EQ(bombTimerCondition.shouldRun(), true);
}

TEST_F(BombTimerConditionTest, ShouldNotRunIfNotEnabled) {
    FakeBombTimerState bombTimerState;
    bombTimerState.enabled = false;
    EXPECT_CALL(mockBombTimerContext, state()).WillOnce(testing::ReturnRef(bombTimerState));

    EXPECT_EQ(bombTimerCondition.shouldRun(), false);
}
