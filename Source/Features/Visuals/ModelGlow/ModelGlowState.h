#pragma once

#include <cstdint>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <Utils/EnumFlags.h>
#include "ModelGlowDeactivationFlags.h"

struct ModelGlowState {
    EnumFlags<ModelGlowDeactivationFlags> deactivationFlags{};

    std::uint64_t(*originalPlayerPawnSceneObjectUpdater)(cs2::C_CSPlayerPawn* playerPawn, void*, bool){nullptr};
    std::uint64_t(*originalWeaponSceneObjectUpdater)(cs2::C_CSWeaponBase* baseWeapon, void*, bool){nullptr};
};
