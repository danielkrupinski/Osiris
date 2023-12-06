#pragma once

#include "BombBeepSound.h"
#include "BombDefuseSound.h"
#include "BombPlantSound.h"
#include "FootstepSound.h"
#include "SoundWatcherImpl.h"
#include "WeaponReloadSound.h"
#include "WeaponScopeSound.h"

using SoundWatcher = SoundWatcherImpl<
    FootstepSound,
    BombPlantSound,
    BombBeepSound,
    BombDefuseSound,
    WeaponScopeSound,
    WeaponReloadSound
>;
