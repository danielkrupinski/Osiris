#pragma once

#include <optional>
#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <CS2/Classes/EntitySystem/CEntityIdentity.h>
#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <CS2/Constants/EntityHandle.h>
#include <CS2/Constants/SceneObjectAttributeNames.h>
#include <GameClient/Entities/LifeState.h>
#include <GameClient/Entities/TeamNumber.h>
#include <MemoryPatterns/PatternTypes/EntityPatternTypes.h>
#include <OutlineGlow/GlowSceneObjects.h>

#include <GameClient/EntitySystem/EntityIdentity.h>
#include <GameClient/GameSceneNode.h>
#include "RenderComponent.h"

template <typename HookContext>
class BaseEntity {
public:
    BaseEntity(HookContext& hookContext, cs2::C_BaseEntity* entity) noexcept
        : hookContext{hookContext}
        , entity{entity}
    {
    }

    [[nodiscard]] decltype(auto) baseEntity() const noexcept
    {
        return *this;
    }

    [[nodiscard]] decltype(auto) entityIdentity() const noexcept
    {
        return hookContext.template make<EntityIdentity>(entity ? entity->identity : nullptr);
    }

    [[nodiscard]] decltype(auto) classify() const noexcept
    {
        return entityIdentity().classify();
    }

    template <template <typename...> typename T, typename... Args>
    [[nodiscard]] decltype(auto) as(Args&&... args) const noexcept
    {
        return hookContext.template make<T<HookContext>>(static_cast<typename T<HookContext>::RawType*>(entity), std::forward<Args>(args)...);
    }

    template <template <typename...> typename EntityType>
    [[nodiscard]] bool is() const noexcept
    {
        return entity && hookContext.entityClassifier().template entityIs<typename EntityType<HookContext>::RawType>(entity->identity->entityClass);
    }

    template <template <typename...> typename EntityType>
    [[nodiscard]] decltype(auto) cast() const noexcept
    {
        if (is<EntityType>())
            return hookContext.template make<EntityType<HookContext>>(static_cast<typename EntityType<HookContext>::RawType*>(entity));
        return hookContext.template make<EntityType<HookContext>>(nullptr);
    }

    [[nodiscard]] decltype(auto) renderComponent() const noexcept
    {
        return hookContext.template make<RenderComponent>(hookContext.clientPatternSearchResults().template get<OffsetToRenderComponent>().of(entity).valueOr(nullptr));
    }

    [[nodiscard]] decltype(auto) gameSceneNode() const noexcept
    {
        return hookContext.template make<GameSceneNode>(hookContext.clientPatternSearchResults().template get<OffsetToGameSceneNode>().of(entity).valueOr(nullptr));
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
        renderComponent().sceneObjectUpdaters().forEachSceneObject([this, color, glowRange](auto&& sceneObject) {
            // this will still show glow for 1 frame when switching spectators as the flags aren't updated yet
            // todo: do player glow in player scene object updater hook to fix it
            if (!sceneObject.isCulledByFirstPersonView().valueOr(false)) {
                auto&& glowSceneObject = hookContext.template make<GlowSceneObjects>().getGlowSceneObject(sceneObject);
                glowSceneObject.apply(sceneObject, color, glowRange);
                glowSceneObject.setGlowEntity(*this);
            }
        });
    }

    void applySpawnProtectionEffect(cs2::Color color) const noexcept
    {
        renderComponent().sceneObjectUpdaters().forEachSceneObject([color](auto&& sceneObject) {
            auto&& sceneObjectAttributes = sceneObject.attributes();
            sceneObjectAttributes.setAttributeFloat(cs2::scene_object_attribute::kSpawnInvulnerabilityHash, 1.0f);
            sceneObjectAttributes.setAttributeColor3(cs2::scene_object_attribute::kInvulnerabilityColorHash, color);
        });
    }

    void removeSpawnProtectionEffect() const noexcept
    {
        renderComponent().sceneObjectUpdaters().forEachSceneObject([](auto&& sceneObject) {
            sceneObject.attributes().setAttributeFloat(cs2::scene_object_attribute::kSpawnInvulnerabilityHash, 0.0f);
        });
    }

    void applySpawnProtectionEffectRecursively(cs2::Color color) const noexcept
    {
        applySpawnProtectionEffect(color);
        forEachChild([color](auto&& entity) { entity.applySpawnProtectionEffect(color); });
    }

    void removeSpawnProtectionEffectRecursively() const noexcept
    {
        removeSpawnProtectionEffect();
        forEachChild([](auto&& entity) { entity.removeSpawnProtectionEffect(); });
    }

    [[nodiscard]] auto hasOwner() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToOwnerEntity>().of(entity).toOptional().notEqual(cs2::CEntityHandle{cs2::INVALID_EHANDLE_INDEX});
    }

    [[nodiscard]] TeamNumber teamNumber() const noexcept
    {
        return TeamNumber{hookContext.clientPatternSearchResults().template get<OffsetToTeamNumber>().of(entity).valueOr({})};
    }

    [[nodiscard]] auto vData() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToVData>().of(entity).toOptional();
    }

    [[nodiscard]] auto health() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToHealth>().of(entity).toOptional();
    }

    [[nodiscard]] std::optional<bool> isAlive() const noexcept
    {
        const auto lifestate = hookContext.clientPatternSearchResults().template get<OffsetToLifeState>().of(entity).get();
        if (lifestate)
            return LifeState{*lifestate} == LifeState::Alive;
        return {};
    }

    operator cs2::C_BaseEntity*() const noexcept
    {
        return entity;
    }

private:
    [[nodiscard]] auto invokeWithGameSceneNodeOwner(auto& f) const noexcept
    {
        return [&f](auto&& gameSceneNode) { f(gameSceneNode.owner()); };
    }

    HookContext& hookContext;
    cs2::C_BaseEntity* entity;
};
