#pragma once

#include "BombPlantSound.h"
#include "FootstepSound.h"
#include "SoundWatcherImpl.h"

using SoundWatcher = SoundWatcherImpl<FootstepSound, BombPlantSound>;
