#pragma once

template <typename HookContext>
class WeaponModelGlowPreview {
public:
    explicit WeaponModelGlowPreview(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void setPreviewWeapon(auto&& previewModel) const noexcept
    {
        state().previewWeaponHandle = previewModel.baseEntity().handle();
    }

    [[nodiscard]] bool isPreviewWeaponSet() const noexcept
    {
        return static_cast<bool>(hookContext.template make<EntitySystem>().getEntityFromHandle(state().previewWeaponHandle));
    }

    void setPreviewFlashbang(auto&& previewModel) const noexcept
    {
        state().previewFlashbangHandle = previewModel.baseEntity().handle();
    }

    [[nodiscard]] bool isPreviewFlashbangSet() const noexcept
    {
        return static_cast<bool>(hookContext.template make<EntitySystem>().getEntityFromHandle(state().previewFlashbangHandle));
    }

    void setPreviewHEGrenade(auto&& previewModel) const noexcept
    {
        state().previewHEGrenadeHandle = previewModel.baseEntity().handle();
    }

    [[nodiscard]] bool isPreviewHEGrenadeSet() const noexcept
    {
        return static_cast<bool>(hookContext.template make<EntitySystem>().getEntityFromHandle(state().previewHEGrenadeHandle));
    }

    void setPreviewSmokeGrenade(auto&& previewModel) const noexcept
    {
        state().previewSmokeGrenadeHandle = previewModel.baseEntity().handle();
    }

    [[nodiscard]] bool isPreviewSmokeGrenadeSet() const noexcept
    {
        return static_cast<bool>(hookContext.template make<EntitySystem>().getEntityFromHandle(state().previewSmokeGrenadeHandle));
    }

    void setPreviewIncendiary(auto&& previewModel) const noexcept
    {
        state().previewIncendiaryHandle = previewModel.baseEntity().handle();
    }

    [[nodiscard]] bool isPreviewIncendiarySet() const noexcept
    {
        return static_cast<bool>(hookContext.template make<EntitySystem>().getEntityFromHandle(state().previewIncendiaryHandle));
    }

    void updateSceneObjectUpdaterHooks() const noexcept
    {
        hookContext.template make<ModelGlow>().updateWeaponSceneObjectUpdaterHook(previewWeapon());
        hookContext.template make<ModelGlow>().updateWeaponSceneObjectUpdaterHook(previewFlashbang());
        hookContext.template make<ModelGlow>().updateWeaponSceneObjectUpdaterHook(previewSmokeGrenade());
        hookContext.template make<ModelGlow>().updateWeaponSceneObjectUpdaterHook(previewHEGrenade());
        hookContext.template make<ModelGlow>().updateWeaponSceneObjectUpdaterHook(previewIncendiary());
    }

    void onUnload() const noexcept
    {
        hookContext.template make<WeaponModelGlow>().onUnload(previewWeapon());
        hookContext.template make<WeaponModelGlow>().onUnload(previewFlashbang());
        hookContext.template make<WeaponModelGlow>().onUnload(previewSmokeGrenade());
        hookContext.template make<WeaponModelGlow>().onUnload(previewHEGrenade());
        hookContext.template make<WeaponModelGlow>().onUnload(previewIncendiary());
    }

private:
    [[nodiscard]] decltype(auto) previewWeapon() const noexcept
    {
        return hookContext.template make<EntitySystem>().getEntityIdentityFromHandle(state().previewWeaponHandle).entity().template as<BaseWeapon>();
    }

    [[nodiscard]] decltype(auto) previewFlashbang() const noexcept
    {
        return hookContext.template make<EntitySystem>().getEntityIdentityFromHandle(state().previewFlashbangHandle).entity().template as<BaseWeapon>();
    }

    [[nodiscard]] decltype(auto) previewHEGrenade() const noexcept
    {
        return hookContext.template make<EntitySystem>().getEntityIdentityFromHandle(state().previewHEGrenadeHandle).entity().template as<BaseWeapon>();
    }

    [[nodiscard]] decltype(auto) previewSmokeGrenade() const noexcept
    {
        return hookContext.template make<EntitySystem>().getEntityIdentityFromHandle(state().previewSmokeGrenadeHandle).entity().template as<BaseWeapon>();
    }

    [[nodiscard]] decltype(auto) previewIncendiary() const noexcept
    {
        return hookContext.template make<EntitySystem>().getEntityIdentityFromHandle(state().previewIncendiaryHandle).entity().template as<BaseWeapon>();
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.weaponModelGlowPreviewState();
    }

    HookContext& hookContext;
};
