#pragma once

#include <gtest/gtest.h>

#include <Features/Visuals/ModelGlow/TickingBombModelGlow/TickingBombModelGlow.h>
#include <Features/FeaturesStates.h>

class TickingBombModelGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockConfig> mockConfig{};
    testing::StrictMock<MockHookContext> mockHookContext{};
    FeaturesStates featuresStates{};
    TickingBombModelGlow<MockHookContext> tickingBombModelGlow{mockHookContext};
};

TEST_F(TickingBombModelGlowTest, Disabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<model_glow_vars::GlowTickingBomb>(false);
    EXPECT_FALSE(tickingBombModelGlow.enabled());
}

TEST_F(TickingBombModelGlowTest, Enabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<model_glow_vars::GlowTickingBomb>(true);
    EXPECT_TRUE(tickingBombModelGlow.enabled());
}

TEST_F(TickingBombModelGlowTest, CorrectDisablingFlagIsReturned) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_THAT(tickingBombModelGlow.disablingFlag(), testing::Ref(featuresStates.visualFeaturesStates.modelGlowState.tickingBombModelGlowDisabling));
}

class TickingBombModelGlowHueTest
    : public TickingBombModelGlowTest,
      public testing::WithParamInterface<color::HueInteger> {
};

TEST_P(TickingBombModelGlowHueTest, CorrectHueIsReturned) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<model_glow_vars::TickingBombHue>(model_glow_vars::TickingBombHue::ValueType{GetParam()});
    EXPECT_EQ(tickingBombModelGlow.hue(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(, TickingBombModelGlowHueTest, testing::Values(
    model_glow_vars::TickingBombHue::ValueType::kMin,
    model_glow_vars::TickingBombHue::ValueType::kMax,
    model_glow_vars::TickingBombHue::kDefaultValue
));

struct TickingBombModelGlowShouldApplyTestParam {
    Optional<bool> isTicking{};
    bool shouldApplyGlow{};
};

class TickingBombModelGlowShouldApplyTest
    : public TickingBombModelGlowTest,
      public testing::WithParamInterface<TickingBombModelGlowShouldApplyTestParam> {
};

TEST_P(TickingBombModelGlowShouldApplyTest, ShouldApplyGlowIfBombIsTicking) {
    testing::StrictMock<MockPlantedC4> mockPlantedC4;
    EXPECT_CALL(mockPlantedC4, isTicking()).WillOnce(testing::Return(GetParam().isTicking));
    EXPECT_EQ(tickingBombModelGlow.shouldApplyGlow(mockPlantedC4), GetParam().shouldApplyGlow);
}

INSTANTIATE_TEST_SUITE_P(, TickingBombModelGlowShouldApplyTest, testing::ValuesIn(
    std::to_array<TickingBombModelGlowShouldApplyTestParam>({
        {.isTicking{std::nullopt}, .shouldApplyGlow = true},
        {.isTicking{true}, .shouldApplyGlow = true},
        {.isTicking{false}, .shouldApplyGlow = false}
    })
));
