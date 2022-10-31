#pragma once

#include "Constants/ItemId.h"
#include "ItemSchema.h"
#include "PODs/EconItem.h"
#include "VirtualMethod.h"

// TODO: remove this dependency
#include "../Memory.h"

struct SOID {
    std::uint64_t id;
    std::uint32_t type;
    std::uint32_t padding;
};

class EconItemView;

namespace csgo::pod { struct SharedObject; }
namespace csgo { enum class Team; }

class CSPlayerInventory : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;
    using VirtualCallable::getThis;

    VIRTUAL_METHOD2(void, soCreated, 0, (SOID owner, csgo::pod::SharedObject* object, int event), (owner, object, event))
    VIRTUAL_METHOD2(void, soUpdated, 1, (SOID owner, csgo::pod::SharedObject* object, int event), (owner, object, event))
    VIRTUAL_METHOD2(void, soDestroyed, 2, (SOID owner, csgo::pod::SharedObject* object, int event), (owner, object, event))
    VIRTUAL_METHOD2_V(void, removeItem, 15, (csgo::ItemId itemID), (itemID))

    auto getSOC(const Memory& memory) const noexcept
    {
        return ClientSharedObjectCache{ VirtualCallable{ getInvoker(), *reinterpret_cast<std::uintptr_t*>(getThis() + WIN32_LINUX(0xB4, 0xF8)) }, memory.createBaseTypeCache };
    }

    csgo::pod::SharedObjectTypeCache* getItemBaseTypeCache(const Memory& memory) const noexcept
    {
        const auto soc = getSOC(memory);
        if (soc.getThis() == 0)
            return nullptr;

        return soc.findBaseTypeCache(1);
    }

    std::pair<csgo::ItemId, std::uint32_t> getHighestIDs(const Memory& memory) const noexcept
    {
        const auto soc = getSOC(memory);
        if (soc.getThis() == 0)
            return {};

        const auto baseTypeCache = soc.findBaseTypeCache(1);
        if (!baseTypeCache)
            return {};

        csgo::ItemId maxItemID = 0;
        std::uint32_t maxInventoryID = 0;
        for (int i = 0; i < baseTypeCache->objectCount; ++i) {
            const auto item = baseTypeCache->objects[i];
            if (const auto isDefaultItem = (((csgo::pod::EconItem*)item)->itemID & 0xF000000000000000) != 0)
                continue;
            maxItemID = (std::max)(maxItemID, ((csgo::pod::EconItem*)item)->itemID);
            maxInventoryID = (std::max)(maxInventoryID, ((csgo::pod::EconItem*)item)->inventory);
        }

        return std::make_pair(maxItemID, maxInventoryID);
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
