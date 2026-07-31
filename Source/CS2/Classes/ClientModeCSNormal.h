#pragma once

#include <cstddef>

namespace cs2
{

struct CViewSetup; // forward

struct ClientModeCSNormal {
    using GetViewmodelFov = float(ClientModeCSNormal* thisptr);
    using OverrideView = void(ClientModeCSNormal* thisptr, CViewSetup* setup);
};

// CViewSetup — verified by disassembling the real OverrideView at
// client.dll RVA 0xC63230. The function copies origin to [rdi+0x4A0]
// and angles to [rdi+0x4B8] via a 12-byte Vector3 copier (RVA 0x6EDBDC).
#pragma pack(push, 1)
struct CViewSetup {
    std::byte pad_0000[0x4A0];
    float m_vecOrigin[3];       // 0x4A0 — camera world position (Vector3, 12 bytes)
    std::byte pad_04AC[0x0C];
    float m_angView[3];         // 0x4B8 — camera view angles (pitch, yaw, roll)
};
#pragma pack(pop)

}
