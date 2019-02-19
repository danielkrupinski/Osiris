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
    if (interfaces.engine->isInGame() && config.knifeChanger.enabled && config.knifeChanger.knife) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        static int bayonet{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_bayonet.mdl") };
        static int bowie{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_survival_bowie.mdl") };
        static int butterfly{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_butterfly.mdl") };
        static int falchion{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_falchion_advanced.mdl") };
        static int flip{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_flip.mdl") };
        static int gut{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_gut.mdl") };
        static int huntsman{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_tactical.mdl") };
        static int karambit{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_karam.mdl") };
        static int m9bayonet{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_m9_bay.mdl") };
        static int daggers{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_push.mdl") };
        static int navaja{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_gypsy_jackknife.mdl") };
        static int stiletto{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_stiletto.mdl") };
        static int talon{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_widowmaker.mdl") };
        static int ursus{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_ursus.mdl") };
        static int golden{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_gg.mdl") };

        if (const auto activeWeapon = interfaces.entityList->getEntityFromHandle(localPlayer->getProperty<int>("m_hActiveWeapon")))
            if (activeWeapon->getClientClass()->classId == ClassId::Knife || activeWeapon->getClientClass()->classId == ClassId::KnifeGG)
                data->intValue = [](int id) {
                switch (id) {
                case 1:
                    return bayonet;
                case 2:
                    return bowie;
                case 3:
                    return butterfly;
                case 4:
                    return falchion;
                case 5:
                    return flip;
                case 6:
                    return gut;
                case 7:
                    return huntsman;
                case 8:
                    return karambit;
                case 9:
                    return m9bayonet;
                case 10:
                    return daggers;
                case 11:
                    return navaja;
                case 12:
                    return stiletto;
                case 13:
                    return talon;
                case 14:
                    return ursus;
                case 15:
                    return golden;
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
