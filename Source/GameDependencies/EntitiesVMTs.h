#pragma once

#include <array>
#include <tuple>
#include <type_traits>

#include <CS2/Classes/Entities/CBaseAnimGraph.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Classes/Entities/GrenadeProjectiles.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <Platform/VmtFinder.h>
#include <Utils/TypeIndex.h>

using KnownEntityTypes = std::tuple<
    cs2::C_CSPlayerPawn,
    cs2::C_WeaponGlock,
    cs2::C_WeaponHKP2000,
    cs2::C_WeaponElite,
    cs2::C_WeaponP250,
    cs2::C_WeaponTec9,
    cs2::C_WeaponFiveSeven,
    cs2::C_DEagle,

    cs2::C_WeaponMAC10,
    cs2::C_WeaponMP7,
    cs2::C_WeaponMP9,
    cs2::C_WeaponBizon,
    cs2::C_WeaponP90,
    cs2::C_WeaponUMP45,

    cs2::C_WeaponMag7,
    cs2::C_WeaponSawedoff,
    cs2::C_WeaponXM1014,
    cs2::C_WeaponNOVA,
    cs2::C_WeaponM249,
    cs2::C_WeaponNegev,

    cs2::C_WeaponGalilAR,
    cs2::C_WeaponFamas,
    cs2::C_AK47,
    cs2::C_WeaponM4A1,
    cs2::C_WeaponSG556,
    cs2::C_WeaponAug,
    cs2::C_WeaponSSG08,
    cs2::C_WeaponAWP,
    cs2::C_WeaponG3SG1,
    cs2::C_WeaponSCAR20,

    cs2::C_WeaponTaser,
    cs2::C_Knife,

    cs2::C_Flashbang,
    cs2::C_HEGrenade,
    cs2::C_SmokeGrenade,
    cs2::C_MolotovGrenade,
    cs2::C_IncendiaryGrenade,
    cs2::C_DecoyGrenade,

    cs2::CBaseAnimGraph,

    cs2::C_HEGrenadeProjectile,
    cs2::C_SmokeGrenadeProjectile,
    cs2::C_MolotovProjectile,
    cs2::C_FlashbangProjectile,

    cs2::C_C4
>;

struct EntitiesVMTs {
    EntitiesVMTs() = default;

    explicit EntitiesVMTs(const VmtFinder& clientVmtFinder) noexcept
    {
        initVmts(clientVmtFinder, std::type_identity<KnownEntityTypes>{});
    }

    std::array<const void*, std::tuple_size_v<KnownEntityTypes>> vmts{};

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
};
