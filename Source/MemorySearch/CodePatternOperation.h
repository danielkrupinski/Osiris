#pragma once

#include <cstdint>

enum class CodePatternOperation : std::uint8_t {
    None,
    Abs4,
    Abs5,
    Read
};
