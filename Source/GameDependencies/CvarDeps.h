#pragma once

#include <GameClasses/OffsetTypes/CvarOffset.h>

struct CvarDeps {
    CvarDeps() = default;

    template <typename CvarPatterns>
    explicit CvarDeps(const CvarPatterns& cvarPatterns) noexcept
        : offsetToConVarList{cvarPatterns.offsetToConVarList()}
    {
    }

    OffsetToConVarList offsetToConVarList;
};
