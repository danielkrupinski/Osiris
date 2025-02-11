#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <limits>
#include <tuple>
#include <type_traits>

#include <CS2/Classes/Entities/C_BaseCSGrenadeProjectile.h>
#include <CS2/Classes/Entities/C_BaseModelEntity.h>
#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <CS2/Classes/EntitySystem/CEntityClass.h>
#include <CS2/Constants/EntityClasses.h>
#include <GameClient/EntitySystem/EntitySystem.h>
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
    void init(auto& hookContext) noexcept
    {
        cs2::kEntityClassNames.forEach([i = 0u, &hookContext, this](const auto typeName) mutable {
            const auto entityClass = hookContext.template make<EntitySystem>().findEntityClass(typeName);
            auto j = i;
            while (j > 0 && std::less{}(entityClass, entityClasses[j - 1])) {
                entityClasses[j] = entityClasses[j - 1];
                entityClassIndexToTypeIndex[j] = entityClassIndexToTypeIndex[j - 1];
                --j;
            }
            entityClasses[j] = entityClass;
            entityClassIndexToTypeIndex[j] = static_cast<EntityTypeInfo::Index>(i);
            ++i;
        });

        for (std::size_t i = 0; i < std::tuple_size_v<cs2::EntityClasses>; ++i)
            entityTypeIndexToClassIndex[entityClassIndexToTypeIndex[i]] = static_cast<EntityTypeInfo::Index>(i);
    }

    template <typename EntityType>
    [[nodiscard]] bool entityIs(const cs2::CEntityClass* entityClass) const noexcept
    {
        return entityClass != nullptr && entityClass == entityClasses[entityTypeIndexToClassIndex[utils::typeIndex<EntityType, cs2::EntityClasses>()]];
    }

    [[nodiscard]] EntityTypeInfo classifyEntity(const cs2::CEntityClass* entityClass) const noexcept
    {
        if (entityClass == nullptr)
            return {};

        const auto it = std::ranges::lower_bound(entityClasses, entityClass);
        if (it != entityClasses.end() && *it == entityClass)
            return EntityTypeInfo{entityClassIndexToTypeIndex[std::distance(entityClasses.begin(), it)]};
        return {};
    }

private:
    std::array<const cs2::CEntityClass*, std::tuple_size_v<cs2::EntityClasses>> entityClasses;
    std::array<EntityTypeInfo::Index, std::tuple_size_v<cs2::EntityClasses>> entityTypeIndexToClassIndex;
    std::array<EntityTypeInfo::Index, std::tuple_size_v<cs2::EntityClasses>> entityClassIndexToTypeIndex;
};
