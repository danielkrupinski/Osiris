#include <fstream>
#include "Interfaces.h"
#include "Netvars.h"

Netvars::Netvars()
{
    for (auto clientClass = interfaces.client->getAllClasses(); clientClass; clientClass = clientClass->next) {
        if (std::string_view{ clientClass->recvTable->m_pNetTableName } == "DT_BasePlayer") {
            loadTable(clientClass->recvTable);
            break;
        }
    }

    std::ofstream dump{ "netvars_dump.txt" };
    for (auto prop : playerOffsets)
        dump << prop.first << ' ' << prop.second << '\n';
    dump.close();
}


void Netvars::loadTable(RecvTable* recvTable, std::size_t offset)
{
    std::vector<std::pair<std::string_view, std::size_t>> properties;

    for (int i = 0; i < recvTable->m_nProps; ++i) {
        auto prop = &recvTable->m_pProps[i];

        if (prop->m_pDataTable)
            loadTable(prop->m_pDataTable, prop->m_Offset + offset);
        else
            playerOffsets[prop->m_pVarName] = prop->m_Offset + offset;
    }
}
