#pragma once

#include <cstdint>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/C_CSWeaponBase.h>

enum class PlayerModelGlowColorType : std::uint8_t {
    PlayerOrTeamColor,
    TeamColor,
    HealthBased
};

struct ModelGlowState {
    enum class State : std::uint8_t {
        Enabled,
        Disabling,
        Disabled
    };

    State masterSwitch{State::Disabled};
    State playerModelGlow{State::Enabled};
    PlayerModelGlowColorType playerModelGlowColorType{PlayerModelGlowColorType::PlayerOrTeamColor};
    bool showOnlyEnemies{true};
    State weaponModelGlow{State::Enabled};
    State droppedBombModelGlow{State::Enabled};
    State tickingBombModelGlow{State::Enabled};
    State defuseKitModelGlow{State::Enabled};
    State grenadeProjectileModelGlow{State::Enabled};

    std::uint64_t(*originalPlayerPawnSceneObjectUpdater)(cs2::C_CSPlayerPawn* playerPawn, void*, bool){nullptr};
    std::uint64_t(*originalWeaponSceneObjectUpdater)(cs2::C_CSWeaponBase* baseWeapon, void*, bool){nullptr};
};
