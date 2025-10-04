#pragma once

#include <gtest/gtest.h>

#include <Features/Visuals/ModelGlow/ModelGlow.h>
#include <Mocks/MockBaseWeapon.h>

class ModelGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext{};
    testing::StrictMock<MockConfig> mockConfig{};
    testing::StrictMock<MockBaseEntity> mockBaseEntity{};
    testing::StrictMock<MockBaseWeapon> mockBaseWeapon{};
    testing::StrictMock<MockPlayerPawn> mockPlayerPawn{};
    testing::StrictMock<MockPlantedC4> mockPlantedC4{};
    FeaturesStates featuresStates{};
    ModelGlow<MockHookContext> modelGlow{mockHookContext};
};

TEST_F(ModelGlowTest, DisablingFlagsAreClearedAfterUpdateInMainThread) {
    auto& modelGlowState = featuresStates.visualFeaturesStates.modelGlowState;
    modelGlowState.modelGlowDisabling = true;
    modelGlowState.defuseKitModelGlowDisabling = true;
    modelGlowState.droppedBombModelGlowDisabling = true;
    modelGlowState.grenadeProjectileModelGlowDisabling = true;
    modelGlowState.playerModelGlowDisabling = true;
    modelGlowState.tickingBombModelGlowDisabling = true;
    modelGlowState.weaponModelGlowDisabling = true;

    EXPECT_CALL(mockHookContext, featuresStates()).WillRepeatedly(testing::ReturnRef(featuresStates));
    modelGlow.postUpdateInMainThread();

    EXPECT_FALSE(modelGlowState.modelGlowDisabling);
    EXPECT_FALSE(modelGlowState.defuseKitModelGlowDisabling);
    EXPECT_FALSE(modelGlowState.droppedBombModelGlowDisabling);
    EXPECT_FALSE(modelGlowState.grenadeProjectileModelGlowDisabling);
    EXPECT_FALSE(modelGlowState.playerModelGlowDisabling);
    EXPECT_FALSE(modelGlowState.tickingBombModelGlowDisabling);
    EXPECT_FALSE(modelGlowState.weaponModelGlowDisabling);
}

struct ModelGlowInactiveTestParam {
    bool modelGlowEnabled{};
    bool glowEnabled{};
};

class ModelGlowInactiveTest
    : public ModelGlowTest,
      public testing::WithParamInterface<ModelGlowInactiveTestParam> {
protected:
    ModelGlowInactiveTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
        EXPECT_CALL(mockHookContext, featuresStates()).WillRepeatedly(testing::ReturnRef(featuresStates));

        mockConfig.expectGetVariable<model_glow_vars::Enabled>(GetParam().modelGlowEnabled);
        featuresStates.visualFeaturesStates.modelGlowState.modelGlowDisabling = false;
    }
};

TEST_P(ModelGlowInactiveTest, DefuseKitUpdateInMainThread) {
    mockConfig.expectGetVariable<model_glow_vars::GlowDefuseKits>().WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.defuseKitModelGlowDisabling = false;
    modelGlow.updateInMainThread()(
        DefuseKitModelGlow{mockHookContext},
        mockBaseEntity,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::CBaseAnimGraph>()});
}

TEST_P(ModelGlowInactiveTest, DroppedBombUpdateInMainThread) {
    mockConfig.expectGetVariable<model_glow_vars::GlowDroppedBomb>()
        .WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.droppedBombModelGlowDisabling = false;
    modelGlow.updateInMainThread()(
        DroppedBombModelGlow{mockHookContext},
        mockBaseWeapon,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_C4>()});
}

TEST_P(ModelGlowInactiveTest, GrenadeProjectileUpdateInMainThread) {
    mockConfig.expectGetVariable<model_glow_vars::GlowGrenadeProjectiles>()
        .WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.grenadeProjectileModelGlowDisabling = false;
    modelGlow.updateInMainThread()(
        GrenadeProjectileModelGlow{mockHookContext},
        mockBaseEntity,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()});
}

