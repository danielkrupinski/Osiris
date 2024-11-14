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

    template <template <typename...> typename EntityType>
    [[nodiscard]] bool is() const noexcept
    {
        return entity && entity->vmt && entity->vmt == hookContext.gameDependencies().entitiesVMTs.vmts[utils::typeIndex<typename EntityType<HookContext>::RawType, KnownEntityTypes>()];
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
            auto&& glowSceneObject = hookContext.template make<GlowSceneObjects>().getGlowSceneObject(sceneObject);
            glowSceneObject.apply(sceneObject, color, glowRange);
            glowSceneObject.setGlowEntity(*this);
        });
    }

    void applySpawnProtectionEffect(cs2::Color color) const noexcept
    {
        renderComponent().sceneObjectUpdaters().forEachSceneObject([this, color](auto&& sceneObject) {
            const auto unknownEntityAttribute = getAttributeInt(0x8AD232BC, 0); // probably checks if entity is a charm being currently applied, most likely redundant
            if (unknownEntityAttribute == 0) {
                auto&& sceneObjectAttributes = sceneObject.attributes();
                sceneObjectAttributes.setAttributeFloat(0x244EC9B0, 1.0f); // "SpawnInvulnerability"
                sceneObjectAttributes.setAttributeColor3(0xB2CAF4DF, color); // "InvulnerabilityColor"
            }
        });
    }

    void applySpawnProtectionEffectRecursively(cs2::Color color) const noexcept
    {
        applySpawnProtectionEffect(color);
        forEachChild([color](auto&& entity) { entity.applySpawnProtectionEffect(color); });
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
    [[nodiscard]] int getAttributeInt([[maybe_unused]] unsigned int attributeNameHash, int defaultValue) const noexcept
    {
#if IS_WIN64()
        if (entity)
            return hookContext.clientPatternSearchResults().template get<GetEntityAttributeInt>()(entity, attributeNameHash, defaultValue, nullptr);
#endif
        return defaultValue;
    }

    [[nodiscard]] auto invokeWithGameSceneNodeOwner(auto& f) const noexcept
    {
        return [&f](auto&& gameSceneNode) { f(gameSceneNode.owner()); };
    }

    HookContext& hookContext;
    cs2::C_BaseEntity* entity;
};
