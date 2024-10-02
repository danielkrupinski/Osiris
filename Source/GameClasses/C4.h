#pragma once

#include <CS2/Classes/Entities/WeaponEntities.h>

template <typename HookContext>
class C4 {
public:
    C4(HookContext& hookContext, cs2::C_C4* c4) noexcept
        : hookContext{hookContext}
        , c4{c4}
    {
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return c4 != nullptr;
    }

    using RawType = cs2::C_C4;

private:
    HookContext& hookContext;
    cs2::C_C4* c4;
};