TEST_P(ModelGlowInactiveTest, PlayerUpdateInMainThread) {
    mockConfig.expectGetVariable<model_glow_vars::GlowPlayers>()
        .WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.playerModelGlowDisabling = false;
    modelGlow.updateInMainThread()(
        PlayerModelGlow{mockHookContext},
        mockPlayerPawn,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_CSPlayerPawn>()});
}

TEST_P(ModelGlowInactiveTest, TickingBombUpdateInMainThread) {
    mockConfig.expectGetVariable<model_glow_vars::GlowTickingBomb>()
        .WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.tickingBombModelGlowDisabling = false;
    modelGlow.updateInMainThread()(
        TickingBombModelGlow{mockHookContext},
        mockPlantedC4,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::CPlantedC4>()});
}

TEST_P(ModelGlowInactiveTest, WeaponUpdateInMainThread) {
    mockConfig.expectGetVariable<model_glow_vars::GlowWeapons>()
        .WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.weaponModelGlowDisabling = false;
    modelGlow.updateInMainThread()(
        WeaponModelGlow{mockHookContext},
        mockBaseWeapon,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_DEagle>()});
}

TEST_P(ModelGlowInactiveTest, DroppedBombUpdateInSceneObjectUpdater) {
    mockConfig.expectGetVariable<model_glow_vars::GlowDroppedBomb>()
        .WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.droppedBombModelGlowDisabling = false;
    modelGlow.updateInSceneObjectUpdater()(
        DroppedBombModelGlow{mockHookContext},
        mockBaseWeapon,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_C4>()});
}

TEST_P(ModelGlowInactiveTest, PlayerUpdateInSceneObjectUpdater) {
    mockConfig.expectGetVariable<model_glow_vars::GlowPlayers>()
        .WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.playerModelGlowDisabling = false;
    modelGlow.updateInSceneObjectUpdater()(
        PlayerModelGlow{mockHookContext},
        mockPlayerPawn,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_CSPlayerPawn>()});
}

TEST_P(ModelGlowInactiveTest, WeaponUpdateInSceneObjectUpdater) {
    mockConfig.expectGetVariable<model_glow_vars::GlowWeapons>()
        .WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.weaponModelGlowDisabling = false;
    modelGlow.updateInSceneObjectUpdater()(
        WeaponModelGlow{mockHookContext},
        mockBaseWeapon,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_DEagle>()});
}

TEST_P(ModelGlowInactiveTest, DefuseKitOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowDefuseKits>().WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.defuseKitModelGlowDisabling = false;
    modelGlow.onUnload()(DefuseKitModelGlow{mockHookContext}, mockBaseEntity);
}

TEST_P(ModelGlowInactiveTest, DroppedBombOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowDroppedBomb>()
        .WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.droppedBombModelGlowDisabling = false;
    modelGlow.onUnload()(DroppedBombModelGlow{mockHookContext}, mockBaseWeapon);
}

TEST_P(ModelGlowInactiveTest, GrenadeProjectileOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowGrenadeProjectiles>()
        .WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.grenadeProjectileModelGlowDisabling = false;
    modelGlow.onUnload()(GrenadeProjectileModelGlow{mockHookContext}, mockBaseEntity);
}

TEST_P(ModelGlowInactiveTest, PlayerOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowPlayers>()
        .WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.playerModelGlowDisabling = false;
    modelGlow.onUnload()(PlayerModelGlow{mockHookContext}, mockPlayerPawn);
}

TEST_P(ModelGlowInactiveTest, TickingBombOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowTickingBomb>()
        .WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.tickingBombModelGlowDisabling = false;
    modelGlow.onUnload()(TickingBombModelGlow{mockHookContext}, mockPlantedC4);
}

TEST_P(ModelGlowInactiveTest, WeaponOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowWeapons>()
        .WillRepeatedly(testing::Return(GetParam().glowEnabled));
    featuresStates.visualFeaturesStates.modelGlowState.weaponModelGlowDisabling = false;
    modelGlow.onUnload()(WeaponModelGlow{mockHookContext}, mockBaseWeapon);
}

