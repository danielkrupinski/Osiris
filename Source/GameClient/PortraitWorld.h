#pragma once

#include <CS2/Classes/CCS_PortraitWorld.h>
#include <CS2/Classes/Entities/C_CSGO_PreviewPlayer.h>
#include <GameClient/Entities/BaseWeapon.h>
#include <GameClient/Entities/PreviewPlayer.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <MemoryPatterns/PatternTypes/PortraitWorldPatternTypes.h>

template <typename HookContext>
class PortraitWorld {
public:
    PortraitWorld(HookContext& hookContext, cs2::CCS_PortraitWorld* portraitWorld) noexcept
        : hookContext{hookContext}
        , portraitWorld{portraitWorld}
    {
    }

    [[nodiscard]] decltype(auto) findPreviewPlayer() const
    {
        return findEntity([](auto&& entityIdentity) { return entityIdentity.template is<cs2::C_CSGO_PreviewPlayer>(); }).template as<PreviewPlayer>();
    }

    [[nodiscard]] decltype(auto) findPreviewWeapon() const
    {
        return findEntity([](auto&& entityIdentity) { return entityIdentity.classify().isWeapon(); }).template as<BaseWeapon>();
    }

    [[nodiscard]] bool isMapLoaded() const
    {
        const auto entityHandles = hookContext.patternSearchResults().template get<OffsetToPortraitWorldMapEntities>().of(portraitWorld).get();
        return entityHandles && entityHandles->size > 0;
    }

private:
    [[nodiscard]] decltype(auto) findEntity(auto&& predicate) const
    {
        const auto entityHandles = hookContext.patternSearchResults().template get<OffsetToPortraitWorldEntities>().of(portraitWorld).get();
        if (!entityHandles)
            return hookContext.template make<BaseEntity>(nullptr);

        for (int i = 0; i < entityHandles->size; ++i) {
            auto&& entityIdentity = hookContext.template make<EntitySystem>().getEntityIdentityFromHandle(entityHandles->memory[i]);
            if (predicate(entityIdentity))
                return entityIdentity.entity();
        }
        return hookContext.template make<BaseEntity>(nullptr);
    }

    HookContext& hookContext;
    cs2::CCS_PortraitWorld* portraitWorld;
};
