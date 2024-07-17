#pragma once

#include <optional>

#include <GameDependencies/ConVarDeps.h>
#include <GameDependencies/ConVars.h>

struct ConVarAccessorState {
    std::optional<bool> mp_teammates_are_enemies;
};

struct ConVarAccessor {
    explicit ConVarAccessor(const ConVars& conVars, const ConVarDeps& conVarDeps, ConVarAccessorState& state) noexcept
        : conVars{conVars}
        , conVarDeps{conVarDeps}
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

        if (!conVarDeps.offsetToConVarValue || !conVarIsBool(conVar))
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
        std::memcpy(&value, conVarDeps.offsetToConVarValue.of(conVar).get(), sizeof(value));
        return value;
    }

    [[nodiscard]] bool conVarIsBool(cs2::ConVar* conVar) const noexcept
    {
        if (!conVarDeps.offsetToConVarValueType)
            return true;

        return cs2::ConVarValueType{*conVarDeps.offsetToConVarValueType.of(conVar).get()} == cs2::ConVarValueType::boolean;
    }

    const ConVars& conVars;
    const ConVarDeps& conVarDeps;
    ConVarAccessorState& state;
};
