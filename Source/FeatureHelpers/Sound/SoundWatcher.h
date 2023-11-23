#pragma once

#include "BombBeepSound.h"
#include "BombDefuseSound.h"
#include "BombPlantSound.h"
#include "FootstepSound.h"
#include "SoundWatcherImpl.h"

using SoundWatcher = SoundWatcherImpl<FootstepSound, BombPlantSound, BombBeepSound, BombDefuseSound>;
