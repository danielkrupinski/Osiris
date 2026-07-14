#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <gtest/gtest.h>

#include <GameClient/Entities/BaseWeapon.h>
#include <GameClient/Entities/C4.h>

namespace {

constexpr std::size_t alignUp(std::size_t value, std::size_t alignment) noexcept
{
    return (value + alignment - 1) / alignment * alignment;
}

struct WeaponStorage {
    static constexpr auto kVDataOffset = alignUp(sizeof(cs2::C_CSWeaponBase), alignof(void*));

    alignas(cs2::C_CSWeaponBase) alignas(void*) std::byte bytes[kVDataOffset + sizeof(cs2::CEntitySubclassVDataBase*)]{};

    [[nodiscard]] auto* weapon() noexcept { return reinterpret_cast<cs2::C_CSWeaponBase*>(bytes); }
};

struct VDataStorage {
    static constexpr auto kNameOffset = alignUp(sizeof(cs2::CCSWeaponBaseVData), alignof(const char*));

    alignas(cs2::CCSWeaponBaseVData) alignas(void*) std::byte bytes[kNameOffset + sizeof(const char*)]{};

    [[nodiscard]] auto* vData() noexcept { return reinterpret_cast<cs2::CCSWeaponBaseVData*>(bytes); }
};

struct PatternResults {
    std::int32_t vDataOffset{};
    std::int32_t nameOffset{};

    template <typename T>
    [[nodiscard]] auto get() const noexcept
    {
        if constexpr (std::is_same_v<T, OffsetToVData>)
            return OffsetToVData::type{vDataOffset};
        else if constexpr (std::is_same_v<T, OffsetToWeaponName>)
            return OffsetToWeaponName::type{nameOffset};
        else
            return typename T::type{};
    }
};

struct FakeEntityClassifier {
    EntityTypeInfo type{};

    [[nodiscard]] EntityTypeInfo classifyEntity(const cs2::CEntityClass*) const noexcept
    {
        return type;
    }

    template <typename T>
    [[nodiscard]] bool entityIs(const cs2::CEntityClass*) const noexcept
    {
        return type.template is<T>();
    }
};

struct HookContext {
    template <template <typename...> typename T, typename RawType>
    [[nodiscard]] auto make(RawType* entity) noexcept
    {
        return T<HookContext>{*this, entity};
    }

    template <typename T, typename RawType>
    [[nodiscard]] auto make(RawType* entity) noexcept
    {
        return T{*this, entity};
    }

    template <typename T>
    [[nodiscard]] auto make(std::nullptr_t) noexcept
    {
        return T{*this, nullptr};
    }

    [[nodiscard]] const PatternResults& patternSearchResults() const noexcept { return patterns; }
    [[nodiscard]] const FakeEntityClassifier& entityClassifier() const noexcept { return classifier; }

    PatternResults patterns{};
    FakeEntityClassifier classifier{};
};

void setVData(WeaponStorage& weapon, std::int32_t offset, cs2::CEntitySubclassVDataBase* vData) noexcept
{
    *reinterpret_cast<cs2::CEntitySubclassVDataBase**>(reinterpret_cast<std::byte*>(weapon.weapon()) + offset) = vData;
}

void setName(VDataStorage& vData, std::int32_t offset, const char* name) noexcept
{
    *reinterpret_cast<const char**>(reinterpret_cast<std::byte*>(vData.vData()) + offset) = name;
}

class BaseWeaponTest : public testing::Test {
protected:
    WeaponStorage weaponStorage;
    VDataStorage vDataStorage;
    cs2::CEntityIdentity identity{};
    cs2::CEntityClass entityClass{};
    HookContext hookContext;

    void SetUp() override
    {
        hookContext.patterns.vDataOffset = static_cast<std::int32_t>(WeaponStorage::kVDataOffset);
        hookContext.patterns.nameOffset = static_cast<std::int32_t>(VDataStorage::kNameOffset);
        weaponStorage.weapon()->identity = &identity;
        identity.entityClass = &entityClass;
        setVData(weaponStorage, hookContext.patterns.vDataOffset, vDataStorage.vData());
    }

    void setClassification(EntityTypeInfo::Index typeIndex) noexcept
    {
        hookContext.classifier.type = EntityTypeInfo{typeIndex};
    }

