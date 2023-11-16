#pragma once

#include "BombBeepSound.h"
#include "BombPlantSound.h"
#include "FootstepSound.h"
#include "SoundWatcherImpl.h"

using SoundWatcher = SoundWatcherImpl<FootstepSound, BombPlantSound, BombBeepSound>;
