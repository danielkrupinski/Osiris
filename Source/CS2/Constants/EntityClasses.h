#pragma once

#include <CS2/Classes/Entities/CBaseAnimGraph.h>
#include <CS2/Classes/Entities/C_CSGO_PreviewPlayer.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/C_Hostage.h>
#include <CS2/Classes/Entities/GrenadeProjectiles.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <CS2/Classes/CPlantedC4.h>
#include <Utils/TypedStaticStringPool.h>

namespace cs2
{

constexpr auto kEntityClassNames = TypedStaticStringPool{}
    .add<C_CSPlayerPawn>("C_CSPlayerPawn")
    .add<C_WeaponGlock>("C_WeaponGlock")
    .add<C_WeaponHKP2000>("C_WeaponHKP2000")
    .add<C_WeaponElite>("C_WeaponElite")
    .add<C_WeaponP250>("C_WeaponP250")
    .add<C_WeaponTec9>("C_WeaponTec9")
    .add<C_WeaponFiveSeven>("C_WeaponFiveSeven")
    .add<C_DEagle>("C_DEagle")
    .add<C_WeaponMAC10>("C_WeaponMAC10")
    .add<C_WeaponMP7>("C_WeaponMP7")
    .add<C_WeaponMP9>("C_WeaponMP9")
    .add<C_WeaponBizon>("C_WeaponBizon")
    .add<C_WeaponP90>("C_WeaponP90")
    .add<C_WeaponUMP45>("C_WeaponUMP45")
    .add<C_WeaponMag7>("C_WeaponMag7")
    .add<C_WeaponSawedoff>("C_WeaponSawedoff")
    .add<C_WeaponXM1014>("C_WeaponXM1014")
    .add<C_WeaponNOVA>("C_WeaponNOVA")
    .add<C_WeaponM249>("C_WeaponM249")
    .add<C_WeaponNegev>("C_WeaponNegev")
    .add<C_WeaponGalilAR>("C_WeaponGalilAR")
    .add<C_WeaponFamas>("C_WeaponFamas")
    .add<C_AK47>("C_AK47")
    .add<C_WeaponM4A1>("C_WeaponM4A1")
    .add<C_WeaponSG556>("C_WeaponSG556")
    .add<C_WeaponAug>("C_WeaponAug")
    .add<C_WeaponSSG08>("C_WeaponSSG08")
    .add<C_WeaponAWP>("C_WeaponAWP")
    .add<C_WeaponG3SG1>("C_WeaponG3SG1")
    .add<C_WeaponSCAR20>("C_WeaponSCAR20")
    .add<C_WeaponTaser>("C_WeaponTaser")
    .add<C_Knife>("C_Knife")
    .add<C_Flashbang>("C_Flashbang")
    .add<C_HEGrenade>("C_HEGrenade")
    .add<C_SmokeGrenade>("C_SmokeGrenade")
    .add<C_MolotovGrenade>("C_MolotovGrenade")
    .add<C_IncendiaryGrenade>("C_IncendiaryGrenade")
    .add<C_DecoyGrenade>("C_DecoyGrenade")
    .add<CBaseAnimGraph>("CBaseAnimGraph")
    .add<C_HEGrenadeProjectile>("C_HEGrenadeProjectile")
    .add<C_SmokeGrenadeProjectile>("C_SmokeGrenadeProjectile")
    .add<C_MolotovProjectile>("C_MolotovProjectile")
    .add<C_FlashbangProjectile>("C_FlashbangProjectile")
    .add<C_C4>("C_C4")
    .add<CPlantedC4>("C_PlantedC4")
    .add<C_Hostage>("C_Hostage")
    .add<C_Item_Healthshot>("C_Item_Healthshot")
    .add<C_CSGO_PreviewPlayer>("C_CSGO_PreviewPlayer");

using EntityClasses = decltype(kEntityClassNames)::TypeList;

}
