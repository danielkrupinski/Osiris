#pragma once

#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>

#include <CS2/Classes/Entities/C_BaseCSGrenadeProjectile.h>
#include <CS2/Classes/Entities/C_BaseModelEntity.h>
#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <CS2/Classes/EntitySystem/CEntityClass.h>
#include <CS2/Constants/EntityClasses.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <MemoryPatterns/PatternTypes/EntitySystemPatternTypes.h>
#include <Platform/Macros/IsPlatform.h>
#include <Utils/TypeIndex.h>

struct EntityTypeInfo {
    using Index = std::uint8_t;
    static_assert((std::numeric_limits<Index>::max)() >= std::tuple_size_v<cs2::EntityClasses>, "Index type is too small");

    Index typeIndex{(std::numeric_limits<Index>::max)()};

    [[nodiscard]] constexpr bool isWeapon() const noexcept
    {
        return EntityBaseTypeInfo<cs2::C_CSWeaponBase>::isBaseOf(typeIndex);
    }

    [[nodiscard]] constexpr bool isGrenadeProjectile() const noexcept
    {
        return EntityBaseTypeInfo<cs2::C_BaseCSGrenadeProjectile>::isBaseOf(typeIndex);
    }

    [[nodiscard]] constexpr bool isModelEntity() const noexcept
    {
        return EntityBaseTypeInfo<cs2::C_BaseModelEntity>::isBaseOf(typeIndex);
    }

    template <typename EntityType>
    [[nodiscard]] constexpr bool is() const noexcept
    {
        return typeIndex == indexOf<EntityType>();
    }

    template <typename EntityType>
    [[nodiscard]] static constexpr auto indexOf() noexcept
    {
        return utils::typeIndex<EntityType, cs2::EntityClasses>();
    }

private:
    template <typename BaseEntityType>
    struct EntityBaseTypeInfo {
        [[nodiscard]] static constexpr bool isBaseOf(Index typeIndex) noexcept
        {
            if (typeIndex < kIsBase.size())
                return kIsBase[typeIndex];
            return false;
        }

        static constexpr auto kIsBase{
            []<typename... EntityTypes>(std::type_identity<std::tuple<EntityTypes...>>) {
                return std::array<bool, sizeof...(EntityTypes)>{
                    std::is_base_of_v<BaseEntityType, EntityTypes>...
                };
            }(std::type_identity<cs2::EntityClasses>{})
        };
    };
};

class EntityClassifier {
public:
    static constexpr std::size_t kTrustedWeaponClassCapacity = 64;

    void init(auto& hookContext) noexcept
    {
        cs2::kEntityClassNames.forEach([i = 0u, &hookContext, this](const auto typeName) mutable {
            auto& entityClassSlot = entityClasses[i];
            if (entityClassSlot.load(std::memory_order_acquire)) {
                ++i;
                return;
            }

            const auto entityClass = hookContext.template make<EntitySystem>().findEntityClass(typeName);
            if (entityClass) {
                auto expectedEntityClass = static_cast<const cs2::CEntityClass*>(nullptr);
                entityClassSlot.compare_exchange_strong(expectedEntityClass, entityClass, std::memory_order_release, std::memory_order_acquire);
            }
            ++i;
        });

#if defined(_WIN64)
        initializeWeaponAncestry(hookContext);
#endif
    }

    template <typename EntityType>
    [[nodiscard]] bool entityIs(const cs2::CEntityClass* entityClass) const noexcept
    {
        return entityClass != nullptr
            && entityClass == entityClasses[utils::typeIndex<EntityType, cs2::EntityClasses>()].load(std::memory_order_acquire);
    }

    void registerWeaponClass(const cs2::CEntityClass* entityClass) noexcept
    {
        cacheWeaponClass(entityClass);
    }

    [[nodiscard]] EntityTypeInfo classifyEntity(const cs2::CEntityClass* entityClass) const noexcept
    {
        if (!entityClass)
            return {};

        for (std::size_t i = 0; i < entityClasses.size(); ++i) {
            if (entityClass == entityClasses[i].load(std::memory_order_acquire))
                return EntityTypeInfo{static_cast<EntityTypeInfo::Index>(i)};
        }

        if (isCachedWeaponClass(entityClass))
            return weaponTypeInfo();

#if defined(_WIN64)
        if (isWeaponRegistration(entityClass)) {
            cacheWeaponClass(entityClass);
            return weaponTypeInfo();
        }
#endif
        return {};
    }

private:
    static constexpr std::size_t kWeaponAncestryMaximumDepth = 16;

