#pragma once

#include <cstdint>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/C_CSWeaponBase.h>

struct ModelGlowState {
    bool modelGlowDisabling{false};
    bool playerModelGlowDisabling{false};
    bool weaponModelGlowDisabling{false};
    bool droppedBombModelGlowDisabling{false};
    bool tickingBombModelGlowDisabling{false};
    bool defuseKitModelGlowDisabling{false};
    bool grenadeProjectileModelGlowDisabling{false};

    std::uint64_t(*originalPlayerPawnSceneObjectUpdater)(cs2::C_CSPlayerPawn* playerPawn, void*, bool){nullptr};
    std::uint64_t(*originalWeaponSceneObjectUpdater)(cs2::C_CSWeaponBase* baseWeapon, void*, bool){nullptr};
};
