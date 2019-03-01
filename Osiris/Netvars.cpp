#include <random>
#include "Config.h"
#include "Interfaces.h"
#include "Netvars.h"
#include "SDK/ClientClass.h"
#include "SDK/Entity.h"

static int random(int min, int max) noexcept
{
    static std::mt19937 gen{ std::random_device{ }() };
    return std::uniform_int_distribution{ min, max }(gen);
}

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

        int bayonet{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_bayonet.mdl") };
        int bowie{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_survival_bowie.mdl") };
        int butterfly{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_butterfly.mdl") };
        int falchion{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_falchion_advanced.mdl") };
        int flip{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_flip.mdl") };
        int gut{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_gut.mdl") };
        int huntsman{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_tactical.mdl") };
        int karambit{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_karam.mdl") };
        int m9bayonet{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_m9_bay.mdl") };
        int daggers{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_push.mdl") };
        int navaja{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_gypsy_jackknife.mdl") };
        int stiletto{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_stiletto.mdl") };
        int talon{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_widowmaker.mdl") };
        int ursus{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_ursus.mdl") };
        int golden{ interfaces.modelInfo->getModelIndex("models/weapons/v_knife_gg.mdl") };

        if (const auto activeWeapon = interfaces.entityList->getEntityFromHandle(localPlayer->getProperty<int>("m_hActiveWeapon")))
            if (activeWeapon->getClientClass()->classId == ClassId::Knife || activeWeapon->getClientClass()->classId == ClassId::KnifeGG)
                data->intValue = [=](int id) {
                switch (id) {
                default:
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

static void viewModelSequenceHook(recvProxyData* data, void* arg2, void* arg3) noexcept
{
    if (interfaces.engine->isInGame() && config.knifeChanger.enabled && config.knifeChanger.knife) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        if (const auto activeWeapon = interfaces.entityList->getEntityFromHandle(localPlayer->getProperty<int>("m_hActiveWeapon")))
            if (activeWeapon->getClientClass()->classId == ClassId::Knife || activeWeapon->getClientClass()->classId == ClassId::KnifeGG) {
                auto& sequence = data->intValue;
                [&sequence](int id) {
                    switch (id) {
                    case 2:
                        if (sequence > 1)
                            sequence--;
                        break;
                    case 3:
                    case 14:
                        if (!sequence)
                            sequence = random(0, 1);
                        else if (sequence == 12)
                            sequence = random(13, 15);
                        else
                            sequence++;
                        break;
                    case 4:
                        if (sequence == 9)
                            sequence = random(8, 9);
                        else if (sequence == 12)
                            sequence = random(12, 13);
                        else if (sequence > 1)
                            sequence--;
                        break;
                    case 10:
                        switch (sequence) {
                        case 0:
                        case 1:
                            break;
                        case 2:
                            sequence = 1;
                            break;
                        case 3:
                        case 4:
                            sequence = random(2, 6);
                            break;
                        case 9:
                            sequence = random(11, 12);
                            break;
                        case 10:
                        case 11:
                        case 12:
                            sequence += 3;
                            break;
                        default:
                            sequence += 2;
                        }
                        break;
                    case 12:
                        if (sequence == 12)
                            sequence = random(12, 13);
                        break;
                    case 13:
                        if (sequence == 12)
                            sequence = random(14, 15);
                        break;
                    case 15:
                        if (sequence == 12)
                            sequence = 1;
                        break;
                    }
                }(config.knifeChanger.knife);
            }
    }
    proxies["m_nSequence"](data, arg2, arg3);
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
            offsets[prop->name] = prop->offset + offset;
            if (name == "m_bSpotted")
                hookProperty(prop, spottedHook);
            else if (tableName == "DT_BaseViewModel" && name == "m_nModelIndex")
                hookProperty(prop, modelIndexHook);
            else if (tableName == "DT_BaseViewModel" && name == "m_nSequence")
                hookProperty(prop, viewModelSequenceHook);
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
