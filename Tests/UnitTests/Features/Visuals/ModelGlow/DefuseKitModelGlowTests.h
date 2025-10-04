#pragma once

#include <gtest/gtest.h>

#include <Features/Visuals/ModelGlow/DefuseKitModelGlow/DefuseKitModelGlow.h>
#include <Features/FeaturesStates.h>

class DefuseKitModelGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockConfig> mockConfig{};
    testing::StrictMock<MockHookContext> mockHookContext{};
    FeaturesStates featuresStates{};
    DefuseKitModelGlow<MockHookContext> defuseKitModelGlow{mockHookContext};
};

TEST_F(DefuseKitModelGlowTest, Disabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<model_glow_vars::GlowDefuseKits>(false);
    EXPECT_FALSE(defuseKitModelGlow.enabled());
}

TEST_F(DefuseKitModelGlowTest, Enabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<model_glow_vars::GlowDefuseKits>(true);
    EXPECT_TRUE(defuseKitModelGlow.enabled());
}

TEST_F(DefuseKitModelGlowTest, CorrectDisablingFlagIsReturned) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_THAT(defuseKitModelGlow.disablingFlag(), testing::Ref(featuresStates.visualFeaturesStates.modelGlowState.defuseKitModelGlowDisabling));
}

class DefuseKitModelGlowHueTest
    : public DefuseKitModelGlowTest,
      public testing::WithParamInterface<color::HueInteger> {
};

TEST_P(DefuseKitModelGlowHueTest, CorrectHueIsReturned) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<model_glow_vars::DefuseKitHue>(model_glow_vars::DefuseKitHue::ValueType{GetParam()});
    EXPECT_EQ(defuseKitModelGlow.hue(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(, DefuseKitModelGlowHueTest, testing::Values(
    model_glow_vars::DefuseKitHue::ValueType::kMin,
    model_glow_vars::DefuseKitHue::ValueType::kMax,
    model_glow_vars::DefuseKitHue::kDefaultValue
));
