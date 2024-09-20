#pragma once

#include "C_BaseCSGrenadeProjectile.h"
#include <Platform/Macros/PlatformSpecific.h>

namespace cs2
{

struct C_HEGrenadeProjectile : C_BaseCSGrenadeProjectile {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_HEGrenadeProjectile@@", "21C_HEGrenadeProjectile")};
};

struct C_SmokeGrenadeProjectile : C_BaseCSGrenadeProjectile {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_SmokeGrenadeProjectile@@", "24C_SmokeGrenadeProjectile")};
};

struct C_MolotovProjectile : C_BaseCSGrenadeProjectile {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_MolotovProjectile@@", "19C_MolotovProjectile")};
};

struct C_FlashbangProjectile : C_BaseCSGrenadeProjectile {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVC_FlashbangProjectile@@", "21C_FlashbangProjectile")};
};

}
