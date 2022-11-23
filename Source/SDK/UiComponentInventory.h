#pragma once

#include "Constants/ItemId.h"
#include "VirtualMethod.h"

namespace csgo
{

namespace pod { struct UiComponentInventory; }

class UiComponentInventory : public VirtualCallableFromPOD<UiComponentInventory, pod::UiComponentInventory> {
public:
    UiComponentInventory(VirtualCallableFromPOD base, std::uintptr_t setItemSessionPropertyValueFn)
        : VirtualCallableFromPOD{ base }, setItemSessionPropertyValueFn{ setItemSessionPropertyValueFn }
    {
    }

    void setItemSessionPropertyValue(csgo::ItemId itemID, const char* type, const char* value) const noexcept
    {
        getInvoker().invokeThiscall<void>(getThis(), setItemSessionPropertyValueFn, itemID, type, value);
    }

private:
    std::uintptr_t setItemSessionPropertyValueFn;
};

}
