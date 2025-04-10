#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Mocks/MockBaseEntity.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>

#include <Features/Visuals/OutlineGlow/DefuseKitOutlineGlow/DefuseKitOutlineGlow.h>

class DefuseKitOutlineGlowTest : public testing::Test {
protected:
    DefuseKitOutlineGlowTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockBaseEntity> mockBaseEntity;

    DefuseKitOutlineGlow<MockHookContext> defuseKitOutlineGlow{mockHookContext};
};

TEST_F(DefuseKitOutlineGlowTest, GlowIsNotAppliedWhenNotEnabled) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<DefuseKitOutlineGlowEnabled>())).WillOnce(testing::Return(false));
    defuseKitOutlineGlow.applyGlowToDefuseKit(mockBaseEntity);
}

TEST_F(DefuseKitOutlineGlowTest, GlowIsAppliedWhenEnabled) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<DefuseKitOutlineGlowEnabled>())).WillOnce(testing::Return(true));

    using namespace outline_glow_params;
    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(kDefuseKitColor, kDefuseKitGlowRange));

    defuseKitOutlineGlow.applyGlowToDefuseKit(mockBaseEntity);
}
