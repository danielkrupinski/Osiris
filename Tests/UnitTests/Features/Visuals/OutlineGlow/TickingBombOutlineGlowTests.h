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
    TickingBombOutlineGlowTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    }

    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockPlantedC4> mockPlantedC4;

    TickingBombOutlineGlow<MockHookContext> tickingBombOutlineGlow{mockHookContext};
};

struct TickingBombOutlineGlowConditionTestParam {
    bool enabled{};
    Optional<bool> isTicking{};
    bool expectPlantedC4Access{};
    bool expectGlowApplied{};
};

class TickingBombOutlineGlowConditionTest
    : public TickingBombOutlineGlowTest,
      public testing::WithParamInterface<TickingBombOutlineGlowConditionTestParam> {
};

TEST_P(TickingBombOutlineGlowConditionTest, ShouldApplyGlowWhenExpected) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<outline_glow_vars::GlowTickingBomb>()))
        .WillOnce(testing::Return(GetParam().enabled));
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

struct TickingBombOutlineGlowHueTestParam {
    color::HueInteger::UnderlyingType configuredHue{};
    color::Hue::ValueType expectedHue{};
};

class TickingBombOutlineGlowHueTest
    : public TickingBombOutlineGlowTest,
      public testing::WithParamInterface<TickingBombOutlineGlowHueTestParam> {
};

TEST_P(TickingBombOutlineGlowHueTest, CorrectGlowHueIsReturned) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<outline_glow_vars::TickingBombHue>()))
        .WillOnce(testing::Return(outline_glow_vars::TickingBombHue::ValueType{color::HueInteger{GetParam().configuredHue}}));

    const auto hue = tickingBombOutlineGlow.getGlowHue(EntityTypeInfo{}, mockPlantedC4);
    ASSERT_TRUE(hue.hasValue());
    EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue);
}

static_assert(outline_glow_vars::TickingBombHue::ValueType::kMin == 0, "Update the test below");
INSTANTIATE_TEST_SUITE_P(MinConfigVar, TickingBombOutlineGlowHueTest,
                         testing::Values(TickingBombOutlineGlowHueTestParam{.configuredHue = 0, .expectedHue = 0.0f}));

static_assert(outline_glow_vars::TickingBombHue::ValueType::kMax == 359, "Update the test below");
INSTANTIATE_TEST_SUITE_P(MaxConfigVar, TickingBombOutlineGlowHueTest,
                         testing::Values(TickingBombOutlineGlowHueTestParam{.configuredHue = 359, .expectedHue = 0.99722222f}));

static_assert(outline_glow_vars::TickingBombHue::kDefaultValue == color::HueInteger{0}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVar, TickingBombOutlineGlowHueTest,
                         testing::Values(TickingBombOutlineGlowHueTestParam{.configuredHue = 0, .expectedHue = 0.0f}));

INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVar, TickingBombOutlineGlowHueTest,
                         testing::Values(TickingBombOutlineGlowHueTestParam{.configuredHue = 123, .expectedHue = 0.34166666f}));
