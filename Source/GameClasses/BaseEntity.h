#pragma once

#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <OutlineGlow/GlowSceneObjects.h>

#include "GameSceneNode.h"
#include "RenderComponent.h"

template <typename HookContext>
class BaseEntity {
public:
    BaseEntity(HookContext& hookContext, cs2::C_BaseEntity* entity) noexcept
        : hookContext{hookContext}
        , entity{entity}
    {
    }

    template <template <typename> typename T>
    [[nodiscard]] decltype(auto) as() const noexcept
    {
        return hookContext.template make<T>(static_cast<typename T<HookContext>::RawType*>(entity));
    }

    template <typename EntityType>
    [[nodiscard]] decltype(auto) cast() const noexcept
    {
        if (entity && entity->vmt && entity->vmt == hookContext.gameDependencies().entitiesVMTs.vmts[utils::typeIndex<typename EntityType::RawType, KnownEntityTypes>()]) {
            return hookContext.template make<EntityType>(static_cast<typename EntityType::RawType*>(entity));
        }
        return hookContext.template make<EntityType>(nullptr);
    }

    [[nodiscard]] decltype(auto) renderComponent() const noexcept
    {
        return hookContext.template make<RenderComponent>(deps().offsetToRenderComponent.of(entity).valueOr(nullptr));
    }

    [[nodiscard]] decltype(auto) gameSceneNode() const noexcept
    {
        return hookContext.template make<GameSceneNode>(deps().offsetToGameSceneNode.of(entity).valueOr(nullptr));
    }

    template <typename F>
    void forEachChild(F f) const noexcept
    {
        gameSceneNode().forEachChild(invokeWithGameSceneNodeOwner(f));
    }

    [[nodiscard]] cs2::CEntityHandle handle() const noexcept
    {
        if (entity)
            return entity->identity->handle;
        return cs2::CEntityHandle{cs2::INVALID_EHANDLE_INDEX};
    }

    void applyGlowRecursively(cs2::Color color, int glowRange = 0) const noexcept
    {
        applyGlow(color, glowRange);
        forEachChild([color, glowRange](auto&& entity) { entity.applyGlow(color, glowRange); });
    }

    void applyGlow(cs2::Color color, int glowRange = 0) const noexcept
    {
        renderComponent().sceneObjectUpdaters().forEachSceneObject([this, color, glowRange](auto&& sceneObject){
            auto&& glowSceneObject = hookContext.template make<GlowSceneObjects>().getGlowSceneObject(sceneObject);
            glowSceneObject.apply(sceneObject, color, glowRange);
            glowSceneObject.setGlowEntity(*this);
        });
    }

    [[nodiscard]] auto hasOwner() const noexcept
    {
        return deps().offsetToOwnerEntity.of(entity).toOptional().notEqual(cs2::CEntityHandle{cs2::INVALID_EHANDLE_INDEX});
    }

    [[nodiscard]] TeamNumber teamNumber() const noexcept
    {
        return TeamNumber{deps().offsetToTeamNumber.of(entity).valueOr({})};
    }
    
    [[nodiscard]] auto vData() const noexcept
    {
        return deps().offsetToVData.of(entity).toOptional();
    }

    [[nodiscard]] auto health() const noexcept
    {
        return deps().offsetToHealth.of(entity).toOptional();
    }

    [[nodiscard]] std::optional<bool> isAlive() const noexcept
    {
        const auto lifestate = deps().offsetToLifeState.of(entity).get();
        if (lifestate)
            return LifeState{*lifestate} == LifeState::Alive;
        return {};
    }

    operator cs2::C_BaseEntity*() const noexcept
    {
        return entity;
    }

private:
    [[nodiscard]] const auto& deps() const noexcept
    {
        return hookContext.gameDependencies().entityDeps;
    }

    [[nodiscard]] auto invokeWithGameSceneNodeOwner(auto& f) const noexcept
    {
        return [&f](auto&& gameSceneNode) { f(gameSceneNode.owner()); };
    }

    HookContext& hookContext;
    cs2::C_BaseEntity* entity;
};
