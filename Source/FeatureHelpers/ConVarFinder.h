#pragma once

#include <cstring>

#include <CS2/Classes/CCvar.h>
#include <CS2/Classes/ConVar.h>

struct ConVarFinder {
    [[nodiscard]] cs2::ConVar* findConVar(const char* name) const noexcept
    {
        for (auto i = conVarList.m_Head; i != conVarList.kInvalidIndex;) {
            const auto& node = conVarList.memory[i];
            const auto conVar = node.m_Element;
            if (std::strcmp(conVar->name, name) == 0)
                return conVar;
            i = node.m_Next;
        }
        return nullptr;
    }

    cs2::CCvar::ConVarList& conVarList;
};