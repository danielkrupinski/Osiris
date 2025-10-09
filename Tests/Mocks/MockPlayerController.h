#pragma once

#include <gmock/gmock.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <Utils/Optional.h>

struct MockPlayerController {
    MOCK_METHOD(Optional<cs2::CCSPlayerController::m_iCompTeammateColor>, playerColorIndex, ());
    MOCK_METHOD(MockBaseEntity&, pawn, ());
};