    void removeVData() noexcept
    {
        setVData(weaponStorage, hookContext.patterns.vDataOffset, nullptr);
    }
};

class BaseWeaponNameParameterizedTest
    : public BaseWeaponTest,
      public testing::WithParamInterface<const char*> {
};

TEST_F(BaseWeaponTest, NullWeaponAndMissingVDataOrNameFailClosed)
{
    BaseWeapon nullWeapon{hookContext, nullptr};
    EXPECT_EQ(nullWeapon.getName(), nullptr);
    EXPECT_FALSE(nullWeapon.grenadeKind().hasValue());
    EXPECT_FALSE(nullWeapon.isC4());

    removeVData();
    BaseWeapon weaponWithoutVData{hookContext, weaponStorage.weapon()};
    EXPECT_EQ(weaponWithoutVData.getName(), nullptr);
    EXPECT_FALSE(weaponWithoutVData.grenadeKind().hasValue());

    setVData(weaponStorage, hookContext.patterns.vDataOffset, vDataStorage.vData());
    setName(vDataStorage, hookContext.patterns.nameOffset, nullptr);
    BaseWeapon weaponWithoutName{hookContext, weaponStorage.weapon()};
    EXPECT_EQ(weaponWithoutName.getName(), nullptr);
    EXPECT_FALSE(weaponWithoutName.grenadeKind().hasValue());
}

TEST_F(BaseWeaponTest, ZeroVDataAndWeaponNameOffsetsFailClosed)
{
    BaseWeapon weapon{hookContext, weaponStorage.weapon()};

    hookContext.patterns.vDataOffset = 0;
    EXPECT_EQ(weapon.getName(), nullptr);
    EXPECT_FALSE(weapon.grenadeKind().hasValue());
    EXPECT_FALSE(weapon.isC4());

    hookContext.patterns.vDataOffset = static_cast<std::int32_t>(WeaponStorage::kVDataOffset);
    hookContext.patterns.nameOffset = 0;
    EXPECT_EQ(weapon.getName(), nullptr);
    EXPECT_FALSE(weapon.grenadeKind().hasValue());
    EXPECT_FALSE(weapon.isC4());
}

TEST_P(BaseWeaponNameParameterizedTest, ReadsWeaponNameThroughPatternOffsets)
{
    setName(vDataStorage, hookContext.patterns.nameOffset, GetParam());
    BaseWeapon weapon{hookContext, weaponStorage.weapon()};
    EXPECT_STREQ(weapon.getName(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(RecognizedNames, BaseWeaponNameParameterizedTest, testing::Values(
    "weapon_ak47", "weapon_flashbang", "weapon_knife", "weapon_c4"));

class GrenadeClassParameterizedTest
    : public BaseWeaponTest,
      public testing::WithParamInterface<std::tuple<std::size_t, cs2::GrenadeKind>> {
};

TEST_P(GrenadeClassParameterizedTest, MapsExactGrenadeClassWithoutVData)
{
    const auto [typeIndex, expectedKind] = GetParam();
    setClassification(static_cast<EntityTypeInfo::Index>(typeIndex));
    removeVData();
    BaseWeapon weapon{hookContext, weaponStorage.weapon()};
    ASSERT_TRUE(weapon.grenadeKind().hasValue());
    EXPECT_EQ(weapon.grenadeKind().value(), expectedKind);
}

INSTANTIATE_TEST_SUITE_P(ExactGrenadeClasses, GrenadeClassParameterizedTest, testing::Values(
    std::tuple{EntityTypeInfo::indexOf<cs2::C_Flashbang>(), cs2::GrenadeKind::Flashbang},
    std::tuple{EntityTypeInfo::indexOf<cs2::C_HEGrenade>(), cs2::GrenadeKind::HEGrenade},
    std::tuple{EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>(), cs2::GrenadeKind::SmokeGrenade},
    std::tuple{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>(), cs2::GrenadeKind::Molotov},
    std::tuple{EntityTypeInfo::indexOf<cs2::C_DecoyGrenade>(), cs2::GrenadeKind::Decoy},
    std::tuple{EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>(), cs2::GrenadeKind::Incendiary}));

class GrenadeNameParameterizedTest
    : public BaseWeaponTest,
      public testing::WithParamInterface<std::tuple<const char*, cs2::GrenadeKind>> {
};

TEST_P(GrenadeNameParameterizedTest, MapsExactGrenadeNameForUnknownClassification)
{
    const auto [name, expectedKind] = GetParam();
    setName(vDataStorage, hookContext.patterns.nameOffset, name);
    BaseWeapon weapon{hookContext, weaponStorage.weapon()};
    ASSERT_TRUE(weapon.grenadeKind().hasValue());
    EXPECT_EQ(weapon.grenadeKind().value(), expectedKind);
}

INSTANTIATE_TEST_SUITE_P(ExactGrenadeNames, GrenadeNameParameterizedTest, testing::Values(
    std::tuple{"weapon_flashbang", cs2::GrenadeKind::Flashbang},
    std::tuple{"weapon_hegrenade", cs2::GrenadeKind::HEGrenade},
    std::tuple{"weapon_smokegrenade", cs2::GrenadeKind::SmokeGrenade},
    std::tuple{"weapon_molotov", cs2::GrenadeKind::Molotov},
    std::tuple{"weapon_decoy", cs2::GrenadeKind::Decoy},
    std::tuple{"weapon_incgrenade", cs2::GrenadeKind::Incendiary}));

TEST_F(BaseWeaponTest, RejectsNullWrongCaseUnknownAndGunNamesAsGrenades)
{
    const char* const names[]{nullptr, "weapon_Flashbang", "weapon_unknown", "weapon_ak47"};
    for (const auto* name : names) {
        setName(vDataStorage, hookContext.patterns.nameOffset, name);
        BaseWeapon weapon{hookContext, weaponStorage.weapon()};
        EXPECT_FALSE(weapon.grenadeKind().hasValue());
    }
}

TEST_F(BaseWeaponTest, ExactGrenadeClassTakesPrecedenceOverVDataName)
{
    setClassification(EntityTypeInfo::indexOf<cs2::C_Flashbang>());
    setName(vDataStorage, hookContext.patterns.nameOffset, "weapon_hegrenade");
    BaseWeapon weapon{hookContext, weaponStorage.weapon()};
    ASSERT_TRUE(weapon.grenadeKind().hasValue());
    EXPECT_EQ(weapon.grenadeKind().value(), cs2::GrenadeKind::Flashbang);
}

TEST_F(BaseWeaponTest, IdentifiesC4ByExactClassOrName)
{
    setClassification(EntityTypeInfo::indexOf<cs2::C_C4>());
    removeVData();
    BaseWeapon weapon{hookContext, weaponStorage.weapon()};
    EXPECT_TRUE(weapon.isC4());

    setClassification(EntityTypeInfo{}.typeIndex);
    setVData(weaponStorage, hookContext.patterns.vDataOffset, vDataStorage.vData());
    setName(vDataStorage, hookContext.patterns.nameOffset, "weapon_c4");
    EXPECT_TRUE(weapon.isC4());

    setName(vDataStorage, hookContext.patterns.nameOffset, "weapon_ak47");
    EXPECT_FALSE(weapon.isC4());
    setName(vDataStorage, hookContext.patterns.nameOffset, nullptr);
    EXPECT_FALSE(weapon.isC4());
}

class SniperClassParameterizedTest
    : public BaseWeaponTest,
      public testing::WithParamInterface<EntityTypeInfo::Index> {
};

TEST_P(SniperClassParameterizedTest, IdentifiesExactSniperClasses)
{
    setClassification(GetParam());
    BaseWeapon weapon{hookContext, weaponStorage.weapon()};
    EXPECT_TRUE(weapon.isSniperRifle());
}

INSTANTIATE_TEST_SUITE_P(ExactSniperClasses, SniperClassParameterizedTest, testing::Values(
    EntityTypeInfo::indexOf<cs2::C_WeaponSSG08>(), EntityTypeInfo::indexOf<cs2::C_WeaponAWP>(),
    EntityTypeInfo::indexOf<cs2::C_WeaponG3SG1>(), EntityTypeInfo::indexOf<cs2::C_WeaponSCAR20>()));

TEST_F(BaseWeaponTest, RejectsNonSniperClasses)
{
    for (const auto typeIndex : {EntityTypeInfo::indexOf<cs2::C_AK47>(), EntityTypeInfo::indexOf<cs2::C_Flashbang>(), EntityTypeInfo::indexOf<cs2::C_C4>()}) {
        setClassification(typeIndex);
        BaseWeapon weapon{hookContext, weaponStorage.weapon()};
        EXPECT_FALSE(weapon.isSniperRifle());
    }
}

TEST_F(BaseWeaponTest, CastsOnlyExactC4Class)
{
    setClassification(EntityTypeInfo::indexOf<cs2::C_C4>());
    BaseWeapon weapon{hookContext, weaponStorage.weapon()};
    EXPECT_TRUE(static_cast<bool>(weapon.cast<C4>()));

    setClassification(EntityTypeInfo::indexOf<cs2::C_AK47>());
    EXPECT_FALSE(static_cast<bool>(weapon.cast<C4>()));
}

}
