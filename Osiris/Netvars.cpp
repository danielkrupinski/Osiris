#include "Config.h"
#include "Hacks/SkinChanger.h"
#include "Interfaces.h"
#include "Netvars.h"

#include "SDK/Client.h"
#include "SDK/ClientClass.h"
#include "SDK/Entity.h"
#include "SDK/ItemSchema.h"
#include "SDK/ModelInfo.h"
#include "SDK/Platform.h"
#include "SDK/Recv.h"

static std::unordered_map<uint32_t, std::pair<recvProxy, recvProxy*>> proxies;

static void __CDECL spottedHook(recvProxyData& data, void* arg2, void* arg3) noexcept
{
    if (config->misc.radarHack)
        data.value._int = 1;

    constexpr auto hash{ fnv::hash("CBaseEntity->m_bSpotted") };
    proxies[hash].first(data, arg2, arg3);
}

static void __CDECL viewModelSequence(recvProxyData& data, void* outStruct, void* arg3) noexcept
{
    const auto viewModel = reinterpret_cast<Entity*>(outStruct);

    if (localPlayer && interfaces->entityList->getEntityFromHandle(viewModel->owner()) == localPlayer.get()) {
        if (const auto weapon = interfaces->entityList->getEntityFromHandle(viewModel->weapon())) {
            if (config->visuals.deagleSpinner && weapon->getClientClass()->classId == ClassId::Deagle && data.value._int == 7)
                data.value._int = 8;

            SkinChanger::fixKnifeAnimation(weapon, data.value._int);
        }
    }
    constexpr auto hash{ fnv::hash("CBaseViewModel->m_nSequence") };
    proxies[hash].first(data, outStruct, arg3);
}

Netvars::Netvars() noexcept
{
    for (auto clientClass = interfaces->client->getAllClasses(); clientClass; clientClass = clientClass->next)
        walkTable(clientClass->networkName, clientClass->recvTable);

    std::sort(offsets.begin(), offsets.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
}

void Netvars::restore() noexcept
{
    for (const auto& [hash, proxyPair] : proxies)
        *proxyPair.second = proxyPair.first;

    proxies.clear();
    offsets.clear();
}

void Netvars::walkTable(const char* networkName, RecvTable* recvTable, const std::size_t offset) noexcept
{
    for (int i = 0; i < recvTable->propCount; ++i) {
        auto& prop = recvTable->props[i];

        if (isdigit(prop.name[0]))
            continue;

        if (fnv::hashRuntime(prop.name) == fnv::hash("baseclass"))
            continue;

        if (prop.type == 6
            && prop.dataTable
            && prop.dataTable->netTableName[0] == 'D')
            walkTable(networkName, prop.dataTable, prop.offset + offset);

        const auto hash{ fnv::hashRuntime((networkName + std::string{ "->" } + prop.name).c_str()) };

        constexpr auto getHook{ [](uint32_t hash) noexcept -> recvProxy {
             switch (hash) {
             case fnv::hash("CBaseEntity->m_bSpotted"):
                 return spottedHook;
             case fnv::hash("CBaseViewModel->m_nSequence"):
                 return viewModelSequence;
             default:
                 return nullptr;
             }
        } };

        offsets.emplace_back(hash, uint16_t(offset + prop.offset));

        constexpr auto hookProperty{ [](uint32_t hash, recvProxy& originalProxy, recvProxy proxy) noexcept {
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