INSTANTIATE_TEST_SUITE_P(MasterSwitchOff, ModelGlowInactiveTest, testing::ValuesIn(
    std::to_array<ModelGlowInactiveTestParam>({
        {.modelGlowEnabled = false, .glowEnabled = true},
        {.modelGlowEnabled = false, .glowEnabled = false}
    })
));

INSTANTIATE_TEST_SUITE_P(GlowSwitchOff, ModelGlowInactiveTest, testing::ValuesIn(
    std::to_array<ModelGlowInactiveTestParam>({
        {.modelGlowEnabled = true, .glowEnabled = false},
        {.modelGlowEnabled = false, .glowEnabled = false}
    })
));

class ModelGlowActiveTest : public ModelGlowTest {
protected:
    ModelGlowActiveTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
        EXPECT_CALL(mockHookContext, featuresStates()).WillRepeatedly(testing::ReturnRef(featuresStates));

        mockConfig.expectGetVariable<model_glow_vars::Enabled>()
            .WillRepeatedly(testing::Return(true));
    }
};

TEST_F(ModelGlowActiveTest, DefuseKitUpdateInMainThread) {
    mockConfig.expectGetVariable<model_glow_vars::GlowDefuseKits>()
        .WillRepeatedly(testing::Return(true));
    mockConfig.expectGetVariable<model_glow_vars::DefuseKitHue>(model_glow_vars::DefuseKitHue::ValueType{color::HueInteger{123}});
    EXPECT_CALL(mockBaseEntity, applySpawnProtectionEffectRecursively(cs2::Color{0, 255, 12}));
    modelGlow.updateInMainThread()(
        DefuseKitModelGlow{mockHookContext},
        mockBaseEntity,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::CBaseAnimGraph>()});
}

TEST_F(ModelGlowActiveTest, DroppedBombUpdateInMainThread) {
    mockConfig.expectGetVariable<model_glow_vars::GlowDroppedBomb>()
        .WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockBaseWeapon, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));
    EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(false));
    const auto dummySceneObjectUpdater = reinterpret_cast<std::uint64_t(*)(cs2::C_CSWeaponBase*, void*, bool)>(0x123123);
    EXPECT_CALL(mockBaseWeapon, getSceneObjectUpdater()).WillRepeatedly(testing::Return(dummySceneObjectUpdater));
    EXPECT_CALL(mockBaseWeapon, setSceneObjectUpdater(&Weapon_sceneObjectUpdater_asm));

    modelGlow.updateInMainThread()(
        DroppedBombModelGlow{mockHookContext},
        mockBaseWeapon,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_C4>()});
    EXPECT_EQ(featuresStates.visualFeaturesStates.modelGlowState.originalWeaponSceneObjectUpdater, dummySceneObjectUpdater);
}

TEST_F(ModelGlowActiveTest, GrenadeProjectileUpdateInMainThread) {
    mockConfig.expectGetVariable<model_glow_vars::GlowGrenadeProjectiles>()
        .WillRepeatedly(testing::Return(true));
    mockConfig.expectGetVariable<model_glow_vars::SmokeGrenadeHue>(model_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{133}});
    EXPECT_CALL(mockBaseEntity, applySpawnProtectionEffectRecursively(cs2::Color{0, 255, 55}));
    modelGlow.updateInMainThread()(
        GrenadeProjectileModelGlow{mockHookContext},
        mockBaseEntity,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()});
}

