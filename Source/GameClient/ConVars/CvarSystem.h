#pragma once

#include <cstring>

#include <CS2/Classes/CCvar.h>
#include <CS2/Classes/ConVar.h>

template <typename HookContext>
class CvarSystem {
public:
    explicit CvarSystem(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] cs2::ConVar* findConVar(const char* name) const noexcept
    {
        const auto conVarList = getConVarList();
        if (!conVarList)
            return nullptr;

        for (auto i = conVarList->m_Head; i != conVarList->kInvalidIndex;) {
            const auto& node = conVarList->memory[i];
            const auto conVar = node.m_Element;
            if (std::strcmp(conVar->name, name) == 0)
                return conVar;
            i = node.m_Next;
        }
        return nullptr;
    }

    template <typename ConVarType>
    [[nodiscard]] auto getConVarValue() const
    {
        return getValue<ConVarType>(hookContext.getConVarsBase().template getConVar<ConVarType>());
    }

private:
    template <typename ConVarType>
    [[nodiscard]] std::optional<typename ConVarType::ValueType> getValue(cs2::ConVar* conVar) const
    {
        if (hookContext.patternSearchResults().template get<OffsetToConVarValueType>().of(conVar).toOptional().equal(conVarValueTypeForType<typename ConVarType::ValueType>()).valueOr(false))
            return readValueAs<typename ConVarType::ValueType>(conVar);
        return {};
    }

    template <typename T>
    [[nodiscard]] static consteval auto conVarValueTypeForType() noexcept
    {
        if constexpr (std::is_same_v<T, bool>)
            return cs2::ConVarValueType::boolean;
        else if constexpr (std::is_same_v<T, float>)
            return cs2::ConVarValueType::float32;
        else if constexpr (std::is_same_v<T, int>)
            return cs2::ConVarValueType::int32;
        else
            static_assert(!std::is_same_v<T, T>, "Unsupported type");
    }

    template <typename T>
    [[nodiscard]] std::optional<T> readValueAs(cs2::ConVar* conVar) const
    {
        const auto pointerToValue = hookContext.patternSearchResults().template get<OffsetToConVarValue>().of(conVar).get();
        if (!pointerToValue)
            return {};

        T value;
        std::memcpy(&value, pointerToValue, sizeof(value));
        return value;
    }

    [[nodiscard]] cs2::CCvar::ConVarList* getConVarList() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToConVarList>().of(cvar()).get();
    }

    [[nodiscard]] cs2::CCvar* cvar() const noexcept
    {
        if (const auto cvarPointer = hookContext.patternSearchResults().template get<CvarPointer>())
            return *cvarPointer;
        return nullptr;
    }

    HookContext& hookContext;
};
