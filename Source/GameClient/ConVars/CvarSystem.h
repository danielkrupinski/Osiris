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

private:
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
