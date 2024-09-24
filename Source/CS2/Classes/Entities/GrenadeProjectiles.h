#pragma once

#include "C_BaseCSGrenadeProjectile.h"
#include <Platform/Macros/PlatformSpecific.h>

namespace cs2
{

struct C_HEGrenadeProjectile : C_BaseCSGrenadeProjectile {
};

struct C_SmokeGrenadeProjectile : C_BaseCSGrenadeProjectile {
    using m_bDidSmokeEffect = bool;
};

struct C_MolotovProjectile : C_BaseCSGrenadeProjectile {
};

struct C_FlashbangProjectile : C_BaseCSGrenadeProjectile {
};

}
