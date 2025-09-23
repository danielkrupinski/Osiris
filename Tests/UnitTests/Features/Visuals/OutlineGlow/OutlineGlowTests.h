#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Visuals/OutlineGlow/OutlineGlow.h>
#include <Mocks/MockGlowProperty.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockModelEntity.h>

class OutlineGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockModelEntity> mockModelEntity;
    testing::StrictMock<MockGlowProperty> mockGlowProperty;
    OutlineGlow<MockHookContext> outlineGlow{mockHookContext};
};

TEST_F(OutlineGlowTest, GlowIsNotAppliedWhenDisabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::Enabled>(false);
    outlineGlow.applyGlowToEntity({}, mockModelEntity);
}

TEST_F(OutlineGlowTest, GlowIsNotAppliedWhenEntityHasGlowAppliedByTheGame) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::Enabled>(true);
    EXPECT_CALL(mockModelEntity, glowProperty()).WillOnce(testing::ReturnRef(mockGlowProperty));
    EXPECT_CALL(mockGlowProperty, isGlowing()).WillOnce(testing::Return(true));
    outlineGlow.applyGlowToEntity({}, mockModelEntity);
}

class OutlineGlowEnabledTest : public OutlineGlowTest {
protected:
    OutlineGlowEnabledTest()
    {
        EXPECT_CALL(mockHookContext, config()).Times(testing::AtLeast(1)).WillRepeatedly(testing::ReturnRef(mockConfig));
        mockConfig.expectGetVariable<outline_glow_vars::Enabled>(true);
        EXPECT_CALL(mockModelEntity, glowProperty()).WillOnce(testing::ReturnRef(mockGlowProperty));
        EXPECT_CALL(mockGlowProperty, isGlowing()).WillOnce(testing::Return(false));
    }

    testing::StrictMock<MockBaseEntity> mockBaseEntity;
};

TEST_F(OutlineGlowEnabledTest, GlowIsNotAppliedToPlayerWhenDisabled) {
    testing::StrictMock<MockPlayerPawn> mockPlayerPawn;
    EXPECT_CALL(mockModelEntity, asPlayerPawn()).WillOnce(testing::ReturnRef(mockPlayerPawn));

    mockConfig.expectGetVariable<outline_glow_vars::GlowPlayers>(false);
    outlineGlow.applyGlowToEntity(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_CSPlayerPawn>()}, mockModelEntity);
}

TEST_F(OutlineGlowEnabledTest, GlowIsNotAppliedToDefuseKitWhenDisabled) {
    EXPECT_CALL(mockModelEntity, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));

    mockConfig.expectGetVariable<outline_glow_vars::GlowDefuseKits>(false);
    outlineGlow.applyGlowToEntity(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::CBaseAnimGraph>()}, mockModelEntity);
}

TEST_F(OutlineGlowEnabledTest, GlowIsNotAppliedToTickingBombWhenDisabled) {
    testing::StrictMock<MockPlantedC4> mockPlantedC4;
    EXPECT_CALL(mockModelEntity, asPlantedC4()).WillOnce(testing::ReturnRef(mockPlantedC4));

    mockConfig.expectGetVariable<outline_glow_vars::GlowTickingBomb>(false);
    outlineGlow.applyGlowToEntity(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::CPlantedC4>()}, mockModelEntity);
}

TEST_F(OutlineGlowEnabledTest, GlowIsNotAppliedToDroppedBombWhenDisabled) {
    EXPECT_CALL(mockModelEntity, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));

    mockConfig.expectGetVariable<outline_glow_vars::GlowDroppedBomb>(false);
    outlineGlow.applyGlowToEntity(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_C4>()}, mockModelEntity);
}

TEST_F(OutlineGlowEnabledTest, GlowIsNotAppliedToHostageWhenDisabled) {
    EXPECT_CALL(mockModelEntity, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));

    mockConfig.expectGetVariable<outline_glow_vars::GlowHostages>(false);
    outlineGlow.applyGlowToEntity(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_Hostage>()}, mockModelEntity);
}

TEST_F(OutlineGlowEnabledTest, GlowIsNotAppliedToGrenadeProjectileWhenDisabled) {
    EXPECT_CALL(mockModelEntity, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));

    mockConfig.expectGetVariable<outline_glow_vars::GlowGrenadeProjectiles>(false);
    outlineGlow.applyGlowToEntity(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()}, mockModelEntity);
}

