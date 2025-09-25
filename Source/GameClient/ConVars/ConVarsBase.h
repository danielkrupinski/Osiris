#pragma once

#include <array>
#include <tuple>

#include <CS2/Classes/ConVar.h>
#include <Utils/TypeIndex.h>

template <typename... ConVarTypes>
struct ConVarsBase {
    explicit ConVarsBase(const auto& cvarSystem) noexcept
        : conVars{cvarSystem.findConVar(ConVarTypes::kName)...}
    {
    }

    template <typename ConVar>
    [[nodiscard]] cs2::ConVar* getConVar() const noexcept
    {
        return conVars[utils::typeIndex<ConVar, std::tuple<ConVarTypes...>>()];
    }

private:
    std::array<cs2::ConVar*, sizeof...(ConVarTypes)> conVars;
};
