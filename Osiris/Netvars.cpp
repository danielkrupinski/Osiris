#include "Interfaces.h"
#include "Netvars.h"

Netvars::Netvars()
{
    for (auto clientClass = interfaces.client->getAllClasses(); clientClass; clientClass = clientClass->next)
        if (clientClass->recvTable)
            recvTables.emplace(clientClass->networkName, clientClass->recvTable);
}

int Netvars::getOffset(const std::string_view, const std::string_view)
{
    return 0;
}
