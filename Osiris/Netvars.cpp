#include "Config.h"
#include "Interfaces.h"
#include "Netvars.h"
#include "SDK/ClientClass.h"

static std::unordered_map<std::string_view, recvProxy> proxies;

static void spottedHook(recvProxyData* data, void* arg2, void* arg3) noexcept
{
    if (config.misc.radarHack)
        data->intValue = 1;
    proxies["m_bSpotted"](data, arg2, arg3);
}

static void viewPunchHook(recvProxyData* data, void* arg2, void* arg3) noexcept
{
    if (!config.visuals.noVisualRecoil)
        proxies["m_viewPunchAngle"](data, arg2, arg3);
}

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
            std::string_view name{ prop->name };
            offsets[name] = prop->offset + offset;
            if (name == "m_bSpotted")
                hookProperty(prop, spottedHook);
            else if (name == "m_viewPunchAngle")
                hookProperty(prop, viewPunchHook);
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
