#include <fstream>
#include "Interfaces.h"
#include "Netvars.h"

Netvars::Netvars()
{
    for (auto clientClass = interfaces.client->getAllClasses(); clientClass; clientClass = clientClass->next) {
        if (std::string_view{ clientClass->networkName } == "CCSPlayer")
            recvTables.emplace("CCSPlayer", clientClass->recvTable);
    }
}

int Netvars::getOffset(const std::string_view propertyName)
{
    for (int i = 0; i != recvTables["CCSPlayer"]->m_nProps; i++)
        if (recvTables["CCSPlayer"]->m_pProps[i].m_pVarName == propertyName)
            return recvTables["CCSPlayer"]->m_pProps[i].m_Offset;
}
