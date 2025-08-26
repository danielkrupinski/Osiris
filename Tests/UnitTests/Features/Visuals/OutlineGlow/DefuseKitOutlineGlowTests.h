#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Mocks/MockBaseEntity.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>

#include <Features/Visuals/OutlineGlow/DefuseKitOutlineGlow/DefuseKitOutlineGlow.h>

class DefuseKitOutlineGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockBaseEntity> mockBaseEntity;

    DefuseKitOutlineGlow<MockHookContext> defuseKitOutlineGlow{mockHookContext};
};

TEST_F(DefuseKitOutlineGlowTest, GlowShouldNotBeAppliedWhenNotEnabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<outline_glow_vars::GlowDefuseKits>())).WillOnce(testing::Return(false));
    EXPECT_FALSE(defuseKitOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockBaseEntity));
}

TEST_F(DefuseKitOutlineGlowTest, GlowShouldBeAppliedWhenEnabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<outline_glow_vars::GlowDefuseKits>())).WillOnce(testing::Return(true));
    EXPECT_TRUE(defuseKitOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockBaseEntity));
}

TEST_F(DefuseKitOutlineGlowTest, CorrectGlowRangeIsReturned) {
    EXPECT_EQ(defuseKitOutlineGlow.getGlowRange(), outline_glow_params::kDefuseKitGlowRange);
}

struct DefuseKitOutlineGlowHueTestParam {
    color::HueInteger::UnderlyingType configuredHue{};
    color::Hue::ValueType expectedHue{};
};

class DefuseKitOutlineGlowHueTest
    : public DefuseKitOutlineGlowTest,
      public testing::WithParamInterface<DefuseKitOutlineGlowHueTestParam> {
};

TEST_P(DefuseKitOutlineGlowHueTest, CorrectGlowHueIsReturned) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<outline_glow_vars::DefuseKitHue>()))
        .WillOnce(testing::Return(outline_glow_vars::DefuseKitHue::ValueType{color::HueInteger{GetParam().configuredHue}}));

    const auto hue = defuseKitOutlineGlow.getGlowHue(EntityTypeInfo{}, mockBaseEntity);
    ASSERT_TRUE(hue.hasValue());
    EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue);
}

static_assert(outline_glow_vars::DefuseKitHue::ValueType::kMin == 0, "Update the test below");
INSTANTIATE_TEST_SUITE_P(MinConfigVar, DefuseKitOutlineGlowHueTest,
                         testing::Values(DefuseKitOutlineGlowHueTestParam{.configuredHue = 0, .expectedHue = 0.0f}));

static_assert(outline_glow_vars::DefuseKitHue::ValueType::kMax == 359, "Update the test below");
INSTANTIATE_TEST_SUITE_P(MaxConfigVar, DefuseKitOutlineGlowHueTest,
                         testing::Values(DefuseKitOutlineGlowHueTestParam{.configuredHue = 359, .expectedHue = 0.99722222f}));

static_assert(outline_glow_vars::DefuseKitHue::kDefaultValue == color::HueInteger{184}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVar, DefuseKitOutlineGlowHueTest,
                         testing::Values(DefuseKitOutlineGlowHueTestParam{.configuredHue = 184, .expectedHue = 0.51111111f}));

INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVar, DefuseKitOutlineGlowHueTest,
                         testing::Values(DefuseKitOutlineGlowHueTestParam{.configuredHue = 222, .expectedHue = 0.61666666f}));
