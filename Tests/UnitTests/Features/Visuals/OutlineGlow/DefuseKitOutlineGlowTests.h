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

TEST_F(DefuseKitOutlineGlowTest, Disabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::GlowDefuseKits>(false);
    EXPECT_FALSE(defuseKitOutlineGlow.enabled());
}

TEST_F(DefuseKitOutlineGlowTest, Enabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::GlowDefuseKits>(true);
    EXPECT_TRUE(defuseKitOutlineGlow.enabled());
}

TEST_F(DefuseKitOutlineGlowTest, CorrectGlowRangeIsReturned) {
    EXPECT_EQ(defuseKitOutlineGlow.getGlowRange(), outline_glow_params::kDefuseKitGlowRange);
}

class DefuseKitOutlineGlowHueTest
    : public DefuseKitOutlineGlowTest,
      public testing::WithParamInterface<color::HueInteger::UnderlyingType> {
};

TEST_P(DefuseKitOutlineGlowHueTest, CorrectGlowHueIsReturned) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::DefuseKitHue>(outline_glow_vars::DefuseKitHue::ValueType{color::HueInteger{GetParam()}});

    EXPECT_EQ(defuseKitOutlineGlow.hue(), GetParam());
}

static_assert(outline_glow_vars::DefuseKitHue::ValueType::kMin == 0, "Update the test below");
INSTANTIATE_TEST_SUITE_P(MinConfigVar, DefuseKitOutlineGlowHueTest, testing::Values(0));

static_assert(outline_glow_vars::DefuseKitHue::ValueType::kMax == 359, "Update the test below");
INSTANTIATE_TEST_SUITE_P(MaxConfigVar, DefuseKitOutlineGlowHueTest, testing::Values(359));

static_assert(outline_glow_vars::DefuseKitHue::kDefaultValue == color::HueInteger{184}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVar, DefuseKitOutlineGlowHueTest, testing::Values(184));
INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVar, DefuseKitOutlineGlowHueTest, testing::Values(222));
