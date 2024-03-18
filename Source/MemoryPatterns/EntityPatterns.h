#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <GameClasses/OffsetTypes/EntityOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

using OffsetToGameSceneNode = EntityOffset<cs2::C_BaseEntity::m_pGameSceneNode, std::int32_t>;
using OffsetToHealth = EntityOffset<cs2::C_BaseEntity::m_iHealth, std::int32_t>;
using OffsetToTeamNumber = EntityOffset<cs2::C_BaseEntity::m_iTeamNum, std::int32_t>;

struct EntityPatterns {
    [[nodiscard]] OffsetToGameSceneNode offsetToGameSceneNode() const noexcept;
    [[nodiscard]] OffsetToHealth offsetToHealth() const noexcept;
    [[nodiscard]] OffsetToTeamNumber offsetToTeamNumber() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