    [[nodiscard]] static constexpr EntityTypeInfo weaponTypeInfo() noexcept
    {
        return EntityTypeInfo{static_cast<EntityTypeInfo::Index>(EntityTypeInfo::indexOf<cs2::C_CSWeaponBaseGun>())};
    }

    void cacheWeaponClass(const cs2::CEntityClass* entityClass) const noexcept
    {
        if (!entityClass)
            return;

        for (auto& trustedWeaponClassSlot : trustedWeaponClasses) {
            const auto registeredEntityClass = trustedWeaponClassSlot.load(std::memory_order_acquire);
            if (registeredEntityClass == entityClass)
                return;

            if (!registeredEntityClass) {
                auto expectedEntityClass = static_cast<const cs2::CEntityClass*>(nullptr);
                if (trustedWeaponClassSlot.compare_exchange_strong(expectedEntityClass, entityClass, std::memory_order_release, std::memory_order_acquire))
                    return;
                if (expectedEntityClass == entityClass)
                    return;
            }
        }
    }

    [[nodiscard]] bool isCachedWeaponClass(const cs2::CEntityClass* entityClass) const noexcept
    {
        for (const auto& trustedWeaponClassSlot : trustedWeaponClasses) {
            if (entityClass == trustedWeaponClassSlot.load(std::memory_order_acquire))
                return true;
        }
        return false;
    }

#if defined(_WIN64)
    void initializeWeaponAncestry(auto& hookContext) noexcept
    {
        if (weaponAncestryReady.load(std::memory_order_acquire))
            return;

        const auto parentNodeOffset = hookContext.patternSearchResults().template get<EntityClassParentNodeOffset>().value();
        const auto parentOuterOffset = hookContext.patternSearchResults().template get<EntityClassParentOuterOffset>().value();
        const auto parentNodeOffsetConsistency = hookContext.patternSearchResults().template get<EntityClassParentNodeOffsetConsistency>();

        if (!areValidWeaponAncestryOffsets(parentNodeOffset, parentOuterOffset) || parentNodeOffsetConsistency != parentNodeOffset)
            return;

        const auto entitySystem = hookContext.template make<EntitySystem>();
        const auto directBaseWeaponRegistration = entitySystem.findEntityClass("C_CSWeaponBase");
        const auto baseWeaponRegistration = directBaseWeaponRegistration
            ? directBaseWeaponRegistration
            : findCommonWeaponBaseRegistration(
                entitySystem.findEntityClass("C_CSWeaponBaseGun"),
                entitySystem.findEntityClass("C_CSWeaponBaseShotgun"),
                parentNodeOffset, parentOuterOffset);
        if (!isValidWeaponAncestry(baseWeaponRegistration, parentNodeOffset, parentOuterOffset))
            return;

        weaponBaseRegistration.store(baseWeaponRegistration, std::memory_order_relaxed);
        weaponParentNodeOffset.store(parentNodeOffset, std::memory_order_relaxed);
        weaponParentOuterOffset.store(parentOuterOffset, std::memory_order_relaxed);
        weaponAncestryReady.store(true, std::memory_order_release);
    }

    [[nodiscard]] static bool isValidWeaponAncestry(const cs2::CEntityClass* baseWeaponRegistration, std::uint8_t parentNodeOffset, std::uint8_t parentOuterOffset) noexcept
    {
        return isAlignedPointer(baseWeaponRegistration) && areValidWeaponAncestryOffsets(parentNodeOffset, parentOuterOffset);
    }

    [[nodiscard]] static bool areValidWeaponAncestryOffsets(std::uint8_t parentNodeOffset, std::uint8_t parentOuterOffset) noexcept
    {
        return parentNodeOffset != 0 && parentNodeOffset % alignof(void*) == 0
            && parentOuterOffset != 0 && parentOuterOffset % alignof(void*) == 0;
    }

    [[nodiscard]] static bool isAlignedPointer(const void* pointer) noexcept
    {
        return pointer != nullptr && reinterpret_cast<std::uintptr_t>(pointer) % alignof(void*) == 0;
    }