TEST_F(OutlineGlowEnabledTest, GlowIsNotAppliedToWeaponWhenDisabled) {
    EXPECT_CALL(mockModelEntity, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));

    mockConfig.expectGetVariable<outline_glow_vars::GlowWeapons>(false);
    outlineGlow.applyGlowToEntity(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_DEagle>()}, mockModelEntity);
}

class OutlineGlowNonDefaultAlphaTest : public OutlineGlowEnabledTest {
};

TEST_F(OutlineGlowNonDefaultAlphaTest, ImmunePlayerHasCorrectAlpha) {
    testing::StrictMock<MockPlayerPawn> mockPlayerPawn;
 
    EXPECT_CALL(mockModelEntity, asPlayerPawn()).WillOnce(testing::ReturnRef(mockPlayerPawn));
    
    mockConfig.expectGetVariable<outline_glow_vars::GlowPlayers>(true);
    mockConfig.expectGetVariable<outline_glow_vars::GlowOnlyEnemies>(true);
    mockConfig.expectGetVariable<outline_glow_vars::PlayerGlowColorMode>(PlayerOutlineGlowColorType::EnemyAlly);
    mockConfig.expectGetVariable<outline_glow_vars::EnemyHue>(outline_glow_vars::EnemyHue::ValueType{color::HueInteger{120}});

    EXPECT_CALL(mockPlayerPawn, isAlive()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, health()).WillOnce(testing::Return(100));
    EXPECT_CALL(mockPlayerPawn, isControlledByLocalPlayer()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockPlayerPawn, isTTorCT()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, isEnemy()).Times(2).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, hasImmunity()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));

    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{127, 255, 127, outline_glow_params::kImmunePlayerGlowAlpha}, 0));
    outlineGlow.applyGlowToEntity(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_CSPlayerPawn>()}, mockModelEntity);
}

TEST_F(OutlineGlowNonDefaultAlphaTest, ImmunePlayerHasCorrectFallbackColorAlpha) {
    testing::StrictMock<MockPlayerPawn> mockPlayerPawn;

    EXPECT_CALL(mockModelEntity, asPlayerPawn()).WillOnce(testing::ReturnRef(mockPlayerPawn));
    
    mockConfig.expectGetVariable<outline_glow_vars::GlowPlayers>(true);
    mockConfig.expectGetVariable<outline_glow_vars::GlowOnlyEnemies>(true);
    mockConfig.expectGetVariable<outline_glow_vars::PlayerGlowColorMode>(PlayerOutlineGlowColorType::EnemyAlly);
 
    EXPECT_CALL(mockPlayerPawn, isAlive()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, health()).WillOnce(testing::Return(100));
    EXPECT_CALL(mockPlayerPawn, isControlledByLocalPlayer()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockPlayerPawn, isTTorCT()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, isEnemy()).Times(2).WillRepeatedly(testing::Return(std::nullopt));
    EXPECT_CALL(mockPlayerPawn, hasImmunity()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));

    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{191, 191, 191, outline_glow_params::kImmunePlayerGlowAlpha}, 0));
    outlineGlow.applyGlowToEntity(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_CSPlayerPawn>()}, mockModelEntity);
}

class OutlineGlowNonDefaultRangeTest : public OutlineGlowEnabledTest {
};

TEST_F(OutlineGlowNonDefaultRangeTest, NonDefaultGlowRangeIsUsedForDefuseKits) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowDefuseKits>(true);
    mockConfig.expectGetVariable<outline_glow_vars::DefuseKitHue>(outline_glow_vars::DefuseKitHue::ValueType{color::HueInteger{188}});

    EXPECT_CALL(mockModelEntity, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));

    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{127, 238, 255, outline_glow_params::kGlowAlpha}, outline_glow_params::kDefuseKitGlowRange));
    outlineGlow.applyGlowToEntity(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::CBaseAnimGraph>()}, mockModelEntity);
}

TEST_F(OutlineGlowNonDefaultRangeTest, NonDefaultGlowRangeIsUsedForWeapons) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowWeapons>(true);
    mockConfig.expectGetVariable<outline_glow_vars::MolotovHue>(outline_glow_vars::MolotovHue::ValueType{color::HueInteger{50}});

    EXPECT_CALL(mockModelEntity, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));

    EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{255, 233, 127, outline_glow_params::kGlowAlpha}, outline_glow_params::kWeaponGlowRange));
    outlineGlow.applyGlowToEntity(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>()}, mockModelEntity);
}
