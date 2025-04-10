#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>

#include <Features/Visuals/OutlineGlow/DroppedBombOutlineGlow/DroppedBombOutlineGlow.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Utils/Optional.h>

struct DroppedBombOutlineGlowTestParam {
    bool enabled{};
    bool expectBombHasOwnerCalled{};
    Optional<bool> bombHasOwner{};
    bool expectGlowApplied{};
};

class DroppedBombOutlineGlowTest : public testing::TestWithParam<DroppedBombOutlineGlowTestParam> {
protected:
    DroppedBombOutlineGlowTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockBaseEntity> mockBaseEntity;

    DroppedBombOutlineGlow<MockHookContext> droppedBombOutlineGlow{mockHookContext};
};

TEST_P(DroppedBombOutlineGlowTest, GlowIsAppliedAsExpected) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<DroppedBombOutlineGlowEnabled>())).WillOnce(testing::Return(GetParam().enabled));
    if (GetParam().expectBombHasOwnerCalled)
        EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(GetParam().bombHasOwner));
    if (GetParam().expectGlowApplied)
        EXPECT_CALL(mockBaseEntity, applyGlowRecursively(outline_glow_params::kDroppedBombColor));
    droppedBombOutlineGlow.applyGlowToBomb(mockBaseEntity);
}

INSTANTIATE_TEST_SUITE_P(, DroppedBombOutlineGlowTest, testing::Values(
    DroppedBombOutlineGlowTestParam{.enabled = false, .expectBombHasOwnerCalled = false, .expectGlowApplied = false},
    DroppedBombOutlineGlowTestParam{.enabled = true, .expectBombHasOwnerCalled = true, .bombHasOwner{std::nullopt}, .expectGlowApplied = false},
    DroppedBombOutlineGlowTestParam{.enabled = true, .expectBombHasOwnerCalled = true, .bombHasOwner{true}, .expectGlowApplied = false},
    DroppedBombOutlineGlowTestParam{.enabled = true, .expectBombHasOwnerCalled = true, .bombHasOwner{false}, .expectGlowApplied = true}
));
