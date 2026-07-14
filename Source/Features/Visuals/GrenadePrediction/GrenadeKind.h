#pragma once

#include <cstdint>

namespace cs2
{
enum class GrenadeKind : std::uint8_t {
    None,
    Flashbang,
    HEGrenade,
    SmokeGrenade,
    Molotov,
    Decoy,
    Incendiary
};
}
