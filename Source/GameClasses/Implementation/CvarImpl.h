#pragma once

#include <MemoryPatterns/CvarPatterns.h>

struct CvarImpl {
    explicit CvarImpl(const CvarPatterns& cvarPatterns) noexcept
        : cvar{cvarPatterns.cvar()}
        , offsetToConVarList{cvarPatterns.offsetToConVarList()}
    {
    }

    cs2::CCvar** cvar;
    OffsetToConVarList offsetToConVarList;
};
