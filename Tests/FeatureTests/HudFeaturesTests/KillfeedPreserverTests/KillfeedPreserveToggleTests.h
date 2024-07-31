#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/KillfeedPreserver/KillfeedPreserveToggle.h>
#include <Features/Hud/KillfeedPreserver/KillfeedPreserverState.h>
#include <Mocks/KillfeedPreserverMocks/MockKillfeedPreserverContext.h>

class KillfeedPreserveToggleTest : public testing::Test {
protected:
    KillfeedPreserveToggleTest()
    {
        EXPECT_CALL(mockContext, state()).WillOnce(testing::ReturnRef(state));
    }

    testing::StrictMock<MockKillfeedPreserverContext> mockContext;
    KillfeedPreserveToggle<MockKillfeedPreserverContext&> killfeedPreserveToggle{mockContext};
    KillfeedPreserverState state;
};

TEST_F(KillfeedPreserveToggleTest, EnablingSetsEnabledVariableToTrue) {
    state.enabled = false;
    killfeedPreserveToggle.enable();
    EXPECT_EQ(state.enabled, true);
}

TEST_F(KillfeedPreserveToggleTest, EnablingDoesNothingIfAlreadyEnabled) {
    state.enabled = true;
    killfeedPreserveToggle.enable();
    EXPECT_EQ(state.enabled, true);
}

TEST_F(KillfeedPreserveToggleTest, DisablingDoesNothingIfAlreadyDisabled) {
    state.enabled = false;
    killfeedPreserveToggle.disable();
    EXPECT_EQ(state.enabled, false);
}

TEST_F(KillfeedPreserveToggleTest, DisablingSetsEnabledVariableToFalse) {
    state.enabled = true;
    killfeedPreserveToggle.disable();
    EXPECT_EQ(state.enabled, false);
}
