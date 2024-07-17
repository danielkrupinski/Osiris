#pragma once

#include <GameClasses/OffsetTypes/ConVarOffset.h>

struct ConVarDeps {
    template <typename ConVarPatterns>
    explicit ConVarDeps(const ConVarPatterns& conVarPatterns) noexcept
        : offsetToConVarValueType{conVarPatterns.offsetToConVarValueType()}
        , offsetToConVarValue{conVarPatterns.offsetToConVarValue()}
    {
    }

    OffsetToConVarValueType offsetToConVarValueType;
    OffsetToConVarValue offsetToConVarValue;
};
