#pragma once

struct WeaponModelGlowPreviewState {
    cs2::CEntityHandle previewWeaponHandle{cs2::INVALID_EHANDLE_INDEX};
    cs2::CEntityHandle previewFlashbangHandle{cs2::INVALID_EHANDLE_INDEX};
    cs2::CEntityHandle previewHEGrenadeHandle{cs2::INVALID_EHANDLE_INDEX};
    cs2::CEntityHandle previewSmokeGrenadeHandle{cs2::INVALID_EHANDLE_INDEX};
    cs2::CEntityHandle previewIncendiaryHandle{cs2::INVALID_EHANDLE_INDEX};
};
