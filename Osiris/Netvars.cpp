#include "Config.h"
#include "Interfaces.h"
#include "Netvars.h"
#include "SDK/ClientClass.h"
#include "SDK/Entity.h"

static std::unordered_map<std::string_view, recvProxy> proxies;

static void spottedHook(recvProxyData* data, void* arg2, void* arg3) noexcept
{
    if (config.misc.radarHack)
        data->intValue = 1;
    proxies["m_bSpotted"](data, arg2, arg3);
}

static void modelIndexHook(recvProxyData* data, void* arg2, void* arg3) noexcept
{
    if (config.knifeChanger.knife && interfaces.engine->isInGame()) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        if (const auto activeWeapon = interfaces.entityList->getEntityFromHandle(localPlayer->getProperty<int>("m_hActiveWeapon")))
            if (activeWeapon->getClientClass()->classId == ClassId::Knife || activeWeapon->getClientClass()->classId == ClassId::KnifeGG)
                data->intValue = [](int id) {
                switch (id) {
                case 1:
                    return 480;
                case 2:
                    return 501;
                case 3:
                    return 504;
                case 4:
                    return 498;
                case 5:
                    return 483;
                case 6:
                    return 486;
                case 7:
                    return 495;
                case 8:
                    return 489;
                case 9:
                    return 492;
                case 10:
                    return 507;
                case 11:
                    return 514;
                case 12:
                    return 517;
                case 13:
                    return 520;
                case 14:
                    return 511;
                case 15:
                    return 390;
                }}(config.knifeChanger.knife);
    }
    proxies["m_nModelIndex"](data, arg2, arg3);
}

static void viewPunchHook(recvProxyData* data, void* arg2, void* arg3) noexcept
{
    if (!config.visuals.noVisualRecoil)
        proxies["m_viewPunchAngle"](data, arg2, arg3);
}

Netvars::Netvars() noexcept
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
            std::string_view tableName{ recvTable->netTableName };
            offsets[name] = prop->offset + offset;
            if (name == "m_bSpotted")
                hookProperty(prop, spottedHook);
            else if (name == "m_viewPunchAngle")
                hookProperty(prop, viewPunchHook);
            else if (tableName == "DT_BaseViewModel" && name == "m_nModelIndex")
                hookProperty(prop, modelIndexHook);
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
