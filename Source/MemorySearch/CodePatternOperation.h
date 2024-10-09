#pragma once

#include <cstdint>

enum class CodePatternOperation : std::uint8_t {
    None,
    Abs,
    Read
};
