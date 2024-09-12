#pragma once

#include <tuple>
#include <type_traits>

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <Platform/VmtFinder.h>
#include <Utils/TypeIndex.h>

using KnownEntityTypes = std::tuple<
    cs2::CCSPlayerController,
    cs2::C_CSPlayerPawn>;

struct EntitiesVMTs {
    explicit EntitiesVMTs(const VmtFinder& clientVmtFinder) noexcept
    {
        initVmts(clientVmtFinder, std::type_identity<KnownEntityTypes>{});
    }

    [[nodiscard]] bool isPlayerPawn(const void* vmt) const noexcept
    {
        return vmt && vmt == getVmt<cs2::C_CSPlayerPawn>();
    }

private:
    template <typename... EntityTypes>
    void initVmts(const VmtFinder& clientVmtFinder, std::type_identity<std::tuple<EntityTypes...>>) noexcept
    {
        (initVmt<EntityTypes>(clientVmtFinder), ...);
    }

    template <typename EntityType>
    void initVmt(const VmtFinder& clientVmtFinder) noexcept
    {
        vmts[utils::typeIndex<EntityType, KnownEntityTypes>()] = clientVmtFinder.findVmt(EntityType::kMangledTypeName);
    }

    template <typename EntityType>
    [[nodiscard]] const void* getVmt() const noexcept
    {
        return vmts[utils::typeIndex<EntityType, KnownEntityTypes>()];
    }

    std::array<const void*, std::tuple_size_v<KnownEntityTypes>> vmts{};
};
