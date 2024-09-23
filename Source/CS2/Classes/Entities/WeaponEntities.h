#pragma once

#include <Platform/Macros/PlatformSpecific.h>
#include "C_CSWeaponBase.h"

namespace cs2
{

struct C_WeaponGlock : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponGlock@@", "13C_WeaponGlock")};
};

struct C_WeaponHKP2000 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponHKP2000@@", "15C_WeaponHKP2000")};
};

struct C_WeaponElite : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponElite@@", "13C_WeaponElite")};
};

struct C_WeaponP250 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponP250@@", "12C_WeaponP250")};
};

struct C_WeaponTec9 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponTec9@@", "12C_WeaponTec9")};
};

struct C_WeaponFiveSeven : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponFiveSeven@@", "17C_WeaponFiveSeven")};
};

struct C_DEagle : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_DEagle@@", "8C_DEagle")};
};

struct C_WeaponMAC10 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponMAC10@@", "13C_WeaponMAC10")};
};

struct C_WeaponMP7 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponMP7@@", "11C_WeaponMP7")};
};

struct C_WeaponMP9 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponMP9@@", "11C_WeaponMP9")};
};

struct C_WeaponUMP45 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponUMP45@@", "13C_WeaponUMP45")};
};

struct C_WeaponP90 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponP90@@", "11C_WeaponP90")};
};

struct C_WeaponBizon : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponBizon@@", "13C_WeaponBizon")};
};

struct C_WeaponMag7 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponMag7@@", "12C_WeaponMag7")};
};

struct C_WeaponSawedoff : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponSawedoff@@", "16C_WeaponSawedoff")};
};

struct C_WeaponXM1014 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponXM1014@@", "14C_WeaponXM1014")};
};

struct C_WeaponNOVA : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponNOVA@@", "12C_WeaponNOVA")};
};

struct C_WeaponM249 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponM249@@", "12C_WeaponM249")};
};

struct C_WeaponNegev : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponNegev@@", "13C_WeaponNegev")};
};

struct C_WeaponGalilAR : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponGalilAR@@", "15C_WeaponGalilAR")};
};

struct C_WeaponFamas : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponFamas@@", "13C_WeaponFamas")};
};

struct C_AK47 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_AK47@@", "6C_AK47")};
};

struct C_WeaponM4A1 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponM4A1@@", "12C_WeaponM4A1")};
};

struct C_WeaponSG556 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponSG556@@", "13C_WeaponSG556")};
};

struct C_WeaponAug : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponAug@@", "11C_WeaponAug")};
};

struct C_WeaponSSG08 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponSSG08@@", "13C_WeaponSSG08")};
};

struct C_WeaponAWP : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponAWP@@", "11C_WeaponAWP")};
};

struct C_WeaponG3SG1 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponG3SG1@@", "13C_WeaponG3SG1")};
};

struct C_WeaponSCAR20 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponSCAR20@@", "14C_WeaponSCAR20")};
};

struct C_WeaponTaser : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_WeaponTaser@@", "13C_WeaponTaser")};
};

struct C_Knife : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_Knife@@", "7C_Knife")};
};

struct C_Flashbang : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_Flashbang@@", "11C_Flashbang")};
};

struct C_HEGrenade : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_HEGrenade@@", "11C_HEGrenade")};
};

struct C_SmokeGrenade : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_SmokeGrenade@@", "14C_SmokeGrenade")};
};

struct C_MolotovGrenade : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_MolotovGrenade@@", "16C_MolotovGrenade")};
};

struct C_IncendiaryGrenade : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_IncendiaryGrenade@@", "19C_IncendiaryGrenade")};
};

struct C_DecoyGrenade : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_DecoyGrenade@@", "14C_DecoyGrenade")};
};

struct C_C4 : C_CSWeaponBase {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_C4@@", "4C_C4")};
};

}
