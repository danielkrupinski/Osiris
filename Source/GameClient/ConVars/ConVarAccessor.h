#pragma once

#include <cassert>
#include <cstring>
#include <optional>

#include <CS2/Classes/ConVar.h>
#include <CS2/Classes/ConVarTypes.h>
#include <GameClient/ConVars/ConVars.h>

struct ConVarAccessorState {
    std::optional<bool> mp_teammates_are_enemies;
};

template <typename HookContext>
struct ConVarAccessor {
    explicit ConVarAccessor(HookContext& hookContext, const ConVars& conVars, ConVarAccessorState& state) noexcept
        : hookContext{hookContext}
        , conVars{conVars}
        , state{state}
    {
    }

    template <typename ConVar>
    [[nodiscard]] bool requestConVar() noexcept
    {
        static_assert(std::is_same_v<ConVar, cs2::mp_teammates_are_enemies>);

        if (state.mp_teammates_are_enemies.has_value())
            return true;

        const auto conVar = conVars.getConVar<ConVar>();
        if (!conVar)
            return false;

        if (!hookContext.tier0PatternSearchResults().template get<OffsetToConVarValue>() || !conVarIsBool(conVar))
            return false;

        state.mp_teammates_are_enemies = readConVarValue<bool>(conVar);
        return true;
    }

    template <typename ConVar>
    [[nodiscard]] bool getConVarValue() const noexcept
    {
        static_assert(std::is_same_v<ConVar, cs2::mp_teammates_are_enemies>);
        assert(state.mp_teammates_are_enemies.has_value());
        return *state.mp_teammates_are_enemies;
    }

private:
    template <typename T>
    [[nodiscard]] T readConVarValue(cs2::ConVar* conVar) const noexcept
    {
        T value;
        std::memcpy(&value, hookContext.tier0PatternSearchResults().template get<OffsetToConVarValue>().of(conVar).get(), sizeof(value));
        return value;
    }

    [[nodiscard]] bool conVarIsBool(cs2::ConVar* conVar) const noexcept
    {
        if (!hookContext.tier0PatternSearchResults().template get<OffsetToConVarValueType>())
            return true;

        return cs2::ConVarValueType{*hookContext.tier0PatternSearchResults().template get<OffsetToConVarValueType>().of(conVar).get()} == cs2::ConVarValueType::boolean;
    }

    HookContext& hookContext;
    const ConVars& conVars;
    ConVarAccessorState& state;
};
