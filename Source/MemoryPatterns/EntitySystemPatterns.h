#pragma once

#include <cstdint>

#include <CS2/Classes/EntitySystem/CEntityIndex.h>
#include <CS2/Classes/EntitySystem/CConcreteEntityList.h>
#include <CS2/Classes/EntitySystem/CGameEntitySystem.h>
#include <GameClasses/OffsetTypes/EntitySystemOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

using HighestEntityIndexOffset = EntitySystemOffset<cs2::CEntityIndex, std::int32_t>;
using EntityListOffset = EntitySystemOffset<cs2::CConcreteEntityList, std::int8_t>;

struct EntitySystemPatterns {
    [[nodiscard]] cs2::CGameEntitySystem** entitySystem() const noexcept;
    [[nodiscard]] HighestEntityIndexOffset highestEntityIndexOffset() const noexcept;
    [[nodiscard]] EntityListOffset entityListOffset() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
