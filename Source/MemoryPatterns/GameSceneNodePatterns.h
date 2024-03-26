#pragma once

#include <cstdint>

#include <CS2/Classes/CGameSceneNode.h>
#include <GameClasses/OffsetTypes/GameSceneNodeOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

using OffsetToAbsOrigin = GameSceneNodeOffset<cs2::CGameSceneNode::m_vecAbsOrigin, std::int32_t>;

struct GameSceneNodePatterns {
    [[nodiscard]] OffsetToAbsOrigin offsetToAbsOrigin() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