    [[nodiscard]] static const cs2::CEntityClass* parentOuterRegistration(const cs2::CEntityClass* childRegistration, std::uint8_t parentNodeOffset, std::uint8_t parentOuterOffset) noexcept
    {
        if (!isAlignedPointer(childRegistration))
            return nullptr;

        const auto parentNode = EntityClassParentNodeOffset::type{parentNodeOffset}.of(const_cast<cs2::CEntityClass*>(childRegistration)).get();
        if (!parentNode || !isAlignedPointer(*parentNode))
            return nullptr;
        const auto parentOuter = EntityClassParentOuterOffset::type{parentOuterOffset}.of(*parentNode).get();
        if (!parentOuter || !isAlignedPointer(*parentOuter) || *parentOuter == childRegistration)
            return nullptr;
        return *parentOuter;
    }

    [[nodiscard]] static const cs2::CEntityClass* findCommonWeaponBaseRegistration(const cs2::CEntityClass* gunRegistration, const cs2::CEntityClass* shotgunRegistration, std::uint8_t parentNodeOffset, std::uint8_t parentOuterOffset) noexcept
    {
        if (!areValidWeaponAncestryOffsets(parentNodeOffset, parentOuterOffset)
            || !isAlignedPointer(gunRegistration) || !isAlignedPointer(shotgunRegistration))
            return nullptr;

        const auto gunParentOuter = parentOuterRegistration(gunRegistration, parentNodeOffset, parentOuterOffset);
        const auto shotgunParentOuter = parentOuterRegistration(shotgunRegistration, parentNodeOffset, parentOuterOffset);
        if (!gunParentOuter || gunParentOuter != shotgunParentOuter
            || gunParentOuter == gunRegistration || gunParentOuter == shotgunRegistration)
            return nullptr;
        return gunParentOuter;
    }

    [[nodiscard]] bool isWeaponRegistration(const cs2::CEntityClass* entityClass) const noexcept
    {
        if (!weaponAncestryReady.load(std::memory_order_acquire))
            return false;

        const auto baseWeaponRegistration = weaponBaseRegistration.load(std::memory_order_relaxed);
        const auto parentNodeOffset = weaponParentNodeOffset.load(std::memory_order_relaxed);
        const auto parentOuterOffset = weaponParentOuterOffset.load(std::memory_order_relaxed);
        if (!isValidWeaponAncestry(baseWeaponRegistration, parentNodeOffset, parentOuterOffset))
            return false;

        std::array<const cs2::CEntityClass*, kWeaponAncestryMaximumDepth> visited{};
        auto currentRegistration = entityClass;
        for (std::size_t depth = 0; depth < visited.size(); ++depth) {
            if (!currentRegistration)
                return false;
            if (currentRegistration == baseWeaponRegistration)
                return true;

            for (std::size_t i = 0; i < depth; ++i) {
                if (visited[i] == currentRegistration)
                    return false;
            }
            visited[depth] = currentRegistration;

            const auto parentNode = EntityClassParentNodeOffset::type{parentNodeOffset}.of(const_cast<cs2::CEntityClass*>(currentRegistration)).get();
            if (!parentNode || !*parentNode)
                return false;
            const auto parentOuter = EntityClassParentOuterOffset::type{parentOuterOffset}.of(*parentNode).get();
            if (!parentOuter)
                return false;
            currentRegistration = *parentOuter;
        }
        return false;
    }
#endif

    std::array<std::atomic<const cs2::CEntityClass*>, std::tuple_size_v<cs2::EntityClasses>> entityClasses{};
    mutable std::array<std::atomic<const cs2::CEntityClass*>, kTrustedWeaponClassCapacity> trustedWeaponClasses{};

#if defined(_WIN64)
    std::atomic<const cs2::CEntityClass*> weaponBaseRegistration{};
    std::atomic<std::uint8_t> weaponParentNodeOffset{};
    std::atomic<std::uint8_t> weaponParentOuterOffset{};
    std::atomic<bool> weaponAncestryReady{};
#endif

#if defined(__cpp_lib_atomic_is_always_lock_free) || defined(_MSC_VER)
    static_assert(std::atomic<const cs2::CEntityClass*>::is_always_lock_free);
#endif

    friend struct EntityClassifierTestAccess;
};
