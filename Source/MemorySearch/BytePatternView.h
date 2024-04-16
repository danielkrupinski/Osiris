#pragma once

#include <cstddef>
#include <span>

template <std::size_t Length>
using BytePatternView = std::span<const char, Length>;
