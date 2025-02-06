#pragma once

#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <CS2/Constants/EntityHandle.h>
#include "PlayerModelGlowPreviewColorMode.h"

struct PlayerModelGlowPreviewState {
    cs2::CEntityHandle previewPlayerHandleTT{cs2::INVALID_EHANDLE_INDEX};
    cs2::CEntityHandle previewPlayerHandleCT{cs2::INVALID_EHANDLE_INDEX};
    float animationProgress{0.0f};
    int previewPlayerHealth{-1};
    PlayerModelGlowPreviewColorMode colorMode{};
    cs2::CCSPlayerController::m_iCompTeammateColor previewPlayerColorIndex{};
    bool shouldUpdatePanel{true};
};
