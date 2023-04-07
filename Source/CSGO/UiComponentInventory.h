#pragma once

#include "Constants/ItemId.h"
#include "Functions.h"
#include <RetSpoof/FunctionInvoker.h>
#include "VirtualMethod.h"

namespace csgo
{

struct UiComponentInventoryPOD;

struct UiComponentInventory : GameClass<UiComponentInventory, UiComponentInventoryPOD> {
    UiComponentInventory(GameClass base, csgo::SetItemSessionPropertyValue setItemSessionPropertyValueFn)
        : GameClass{ base }, setItemSessionPropertyValueFn{ setItemSessionPropertyValueFn }
    {
    }

    void setItemSessionPropertyValue(ItemId itemID, const char* type, const char* value) const noexcept
    {
        FunctionInvoker{ getInvoker(), setItemSessionPropertyValueFn }(getPOD(), itemID, type, value);
    }

private:
    csgo::SetItemSessionPropertyValue setItemSessionPropertyValueFn;
};

}
