#pragma once

#include <GameClasses/OffsetTypes/CvarOffset.h>

struct CvarDeps {
    CvarDeps() = default;

    template <typename CvarPatterns>
    explicit CvarDeps(const CvarPatterns& cvarPatterns) noexcept
        : cvar{cvarPatterns.cvar()}
        , offsetToConVarList{cvarPatterns.offsetToConVarList()}
    {
    }

    cs2::CCvar** cvar;
    OffsetToConVarList offsetToConVarList;
};
