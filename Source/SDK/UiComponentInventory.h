#pragma once

#include "Constants/ItemId.h"
#include "VirtualMethod.h"

namespace csgo
{

struct UiComponentInventoryPOD;

class UiComponentInventory : public VirtualCallableFromPOD<UiComponentInventory, UiComponentInventoryPOD> {
public:
    UiComponentInventory(VirtualCallableFromPOD base, std::uintptr_t setItemSessionPropertyValueFn)
        : VirtualCallableFromPOD{ base }, setItemSessionPropertyValueFn{ setItemSessionPropertyValueFn }
    {
    }

    void setItemSessionPropertyValue(ItemId itemID, const char* type, const char* value) const noexcept
    {
        getInvoker().invokeThiscall<void>(getThis(), setItemSessionPropertyValueFn, itemID, type, value);
    }

private:
    std::uintptr_t setItemSessionPropertyValueFn;
};

}
