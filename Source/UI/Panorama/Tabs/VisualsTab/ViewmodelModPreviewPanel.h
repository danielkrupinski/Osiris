#pragma once

template <typename HookContext>
class ViewmodelModificationPreviewPanel {
public:
    using RawType = cs2::CUI_Item3dPanel;

    ViewmodelModificationPreviewPanel(HookContext& hookContext, cs2::CUI_Item3dPanel* item3dPanel) noexcept
        : hookContext{hookContext}
        , item3dPanel{item3dPanel}
    {
    }

    void update()
    {
        auto&& previewPanel = panel();
        setupPreviewModel(previewPanel);
        setFov(previewPanel);
    }

private:
    [[nodiscard]] decltype(auto) panel()
    {
        return hookContext.template make<Ui3dPanel>(item3dPanel);
    }

    void setupPreviewModel(auto&& previewPanel)
    {
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
            previewPanel.template as<UiItem3dPanel>().createItem(17293822569129771516ull);
            state().hadPreviewWeaponHandle = false;
            state().recreatedPreviewWeapon = true;
        }
        state().previewWeaponHandle = portraitWorld.findPreviewWeapon().baseEntity().handle();
    }

    void setFov(auto&& previewPanel)
    {
        previewPanel.setFov(hookContext.template make<ViewmodelModPreview>().viewmodelFovForPreview());
    }

    [[nodiscard]] auto& state() const
    {
        return hookContext.featuresStates().visualFeaturesStates.viewmodelModState;
    }

    HookContext& hookContext;
    cs2::CUI_Item3dPanel* item3dPanel;
};
