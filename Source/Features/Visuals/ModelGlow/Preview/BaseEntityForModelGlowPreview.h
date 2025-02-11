#pragma once

#include <utility>

#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <GameClient/Entities/BaseEntity.h>

template <typename HookContext>
class BaseEntityForModelGlowPreview {
public:
    BaseEntityForModelGlowPreview(HookContext& hookContext, cs2::C_BaseEntity* baseEntity) noexcept
        : hookContext{hookContext}
        , baseEntity{baseEntity}
    {
    }

    template <typename... Args>
    void applySpawnProtectionEffectRecursively(Args&&... args) const noexcept
    {
        hookContext.template make<BaseEntity>(baseEntity).applySpawnProtectionEffectRecursively(std::forward<Args>(args)...);
    }

private:
    HookContext& hookContext;
    cs2::C_BaseEntity* baseEntity;
};
