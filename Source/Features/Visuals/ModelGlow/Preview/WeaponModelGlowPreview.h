#pragma once

#include <GameClient/EntitySystem/EntityHandle.h>
#include <GameClient/Panorama/UiItem3dPanel.h>
#include <Platform/Macros/FunctionAttributes.h>

template <typename HookContext>
class WeaponModelGlowPreview {
public:
    explicit WeaponModelGlowPreview(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void updateSceneObjectUpdaterHooks() const noexcept
    {
        hookContext.template make<ModelGlow>().updateInMainThread()(WeaponModelGlow{hookContext}, previewWeapon(), EntityTypeInfo{});
        hookContext.template make<ModelGlow>().updateInMainThread()(WeaponModelGlow{hookContext}, previewFlashbang(), EntityTypeInfo{});
        hookContext.template make<ModelGlow>().updateInMainThread()(WeaponModelGlow{hookContext}, previewSmokeGrenade(), EntityTypeInfo{});
        hookContext.template make<ModelGlow>().updateInMainThread()(WeaponModelGlow{hookContext}, previewHEGrenade(), EntityTypeInfo{});
        hookContext.template make<ModelGlow>().updateInMainThread()(WeaponModelGlow{hookContext}, previewIncendiary(), EntityTypeInfo{});
    }

    void onUnload() const noexcept
    {
        hookContext.template make<ModelGlow>().onUnload()(WeaponModelGlow{hookContext}, previewWeapon());
        hookContext.template make<ModelGlow>().onUnload()(WeaponModelGlow{hookContext}, previewFlashbang());
        hookContext.template make<ModelGlow>().onUnload()(WeaponModelGlow{hookContext}, previewSmokeGrenade());
        hookContext.template make<ModelGlow>().onUnload()(WeaponModelGlow{hookContext}, previewHEGrenade());
        hookContext.template make<ModelGlow>().onUnload()(WeaponModelGlow{hookContext}, previewIncendiary());
    }

private:
    [[nodiscard]] [[NOINLINE]] decltype(auto) previewWeapon(cs2::CEntityHandle& handle, bool& hadHandle, const char* panelId, std::uint64_t itemId) const noexcept
    {
        auto&& entityIdentity = hookContext.template make<EntityHandle>(handle).getOrInit(
            [&hadHandle] { hadHandle = true; },
            [this, &hadHandle, panelId, itemId] {
                auto&& previewPanel = hookContext.gui().modelGlowPreviewPanel(panelId);
                auto&& portraitWorld = previewPanel.portraitWorld();
                if (hadHandle && portraitWorld.isMapLoaded()) {
                    previewPanel.template as<UiItem3dPanel>().createItem(itemId);
                    hadHandle = false;
                }
                return portraitWorld.findPreviewWeapon().baseEntity().handle();
            });
        return entityIdentity.template as<BaseWeapon>();
    }

    [[nodiscard]] decltype(auto) previewWeapon() const noexcept
    {
        return previewWeapon(state().previewWeaponHandle, state().hadPreviewWeaponHandle, "ModelGlowPreviewWeapon", 17293822569119416336ull);
    }

    [[nodiscard]] decltype(auto) previewFlashbang() const noexcept
    {
        return previewWeapon(state().previewFlashbangHandle, state().hadPreviewFlashbangHandle, "ModelGlowPreviewFlashbang", 17293822569102704683ull);
    }

    [[nodiscard]] decltype(auto) previewHEGrenade() const noexcept
    {
        return previewWeapon(state().previewHEGrenadeHandle, state().hadPreviewHEGrenadeHandle, "ModelGlowPreviewHEGrenade", 17293822569102704684ull);
    }

    [[nodiscard]] decltype(auto) previewSmokeGrenade() const noexcept
    {
        return previewWeapon(state().previewSmokeGrenadeHandle, state().hadPreviewSmokeGrenadeHandle, "ModelGlowPreviewSmoke", 17293822569102704685ull);
    }

    [[nodiscard]] decltype(auto) previewIncendiary() const noexcept
    {
        return previewWeapon(state().previewIncendiaryHandle, state().hadPreviewIncendiaryHandle, "ModelGlowPreviewIncendiary", 17293822569102704688ull);
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.weaponModelGlowPreviewState();
    }

    HookContext& hookContext;
};
