#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Mocks/MockBaseEntity.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>

#include <Features/Visuals/OutlineGlow/HostageOutlineGlow/HostageOutlineGlow.h>

class HostageOutlineGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockBaseEntity> mockBaseEntity;

    HostageOutlineGlow<MockHookContext> hostageOutlineGlow{mockHookContext};
};

TEST_F(HostageOutlineGlowTest, GlowShouldNotBeAppliedWhenNotEnabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<HostageOutlineGlowEnabled>())).WillOnce(testing::Return(false));
    EXPECT_FALSE(hostageOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockBaseEntity));
}

TEST_F(HostageOutlineGlowTest, GlowShouldBeAppliedWhenEnabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<HostageOutlineGlowEnabled>())).WillOnce(testing::Return(true));
    EXPECT_TRUE(hostageOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockBaseEntity));
}

TEST_F(HostageOutlineGlowTest, CorrectGlowColorIsReturned) {
    EXPECT_EQ(hostageOutlineGlow.getGlowColor(EntityTypeInfo{}, mockBaseEntity), outline_glow_params::kHostageColor);
}
