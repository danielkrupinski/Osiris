#pragma once

#include <Features/Hud/BombTimer/BombTimerConfigVariables.h>
#include <Features/Hud/DefusingAlert/DefusingAlertConfigVariables.h>
#include <Features/Hud/KillfeedPreserver/KillfeedPreserverConfigVariables.h>
#include <Features/Hud/PostRoundTimer/PostRoundTimerConfigVariables.h>
#include <Features/Sound/SoundVisualizationConfigVariables.h>
#include <Features/Visuals/ModelGlow/ModelGlowConfigVariables.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <Utils/TypeList.h>

using ConfigVariableTypes = TypeList<
    BombTimerEnabled,
    DefusingAlertEnabled,
    KillfeedPreserverEnabled,
    PostRoundTimerEnabled,
    BombBeepSoundVisualizationEnabled,
    BombDefuseSoundVisualizationEnabled,
    BombPlantSoundVisualizationEnabled,
    FootstepSoundVisualizationEnabled,
    WeaponReloadSoundVisualizationEnabled,
    WeaponScopeSoundVisualizationEnabled,
    ModelGlowEnabled,
    PlayerModelGlowEnabled,
    PlayerModelGlowColorMode,
    PlayerModelGlowOnlyEnemies,
    WeaponModelGlowEnabled,
    DroppedBombModelGlowEnabled,
    TickingBombModelGlowEnabled,
    DefuseKitModelGlowEnabled,
    GrenadeProjectileModelGlowEnabled,
    OutlineGlowEnabled,
    DefuseKitOutlineGlowEnabled,
    DroppedBombOutlineGlowEnabled,
    GrenadeProjectileOutlineGlowEnabled,
    HostageOutlineGlowEnabled,
    PlayerOutlineGlowEnabled,
    PlayerOutlineGlowOnlyEnemies,
    PlayerOutlineGlowColorMode,
    TickingBombOutlineGlowEnabled,
    WeaponOutlineGlowEnabled,
    PlayerInfoInWorldEnabled,
    PlayerInfoInWorldOnlyEnemies,
    PlayerInfoInWorldPlayerPositionArrowEnabled,
    PlayerInfoInWorldPlayerPositionArrowColorMode,
    PlayerInfoInWorldPlayerHealthEnabled,
    PlayerInfoInWorldPlayerHealthColorMode,
    PlayerInfoInWorldActiveWeaponIconEnabled,
    PlayerInfoInWorldActiveWeaponAmmoEnabled,
    PlayerInfoInWorldBombCarrierIconEnabled,
    PlayerInfoInWorldBombPlantIconEnabled,
    PlayerInfoInWorldBombDefuseIconEnabled,
    PlayerInfoInWorldHostagePickupIconEnabled,
    PlayerInfoInWorldHostageRescueIconEnabled,
    PlayerInfoInWorldBlindedIconEnabled
>;
