#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "InventoryChanger/InventoryChanger.h"
#include "Hacks/Misc.h"
#include "Hacks/Visuals.h"
#include "Interfaces.h"
#include "Netvars.h"

#include "SDK/ClassId.h"
#include "SDK/Client.h"
#include "SDK/ClientClass.h"
#include "SDK/Entity.h"
#include "SDK/EntityList.h"
#include "SDK/LocalPlayer.h"
#include "SDK/Platform.h"
#include "SDK/Recv.h"

static std::unordered_map<std::uint32_t, std::pair<recvProxy, recvProxy*>> proxies;

static void CDECL_CONV spottedHook(recvProxyData& data, void* arg2, void* arg3) noexcept
{
    if (Misc::isRadarHackOn())
        data.value._int = 1;

    constexpr auto hash{ fnv::hash("CBaseEntity->m_bSpotted") };
    proxies[hash].first(data, arg2, arg3);
}

static void CDECL_CONV viewModelSequence(recvProxyData& data, void* outStruct, void* arg3) noexcept
{
    const auto viewModel = reinterpret_cast<Entity*>(outStruct);

    if (localPlayer && interfaces->entityList->getEntityFromHandle(viewModel->owner()) == localPlayer.get()) {
        if (const auto weapon = interfaces->entityList->getEntityFromHandle(viewModel->weapon())) {
            if (Visuals::isDeagleSpinnerOn() && weapon->getClientClass()->classId == ClassId::Deagle && data.value._int == 7)
                data.value._int = 8;

            InventoryChanger::fixKnifeAnimation(weapon, data.value._int);
        }
    }
    constexpr auto hash{ fnv::hash("CBaseViewModel->m_nSequence") };
    proxies[hash].first(data, outStruct, arg3);
}

static std::vector<std::pair<std::uint32_t, std::uint32_t>> offsets;

static void walkTable(const char* networkName, RecvTable* recvTable, const std::size_t offset = 0) noexcept
{
    for (int i = 0; i < recvTable->propCount; ++i) {
        auto& prop = recvTable->props[i];

        if (std::isdigit(prop.name[0]))
            continue;

        if (fnv::hashRuntime(prop.name) == fnv::hash("baseclass"))
            continue;

        if (prop.type == 6
            && prop.dataTable
            && prop.dataTable->netTableName[0] == 'D')
            walkTable(networkName, prop.dataTable, prop.offset + offset);

        const auto hash{ fnv::hashRuntime((networkName + std::string{ "->" } + prop.name).c_str()) };

        constexpr auto getHook{ [](std::uint32_t hash) noexcept -> recvProxy {
             switch (hash) {
             case fnv::hash("CBaseEntity->m_bSpotted"):
                 return spottedHook;
             case fnv::hash("CBaseViewModel->m_nSequence"):
                 return viewModelSequence;
             default:
                 return nullptr;
             }
        } };

        offsets.emplace_back(hash, offset + prop.offset);

        constexpr auto hookProperty{ [](std::uint32_t hash, recvProxy& originalProxy, recvProxy proxy) noexcept {
            if (originalProxy != proxy) {
                proxies[hash].first = originalProxy;
                proxies[hash].second = &originalProxy;
                originalProxy = proxy;
            }
        } };

        if (auto hook{ getHook(hash) })
            hookProperty(hash, prop.proxy, hook);
    }
}

void Netvars::init() noexcept
{
    for (auto clientClass = interfaces->client->getAllClasses(); clientClass; clientClass = clientClass->next)
        walkTable(clientClass->networkName, clientClass->recvTable);

    std::ranges::sort(offsets, {}, &std::pair<std::uint32_t, std::uint32_t>::first);
    offsets.shrink_to_fit();
}

void Netvars::restore() noexcept
{
    for (const auto& [hash, proxyPair] : proxies)
        *proxyPair.second = proxyPair.first;

    proxies.clear();
    offsets.clear();
}

std::uint32_t Netvars::get(std::uint32_t hash) noexcept
{
    if (const auto it = std::ranges::lower_bound(offsets, hash, {}, &std::pair<std::uint32_t, std::uint32_t>::first); it != offsets.end() && it->first == hash)
        return it->second;
    assert(false);
    return 0;
}
