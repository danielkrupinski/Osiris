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
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<DefuseKitOutlineGlowEnabled>())).WillOnce(testing::Return(false));
    EXPECT_FALSE(defuseKitOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockBaseEntity));
}

TEST_F(DefuseKitOutlineGlowTest, GlowShouldBeAppliedWhenEnabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<DefuseKitOutlineGlowEnabled>())).WillOnce(testing::Return(true));
    EXPECT_TRUE(defuseKitOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockBaseEntity));
}

TEST_F(DefuseKitOutlineGlowTest, CorrectGlowColorIsReturned) {
    EXPECT_EQ(defuseKitOutlineGlow.getGlowColor(EntityTypeInfo{}, mockBaseEntity), outline_glow_params::kDefuseKitColor);
}

TEST_F(DefuseKitOutlineGlowTest, CorrectGlowRangeIsReturned) {
    EXPECT_EQ(defuseKitOutlineGlow.getGlowRange(), outline_glow_params::kDefuseKitGlowRange);
}
