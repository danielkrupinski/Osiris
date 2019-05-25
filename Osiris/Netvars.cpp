#include <random>
#include "Config.h"
#include "Interfaces.h"
#include "Netvars.h"
#include "SDK/Client.h"
#include "SDK/ClientClass.h"
#include "SDK/Entity.h"
#include "SDK/Recv.h"
#include "SDK/ModelInfo.h"

static int random(int min, int max) noexcept
{
    static std::mt19937 gen{ std::random_device{ }() };
    return std::uniform_int_distribution{ min, max }(gen);
}

static std::unordered_map<std::string_view, recvProxy> proxies;

constexpr void hookProperty(RecvProp& prop, recvProxy proxy) noexcept
{
    if (prop.proxy != proxy) {
        proxies[prop.name] = prop.proxy;
        prop.proxy = proxy;
    }
}

static void unhookProperty(RecvProp& prop) noexcept
{
    prop.proxy = proxies[prop.name];
}

static void spottedHook(recvProxyData& data, void* arg2, void* arg3) noexcept
{
    if (config.misc.radarHack)
        data.intValue = 1;
    proxies["m_bSpotted"](data, arg2, arg3);
}

static void viewModelSequenceHook(recvProxyData& data, void* arg2, void* arg3) noexcept
{
    if (interfaces.engine->isInGame() && config.knifeChanger.enabled && config.knifeChanger.knife) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        if (const auto activeWeapon = interfaces.entityList->getEntityFromHandle(localPlayer->getProperty<int>("m_hActiveWeapon")))
            if (activeWeapon->getClientClass()->classId == ClassId::Knife || activeWeapon->getClientClass()->classId == ClassId::KnifeGG) {
                auto& sequence = data.intValue;
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

#include "nSkinz/src/Hooks/Hooks.hpp"
#include "nSkinz/src/nSkinz.hpp"
#include "nSkinz/src/config_.hpp"

static int get_new_animation(const fnv::hash model, const int sequence) noexcept
{
    enum Sequence
    {
        SEQUENCE_DEFAULT_DRAW = 0,
        SEQUENCE_DEFAULT_IDLE1 = 1,
        SEQUENCE_DEFAULT_IDLE2 = 2,
        SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
        SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
        SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
        SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
        SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
        SEQUENCE_DEFAULT_LOOKAT01 = 12,

        SEQUENCE_BUTTERFLY_DRAW = 0,
        SEQUENCE_BUTTERFLY_DRAW2 = 1,
        SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
        SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

        SEQUENCE_FALCHION_IDLE1 = 1,
        SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
        SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
        SEQUENCE_FALCHION_LOOKAT01 = 12,
        SEQUENCE_FALCHION_LOOKAT02 = 13,

        SEQUENCE_DAGGERS_IDLE1 = 1,
        SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
        SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
        SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
        SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

        SEQUENCE_BOWIE_IDLE1 = 1,
    };

    // Hashes for best performance.
    switch (model)
    {
    case FNV("models/weapons/v_knife_butterfly.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_DRAW:
            return random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
        default:
            return sequence + 1;
        }
    }
    case FNV("models/weapons/v_knife_falchion_advanced.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_FALCHION_IDLE1;
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return random(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        default:
            return sequence - 1;
        }
    }
    case FNV("models/weapons/v_knife_push.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_DAGGERS_IDLE1;
        case SEQUENCE_DEFAULT_LIGHT_MISS1:
        case SEQUENCE_DEFAULT_LIGHT_MISS2:
            return random(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return random(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
        case SEQUENCE_DEFAULT_HEAVY_HIT1:
        case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
        case SEQUENCE_DEFAULT_LOOKAT01:
            return sequence + 3;
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        default:
            return sequence + 2;
        }
    }
    case FNV("models/weapons/v_knife_survival_bowie.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_BOWIE_IDLE1;
        default:
            return sequence - 1;
        }
    }
    case FNV("models/weapons/v_knife_ursus.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_DRAW:
            return random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
        default:
            return sequence + 1;
        }
    }
    case FNV("models/weapons/v_knife_stiletto.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(12, 13);
        }
    }
    case FNV("models/weapons/v_knife_widowmaker.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(14, 15);
        }
    }
    default:
        return sequence;
    }
}

static void do_sequence_remapping(recvProxyData& data, void* entity)
{
    if (interfaces.engine->isInGame()) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        if (const auto activeWeapon = interfaces.entityList->getEntityFromHandle(localPlayer->getProperty<int>("m_hActiveWeapon"))) {
            const auto weapon_info = game_data::get_weapon_info(activeWeapon->getProperty<int>("m_iItemDefinitionIndex"));

            if (!weapon_info)
                return;

            const auto override_model = weapon_info->model;

            auto& sequence = data.intValue;
            sequence = get_new_animation(fnv::hash_runtime(override_model), sequence);
        }
    }
}

void viewModelSequence(recvProxyData& data, void* arg2, void* arg3) noexcept
{
    ensure_dynamic_hooks();
    do_sequence_remapping(data, arg2);
    proxies["m_nSequence"](data, arg2, arg3);
}


Netvars::Netvars() noexcept
{
    for (auto clientClass = interfaces.client->getAllClasses(); clientClass; clientClass = clientClass->next)
        loadTable(clientClass->recvTable);
}

void Netvars::restore() noexcept
{
    for (auto clientClass = interfaces.client->getAllClasses(); clientClass; clientClass = clientClass->next)
        unloadTable(clientClass->recvTable);

    proxies.clear();
    offsets.clear();
}

void Netvars::loadTable(RecvTable* recvTable, const std::size_t offset) noexcept
{
    for (int i = 0; i < recvTable->propCount; ++i) {
        auto& prop = recvTable->props[i];

        if (isdigit(prop.name[0]))
            continue;

        if (prop.dataTable)
            loadTable(prop.dataTable, prop.offset + offset);
        else {
            std::string_view name{ prop.name };
            std::string_view tableName{ recvTable->netTableName };
            offsets[prop.name] = prop.offset + offset;
            if (name == "m_bSpotted")
                hookProperty(prop, spottedHook);
           // else if (tableName == "DT_BaseViewModel" && name == "m_nModelIndex")
           //     hookProperty(prop, modelIndexHook);
           else if (tableName == "DT_BaseViewModel" && name == "m_nSequence")
               hookProperty(prop, viewModelSequence);
        }
    }
}

void Netvars::unloadTable(RecvTable* recvTable) noexcept
{
    for (int i = 0; i < recvTable->propCount; ++i) {
        auto& prop = recvTable->props[i];

        if (isdigit(prop.name[0]))
            continue;

        if (prop.dataTable)
            unloadTable(prop.dataTable);
        else {
            std::string_view name{ prop.name };
            std::string_view tableName{ recvTable->netTableName };
            if (name == "m_bSpotted")
                unhookProperty(prop);
            //else if (tableName == "DT_BaseViewModel" && name == "m_nModelIndex")
            //    unhookProperty(prop);
            else if (tableName == "DT_BaseViewModel" && name == "m_nSequence")
                unhookProperty(prop);
        }
    }
}
