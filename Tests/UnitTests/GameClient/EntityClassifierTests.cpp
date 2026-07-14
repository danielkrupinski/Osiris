#include <gtest/gtest.h>

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include <GameClient/Entities/EntityClassifier.h>

struct EntityClassifierTestAccess {
#if defined(_WIN64)
    static void configureWeaponAncestry(EntityClassifier& classifier, const cs2::CEntityClass* baseWeaponRegistration, std::uint8_t parentNodeOffset, std::uint8_t parentOuterOffset) noexcept
    {
        classifier.weaponBaseRegistration.store(baseWeaponRegistration, std::memory_order_relaxed);
        classifier.weaponParentNodeOffset.store(parentNodeOffset, std::memory_order_relaxed);
        classifier.weaponParentOuterOffset.store(parentOuterOffset, std::memory_order_relaxed);
        classifier.weaponAncestryReady.store(EntityClassifier::isValidWeaponAncestry(baseWeaponRegistration, parentNodeOffset, parentOuterOffset), std::memory_order_release);
    }

    [[nodiscard]] static const cs2::CEntityClass* findCommonWeaponBaseRegistration(const cs2::CEntityClass* gunRegistration, const cs2::CEntityClass* shotgunRegistration, std::uint8_t parentNodeOffset, std::uint8_t parentOuterOffset) noexcept
    {
        return EntityClassifier::findCommonWeaponBaseRegistration(gunRegistration, shotgunRegistration, parentNodeOffset, parentOuterOffset);
    }

    template <typename HookContext>
    static void initializeWeaponAncestry(EntityClassifier& classifier, HookContext& hookContext) noexcept
    {
        classifier.initializeWeaponAncestry(hookContext);
    }
#endif
};

template <typename EntityType>
constexpr bool isWeaponEntityType() noexcept
{
    return EntityTypeInfo{EntityTypeInfo::indexOf<EntityType>()}.isWeapon();
}

static_assert(isWeaponEntityType<cs2::C_WeaponP250>());
static_assert(isWeaponEntityType<cs2::C_WeaponSSG08>());
static_assert(isWeaponEntityType<cs2::C_WeaponSG556>());
static_assert(isWeaponEntityType<cs2::C_WeaponSawedoff>());
static_assert(isWeaponEntityType<cs2::C_WeaponUMP45>());
static_assert(isWeaponEntityType<cs2::C_WeaponXM1014>());
static_assert(isWeaponEntityType<cs2::C_WeaponSCAR20>());
static_assert(isWeaponEntityType<cs2::C_WeaponTec9>());
static_assert(isWeaponEntityType<cs2::C_WeaponP90>());

TEST(EntityTypeInfoTest, RecognizesRegisteredWeaponLeavesAsWeapons) {
    EXPECT_TRUE(isWeaponEntityType<cs2::C_WeaponP250>());
    EXPECT_TRUE(isWeaponEntityType<cs2::C_WeaponSSG08>());
    EXPECT_TRUE(isWeaponEntityType<cs2::C_WeaponSG556>());
    EXPECT_TRUE(isWeaponEntityType<cs2::C_WeaponSawedoff>());
    EXPECT_TRUE(isWeaponEntityType<cs2::C_WeaponUMP45>());
    EXPECT_TRUE(isWeaponEntityType<cs2::C_WeaponXM1014>());
    EXPECT_TRUE(isWeaponEntityType<cs2::C_WeaponSCAR20>());
    EXPECT_TRUE(isWeaponEntityType<cs2::C_WeaponTec9>());
    EXPECT_TRUE(isWeaponEntityType<cs2::C_WeaponP90>());
}

