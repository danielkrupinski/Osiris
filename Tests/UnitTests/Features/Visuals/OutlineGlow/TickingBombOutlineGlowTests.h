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
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockPlantedC4> mockPlantedC4;

    TickingBombOutlineGlow<MockHookContext> tickingBombOutlineGlow{mockHookContext};
};

TEST_F(TickingBombOutlineGlowTest, Disabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::GlowTickingBomb>(false);
    EXPECT_FALSE(tickingBombOutlineGlow.enabled());
}

TEST_F(TickingBombOutlineGlowTest, Enabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::GlowTickingBomb>(true);
    EXPECT_TRUE(tickingBombOutlineGlow.enabled());
}

struct TickingBombOutlineGlowConditionTestParam {
    Optional<bool> isTicking{};
    bool expectGlowApplied{};
};

class TickingBombOutlineGlowConditionTest
    : public TickingBombOutlineGlowTest,
      public testing::WithParamInterface<TickingBombOutlineGlowConditionTestParam> {
};

TEST_P(TickingBombOutlineGlowConditionTest, ShouldApplyGlowWhenExpected) {
    EXPECT_CALL(mockPlantedC4, isTicking()).WillOnce(testing::Return(GetParam().isTicking));
    EXPECT_EQ(tickingBombOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockPlantedC4), GetParam().expectGlowApplied);
}

INSTANTIATE_TEST_SUITE_P(, TickingBombOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<TickingBombOutlineGlowConditionTestParam>({
        {.isTicking{true}, .expectGlowApplied = true},
        {.isTicking{false}, .expectGlowApplied = false},
        {.isTicking{std::nullopt}, .expectGlowApplied = true}
    })
));

class TickingBombOutlineGlowHueTest
    : public TickingBombOutlineGlowTest,
      public testing::WithParamInterface<color::HueInteger> {
};

TEST_P(TickingBombOutlineGlowHueTest, CorrectGlowHueIsReturned) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::TickingBombHue>(outline_glow_vars::TickingBombHue::ValueType{color::HueInteger{GetParam()}});
    EXPECT_EQ(tickingBombOutlineGlow.hue(), GetParam());
}

static_assert(outline_glow_vars::TickingBombHue::ValueType::kMin == 0, "Update the test below");
INSTANTIATE_TEST_SUITE_P(MinConfigVar, TickingBombOutlineGlowHueTest, testing::Values(color::HueInteger{0}));

static_assert(outline_glow_vars::TickingBombHue::ValueType::kMax == 359, "Update the test below");
INSTANTIATE_TEST_SUITE_P(MaxConfigVar, TickingBombOutlineGlowHueTest, testing::Values(color::HueInteger{359}));

static_assert(outline_glow_vars::TickingBombHue::kDefaultValue == color::HueInteger{0}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVar, TickingBombOutlineGlowHueTest, testing::Values(color::HueInteger{0}));
INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVar, TickingBombOutlineGlowHueTest, testing::Values(color::HueInteger{123}));
