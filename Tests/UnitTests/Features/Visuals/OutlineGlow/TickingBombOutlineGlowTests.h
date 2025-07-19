#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Visuals/OutlineGlow/TickingBombOutlineGlow/TickingBombOutlineGlow.h>
#include <Mocks/MockBaseEntity.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockPlantedC4.h>
#include <Utils/Optional.h>

class TickingBombOutlineGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockPlantedC4> mockPlantedC4;

    TickingBombOutlineGlow<MockHookContext> tickingBombOutlineGlow{mockHookContext};
};

TEST_F(TickingBombOutlineGlowTest, CorrectGlowColorIsReturned) {
    EXPECT_EQ(tickingBombOutlineGlow.getGlowHue(EntityTypeInfo{}, mockPlantedC4), outline_glow_params::kTickingBombHue.toHueFloat());
}

struct TickingBombOutlineGlowConditionTestParam {
    bool enabled{};
    Optional<bool> isTicking{};
    bool expectPlantedC4Access{};
    bool expectGlowApplied{};
};

class TickingBombOutlineGlowConditionTest : public TickingBombOutlineGlowTest, public testing::WithParamInterface<TickingBombOutlineGlowConditionTestParam> {
protected:
    testing::StrictMock<MockConfig> mockConfig;
};

TEST_P(TickingBombOutlineGlowConditionTest, ShouldApplyGlowWhenExpected) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<TickingBombOutlineGlowEnabled>())).WillOnce(testing::Return(GetParam().enabled));
    if (GetParam().expectPlantedC4Access)
        EXPECT_CALL(mockPlantedC4, isTicking()).WillOnce(testing::Return(GetParam().isTicking));

    EXPECT_EQ(tickingBombOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockPlantedC4), GetParam().expectGlowApplied);
}

INSTANTIATE_TEST_SUITE_P(, TickingBombOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<TickingBombOutlineGlowConditionTestParam>({
        {.enabled = false, .expectPlantedC4Access = false, .expectGlowApplied = false},
        {.enabled = true, .isTicking{true}, .expectPlantedC4Access = true, .expectGlowApplied = true},
        {.enabled = true, .isTicking{false}, .expectPlantedC4Access = true, .expectGlowApplied = false},
        {.enabled = true, .isTicking{std::nullopt}, .expectPlantedC4Access = true, .expectGlowApplied = true}
    })
));
