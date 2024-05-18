#pragma once

#include <GameClasses/OffsetTypes/CvarOffset.h>

struct CvarImpl {
    template <typename CvarPatterns>
    explicit CvarImpl(const CvarPatterns& cvarPatterns) noexcept
        : cvar{cvarPatterns.cvar()}
        , offsetToConVarList{cvarPatterns.offsetToConVarList()}
    {
    }

    cs2::CCvar** cvar;
    OffsetToConVarList offsetToConVarList;
};
