#include "Interfaces.h"
#include "Netvars.h"

Netvars::Netvars()
{
    auto clientClass = interfaces.client->getAllClasses();

    while (clientClass) {
        if (auto recvTable = clientClass->recvTable)
            recvTables.emplace(clientClass->networkName, recvTable);

        clientClass = clientClass->next;
    }
}
