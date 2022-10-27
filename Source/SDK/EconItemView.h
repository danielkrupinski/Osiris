#pragma once

#include <cstdint>

#include <Platform/PlatformSpecific.h>

#include "VirtualMethod.h"

template <typename T> class UtlVector;

namespace csgo::pod { struct EconItemView; }

class EconItemView : public VirtualCallableFromPOD<EconItemView, csgo::pod::EconItemView> {
public:
    EconItemView(VirtualCallableFromPOD<EconItemView, csgo::pod::EconItemView> base, std::uintptr_t clearInventoryImageRGBA_)
        : VirtualCallableFromPOD<EconItemView, csgo::pod::EconItemView>{ base }, clearInventoryImageRGBA_{ clearInventoryImageRGBA_ }
    {
    }

    void clearInventoryImageRGBA() const noexcept
    {
        getInvoker().invokeThiscall<void>(getThis(), clearInventoryImageRGBA_);
    }

    std::uintptr_t getAttributeList() const noexcept
    {
        return getThis() + WIN32_LINUX(0x244, 0x2F8);
    }

    UtlVector<void*>& customMaterials() const noexcept
    {
        return *reinterpret_cast<UtlVector<void*>*>(getThis() + WIN32_LINUX(0x14, 0x28));
    }

    UtlVector<void*>& visualDataProcessors() const noexcept
    {
        return *reinterpret_cast<UtlVector<void*>*>(getThis() + WIN32_LINUX(0x230, 0x2D8));
    }

private:
    std::uintptr_t clearInventoryImageRGBA_;
    
};
