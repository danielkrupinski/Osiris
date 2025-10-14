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
    testing::StrictMock<MockBaseEntity> mockBaseEntity;
    testing::StrictMock<MockModelEntity> mockModelEntity;
    testing::StrictMock<MockGlowProperty> mockGlowProperty;
    testing::StrictMock<MockPlayerPawn> mockPlayerPawn;
    testing::StrictMock<MockPlantedC4> mockPlantedC4;
    OutlineGlow<MockHookContext> outlineGlow{mockHookContext};
};

struct OutlineGlowInactiveTestParam {
    bool outlineGlowEnabled{};
    bool glowEnabled{};
    bool isGlowAppliedByTheGame{false};
};

class OutlineGlowInactiveTest
    : public OutlineGlowTest,
      public testing::WithParamInterface<OutlineGlowInactiveTestParam> {
protected:
    OutlineGlowInactiveTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
        mockConfig.expectGetVariable<outline_glow_vars::Enabled>().WillRepeatedly(testing::Return(GetParam().outlineGlowEnabled));
        if (GetParam().isGlowAppliedByTheGame) {
            EXPECT_CALL(mockBaseEntity, asModelEntity()).WillRepeatedly(testing::ReturnRef(mockModelEntity));
            EXPECT_CALL(mockModelEntity, glowProperty()).WillOnce(testing::ReturnRef(mockGlowProperty));
            EXPECT_CALL(mockGlowProperty, isGlowing()).WillOnce(testing::Return(true));
        }
    }
};

TEST_P(OutlineGlowInactiveTest, DefuseKit) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowDefuseKits>().WillRepeatedly(testing::Return(GetParam().glowEnabled));
    outlineGlow.applyGlow()(DefuseKitOutlineGlow{mockHookContext}, mockBaseEntity, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::CBaseAnimGraph>()});
}

TEST_P(OutlineGlowInactiveTest, DroppedBomb) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowDroppedBomb>().WillRepeatedly(testing::Return(GetParam().glowEnabled));
    outlineGlow.applyGlow()(DroppedBombOutlineGlow{mockHookContext}, mockBaseEntity, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_C4>()});
}

TEST_P(OutlineGlowInactiveTest, GrenadeProjectile) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowGrenadeProjectiles>().WillRepeatedly(testing::Return(GetParam().glowEnabled));
    outlineGlow.applyGlow()(GrenadeProjectileOutlineGlow{mockHookContext}, mockBaseEntity, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()});
}

TEST_P(OutlineGlowInactiveTest, Hostage) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowHostages>().WillRepeatedly(testing::Return(GetParam().glowEnabled));
    outlineGlow.applyGlow()(HostageOutlineGlow{mockHookContext}, mockBaseEntity, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_Hostage>()});
}

TEST_P(OutlineGlowInactiveTest, Player) {
    EXPECT_CALL(mockPlayerPawn, baseEntity()).WillRepeatedly(testing::ReturnRef(mockBaseEntity));

    mockConfig.expectGetVariable<outline_glow_vars::GlowPlayers>().WillRepeatedly(testing::Return(GetParam().glowEnabled));
    outlineGlow.applyGlow()(PlayerOutlineGlow{mockHookContext}, mockPlayerPawn, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_CSPlayerPawn>()});
}

TEST_P(OutlineGlowInactiveTest, TickingBomb) {
    EXPECT_CALL(mockPlantedC4, baseEntity()).WillRepeatedly(testing::ReturnRef(mockBaseEntity));

    mockConfig.expectGetVariable<outline_glow_vars::GlowTickingBomb>().WillRepeatedly(testing::Return(GetParam().glowEnabled));
    outlineGlow.applyGlow()(TickingBombOutlineGlow{mockHookContext}, mockPlantedC4, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::CPlantedC4>()});
}

TEST_P(OutlineGlowInactiveTest, Weapon) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowWeapons>().WillRepeatedly(testing::Return(GetParam().glowEnabled));
    outlineGlow.applyGlow()(WeaponOutlineGlow{mockHookContext}, mockBaseEntity, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_DEagle>()});
}

