#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <tuple>

#include <GameDependencies/EntitiesVMTs.h>
#include <Utils/TypeIndex.h>

struct EntityTypeInfo {
    std::uint8_t typeIndex;

    [[nodiscard]] constexpr bool isWeapon() const noexcept
    {
        return EntityBaseTypeInfo<cs2::C_CSWeaponBase>::isBaseOf(typeIndex);
    }

    [[nodiscard]] constexpr bool isGrenadeProjectile() const noexcept
    {
        return EntityBaseTypeInfo<cs2::C_BaseCSGrenadeProjectile>::isBaseOf(typeIndex);
    }

    template <typename EntityType>
    [[nodiscard]] constexpr bool is() const noexcept
    {
        return typeIndex == indexOf<EntityType>();
    }

    template <typename EntityType>
    [[nodiscard]] static constexpr auto indexOf() noexcept
    {
        return utils::typeIndex<EntityType, KnownEntityTypes>();
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
            }(std::type_identity<KnownEntityTypes>{})
        };
    };
};

class EntityClassifier {
public:
    EntityClassifier(const EntitiesVMTs& entitiesVMTs) noexcept
    {
        for (std::uint8_t i = 0; i < sortedVmtIndices.size(); ++i) {
            auto j = i;
            while (j > 0 && std::less{}(entitiesVMTs.vmts[i], entitiesVMTs.vmts[sortedVmtIndices[j - 1]])) {
                sortedVmtIndices[j] = sortedVmtIndices[j - 1];
                --j;
            }
            sortedVmtIndices[j] = i;
        }
    }

    [[nodiscard]] EntityTypeInfo classifyEntity(const EntitiesVMTs& entitiesVMTs, const void* vmt) noexcept
    {
        if (vmt == nullptr)
            return EntityTypeInfo{static_cast<std::uint8_t>(-1)};

        const auto it = std::ranges::lower_bound(sortedVmtIndices, vmt, {}, [&entitiesVMTs](const auto index) { return entitiesVMTs.vmts[index]; });
        if (it != sortedVmtIndices.end() && entitiesVMTs.vmts[*it] == vmt)
            return EntityTypeInfo{*it};
        return EntityTypeInfo{static_cast<std::uint8_t>(-1)};
    }

private:
    std::array<std::uint8_t, std::tuple_size_v<KnownEntityTypes>> sortedVmtIndices;
};
