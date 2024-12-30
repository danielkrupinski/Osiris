#pragma once

#include <CS2/Classes/Entities/CEntityInstance.h>
#include <Features/Visuals/ModelGlow/ModelGlow.h>
#include <Features/Visuals/OutlineGlow/OutlineGlow.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorld.h>

template <typename HookContext>
class RenderingHookEntityLoop {
public:
    explicit RenderingHookEntityLoop(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void run() const noexcept
    {
        hookContext.template make<EntitySystem>().forEachEntityIdentity([this](const auto& entityIdentity) { handleEntityIdentity(entityIdentity); });
        hookContext.template make<ModelGlow>().onEntityListTraversed();
    }

private:
    void handleEntityIdentity(const cs2::CEntityIdentity& entityIdentity) const noexcept
    {
        const auto entityTypeInfo = hookContext.entityClassifier().classifyEntity(entityIdentity.entityClass);
        auto&& baseEntity = hookContext.template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(entityIdentity.entity));

        if (entityTypeInfo.template is<cs2::C_CSPlayerPawn>())
            hookContext.template make<PlayerInfoInWorld>().drawPlayerInformation(baseEntity.template as<PlayerPawn>());

        if (entityTypeInfo.isModelEntity())
            hookContext.template make<OutlineGlow>().applyGlowToEntity(entityTypeInfo, baseEntity.template as<BaseModelEntity>());
        hookContext.template make<ModelGlow>().updateSceneObjectUpdaterHook(entityTypeInfo, baseEntity);
    }

    HookContext& hookContext;
};
