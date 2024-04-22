#pragma once

#include <MemoryPatterns/ConVarPatterns.h>

struct ConVarImpl {
    explicit ConVarImpl(const ConVarPatterns& conVarPatterns) noexcept
        : offsetToConVarValueType{conVarPatterns.offsetToConVarValueType()}
        , offsetToConVarValue{conVarPatterns.offsetToConVarValue()}
    {
    }

    OffsetToConVarValueType offsetToConVarValueType;
    OffsetToConVarValue offsetToConVarValue;
};
