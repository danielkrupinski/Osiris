#pragma once

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/C_Hostage.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <CS2/Classes/CPlantedC4.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Utils/TypedStaticStringPool.h>

namespace cs2
{

constexpr auto kEntityTypeNames = TypedStaticStringPool{}
    .add<C_CSPlayerPawn>(WIN64_LINUX(".?AVC_CSPlayerPawn@@", "14C_CSPlayerPawn"))
    .add<C_WeaponGlock>(WIN64_LINUX(".?AVC_WeaponGlock@@", "13C_WeaponGlock"))
    .add<C_WeaponHKP2000>(WIN64_LINUX(".?AVC_WeaponHKP2000@@", "15C_WeaponHKP2000"))
    .add<C_WeaponElite>(WIN64_LINUX(".?AVC_WeaponElite@@", "13C_WeaponElite"))
    .add<C_WeaponP250>(WIN64_LINUX(".?AVC_WeaponP250@@", "12C_WeaponP250"))
    .add<C_WeaponTec9>(WIN64_LINUX(".?AVC_WeaponTec9@@", "12C_WeaponTec9"))
    .add<C_WeaponFiveSeven>(WIN64_LINUX(".?AVC_WeaponFiveSeven@@", "17C_WeaponFiveSeven"))
    .add<C_DEagle>(WIN64_LINUX(".?AVC_DEagle@@", "8C_DEagle"))
    .add<C_WeaponMAC10>(WIN64_LINUX(".?AVC_WeaponMAC10@@", "13C_WeaponMAC10"))
    .add<C_WeaponMP7>(WIN64_LINUX(".?AVC_WeaponMP7@@", "11C_WeaponMP7"))
    .add<C_WeaponMP9>(WIN64_LINUX(".?AVC_WeaponMP9@@", "11C_WeaponMP9"))
    .add<C_WeaponBizon>(WIN64_LINUX(".?AVC_WeaponBizon@@", "13C_WeaponBizon"))
    .add<C_WeaponP90>(WIN64_LINUX(".?AVC_WeaponP90@@", "11C_WeaponP90"))
    .add<C_WeaponUMP45>(WIN64_LINUX(".?AVC_WeaponUMP45@@", "13C_WeaponUMP45"))
    .add<C_WeaponMag7>(WIN64_LINUX(".?AVC_WeaponMag7@@", "12C_WeaponMag7"))
    .add<C_WeaponSawedoff>(WIN64_LINUX(".?AVC_WeaponSawedoff@@", "16C_WeaponSawedoff"))
    .add<C_WeaponXM1014>(WIN64_LINUX(".?AVC_WeaponXM1014@@", "14C_WeaponXM1014"))
    .add<C_WeaponNOVA>(WIN64_LINUX(".?AVC_WeaponNOVA@@", "12C_WeaponNOVA"))
    .add<C_WeaponM249>(WIN64_LINUX(".?AVC_WeaponM249@@", "12C_WeaponM249"))
    .add<C_WeaponNegev>(WIN64_LINUX(".?AVC_WeaponNegev@@", "13C_WeaponNegev"))
    .add<C_WeaponGalilAR>(WIN64_LINUX(".?AVC_WeaponGalilAR@@", "15C_WeaponGalilAR"))
    .add<C_WeaponFamas>(WIN64_LINUX(".?AVC_WeaponFamas@@", "13C_WeaponFamas"))
    .add<C_AK47>(WIN64_LINUX(".?AVC_AK47@@", "6C_AK47"))
    .add<C_WeaponM4A1>(WIN64_LINUX(".?AVC_WeaponM4A1@@", "12C_WeaponM4A1"))
    .add<C_WeaponSG556>(WIN64_LINUX(".?AVC_WeaponSG556@@", "13C_WeaponSG556"))
    .add<C_WeaponAug>(WIN64_LINUX(".?AVC_WeaponAug@@", "11C_WeaponAug"))
    .add<C_WeaponSSG08>(WIN64_LINUX(".?AVC_WeaponSSG08@@", "13C_WeaponSSG08"))
    .add<C_WeaponAWP>(WIN64_LINUX(".?AVC_WeaponAWP@@", "11C_WeaponAWP"))
    .add<C_WeaponG3SG1>(WIN64_LINUX(".?AVC_WeaponG3SG1@@", "13C_WeaponG3SG1"))
    .add<C_WeaponSCAR20>(WIN64_LINUX(".?AVC_WeaponSCAR20@@", "14C_WeaponSCAR20"))
    .add<C_WeaponTaser>(WIN64_LINUX(".?AVC_WeaponTaser@@", "13C_WeaponTaser"))
    .add<C_Knife>(WIN64_LINUX(".?AVC_Knife@@", "7C_Knife"))
    .add<C_Flashbang>(WIN64_LINUX(".?AVC_Flashbang@@", "11C_Flashbang"))
    .add<C_HEGrenade>(WIN64_LINUX(".?AVC_HEGrenade@@", "11C_HEGrenade"))
    .add<C_SmokeGrenade>(WIN64_LINUX(".?AVC_SmokeGrenade@@", "14C_SmokeGrenade"))
    .add<C_MolotovGrenade>(WIN64_LINUX(".?AVC_MolotovGrenade@@", "16C_MolotovGrenade"))
    .add<C_IncendiaryGrenade>(WIN64_LINUX(".?AVC_IncendiaryGrenade@@", "19C_IncendiaryGrenade"))
    .add<C_DecoyGrenade>(WIN64_LINUX(".?AVC_DecoyGrenade@@", "14C_DecoyGrenade"))
    .add<CBaseAnimGraph>(WIN64_LINUX(".?AVCBaseAnimGraph@@", "14CBaseAnimGraph"))
    .add<C_HEGrenadeProjectile>(WIN64_LINUX(".?AVC_HEGrenadeProjectile@@", "21C_HEGrenadeProjectile"))
    .add<C_SmokeGrenadeProjectile>(WIN64_LINUX(".?AVC_SmokeGrenadeProjectile@@", "24C_SmokeGrenadeProjectile"))
    .add<C_MolotovProjectile>(WIN64_LINUX(".?AVC_MolotovProjectile@@", "19C_MolotovProjectile"))
    .add<C_FlashbangProjectile>(WIN64_LINUX(".?AVC_FlashbangProjectile@@", "21C_FlashbangProjectile"))
    .add<C_C4>(WIN64_LINUX(".?AVC_C4@@", "4C_C4"))
    .add<CPlantedC4>(WIN64_LINUX(".?AVC_PlantedC4@@", "11C_PlantedC4"))
    .add<C_Hostage>(WIN64_LINUX(".?AVC_Hostage@@", "9C_Hostage"));

}