TEST(EntityClassifierTest, ClassifiesOnlyTrustedWeaponDescriptorsAsBroadWeapons) {
    EntityClassifier classifier;
    const cs2::CEntityClass trustedWeaponClass;
    const cs2::CEntityClass unrelatedClass;

    EXPECT_FALSE(classifier.classifyEntity(nullptr).isWeapon());
    EXPECT_FALSE(classifier.classifyEntity(&unrelatedClass).isWeapon());

    classifier.registerWeaponClass(nullptr);
    classifier.registerWeaponClass(&trustedWeaponClass);
    classifier.registerWeaponClass(&trustedWeaponClass);

    const auto trustedWeaponClassification = classifier.classifyEntity(&trustedWeaponClass);
    EXPECT_TRUE(trustedWeaponClassification.isWeapon());
    EXPECT_TRUE(trustedWeaponClassification.isModelEntity());
    EXPECT_TRUE(trustedWeaponClassification.is<cs2::C_CSWeaponBaseGun>());
    EXPECT_FALSE(classifier.entityIs<cs2::C_CSWeaponBaseGun>(&trustedWeaponClass));
    EXPECT_FALSE(classifier.entityIs<cs2::C_WeaponP250>(&trustedWeaponClass));
    EXPECT_FALSE(classifier.classifyEntity(&unrelatedClass).isWeapon());
}

#if defined(_WIN64)
namespace
{
struct EntityLookupCall {
    cs2::CGameEntitySystem* entitySystem{};
    const char* className{};
    cs2::CUtlString* output{};
    cs2::CEntityClass* result{};
};

EntityLookupCall entityLookupCall;

[[nodiscard]] cs2::CEntityClass* findEntityClassForTest(cs2::CGameEntitySystem* entitySystem, const char* className, cs2::CUtlString* output) noexcept
{
    entityLookupCall.entitySystem = entitySystem;
    entityLookupCall.className = className;
    entityLookupCall.output = output;
    return entityLookupCall.result;
}

struct EntityLookupPatternSearchResults {
    cs2::CGameEntitySystem** entitySystemPointer{};
    FindEntityClassFunctionPointer::type findEntityClass{};

    template <typename T>
    [[nodiscard]] auto get() const noexcept
    {
        if constexpr (std::is_same_v<T, EntitySystemPointer>)
            return entitySystemPointer;
        else
            return findEntityClass;
    }
};

struct EntityLookupHookContext {
    EntityLookupPatternSearchResults results;

    [[nodiscard]] const EntityLookupPatternSearchResults& patternSearchResults() const noexcept
    {
        return results;
    }
};

struct TestParentNode {
    std::array<std::byte, 0x18> padding{};
    cs2::CEntityClass* owningOuter{};
};

struct TestRegistration {
    std::array<std::byte, 0x60> padding{};
    TestParentNode* parentNode{};
};

TEST(EntitySystemTest, ReturnsNullWhenWindowsLookupPrerequisitesAreMissing) {
    EntityLookupHookContext hookContext;
    EntitySystem entitySystem{hookContext};

    EXPECT_EQ(nullptr, entitySystem.findEntityClass("C_Test"));

    hookContext.results.findEntityClass = findEntityClassForTest;
    EXPECT_EQ(nullptr, entitySystem.findEntityClass("C_Test"));

    cs2::CGameEntitySystem* nullEntitySystem{};
    hookContext.results.entitySystemPointer = &nullEntitySystem;
    EXPECT_EQ(nullptr, entitySystem.findEntityClass("C_Test"));

    EXPECT_EQ(nullptr, entitySystem.findEntityClass(nullptr));
}

TEST(EntitySystemTest, ForwardsWindowsLookupArgumentsAndResult) {
    cs2::CGameEntitySystem gameEntitySystem;
    cs2::CGameEntitySystem* gameEntitySystemPointer = &gameEntitySystem;
    cs2::CEntityClass expectedEntityClass;
    entityLookupCall = {.result = &expectedEntityClass};
    EntityLookupHookContext hookContext{.results = {&gameEntitySystemPointer, findEntityClassForTest}};
    EntitySystem entitySystem{hookContext};

    EXPECT_EQ(&expectedEntityClass, entitySystem.findEntityClass("C_Test"));
    EXPECT_EQ(&gameEntitySystem, entityLookupCall.entitySystem);
    EXPECT_STREQ("C_Test", entityLookupCall.className);
    EXPECT_EQ(nullptr, entityLookupCall.output);
}

struct WeaponAncestryPatternSearchResults {
    cs2::CGameEntitySystem** entitySystemPointer{};
    FindEntityClassFunctionPointer::type findEntityClass{};
    std::uint8_t parentNodeOffset{};
    std::uint8_t parentOuterOffset{};
    std::uint8_t parentNodeOffsetConsistency{};

