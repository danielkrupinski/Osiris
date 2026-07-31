#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

// Persistent state for BoneDumper. Buffer lives here (not on stack) so the
// dumper doesn't blow the 4KB MSVC stack-probe threshold each frame.
struct BoneDumperState {
    static constexpr std::size_t kBufferCapacity = 4096;

    float lastDumpCurtime{0.0f};        // throttle gate
    std::uint64_t frameCounter{0};       // monotonic dump id
    std::array<char, kBufferCapacity> buffer{}; // JSON staging
};
