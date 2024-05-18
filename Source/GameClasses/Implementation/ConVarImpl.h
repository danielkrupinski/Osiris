#pragma once

#include <GameClasses/OffsetTypes/ConVarOffset.h>

struct ConVarImpl {
    template <typename ConVarPatterns>
    explicit ConVarImpl(const ConVarPatterns& conVarPatterns) noexcept
        : offsetToConVarValueType{conVarPatterns.offsetToConVarValueType()}
        , offsetToConVarValue{conVarPatterns.offsetToConVarValue()}
    {
    }

    OffsetToConVarValueType offsetToConVarValueType;
    OffsetToConVarValue offsetToConVarValue;
};
