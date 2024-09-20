#pragma once

#include "SoundWatcherImpl.h"
#include "SoundWatcherState.h"

template <typename HookContext>
using SoundWatcher = SoundWatcherImpl<HookContext, SoundWatcherState>;
