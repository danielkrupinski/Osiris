#pragma once

#include <cstdint>
#include <type_traits>

namespace cs2
{

enum class ConVarValueType : std::uint16_t {
    boolean,
    int16,
    uint16,
    int32,
    uint32,
    int64,
    uint64,
    float32,
    float64,
    string
};

struct ConVar {
    const char* name;

    using ValueType = std::underlying_type_t<ConVarValueType>;
    using Value = void;
};

}
