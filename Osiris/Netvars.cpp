#include "Interfaces.h"
#include "Netvars.h"

Netvars::Netvars()
{
    for (auto clientClass = interfaces.client->getAllClasses(); clientClass; clientClass = clientClass->next)
        loadTable(clientClass->recvTable);
}

std::size_t Netvars::getOffset(const std::string_view propertyName)
{
    return playerOffsets[propertyName];
}

void Netvars::loadTable(RecvTable* recvTable, const std::size_t offset)
{
    for (int i = 0; i < recvTable->propCount; ++i) {
        auto prop = &recvTable->props[i];

        if (isdigit(prop->name[0]))
            continue;

        if (prop->dataTable)
            loadTable(prop->dataTable, prop->offset + offset);
        else
            playerOffsets[prop->name] = prop->offset + offset;
    }
}