    template <typename T>
    [[nodiscard]] auto get() const noexcept
    {
        if constexpr (std::is_same_v<T, EntitySystemPointer>)
            return entitySystemPointer;
        else if constexpr (std::is_same_v<T, FindEntityClassFunctionPointer>)
            return findEntityClass;
        else if constexpr (std::is_same_v<T, EntityClassParentNodeOffset>)
            return EntityClassParentNodeOffset::type{parentNodeOffset};
        else if constexpr (std::is_same_v<T, EntityClassParentOuterOffset>)
            return EntityClassParentOuterOffset::type{parentOuterOffset};
        else
            return parentNodeOffsetConsistency;
    }
};

struct WeaponAncestryHookContext {
    WeaponAncestryPatternSearchResults results;

    [[nodiscard]] const WeaponAncestryPatternSearchResults& patternSearchResults() const noexcept
    {
        return results;
    }

    template <template <typename> typename T>
    [[nodiscard]] auto make() noexcept
    {
        return T<WeaponAncestryHookContext>{*this};
    }
};

cs2::CEntityClass* baseWeaponRegistrationForLookup{};

[[nodiscard]] cs2::CEntityClass* findBaseWeaponRegistrationForTest(cs2::CGameEntitySystem*, const char* className, cs2::CUtlString*) noexcept
{
    if (std::strcmp(className, "C_CSWeaponBase") == 0)
        return baseWeaponRegistrationForLookup;
    return nullptr;
}

void configureWeaponAncestryLookup(WeaponAncestryHookContext& hookContext, cs2::CGameEntitySystem*& entitySystem, std::uint8_t consistency) noexcept
{
    hookContext.results = {
        &entitySystem, findBaseWeaponRegistrationForTest, 0x60, 0x18, consistency
    };
}
}

TEST(EntityClassifierTest, RequiresMatchingWeaponAncestryConsistencyOffset) {
    EntityClassifier classifier;
    cs2::CGameEntitySystem gameEntitySystem;
    cs2::CGameEntitySystem* gameEntitySystemPointer = &gameEntitySystem;
    alignas(void*) cs2::CEntityClass baseWeaponRegistration;
    baseWeaponRegistrationForLookup = &baseWeaponRegistration;
    WeaponAncestryHookContext hookContext;
    configureWeaponAncestryLookup(hookContext, gameEntitySystemPointer, 0x60);

    EntityClassifierTestAccess::initializeWeaponAncestry(classifier, hookContext);
    EXPECT_TRUE(classifier.classifyEntity(&baseWeaponRegistration).isWeapon());
}

TEST(EntityClassifierTest, RejectsMismatchedOrMissingWeaponAncestryConsistencyOffset) {
    cs2::CGameEntitySystem gameEntitySystem;
    cs2::CGameEntitySystem* gameEntitySystemPointer = &gameEntitySystem;
    alignas(void*) cs2::CEntityClass baseWeaponRegistration;
    baseWeaponRegistrationForLookup = &baseWeaponRegistration;

    for (const auto consistency : {std::uint8_t{0x58}, std::uint8_t{0}}) {
        EntityClassifier classifier;
        WeaponAncestryHookContext hookContext;
        configureWeaponAncestryLookup(hookContext, gameEntitySystemPointer, consistency);
        EntityClassifierTestAccess::initializeWeaponAncestry(classifier, hookContext);
        EXPECT_FALSE(classifier.classifyEntity(&baseWeaponRegistration).isWeapon());
    }
}