INSTANTIATE_TEST_SUITE_P(, OutlineGlowInactiveTest, testing::ValuesIn(
    std::to_array<OutlineGlowInactiveTestParam>({
        {.outlineGlowEnabled = false, .glowEnabled = false},
        {.outlineGlowEnabled = true, .glowEnabled = false},
        {.outlineGlowEnabled = false, .glowEnabled = true},
        {.outlineGlowEnabled = true, .glowEnabled = true, .isGlowAppliedByTheGame = true}
    })
));

class OutlineGlowActiveTest : public OutlineGlowTest {
protected:
    OutlineGlowActiveTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
        mockConfig.expectGetVariable<outline_glow_vars::Enabled>(true);
        EXPECT_CALL(mockBaseEntity, asModelEntity()).WillOnce(testing::ReturnRef(mockModelEntity));
        EXPECT_CALL(mockModelEntity, glowProperty()).WillOnce(testing::ReturnRef(mockGlowProperty));
        EXPECT_CALL(mockGlowProperty, isGlowing()).WillOnce(testing::Return(false));
    }
};

TEST_F(OutlineGlowActiveTest, DefuseKit) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowDefuseKits>(true);
    mockConfig.expectGetVariable<outline_glow_vars::DefuseKitHue>(outline_glow_vars::DefuseKitHue::ValueType{color::HueInteger{190}});
    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{127, 233, 255, outline_glow_params::kGlowAlpha}, outline_glow_params::kDefuseKitGlowRange));
    outlineGlow.applyGlow()(DefuseKitOutlineGlow{mockHookContext}, mockBaseEntity, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::CBaseAnimGraph>()});
}

TEST_F(OutlineGlowActiveTest, DroppedBomb) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowDroppedBomb>(true);
    mockConfig.expectGetVariable<outline_glow_vars::DroppedBombHue>(outline_glow_vars::DroppedBombHue::ValueType{color::HueInteger{102}});
    EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{165, 255, 127, outline_glow_params::kGlowAlpha}, 0));
    outlineGlow.applyGlow()(DroppedBombOutlineGlow{mockHookContext}, mockBaseEntity, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_C4>()});
}

TEST_F(OutlineGlowActiveTest, GrenadeProjectile) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowGrenadeProjectiles>(true);
    mockConfig.expectGetVariable<outline_glow_vars::FlashbangHue>(outline_glow_vars::FlashbangHue::ValueType{color::HueInteger{240}});
    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{127, 127, 255, outline_glow_params::kGlowAlpha}, 0));
    outlineGlow.applyGlow()(GrenadeProjectileOutlineGlow{mockHookContext}, mockBaseEntity, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()});
}

TEST_F(OutlineGlowActiveTest, Hostage) {
    static_assert(outline_glow_params::kHostageHue == 50, "Update this test");
    mockConfig.expectGetVariable<outline_glow_vars::GlowHostages>(true);
    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{255, 233, 127, outline_glow_params::kGlowAlpha}, 0));
    outlineGlow.applyGlow()(HostageOutlineGlow{mockHookContext}, mockBaseEntity, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_Hostage>()});
}

TEST_F(OutlineGlowActiveTest, Player) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowPlayers>(true);
    mockConfig.expectGetVariable<outline_glow_vars::GlowOnlyEnemies>(false);
    mockConfig.expectGetVariable<outline_glow_vars::PlayerGlowColorMode>(PlayerOutlineGlowColorType::TeamColor);
    mockConfig.expectGetVariable<outline_glow_vars::TeamTHue>(outline_glow_vars::TeamTHue::ValueType{color::HueInteger{36}});

    EXPECT_CALL(mockPlayerPawn, isAlive()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, health()).WillOnce(testing::Return(100));
    EXPECT_CALL(mockPlayerPawn, isControlledByLocalPlayer()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockPlayerPawn, isTTorCT()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, hasImmunity()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockPlayerPawn, baseEntity()).WillRepeatedly(testing::ReturnRef(mockBaseEntity));
    EXPECT_CALL(mockPlayerPawn, teamNumber()).WillOnce(testing::Return(TeamNumber::TT));

    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{255, 204, 127, outline_glow_params::kGlowAlpha}, 0));
    outlineGlow.applyGlow()(PlayerOutlineGlow{mockHookContext}, mockPlayerPawn, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_CSPlayerPawn>()});
}

