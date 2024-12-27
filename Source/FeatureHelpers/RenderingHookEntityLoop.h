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
        hookContext.template make<EntitySystem>().iterateEntities([this](auto& entity) { handleEntity(entity); });
        hookContext.template make<ModelGlow>().onEntityListTraversed();
    }

private:
    void handleEntity(cs2::CEntityInstance& entity) const noexcept
    {
        auto&& baseEntity = hookContext.template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(&entity));

        const auto entityTypeInfo = baseEntity.classify();

        if (entityTypeInfo.template is<cs2::C_CSPlayerPawn>()) {
            auto&& playerPawn = hookContext.template make<PlayerPawn>(static_cast<cs2::C_CSPlayerPawn*>(&entity));
            hookContext.template make<PlayerInfoInWorld>().drawPlayerInformation(playerPawn);
        }

        if (entityTypeInfo.isModelEntity())
            hookContext.template make<OutlineGlow>().applyGlowToEntity(entityTypeInfo, baseEntity.template as<BaseModelEntity>());
        hookContext.template make<ModelGlow>().updateSceneObjectUpdaterHook(entityTypeInfo, baseEntity);
    }

    HookContext& hookContext;
};
