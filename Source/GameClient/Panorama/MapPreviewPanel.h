#pragma once

#include <CS2/Classes/Entities/C_CSGO_PreviewPlayer.h>
#include <CS2/Panorama/CUI_MapPreviewPanel.h>
#include <GameClient/Entities/PreviewPlayer.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <MemoryPatterns/PatternTypes/MapPreviewPanelPatternTypes.h>

template <typename HookContext>
class MapPreviewPanel {
public:
    MapPreviewPanel(HookContext& hookContext, cs2::CUI_MapPreviewPanel* mapPreviewPanel) noexcept
        : hookContext{hookContext}
        , mapPreviewPanel{mapPreviewPanel}
    {
    }

    using RawType = cs2::CUI_MapPreviewPanel;

    [[nodiscard]] decltype(auto) findPreviewPlayer() const noexcept
    {
        return findEntity([](auto&& entityIdentity) { return entityIdentity.template is<cs2::C_CSGO_PreviewPlayer>(); }).template as<PreviewPlayer>();
    }

    [[nodiscard]] decltype(auto) findPreviewWeapon() const noexcept
    {
        return findEntity([](auto&& entityIdentity) { return entityIdentity.classify().isWeapon(); }).template as<BaseWeapon>();
    }

private:
    [[nodiscard]] decltype(auto) findEntity(auto&& predicate) const noexcept
    {
        const auto entityHandles = hookContext.clientPatternSearchResults().template get<OffsetToMapPreviewPanelEntities>().of(mapPreviewPanel).get();
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
    cs2::CUI_MapPreviewPanel* mapPreviewPanel;
};
