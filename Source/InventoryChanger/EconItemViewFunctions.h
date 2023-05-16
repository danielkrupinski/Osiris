#pragma once

#include <cstdint>

#include <BytePatterns/ClientPatternFinder.h>
#include <CSGO/Helpers/EconItemViewFunctions.h>

[[nodiscard]] EconItemViewFunctions createEconItemViewFunctions(const ClientPatternFinder& clientPatternFinder);
