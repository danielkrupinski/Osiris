#pragma once

#include "Constants/ItemId.h"
#include "ItemSchema.h"
#include "PODs/EconItem.h"
#include "PODs/SharedObjectTypeCache.h"
#include "VirtualMethod.h"

namespace csgo
{

struct SOID {
    std::uint64_t id;
    std::uint32_t type;
    std::uint32_t padding;
};

struct EconItemView;
enum class Team;

struct SharedObjectPOD;
struct ClientSharedObjectCachePOD;
struct CSPlayerInventoryPOD;

struct CSPlayerInventory : GameClass<CSPlayerInventory, CSPlayerInventoryPOD> {
    VIRTUAL_METHOD(void, soCreated, 0, (SOID owner, SharedObjectPOD* object, int event), (owner, object, event))
    VIRTUAL_METHOD(void, soUpdated, 1, (SOID owner, SharedObjectPOD* object, int event), (owner, object, event))
    VIRTUAL_METHOD(void, soDestroyed, 2, (SOID owner, SharedObjectPOD* object, int event), (owner, object, event))
    VIRTUAL_METHOD_V(void, removeItem, 15, (ItemId itemID), (itemID))

    auto getSOC() const noexcept
    {
        return *reinterpret_cast<ClientSharedObjectCachePOD**>(getThis() + WIN32_LINUX(0xB4, 0xF8));
    }

    auto getAccountID() const noexcept
    {
        return *reinterpret_cast<std::uint32_t*>(getThis() + WIN32_LINUX(0x8, 0x10));
    }

    auto getSOID() const noexcept
    {
        return *reinterpret_cast<SOID*>(getThis() + WIN32_LINUX(0x8, 0x10));
    }
};

}

inline csgo::SharedObjectTypeCachePOD* getItemBaseTypeCache(const csgo::CSPlayerInventory& inventory, csgo::CreateBaseTypeCache createBaseTypeCacheFn) noexcept
{
    if (const auto soc = csgo::ClientSharedObjectCache::from(inventory.getInvoker(), inventory.getSOC(), createBaseTypeCacheFn); soc.getPOD() != nullptr)
        return soc.findBaseTypeCache(1);
    return nullptr;
}
