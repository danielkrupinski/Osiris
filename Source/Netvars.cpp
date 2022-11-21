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
#include "Netvars.h"

#include "SDK/Constants/ClassId.h"
#include "SDK/Client.h"
#include "SDK/ClientClass.h"
#include "SDK/Entity.h"
#include "SDK/EntityList.h"
#include "SDK/LocalPlayer.h"
#include "SDK/Recv.h"

#include "GlobalContext.h"

static void CDECL_CONV spottedHook(recvProxyData& data, void* outStruct, void* arg3) noexcept
{
    const auto entity = Entity::from(retSpoofGadgets->client, static_cast<csgo::pod::Entity*>(outStruct));

    if (Misc::isRadarHackOn()) {
        data.value._int = 1;

        if (localPlayer) {
            if (const auto index = localPlayer.get().getNetworkable().index(); index > 0 && index <= 32)
                entity.spottedByMask() |= 1 << (index - 1);
        }
    }

    proxyHooks.spotted.originalProxy(data, outStruct, arg3);
}

static void CDECL_CONV viewModelSequence(recvProxyData& data, void* outStruct, void* arg3) noexcept
{
    globalContext->viewModelSequenceNetvarHook(data, outStruct, arg3);
}

static std::vector<std::pair<std::uint32_t, std::uint32_t>> offsets;

static void walkTable(const char* networkName, RecvTable* recvTable, const std::size_t offset = 0) noexcept
{
    for (int i = 0; i < recvTable->propCount; ++i) {
        auto& prop = recvTable->props[i];

        if (fnv::hashRuntime(prop.name) == fnv::hash("baseclass"))
            continue;

        if (prop.type == 6
            && prop.dataTable
            && prop.dataTable->netTableName[0] == 'D')
            walkTable(networkName, prop.dataTable, prop.offset + offset);

        const auto hash{ fnv::hashRuntime((networkName + std::string{ "->" } + prop.name).c_str()) };

        switch (hash) {
        case fnv::hash("CBaseEntity->m_bSpotted"):
            if (proxyHooks.spotted.addressOfOriginalProxy == nullptr) {
                proxyHooks.spotted.addressOfOriginalProxy = &prop.proxy;
                proxyHooks.spotted.originalProxy = prop.proxy;
                prop.proxy = spottedHook;
            }
            break;
        case fnv::hash("CBaseViewModel->m_nSequence"):
            if (proxyHooks.viewModelSequence.addressOfOriginalProxy == nullptr) {
                proxyHooks.viewModelSequence.addressOfOriginalProxy = &prop.proxy;
                proxyHooks.viewModelSequence.originalProxy = prop.proxy;
                prop.proxy = viewModelSequence;
            }
            break;
        }

        offsets.emplace_back(hash, offset + prop.offset);
    }
}

void Netvars::init(const Client& client) noexcept
{
    for (auto clientClass = client.getAllClasses(); clientClass; clientClass = clientClass->next)
        walkTable(clientClass->networkName, clientClass->recvTable);

    std::ranges::sort(offsets, {}, &std::pair<std::uint32_t, std::uint32_t>::first);
    offsets.shrink_to_fit();
}

void Netvars::restore() noexcept
{
    if (proxyHooks.spotted.addressOfOriginalProxy != nullptr)
        *proxyHooks.spotted.addressOfOriginalProxy = proxyHooks.spotted.originalProxy;

    if (proxyHooks.viewModelSequence.addressOfOriginalProxy != nullptr)
        *proxyHooks.viewModelSequence.addressOfOriginalProxy = proxyHooks.viewModelSequence.originalProxy;

    offsets.clear();
}

std::uint32_t Netvars::get(std::uint32_t hash) noexcept
{
    if (const auto it = std::ranges::lower_bound(offsets, hash, {}, &std::pair<std::uint32_t, std::uint32_t>::first); it != offsets.end() && it->first == hash)
        return it->second;
    assert(false);
    return 0;
}
