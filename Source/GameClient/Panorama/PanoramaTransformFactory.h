#pragma once

#include <utility>
#include <type_traits>

#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/Transform3D.h>
#include <GameClient/MemAlloc.h>

template <typename HookContext>
struct PanoramaTransformFactory {
    explicit PanoramaTransformFactory(HookContext& hookContext, const void* transformTranslate3dVmt, const void* transformScale3dVmt) noexcept
        : hookContext{hookContext}
        , transformTranslate3dVmt{transformTranslate3dVmt}
        , transformScale3dVmt{transformScale3dVmt}
    {
    }

    [[nodiscard]] cs2::CTransform3D* scale(float scale) const noexcept
    {
        return create<cs2::CTransformScale3D>(scale, scale, 1.0f);
    }

    [[nodiscard]] cs2::CTransform3D* scale(float scaleX, float scaleY) const noexcept
    {
        return create<cs2::CTransformScale3D>(scaleX, scaleY, 1.0f);
    }

    [[nodiscard]] cs2::CTransform3D* translate(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        return create<cs2::CTransformTranslate3D>(x, y, cs2::CUILength{ 0.0f, cs2::CUILength::k_EUILengthLength });
    }

private:
    template <typename T, typename... Args>
    [[nodiscard]] T* create(Args&&... args) const noexcept
    {
        if (const auto vmt = getVmt<T>()) {
            if (const auto memory = hookContext.template make<MemAlloc>().allocate(sizeof(T)))
                return new (memory) T{ vmt, std::forward<Args>(args)... };
        }
        return nullptr;
    }

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

    HookContext& hookContext;
    const void* transformTranslate3dVmt;
    const void* transformScale3dVmt;
};
