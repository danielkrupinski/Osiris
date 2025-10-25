#pragma once

#include <GameClient/Econ/FauxItemId.h>

template <typename HookContext>
class ViewmodelModPreviewPanel {
public:
    using RawType = cs2::CUI_Item3dPanel;

    ViewmodelModPreviewPanel(HookContext& hookContext, cs2::CUI_Item3dPanel* item3dPanel) noexcept
        : hookContext{hookContext}
        , item3dPanel{item3dPanel}
    {
    }

    void setupPreviewModel() const
    {
        auto&& previewPanel = panel();
        if (hookContext.template make<EntitySystem>().getEntityFromHandle2(state().previewWeaponHandle)) {
            state().hadPreviewWeaponHandle = true;
            if (state().recreatedPreviewWeapon) {
                previewPanel.template as<UiItem3dPanel>().startWeaponLookAt();
                state().recreatedPreviewWeapon = false;
            }
            return;
        }

        auto&& portraitWorld = previewPanel.portraitWorld();
        if (state().hadPreviewWeaponHandle && portraitWorld.isMapLoaded()) {
            previewPanel.template as<UiItem3dPanel>().createItem(FauxItemId{cs2::ItemDefinitionIndex::M9Bayonet, cs2::PaintKitIndex::MarbleFade});
            state().hadPreviewWeaponHandle = false;
            state().recreatedPreviewWeapon = true;
        }
        state().previewWeaponHandle = portraitWorld.findPreviewWeapon().baseEntity().handle();
    }

    void setFov() const
    {
        panel().setFov(fovForPreview());
    }

private:
    [[nodiscard]] decltype(auto) panel() const
    {
        return hookContext.template make<Ui3dPanel>(item3dPanel);
    }

    [[nodiscard]] float fovForPreview() const
    {
        auto&& viewmodelMod = hookContext.template make<ViewmodelMod>();
        if (viewmodelMod.fovModificationActive())
            return viewmodelMod.viewmodelFov();
        return viewmodelFovFromConVar();
    }

    [[nodiscard]] float viewmodelFovFromConVar() const
    {
        return GET_CONVAR_VALUE(cs2::viewmodel_fov).value_or(viewmodel_mod_params::kPreviewFallbackFov);
    }

    [[nodiscard]] auto& state() const
    {
        return hookContext.panoramaGuiState().viewmodelModPreviewPanelState;
    }

    HookContext& hookContext;
    cs2::CUI_Item3dPanel* item3dPanel;
};
