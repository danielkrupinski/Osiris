#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Mocks/MockBaseEntity.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>

#include <Features/Visuals/OutlineGlow/HostageOutlineGlow/HostageOutlineGlow.h>

class HostageOutlineGlowTest : public testing::Test {
protected:
    HostageOutlineGlowTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockBaseEntity> mockBaseEntity;

    HostageOutlineGlow<MockHookContext> hostageOutlineGlow{mockHookContext};
};

TEST_F(HostageOutlineGlowTest, GlowIsNotAppliedWhenNotEnabled) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<HostageOutlineGlowEnabled>())).WillOnce(testing::Return(false));
    hostageOutlineGlow.applyGlowToHostage(mockBaseEntity);
}

TEST_F(HostageOutlineGlowTest, GlowIsAppliedWhenEnabled) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<HostageOutlineGlowEnabled>())).WillOnce(testing::Return(true));
    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(outline_glow_params::kHostageColor));
    hostageOutlineGlow.applyGlowToHostage(mockBaseEntity);
}
