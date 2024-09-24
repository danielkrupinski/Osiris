#pragma once

#include <array>
#include <tuple>
#include <type_traits>

#include <CS2/Classes/Entities/CBaseAnimGraph.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Classes/Entities/GrenadeProjectiles.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <CS2/Constants/EntityTypeNames.h>
#include <Platform/VmtFinder.h>
#include <Utils/TypeIndex.h>

using KnownEntityTypes = decltype(cs2::kEntityTypeNames)::TypeList;

struct EntitiesVMTs {
    EntitiesVMTs() = default;

    explicit EntitiesVMTs(const VmtFinder& clientVmtFinder) noexcept
    {
        cs2::kEntityTypeNames.forEach([i = 0u, &clientVmtFinder, this] (const auto typeName) mutable { vmts[i] = clientVmtFinder.findVmt(typeName); ++i; });
    }

    std::array<const void*, std::tuple_size_v<KnownEntityTypes>> vmts;
};
