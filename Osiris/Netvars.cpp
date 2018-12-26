#include <fstream>
#include "Interfaces.h"
#include "Netvars.h"

Netvars::Netvars()
{
    for (auto clientClass = interfaces.client->getAllClasses(); clientClass; clientClass = clientClass->next)
             recvTables.emplace(clientClass->networkName, clientClass->recvTable);

    std::ofstream dump{ "netvar_dump.txt" };

    for (auto recvTable : recvTables) {
        dump << recvTable.first << '\n';
    }
    dump.close();
}
