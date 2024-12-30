#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <tuple>

#include <CS2/Constants/EntityClasses.h>
#include <GameClasses/EntitySystem.h>
#include <Utils/TypeIndex.h>

struct EntityTypeInfo {
    std::uint8_t typeIndex{static_cast<std::uint8_t>(-1)};

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
        [[nodiscard]] static constexpr bool isBaseOf(std::uint8_t typeIndex) noexcept
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
    void init(auto& hookContext) noexcept
    {
        cs2::kEntityClassNames.forEach([i = 0u, &hookContext, this](const auto typeName) mutable { entityClasses[i] = hookContext.template make<EntitySystem>().findEntityClass(typeName); ++i; });
    
        for (std::uint8_t i = 0; i < sortedEntityClassIndices.size(); ++i) {
            auto j = i;
            while (j > 0 && std::less{}(entityClasses[i], entityClasses[sortedEntityClassIndices[j - 1]])) {
                sortedEntityClassIndices[j] = sortedEntityClassIndices[j - 1];
                --j;
            }
            sortedEntityClassIndices[j] = i;
        }
    }

    template <typename EntityType>
    [[nodiscard]] bool entityIs(const cs2::CEntityClass* entityClass) const noexcept
    {
        return entityClass != nullptr && entityClass == entityClasses[utils::typeIndex<EntityType, cs2::EntityClasses>()];
    }

    [[nodiscard]] EntityTypeInfo classifyEntity(const cs2::CEntityClass* entityClass) const noexcept
    {
        if (entityClass == nullptr)
            return {};

        const auto it = std::ranges::lower_bound(sortedEntityClassIndices, entityClass, {}, [this](const auto index) { return entityClasses[index]; });
        if (it != sortedEntityClassIndices.end() && entityClasses[*it] == entityClass)
            return EntityTypeInfo{*it};
        return {};
    }

private:
    std::array<const cs2::CEntityClass*, std::tuple_size_v<cs2::EntityClasses>> entityClasses;
    std::array<std::uint8_t, std::tuple_size_v<cs2::EntityClasses>> sortedEntityClassIndices;
};
