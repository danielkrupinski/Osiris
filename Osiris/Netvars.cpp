#include "Interfaces.h"
#include "Netvars.h"
#include "SDK/ClientClass.h"

static std::unordered_map<std::string_view, recvProxy> proxies;

Netvars::Netvars()
{
    for (auto clientClass = interfaces.client->getAllClasses(); clientClass; clientClass = clientClass->next)
        loadTable(clientClass->recvTable);
}

void Netvars::loadTable(RecvTable* recvTable, const std::size_t offset) noexcept
{
    for (int i = 0; i < recvTable->propCount; ++i) {
        auto prop = &recvTable->props[i];

        if (isdigit(prop->name[0]))
            continue;

        if (prop->dataTable)
            loadTable(prop->dataTable, prop->offset + offset);
        else {
            offsets[prop->name] = prop->offset + offset;

        }
    }
}

void Netvars::hookProperty(RecvProp* prop, recvProxy proxy) noexcept
{
    if (prop->proxy != proxy) {
        proxies[prop->name] = prop->proxy;
        prop->proxy = proxy;
    }
}