TEST_F(ModelGlowActiveTest, PlayerUpdateInMainThread) {
    mockConfig.expectGetVariable<model_glow_vars::GlowPlayers>()
        .WillRepeatedly(testing::Return(true));
    mockConfig.expectGetVariable<model_glow_vars::GlowOnlyEnemies>(false);

    const auto dummySceneObjectUpdater = reinterpret_cast<std::uint64_t(*)(cs2::C_CSPlayerPawn*, void*, bool)>(0xbadc0de);
    EXPECT_CALL(mockPlayerPawn, isAlive()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, health()).WillOnce(testing::Return(100));
    EXPECT_CALL(mockPlayerPawn, isTTorCT()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, isControlledByLocalPlayer()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockPlayerPawn, getSceneObjectUpdater()).WillRepeatedly(testing::Return(dummySceneObjectUpdater));
    EXPECT_CALL(mockPlayerPawn, setSceneObjectUpdater(&PlayerPawn_sceneObjectUpdater_asm));

    modelGlow.updateInMainThread()(
        PlayerModelGlow{mockHookContext},
        mockPlayerPawn,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_CSPlayerPawn>()});
    EXPECT_EQ(featuresStates.visualFeaturesStates.modelGlowState.originalPlayerPawnSceneObjectUpdater, dummySceneObjectUpdater);
}

TEST_F(ModelGlowActiveTest, TickingBombUpdateInMainThread) {
    mockConfig.expectGetVariable<model_glow_vars::GlowTickingBomb>()
        .WillRepeatedly(testing::Return(true));
    mockConfig.expectGetVariable<model_glow_vars::TickingBombHue>(model_glow_vars::TickingBombHue::ValueType{color::HueInteger{255}});
    EXPECT_CALL(mockPlantedC4, isTicking()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlantedC4, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));
    EXPECT_CALL(mockBaseEntity, applySpawnProtectionEffectRecursively(cs2::Color{63, 0, 255}));
    modelGlow.updateInMainThread()(
        TickingBombModelGlow{mockHookContext},
        mockPlantedC4,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::CPlantedC4>()});
}

TEST_F(ModelGlowActiveTest, WeaponUpdateInMainThread) {
    mockConfig.expectGetVariable<model_glow_vars::GlowWeapons>()
        .WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(false));
    const auto dummySceneObjectUpdater = reinterpret_cast<std::uint64_t(*)(cs2::C_CSWeaponBase*, void*, bool)>(0x567890abcdef);
    EXPECT_CALL(mockBaseWeapon, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));
    EXPECT_CALL(mockBaseWeapon, getSceneObjectUpdater()).WillRepeatedly(testing::Return(dummySceneObjectUpdater));
    EXPECT_CALL(mockBaseWeapon, setSceneObjectUpdater(&Weapon_sceneObjectUpdater_asm));

    modelGlow.updateInMainThread()(
        WeaponModelGlow{mockHookContext},
        mockBaseWeapon,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_WeaponAWP>()});
    EXPECT_EQ(featuresStates.visualFeaturesStates.modelGlowState.originalWeaponSceneObjectUpdater, dummySceneObjectUpdater);
}

TEST_F(ModelGlowActiveTest, DroppedBombUpdateInSceneObjectUpdater) {
    mockConfig.expectGetVariable<model_glow_vars::GlowDroppedBomb>(true);
    mockConfig.expectGetVariable<model_glow_vars::DroppedBombHue>(model_glow_vars::DroppedBombHue::ValueType{color::HueInteger{72}});
    
    EXPECT_CALL(mockBaseWeapon, baseEntity()).WillRepeatedly(testing::ReturnRef(mockBaseEntity));
    EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockBaseEntity, applySpawnProtectionEffectRecursively(cs2::Color{203, 255, 0}));

    modelGlow.updateInSceneObjectUpdater()(
        DroppedBombModelGlow{mockHookContext},
        mockBaseWeapon,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_C4>()});
}

TEST_F(ModelGlowActiveTest, PlayerUpdateInSceneObjectUpdater) {
    mockConfig.expectGetVariable<model_glow_vars::GlowPlayers>(true);
    mockConfig.expectGetVariable<model_glow_vars::GlowOnlyEnemies>(false);
    mockConfig.expectGetVariable<model_glow_vars::PlayerGlowColorMode>(PlayerModelGlowColorType::EnemyAlly);
    mockConfig.expectGetVariable<model_glow_vars::EnemyHue>(model_glow_vars::EnemyHue::ValueType{color::HueInteger{321}});
    
    EXPECT_CALL(mockPlayerPawn, isAlive()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, hasImmunity()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockPlayerPawn, isEnemy()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, health()).WillOnce(testing::Return(100));
    EXPECT_CALL(mockPlayerPawn, isTTorCT()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, isControlledByLocalPlayer()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockPlayerPawn, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));
    EXPECT_CALL(mockBaseEntity, applySpawnProtectionEffectRecursively(cs2::Color{255, 0, 165}));

    modelGlow.updateInSceneObjectUpdater()(
        PlayerModelGlow{mockHookContext},
        mockPlayerPawn,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_CSPlayerPawn>()});
}

