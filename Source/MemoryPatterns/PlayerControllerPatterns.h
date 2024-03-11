#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <GameClasses/OffsetTypes/PlayerControllerOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

using OffsetToPlayerPawnHandle = PlayerControllerOffset<cs2::CCSPlayerController::m_hPawn, std::int32_t>;

struct PlayerControllerPatterns {
    [[nodiscard]] OffsetToPlayerPawnHandle offsetToPlayerPawnHandle() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
