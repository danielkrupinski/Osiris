#pragma once

#include <cstdint>

enum class MemoryValidationStatus : std::uint8_t {
    Unvalidated,
    Ok,
    Nok
};
