#include "Interfaces.h"
#include "Netvars.h"

Netvars::Netvars()
{
    auto clientClass = interfaces.client->getAllClasses();

    while (clientClass) {
        auto recvTable = clientClass->recvTable;

        //if (recvTable)
        //    tables.emplace(std::string(client->m_pNetworkName), recvTable);

        clientClass = clientClass->next;
    }

}
