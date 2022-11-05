#pragma once

#include "Constants/ItemId.h"
#include "ItemSchema.h"
#include "PODs/EconItem.h"
#include "PODs/SharedObjectTypeCache.h"
#include "VirtualMethod.h"

struct SOID {
    std::uint64_t id;
    std::uint32_t type;
    std::uint32_t padding;
};

class EconItemView;

namespace csgo::pod { struct SharedObject; }
namespace csgo::pod { struct ClientSharedObjectCache; }
namespace csgo { enum class Team; }

namespace csgo::pod { struct CSPlayerInventory; }
class CSPlayerInventory : public VirtualCallableFromPOD<CSPlayerInventory, csgo::pod::CSPlayerInventory> {
public:
    VIRTUAL_METHOD(void, soCreated, 0, (SOID owner, csgo::pod::SharedObject* object, int event), (owner, object, event))
    VIRTUAL_METHOD(void, soUpdated, 1, (SOID owner, csgo::pod::SharedObject* object, int event), (owner, object, event))
    VIRTUAL_METHOD(void, soDestroyed, 2, (SOID owner, csgo::pod::SharedObject* object, int event), (owner, object, event))
    VIRTUAL_METHOD_V(void, removeItem, 15, (csgo::ItemId itemID), (itemID))

    auto getSOC() const noexcept
    {
        return *reinterpret_cast<csgo::pod::ClientSharedObjectCache**>(getThis() + WIN32_LINUX(0xB4, 0xF8));
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

inline csgo::pod::SharedObjectTypeCache* getItemBaseTypeCache(const CSPlayerInventory& inventory, std::uintptr_t createBaseTypeCacheFn) noexcept
{
    if (const auto soc = ClientSharedObjectCache{ VirtualCallable{ inventory.getInvoker(), std::uintptr_t(inventory.getSOC()) }, createBaseTypeCacheFn }; soc.getThis() != 0)
        return soc.findBaseTypeCache(1);
    return nullptr;
}
