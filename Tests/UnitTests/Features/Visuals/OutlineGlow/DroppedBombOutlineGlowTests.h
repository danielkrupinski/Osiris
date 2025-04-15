#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>

#include <Features/Visuals/OutlineGlow/DroppedBombOutlineGlow/DroppedBombOutlineGlow.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Utils/Optional.h>

class DroppedBombOutlineGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockBaseEntity> mockBaseEntity;

    DroppedBombOutlineGlow<MockHookContext> droppedBombOutlineGlow{mockHookContext};
};

TEST_F(DroppedBombOutlineGlowTest, CorrectGlowColorIsReturned) {
    EXPECT_EQ(droppedBombOutlineGlow.getGlowColor(EntityTypeInfo{}, mockBaseEntity), outline_glow_params::kDroppedBombColor);
}

struct DroppedBombOutlineGlowConditionTestParam {
    bool enabled{};
    bool expectBombHasOwnerCalled{};
    Optional<bool> bombHasOwner{};
    bool expectGlowApplied{};
};

class DroppedBombOutlineGlowConditionTest : public DroppedBombOutlineGlowTest, public testing::WithParamInterface<DroppedBombOutlineGlowConditionTestParam> {
};

TEST_P(DroppedBombOutlineGlowConditionTest, ShouldApplyGlowWhenExpected) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<DroppedBombOutlineGlowEnabled>())).WillOnce(testing::Return(GetParam().enabled));
    if (GetParam().expectBombHasOwnerCalled)
        EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(GetParam().bombHasOwner));
    EXPECT_EQ(droppedBombOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockBaseEntity), GetParam().expectGlowApplied);
}

INSTANTIATE_TEST_SUITE_P(, DroppedBombOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<DroppedBombOutlineGlowConditionTestParam>({
        {.enabled = false, .expectBombHasOwnerCalled = false, .expectGlowApplied = false},
        {.enabled = true, .expectBombHasOwnerCalled = true, .bombHasOwner{std::nullopt}, .expectGlowApplied = false},
        {.enabled = true, .expectBombHasOwnerCalled = true, .bombHasOwner{true}, .expectGlowApplied = false},
        {.enabled = true, .expectBombHasOwnerCalled = true, .bombHasOwner{false}, .expectGlowApplied = true}
    })
));
