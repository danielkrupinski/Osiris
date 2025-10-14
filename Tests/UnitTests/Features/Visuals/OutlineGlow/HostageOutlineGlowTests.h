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

TEST_F(HostageOutlineGlowTest, Disabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::GlowHostages>(false);
    EXPECT_FALSE(hostageOutlineGlow.enabled());
}

TEST_F(HostageOutlineGlowTest, Enabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::GlowHostages>(true);
    EXPECT_TRUE(hostageOutlineGlow.enabled());
}

TEST_F(HostageOutlineGlowTest, CorrectGlowHueIsReturned) {
    EXPECT_EQ(hostageOutlineGlow.hue(), outline_glow_params::kHostageHue);
}
