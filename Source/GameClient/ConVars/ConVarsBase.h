#pragma once

#include <array>
#include <tuple>

#include <CS2/Classes/ConVar.h>
#include <Utils/TypeIndex.h>
#include "ConVarTypes.h"

struct ConVarsBase {
    explicit ConVarsBase(auto&& cvarSystem) noexcept
    {
        ConVarTypes::forEach([i = std::size_t{0}, this, &cvarSystem] <typename ConVar> (std::type_identity<ConVar>) mutable {
            conVars[i++] = cvarSystem.findConVar(ConVar::kName);
        });
    }

    template <typename ConVar>
    [[nodiscard]] cs2::ConVar* getConVar() const noexcept
    {
        return conVars[ConVarTypes::indexOf<ConVar>()];
    }

private:
    std::array<cs2::ConVar*, ConVarTypes::size()> conVars;
};
