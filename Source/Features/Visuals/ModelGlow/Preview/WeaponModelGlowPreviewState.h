#pragma once

#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <CS2/Constants/EntityHandle.h>

struct WeaponModelGlowPreviewState {
    cs2::CEntityHandle previewWeaponHandle{cs2::INVALID_EHANDLE_INDEX};
    cs2::CEntityHandle previewFlashbangHandle{cs2::INVALID_EHANDLE_INDEX};
    cs2::CEntityHandle previewHEGrenadeHandle{cs2::INVALID_EHANDLE_INDEX};
    cs2::CEntityHandle previewSmokeGrenadeHandle{cs2::INVALID_EHANDLE_INDEX};
    cs2::CEntityHandle previewIncendiaryHandle{cs2::INVALID_EHANDLE_INDEX};
    bool hadPreviewWeaponHandle{false};
    bool hadPreviewFlashbangHandle{false};
    bool hadPreviewHEGrenadeHandle{false};
    bool hadPreviewSmokeGrenadeHandle{false};
    bool hadPreviewIncendiaryHandle{false};
};