TEST(EntityClassifierTest, ClassifiesWeaponRegistrationsByBoundedAncestry) {
    EntityClassifier classifier;
    alignas(void*) cs2::CEntityClass baseWeaponRegistration;
    TestParentNode oneHopParent{.owningOuter = &baseWeaponRegistration};
    TestRegistration oneHopRegistration{.parentNode = &oneHopParent};
    TestParentNode twoHopParent{.owningOuter = reinterpret_cast<cs2::CEntityClass*>(&oneHopRegistration)};
    TestRegistration twoHopRegistration{.parentNode = &twoHopParent};
    TestRegistration unrelatedRoot;

    EntityClassifierTestAccess::configureWeaponAncestry(classifier, &baseWeaponRegistration, 0x60, 0x18);

    EXPECT_TRUE(classifier.classifyEntity(&baseWeaponRegistration).isWeapon());
    EXPECT_TRUE(classifier.classifyEntity(reinterpret_cast<const cs2::CEntityClass*>(&oneHopRegistration)).isWeapon());
    EXPECT_TRUE(classifier.classifyEntity(reinterpret_cast<const cs2::CEntityClass*>(&twoHopRegistration)).isWeapon());
    EXPECT_FALSE(classifier.classifyEntity(reinterpret_cast<const cs2::CEntityClass*>(&unrelatedRoot)).isWeapon());
    EXPECT_FALSE(classifier.entityIs<cs2::C_CSWeaponBaseGun>(reinterpret_cast<const cs2::CEntityClass*>(&twoHopRegistration)));
}

TEST(EntityClassifierTest, ResolvesCommonWeaponBaseRegistrationFallback) {
    alignas(void*) cs2::CEntityClass commonBaseWeaponRegistration;
    TestParentNode gunParent{.owningOuter = &commonBaseWeaponRegistration};
    TestParentNode shotgunParent{.owningOuter = &commonBaseWeaponRegistration};
    TestRegistration gunRegistration{.parentNode = &gunParent};
    TestRegistration shotgunRegistration{.parentNode = &shotgunParent};

    EXPECT_EQ(&commonBaseWeaponRegistration, EntityClassifierTestAccess::findCommonWeaponBaseRegistration(
        reinterpret_cast<const cs2::CEntityClass*>(&gunRegistration),
        reinterpret_cast<const cs2::CEntityClass*>(&shotgunRegistration), 0x60, 0x18));
}

TEST(EntityClassifierTest, RejectsInvalidCommonWeaponBaseRegistrationFallback) {
    alignas(void*) cs2::CEntityClass firstBaseWeaponRegistration;
    alignas(void*) cs2::CEntityClass secondBaseWeaponRegistration;
    TestParentNode gunParent{.owningOuter = &firstBaseWeaponRegistration};
    TestParentNode shotgunParent{.owningOuter = &secondBaseWeaponRegistration};
    TestRegistration gunRegistration{.parentNode = &gunParent};
    TestRegistration shotgunRegistration{.parentNode = &shotgunParent};
    TestParentNode selfParent;
    TestRegistration selfLoop{.parentNode = &selfParent};
    selfParent.owningOuter = reinterpret_cast<cs2::CEntityClass*>(&selfLoop);

    EXPECT_EQ(nullptr, EntityClassifierTestAccess::findCommonWeaponBaseRegistration(
        reinterpret_cast<const cs2::CEntityClass*>(&gunRegistration),
        reinterpret_cast<const cs2::CEntityClass*>(&shotgunRegistration), 0x60, 0x18));
    EXPECT_EQ(nullptr, EntityClassifierTestAccess::findCommonWeaponBaseRegistration(
        nullptr, reinterpret_cast<const cs2::CEntityClass*>(&shotgunRegistration), 0x60, 0x18));
    EXPECT_EQ(nullptr, EntityClassifierTestAccess::findCommonWeaponBaseRegistration(
        reinterpret_cast<const cs2::CEntityClass*>(&selfLoop), reinterpret_cast<const cs2::CEntityClass*>(&shotgunRegistration), 0x60, 0x18));
}

