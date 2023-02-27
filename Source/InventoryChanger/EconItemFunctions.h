#pragma once

#include <cstdint>

#include <MemorySearch/PatternFinder.h>
#include <CSGO/Helpers/EconItemFunctions.h>
#include <MemorySearch/BytePatternLiteral.h>

[[nodiscard]] EconItemFunctions createEconItemFunctions(const PatternFinder& clientPatternFinder);