TEST_F(ModelGlowActiveTest, ImmunePlayerUpdateInSceneObjectUpdater) {
    mockConfig.expectGetVariable<model_glow_vars::GlowPlayers>(true);
    mockConfig.expectGetVariable<model_glow_vars::GlowOnlyEnemies>(false);
    mockConfig.expectGetVariable<model_glow_vars::PlayerGlowColorMode>(PlayerModelGlowColorType::EnemyAlly);
    mockConfig.expectGetVariable<model_glow_vars::EnemyHue>(model_glow_vars::EnemyHue::ValueType{color::HueInteger{321}});
    
    EXPECT_CALL(mockPlayerPawn, isAlive()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, hasImmunity()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, isEnemy()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, health()).WillOnce(testing::Return(100));
    EXPECT_CALL(mockPlayerPawn, isTTorCT()).WillOnce(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, isControlledByLocalPlayer()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockPlayerPawn, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));
    EXPECT_CALL(mockBaseEntity, applySpawnProtectionEffectRecursively(cs2::Color{255, 127, 210}));

    modelGlow.updateInSceneObjectUpdater()(
        PlayerModelGlow{mockHookContext},
        mockPlayerPawn,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_CSPlayerPawn>()});
}

TEST_F(ModelGlowActiveTest, WeaponUpdateInSceneObjectUpdater) {
    mockConfig.expectGetVariable<model_glow_vars::GlowWeapons>(true);
    mockConfig.expectGetVariable<model_glow_vars::MolotovHue>(model_glow_vars::MolotovHue::ValueType{color::HueInteger{43}});

    EXPECT_CALL(mockBaseWeapon, baseEntity()).WillRepeatedly(testing::ReturnRef(mockBaseEntity));
    EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockBaseEntity, applySpawnProtectionEffectRecursively(cs2::Color{255, 182, 0}));

    modelGlow.updateInSceneObjectUpdater()(
        WeaponModelGlow{mockHookContext},
        mockBaseWeapon,
        EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>()});
}

TEST_F(ModelGlowActiveTest, DefuseKitUpdateOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowDefuseKits>(true);
    EXPECT_CALL(mockBaseEntity, removeSpawnProtectionEffectRecursively());
    modelGlow.onUnload()(DefuseKitModelGlow{mockHookContext}, mockBaseEntity);
}

TEST_F(ModelGlowActiveTest, HookedDroppedBombOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowDroppedBomb>(true);
    const auto dummySceneObjectUpdater = reinterpret_cast<std::uint64_t(*)(cs2::C_CSWeaponBase*, void*, bool)>(0x123123);
    featuresStates.visualFeaturesStates.modelGlowState.originalWeaponSceneObjectUpdater = dummySceneObjectUpdater;
    EXPECT_CALL(mockBaseWeapon, getSceneObjectUpdater()).WillOnce(testing::Return(&Weapon_sceneObjectUpdater_asm));
    EXPECT_CALL(mockBaseWeapon, setSceneObjectUpdater(dummySceneObjectUpdater));

    modelGlow.onUnload()(DroppedBombModelGlow{mockHookContext}, mockBaseWeapon);
}

TEST_F(ModelGlowActiveTest, NotHookedDroppedBombOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowDroppedBomb>(true);
    featuresStates.visualFeaturesStates.modelGlowState.originalWeaponSceneObjectUpdater = nullptr;
    const auto dummySceneObjectUpdater = reinterpret_cast<std::uint64_t(*)(cs2::C_CSWeaponBase*, void*, bool)>(0x123123);
    EXPECT_CALL(mockBaseWeapon, getSceneObjectUpdater()).WillOnce(testing::Return(dummySceneObjectUpdater));

    modelGlow.onUnload()(DroppedBombModelGlow{mockHookContext}, mockBaseWeapon);
}

