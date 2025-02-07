#include <Features/Sound/Details/BombBeepSound.h>
#include <Features/Sound/Details/BombDefuseSound.h>
#include <Features/Sound/Details/BombPlantSound.h>
#include <Features/Sound/Details/FootstepSound.h>
#include <Features/Sound/Details/WeaponReloadSound.h>
#include <Features/Sound/Details/WeaponScopeSound.h>

#include "SoundWatcherImplState.h"

using SoundWatcherState = SoundWatcherImplState<
    FootstepSound,
    BombPlantSound,
    BombBeepSound,
    BombDefuseSound,
    WeaponScopeSound,
    WeaponReloadSound
>;
