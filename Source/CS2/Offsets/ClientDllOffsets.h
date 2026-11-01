#pragma once

#include <cstddef>

namespace cs2::client_dll_offsets
{

// CS2 1.41.9.2 (2026-10-23): RVA of the CCSGOInput* singleton pointer global.
// The input object itself is created by a factory at startup (constructor
// store at client.dll+0xb65dcf) and its address is stored here; every access
// in client.dll loads the pointer first (~1200 references). Angles therefore
// must be resolved indirectly via *reinterpret_cast<byte**>(clientDll + kCSGOInput).
inline constexpr std::ptrdiff_t kCSGOInput{0x2562E80};

// Offset of the slot-0 view angles (pitch, yaw, roll) inside the CCSGOInput
// object. Slots are strided by 0x928; the local player is slot 0.
// Verified against the current binary: movsd xmm0, [rax + r13*1 + 0x688]
// where r13 = input object and rax = slot * 0x928 (client.dll+0xb609eb).
inline constexpr std::ptrdiff_t kViewAnglesInInput{0x688};

// Do not write legacy global button RVAs here: they overlap input metadata.
// BHop uses the pawn movement-services button masks instead.
// kLocalPlayerPawn was removed: its 1.41.8.8 RVA went stale in 1.41.9.2 and a
// stale dereference can hand back a wrong-but-readable pointer. Features use
// the controller -> pawn-handle entity-system path instead.

}
