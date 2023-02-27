#pragma once

#include <cstdint>

#include <MemorySearch/PatternFinder.h>
#include <CSGO/Helpers/EconItemViewFunctions.h>

[[nodiscard]] EconItemViewFunctions createEconItemViewFunctions(const PatternFinder& clientPatternFinder);