TEST_F(ModelGlowActiveTest, GrenadeProjectileOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowGrenadeProjectiles>(true);
    EXPECT_CALL(mockBaseEntity, removeSpawnProtectionEffectRecursively());
    modelGlow.onUnload()(GrenadeProjectileModelGlow{mockHookContext}, mockBaseEntity);
}

TEST_F(ModelGlowActiveTest, HookedPlayerOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowPlayers>(true);
    const auto dummySceneObjectUpdater = reinterpret_cast<std::uint64_t(*)(cs2::C_CSPlayerPawn*, void*, bool)>(0xbadc0de);
    featuresStates.visualFeaturesStates.modelGlowState.originalPlayerPawnSceneObjectUpdater = dummySceneObjectUpdater;
    EXPECT_CALL(mockPlayerPawn, getSceneObjectUpdater()).WillOnce(testing::Return(&PlayerPawn_sceneObjectUpdater_asm));
    EXPECT_CALL(mockPlayerPawn, setSceneObjectUpdater(dummySceneObjectUpdater));

    modelGlow.onUnload()(PlayerModelGlow{mockHookContext}, mockPlayerPawn);
}

TEST_F(ModelGlowActiveTest, NotHookedPlayerOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowPlayers>(true);
    featuresStates.visualFeaturesStates.modelGlowState.originalPlayerPawnSceneObjectUpdater = nullptr;
    const auto dummySceneObjectUpdater = reinterpret_cast<std::uint64_t(*)(cs2::C_CSPlayerPawn*, void*, bool)>(0xbadc0de);
    EXPECT_CALL(mockPlayerPawn, getSceneObjectUpdater()).WillOnce(testing::Return(dummySceneObjectUpdater));

    modelGlow.onUnload()(PlayerModelGlow{mockHookContext}, mockPlayerPawn);
}

TEST_F(ModelGlowActiveTest, TickingBombOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowTickingBomb>(true);
    EXPECT_CALL(mockPlantedC4, baseEntity()).WillOnce(testing::ReturnRef(mockBaseEntity));
    EXPECT_CALL(mockBaseEntity, removeSpawnProtectionEffectRecursively());
    modelGlow.onUnload()(TickingBombModelGlow{mockHookContext}, mockPlantedC4);
}

TEST_F(ModelGlowActiveTest, HookedWeaponOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowWeapons>(true);
    const auto dummySceneObjectUpdater = reinterpret_cast<std::uint64_t(*)(cs2::C_CSWeaponBase*, void*, bool)>(0x567890abcdef);
    featuresStates.visualFeaturesStates.modelGlowState.originalWeaponSceneObjectUpdater = dummySceneObjectUpdater;
    EXPECT_CALL(mockBaseWeapon, getSceneObjectUpdater()).WillOnce(testing::Return(&Weapon_sceneObjectUpdater_asm));
    EXPECT_CALL(mockBaseWeapon, setSceneObjectUpdater(dummySceneObjectUpdater));

    modelGlow.onUnload()(WeaponModelGlow{mockHookContext}, mockBaseWeapon);
}

TEST_F(ModelGlowActiveTest, NotHookedWeaponOnUnload) {
    mockConfig.expectGetVariable<model_glow_vars::GlowWeapons>(true);
    featuresStates.visualFeaturesStates.modelGlowState.originalWeaponSceneObjectUpdater = nullptr;
    const auto dummySceneObjectUpdater = reinterpret_cast<std::uint64_t(*)(cs2::C_CSWeaponBase*, void*, bool)>(0x567890abcdef);
    EXPECT_CALL(mockBaseWeapon, getSceneObjectUpdater()).WillOnce(testing::Return(dummySceneObjectUpdater));

    modelGlow.onUnload()(WeaponModelGlow{mockHookContext}, mockBaseWeapon);
}

