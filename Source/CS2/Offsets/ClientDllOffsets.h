#pragma once

#include <cstddef>

namespace cs2::client_dll_offsets
{

// CS2 1.41.8.8 (2026-10-02): input singleton confirmed by its constructor
// and getter in client.dll. GetViewAngles returns input + 0x688 for slot 0.
inline constexpr std::ptrdiff_t kCSGOInput{0x2576150};
// Historical dumper value; unused by the current controller-backed pawn path.
inline constexpr std::ptrdiff_t kLocalPlayerPawn{0x25606D8};
inline constexpr std::ptrdiff_t kViewAngles{kCSGOInput + 0x688};

// Do not write legacy global button RVAs here: in 1.41.8.8 they overlap
// input metadata. BHop uses the pawn movement-services button masks instead.

}
