#pragma once

#include <any>
#include <gmock/gmock.h>
#include <GameClient/ConVars/ConVarTypes.h>

struct MockCvarSystem {
    MOCK_METHOD(std::any, getConVarValue, (std::size_t conVarTypeIndex));

    template <typename ConVarType>
    [[nodiscard]] auto getConVarValue()
    {
        return std::any_cast<std::optional<typename ConVarType::ValueType>>(getConVarValue(ConVarTypes::indexOf<ConVarType>()));
    }
};

