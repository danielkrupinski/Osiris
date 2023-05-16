#pragma once

#include <cstdint>

#include <CSGO/Helpers/EconItemFunctions.h>
#include <BytePatterns/ClientPatternFinder.h>

[[nodiscard]] EconItemFunctions createEconItemFunctions(const ClientPatternFinder& clientPatternFinder);
