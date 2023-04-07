#pragma once

#include <cstdint>

#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include "Helpers/EconItemViewFunctions.h"
#include "VirtualMethod.h"

namespace csgo
{

template <typename T> struct UtlVector;

struct EconItemViewPOD {
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

struct EconItemPOD;

struct EconItemView : GameClass<EconItemView, EconItemViewPOD> {
    EconItemView(GameClass base, const EconItemViewFunctions& functions)
        : GameClass{ base }, functions{ functions }
    {
    }

    void clearInventoryImageRGBA() const noexcept
    {
        FunctionInvoker{ getInvoker(), functions.clearInventoryImageRGBA }(getPOD());
    }

    [[nodiscard]] EconItemPOD* getSOCData() const noexcept
    {
        return FunctionInvoker{ getInvoker(), functions.getSOCData }(getPOD());
    }

private:
    const EconItemViewFunctions& functions;
};

}