TEST_F(OutlineGlowActiveTest, TickingBomb) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowTickingBomb>(true);
    mockConfig.expectGetVariable<outline_glow_vars::TickingBombHue>(outline_glow_vars::TickingBombHue::ValueType{color::HueInteger{87}});
    EXPECT_CALL(mockPlantedC4, baseEntity()).WillRepeatedly(testing::ReturnRef(mockBaseEntity));
    EXPECT_CALL(mockPlantedC4, isTicking()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{197, 255, 127, outline_glow_params::kGlowAlpha}, 0));
    outlineGlow.applyGlow()(TickingBombOutlineGlow{mockHookContext}, mockPlantedC4, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::CPlantedC4>()});
}

TEST_F(OutlineGlowActiveTest, Weapon) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowWeapons>(true);
    EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(outline_glow_params::kFallbackColor, outline_glow_params::kWeaponGlowRange));
    outlineGlow.applyGlow()(WeaponOutlineGlow{mockHookContext}, mockBaseEntity, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_DEagle>()});
}

class OutlineGlowNonDefaultAlphaTest : public OutlineGlowActiveTest {
};

TEST_F(OutlineGlowNonDefaultAlphaTest, ImmunePlayerHasCorrectAlpha) {
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
    EXPECT_CALL(mockPlayerPawn, baseEntity()).WillRepeatedly(testing::ReturnRef(mockBaseEntity));

    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{127, 255, 127, outline_glow_params::kImmunePlayerGlowAlpha}, 0));
    outlineGlow.applyGlow()(PlayerOutlineGlow{mockHookContext}, mockPlayerPawn, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_CSPlayerPawn>()});
}

TEST_F(OutlineGlowNonDefaultAlphaTest, ImmunePlayerHasCorrectFallbackColorAlpha) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowPlayers>(true);
    mockConfig.expectGetVariable<outline_glow_vars::GlowOnlyEnemies>(true);
    mockConfig.expectGetVariable<outline_glow_vars::PlayerGlowColorMode>(PlayerOutlineGlowColorType::EnemyAlly);
 
    EXPECT_CALL(mockPlayerPawn, isAlive()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, health()).WillOnce(testing::Return(100));
    EXPECT_CALL(mockPlayerPawn, isControlledByLocalPlayer()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockPlayerPawn, isTTorCT()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, isEnemy()).Times(2).WillRepeatedly(testing::Return(std::nullopt));
    EXPECT_CALL(mockPlayerPawn, hasImmunity()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, baseEntity()).WillRepeatedly(testing::ReturnRef(mockBaseEntity));

    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{191, 191, 191, outline_glow_params::kImmunePlayerGlowAlpha}, 0));
    outlineGlow.applyGlow()(PlayerOutlineGlow{mockHookContext}, mockPlayerPawn, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_CSPlayerPawn>()});
}

class OutlineGlowNonDefaultRangeTest : public OutlineGlowActiveTest {
};

TEST_F(OutlineGlowNonDefaultRangeTest, NonDefaultGlowRangeIsUsedForDefuseKits) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowDefuseKits>(true);
    mockConfig.expectGetVariable<outline_glow_vars::DefuseKitHue>(outline_glow_vars::DefuseKitHue::ValueType{color::HueInteger{188}});

    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{127, 238, 255, outline_glow_params::kGlowAlpha}, outline_glow_params::kDefuseKitGlowRange));
    outlineGlow.applyGlow()(DefuseKitOutlineGlow{mockHookContext}, mockBaseEntity, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::CBaseAnimGraph>()});
}

TEST_F(OutlineGlowNonDefaultRangeTest, NonDefaultGlowRangeIsUsedForWeapons) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowWeapons>(true);
    mockConfig.expectGetVariable<outline_glow_vars::MolotovHue>(outline_glow_vars::MolotovHue::ValueType{color::HueInteger{50}});

    EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(cs2::Color{255, 233, 127, outline_glow_params::kGlowAlpha}, outline_glow_params::kWeaponGlowRange));
    outlineGlow.applyGlow()(WeaponOutlineGlow{mockHookContext}, mockBaseEntity, EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>()});
}
