#pragma once

#include <utility>
#include <type_traits>

#include <CS2/Classes/Panorama.h>
#include <GameClasses/MemAlloc.h>
#include <MemoryPatterns/ClientPatterns.h>

struct PanoramaTransformFactory {
    template <typename T, typename... Args>
    [[nodiscard]] T* create(Args&&... args) const noexcept
    {
        if (const auto vmt = getVmt<T>()) {
            if (const auto memory = MemAlloc::allocate(sizeof(T)))
                return new (memory) T{ vmt, std::forward<Args>(args)... };
        }
        return nullptr;
    }

private:
    template <typename T>
    [[nodiscard]] const void* getVmt() const noexcept
    {
        if constexpr (std::is_same_v<T, cs2::CTransformTranslate3D>)
            return transformTranslate3dVmt;
        else if constexpr (std::is_same_v<T, cs2::CTransformScale3D>)
            return transformScale3dVmt;
        else
            static_assert(!std::is_same_v<T, T>, "Unsupported type");
    }

    const void* transformTranslate3dVmt{ ClientPatterns::transformTranslate3dVMT() };
    const void* transformScale3dVmt{ ClientPatterns::transformScale3dVMT() };
};
