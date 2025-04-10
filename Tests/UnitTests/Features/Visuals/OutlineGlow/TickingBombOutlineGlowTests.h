#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Visuals/OutlineGlow/TickingBombOutlineGlow/TickingBombOutlineGlow.h>
#include <Mocks/MockBaseEntity.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockPlantedC4.h>
#include <Utils/Optional.h>

struct TickingBombOutlineGlowTestParam {
    bool enabled{};
    Optional<bool> isTicking{};
    bool expectPlantedC4Access{};
    bool expectGlowApplied{};
};

class TickingBombOutlineGlowTest : public testing::TestWithParam<TickingBombOutlineGlowTestParam> {
protected:
    TickingBombOutlineGlowTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockPlantedC4> mockPlantedC4;
    testing::StrictMock<MockBaseEntity> mockBaseEntity;

    TickingBombOutlineGlow<MockHookContext> tickingBombOutlineGlow{mockHookContext};
};

TEST_P(TickingBombOutlineGlowTest, GlowIsAppliedAsExpected) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<TickingBombOutlineGlowEnabled>())).WillOnce(testing::Return(GetParam().enabled));
    if (GetParam().expectPlantedC4Access) {
        EXPECT_CALL(mockPlantedC4, isTicking()).WillOnce(testing::Return(GetParam().isTicking));
        EXPECT_CALL(mockPlantedC4, baseEntity()).WillRepeatedly(testing::ReturnRef(mockBaseEntity));
    }
    if (GetParam().expectGlowApplied)
        EXPECT_CALL(mockBaseEntity, applyGlowRecursively(outline_glow_params::kTickingBombColor));
    tickingBombOutlineGlow.applyGlowToPlantedBomb(mockPlantedC4);
}

INSTANTIATE_TEST_SUITE_P(, TickingBombOutlineGlowTest, testing::ValuesIn(
    std::to_array<TickingBombOutlineGlowTestParam>({
        {.enabled = false, .expectPlantedC4Access = false, .expectGlowApplied = false},
        {.enabled = true, .isTicking{true}, .expectPlantedC4Access = true, .expectGlowApplied = true},
        {.enabled = true, .isTicking{false}, .expectPlantedC4Access = true, .expectGlowApplied = false},
        {.enabled = true, .isTicking{std::nullopt}, .expectPlantedC4Access = true, .expectGlowApplied = true}
    })
));
