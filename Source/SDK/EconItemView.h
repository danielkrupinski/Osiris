#pragma once

#include <cstdint>

#include <Platform/Macros/PlatformSpecific.h>
#include "Helpers/EconItemViewFunctions.h"
#include "VirtualMethod.h"

namespace csgo
{

template <typename T> class UtlVector;

namespace pod
{

struct EconItemView {
    std::uintptr_t getAttributeList() const noexcept
    {
        return std::uintptr_t(this) + WIN32_LINUX(0x244, 0x2F8);
    }

    UtlVector<void*>& customMaterials() const noexcept
    {
        return *reinterpret_cast<UtlVector<void*>*>(std::uintptr_t(this) + WIN32_LINUX(0x14, 0x28));
    }

    UtlVector<void*>& visualDataProcessors() const noexcept
    {
        return *reinterpret_cast<UtlVector<void*>*>(std::uintptr_t(this) + WIN32_LINUX(0x230, 0x2D8));
    }
};

struct EconItem;

}

class EconItemView : public VirtualCallableFromPOD<EconItemView, pod::EconItemView> {
public:
    EconItemView(VirtualCallableFromPOD base, const EconItemViewFunctions& functions)
        : VirtualCallableFromPOD{ base }, functions{ functions }
    {
    }

    void clearInventoryImageRGBA() const noexcept
    {
        getInvoker().invokeThiscall<void>(getThis(), functions.clearInventoryImageRGBA);
    }

    pod::EconItem* getSOCData() const noexcept
    {
        return getInvoker().invokeThiscall<pod::EconItem*>(getThis(), functions.getSOCData);
    }

private:
    const EconItemViewFunctions& functions;
};

}