TEST(EntityClassifierTest, RejectsNullAndCyclicWeaponRegistrationAncestry) {
    EntityClassifier classifier;
    alignas(void*) cs2::CEntityClass baseWeaponRegistration;
    TestRegistration nullParent;
    TestParentNode selfParent;
    TestRegistration selfLoop{.parentNode = &selfParent};
    selfParent.owningOuter = reinterpret_cast<cs2::CEntityClass*>(&selfLoop);
    TestParentNode firstParent;
    TestParentNode secondParent;
    TestRegistration first{.parentNode = &firstParent};
    TestRegistration second{.parentNode = &secondParent};
    firstParent.owningOuter = reinterpret_cast<cs2::CEntityClass*>(&second);
    secondParent.owningOuter = reinterpret_cast<cs2::CEntityClass*>(&first);

    EntityClassifierTestAccess::configureWeaponAncestry(classifier, &baseWeaponRegistration, 0x60, 0x18);

    EXPECT_FALSE(classifier.classifyEntity(nullptr).isWeapon());
    EXPECT_FALSE(classifier.classifyEntity(reinterpret_cast<const cs2::CEntityClass*>(&nullParent)).isWeapon());
    EXPECT_FALSE(classifier.classifyEntity(reinterpret_cast<const cs2::CEntityClass*>(&selfLoop)).isWeapon());
    EXPECT_FALSE(classifier.classifyEntity(reinterpret_cast<const cs2::CEntityClass*>(&first)).isWeapon());
}

TEST(EntityClassifierTest, RejectsWeaponRegistrationAncestryBeyondMaximumDepth) {
    EntityClassifier classifier;
    alignas(void*) cs2::CEntityClass baseWeaponRegistration;
    std::array<TestParentNode, 17> parents{};
    std::array<TestRegistration, 17> registrations{};
    for (std::size_t i = 0; i < registrations.size(); ++i) {
        registrations[i].parentNode = &parents[i];
        parents[i].owningOuter = i + 1 < registrations.size()
            ? reinterpret_cast<cs2::CEntityClass*>(&registrations[i + 1])
            : &baseWeaponRegistration;
    }

    EntityClassifierTestAccess::configureWeaponAncestry(classifier, &baseWeaponRegistration, 0x60, 0x18);
    EXPECT_FALSE(classifier.classifyEntity(reinterpret_cast<const cs2::CEntityClass*>(&registrations[0])).isWeapon());
}
#else
namespace
{
struct LinuxEntitySystemStorage {
    cs2::CGameEntitySystem::EntityClasses entityClasses;
};

struct LinuxEntityLookupPatternSearchResults {
    cs2::CGameEntitySystem** entitySystemPointer{};
    OffsetToEntityClasses::type entityClassesOffset{0};

    template <typename T>
    [[nodiscard]] auto get() const noexcept
    {
        if constexpr (std::is_same_v<T, EntitySystemPointer>)
            return entitySystemPointer;
        else
            return entityClassesOffset;
    }
};

struct LinuxEntityLookupHookContext {
    LinuxEntityLookupPatternSearchResults results;

    [[nodiscard]] const LinuxEntityLookupPatternSearchResults& patternSearchResults() const noexcept
    {
        return results;
    }
};
}

TEST(EntitySystemTest, UsesLinuxEntityClassMapLookup) {
    cs2::CEntityClass expectedEntityClass;
    cs2::Node<const char*, cs2::CEntityClass*, std::uint16_t> nodes[]{{.key = "C_Test", .value = &expectedEntityClass}};
    LinuxEntitySystemStorage storage{.entityClasses = {.size = 1, .allocationCount = 1, .memory = nodes, .root = 0, .numElements = 1}};
    auto* const gameEntitySystem = reinterpret_cast<cs2::CGameEntitySystem*>(&storage);
    LinuxEntityLookupHookContext hookContext{.results = {
        &gameEntitySystem,
        OffsetToEntityClasses::type{static_cast<std::int32_t>(offsetof(LinuxEntitySystemStorage, entityClasses) + offsetof(cs2::CGameEntitySystem::EntityClasses, memory))}
    }};
    EntitySystem entitySystem{hookContext};

    EXPECT_EQ(&expectedEntityClass, entitySystem.findEntityClass("C_Test"));
    EXPECT_EQ(nullptr, entitySystem.findEntityClass("C_Missing"));
    EXPECT_EQ(nullptr, entitySystem.findEntityClass(nullptr));
}
#endif
