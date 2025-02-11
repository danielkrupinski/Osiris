#pragma once

#include <CS2/Classes/Entities/C_CSGO_PreviewPlayer.h>
#include <CS2/Panorama/CUI_MapPlayerPreviewPanel.h>
#include <GameClient/Entities/PreviewPlayer.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <MemoryPatterns/PatternTypes/MapPlayerPreviewPanelPatternTypes.h>

template <typename HookContext>
class MapPlayerPreviewPanel {
public:
    MapPlayerPreviewPanel(HookContext& hookContext, cs2::CUI_MapPlayerPreviewPanel* mapPlayerPreviewPanel) noexcept
        : hookContext{hookContext}
        , mapPlayerPreviewPanel{mapPlayerPreviewPanel}
    {
    }

    using RawType = cs2::CUI_MapPlayerPreviewPanel;

    [[nodiscard]] decltype(auto) findPreviewPlayer() const noexcept
    {
        const auto entityHandles = hookContext.clientPatternSearchResults().template get<OffsetToPlayerPreviewPanelEntities>().of(mapPlayerPreviewPanel).get();
        if (!entityHandles)
            return hookContext.template make<PreviewPlayer>(nullptr);

        for (int i = 0; i < entityHandles->size; ++i) {
            auto&& entityIdentity = hookContext.template make<EntitySystem>().getEntityIdentityFromHandle(entityHandles->memory[i]);
            if (entityIdentity.template is<cs2::C_CSGO_PreviewPlayer>())
                return entityIdentity.entity().template as<PreviewPlayer>();
        }
        return hookContext.template make<PreviewPlayer>(nullptr);
    }

private:
    HookContext& hookContext;
    cs2::CUI_MapPlayerPreviewPanel* mapPlayerPreviewPanel;
};
