#pragma once

#include <CS2/Classes/CGameSceneNode.h>
#include <CS2/Classes/ConVarTypes.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Constants/ColorConstants.h>
#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/LifeState.h>
#include <FeatureHelpers/PanoramaTransformations.h>
#include <FeatureHelpers/FeatureToggle.h>
#include <FeatureHelpers/TeamNumber.h>
#include <GameClasses/PanoramaLabel.h>
#include <GameClasses/PanoramaImagePanel.h>
#include <GameClasses/PlayerPawn.h>
#include <Hooks/ViewRenderHook.h>
#include <Utils/ColorUtils.h>
#include <Utils/CString.h>

#include "PlayerInformationPanel.h"
#include "PlayerPositionArrow/PlayerPositionArrowColorCalculator.h"
#include "PlayerPositionArrow/TeamColorCalculator.h"
#include "PlayerInformationThroughWallsPanelFactory.h"
#include "PlayerInformationThroughWallsState.h"

#include <HookDependencies/HookDependenciesMask.h>

struct PlayerPositionToggle : public FeatureToggle<PlayerPositionToggle> {
    explicit PlayerPositionToggle(PlayerInformationThroughWallsState& state) noexcept
        : state{state}
    {
    }

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.showPlayerPosition;
    }

    PlayerInformationThroughWallsState& state;
};

struct PlayerPositionArrowColorToggle {
    explicit PlayerPositionArrowColorToggle(PlayerPositionArrowColorType& color) noexcept
        : color{color}
    {
    }

    void update(char option) const noexcept
    {
        switch (option) {
        case '0': color = PlayerPositionArrowColorType::PlayerOrTeamColor; break;
        case '1': color = PlayerPositionArrowColorType::TeamColor; break;
        }
    }

private:
    PlayerPositionArrowColorType& color;
};

struct PlayerHealthToggle : public FeatureToggle<PlayerHealthToggle> {
    explicit PlayerHealthToggle(PlayerInformationThroughWallsState& state) noexcept
        : state{state}
    {
    }

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.showPlayerHealth;
    }

    PlayerInformationThroughWallsState& state;
};

struct PlayerHealthTextColorToggle {
    explicit PlayerHealthTextColorToggle(PlayerHealthTextColor& color) noexcept
        : color{color}
    {
    }

    void update(char option) const noexcept
    {
        switch (option) {
        case '0': color = PlayerHealthTextColor::HealthBased; break;
        case '1': color = PlayerHealthTextColor::White; break;
        }
    }

private:
    PlayerHealthTextColor& color;
};

struct PlayerActiveWeaponToggle : public FeatureToggle<PlayerActiveWeaponToggle> {
    explicit PlayerActiveWeaponToggle(PlayerInformationThroughWallsState& state) noexcept
        : state{state}
    {
    }

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.showPlayerActiveWeapon;
    }

    PlayerInformationThroughWallsState& state;
};

struct PlayerActiveWeaponAmmoToggle : public FeatureToggle<PlayerActiveWeaponAmmoToggle> {
    explicit PlayerActiveWeaponAmmoToggle(PlayerInformationThroughWallsState& state) noexcept
        : state{state}
    {
    }

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.showPlayerActiveWeaponAmmo;
    }

    PlayerInformationThroughWallsState& state;
};

template <typename IconPanel>
struct PlayerStateIconToggle {
    explicit PlayerStateIconToggle(PlayerStateIconsToShow& playerStateIconsToShow) noexcept
        : playerStateIconsToShow{playerStateIconsToShow}
    {
    }

    void update(char option) noexcept
    {
        switch (option) {
        case '0': playerStateIconsToShow.set<IconPanel>(); break;
        case '1': playerStateIconsToShow.unset<IconPanel>(); break;
        }
    }

    PlayerStateIconsToShow& playerStateIconsToShow;
};

using PlayerDefuseIconToggle = PlayerStateIconToggle<DefuseIconPanel>;
using HostagePickupIconToggle = PlayerStateIconToggle<HostagePickupPanel>;
using HostageRescueIconToggle = PlayerStateIconToggle<HostageRescuePanel>;
using BlindedIconToggle = PlayerStateIconToggle<BlindedIconPanel>;

template <typename HookContext>
struct PlayerInformationThroughWallsToggle : FeatureToggleOnOff<PlayerInformationThroughWallsToggle<HookContext>> {
    PlayerInformationThroughWallsToggle(PlayerInformationThroughWallsState& state, HookContext& hookContext, ViewRenderHook& viewRenderHook) noexcept
        : state{state}
        , hookContext{hookContext}
        , viewRenderHook{viewRenderHook}
    {
    }

    void update(char option) noexcept
    {
        switch (option) {
        case '0': this->enable(); state.showOnlyEnemies = true; break;
        case '1': this->enable(); state.showOnlyEnemies = false; break;
        case '2': this->disable(); break;
        }
    }

    [[nodiscard]] auto& enabledVariable(typename PlayerInformationThroughWallsToggle::ToggleMethod) const noexcept
    {
        return state.enabled;
    }

    void onEnable(typename PlayerInformationThroughWallsToggle::ToggleMethod) noexcept
    {
        viewRenderHook.incrementReferenceCount();
    }

    void onDisable(typename PlayerInformationThroughWallsToggle::ToggleMethod) noexcept
    {
        viewRenderHook.decrementReferenceCount();

        if (const auto containerPanel{hookContext.template make<InWorldPanelContainer>().get()}) {
            if (const auto containerPanelChildren{containerPanel.children().vector})
                hideRemainingPanels(HudInWorldPanels{*containerPanelChildren}, 0);
        }
    }

private:
    void hideRemainingPanels(HudInWorldPanels inWorldPanels, std::size_t firstIndexToHide) const noexcept
    {
        for (std::size_t i = firstIndexToHide; i < state.panelIndices.getSize(); ++i) {
            if (const auto panel{inWorldPanels.getPanel(state.panelIndices[i], hookContext)}) {
                panel.setOpacity(0.0f);
            }
        }
    }

    PlayerInformationThroughWallsState& state;
    HookContext& hookContext;
    ViewRenderHook& viewRenderHook;
};

template <typename HookContext>
class PlayerInformationThroughWalls {
public:
    PlayerInformationThroughWalls(PlayerInformationThroughWallsState& state, HookContext& hookContext) noexcept
        : state{state}
        , hookContext{hookContext}
    {
    }

    void drawPlayerInformation(auto&& playerPawn) noexcept
    {
        if (!state.enabled)
            return;

        if (!requestCrucialDependencies())
            return;

        if (!shouldDrawOnPawn(playerPawn))
            return;

        const auto absOrigin = playerPawn.absOrigin();
        if (!absOrigin.hasValue())
            return;

        const auto positionInClipSpace = hookContext.template getDependency<WorldToClipSpaceConverter>().toClipSpace(absOrigin.value());
        if (!positionInClipSpace.onScreen())
            return;

        const auto containerPanel{hookContext.template make<InWorldPanelContainer>().get()};
        if (!containerPanel)
            return;

        const auto containerPanelChildren{containerPanel.children().vector};
        if (!containerPanelChildren)
            return;

        const HudInWorldPanels panels{*containerPanelChildren};

        if (state.containerPanelHandle != PanoramaUiEngine::getPanelHandle(containerPanel)) {
            state.panelIndices.clear();
            state.containerPanelHandle = PanoramaUiEngine::getPanelHandle(containerPanel);
        }

        const auto panel = getPanel(containerPanel, panels);
        if (!panel)
            return;

        PlayerInformationPanel playerInformationPanel{panel};
        if (!playerInformationPanel.isValid())
            return;

        setArrowColor(hookContext.template make<PanoramaUiPanel>(playerInformationPanel.positionArrowPanel), playerPawn.playerController(), playerPawn.teamNumber());
        setHealth(hookContext.template make<PanoramaUiPanel>(playerInformationPanel.healthPanel), playerPawn);
        setActiveWeapon(hookContext.template make<PanoramaUiPanel>(playerInformationPanel.weaponIconPanel), playerPawn);
        setActiveWeaponAmmo(hookContext.template make<PanoramaUiPanel>(playerInformationPanel.weaponAmmoPanel), playerPawn);
        setPlayerStateIcons(hookContext.template make<PanoramaUiPanel>(playerInformationPanel.playerStateIconsPanel), playerPawn);

        panel.setOpacity(playerPawn.hasImmunity().valueOr(false) ? 0.5f : 1.0f);
        panel.setZIndex(-positionInClipSpace.z);

        const auto deviceCoordinates = positionInClipSpace.toNormalizedDeviceCoordinates();

        constexpr auto kMaxScale{1.0f};
        const auto scale = std::clamp(500.0f / (positionInClipSpace.z / getFovScale() + 400.0f), 0.4f, kMaxScale);

        PanoramaTransformations{
            hookContext.panoramaTransformFactory().scale(scale),
            hookContext.panoramaTransformFactory().translate(deviceCoordinates.getX(), deviceCoordinates.getY())
        }.applyTo(panel);

        ++currentIndex;
    }

    void hideUnusedPanels() const noexcept
    {
        if (!requestCrucialDependencies())
            return;

        const auto containerPanel{hookContext.template make<InWorldPanelContainer>().get()};
        if (!containerPanel)
            return;

        const auto containerPanelChildren{containerPanel.children().vector};
        if (!containerPanelChildren)
            return;

        const HudInWorldPanels panels{*containerPanelChildren};

        for (std::size_t i = currentIndex; i < state.panelIndices.getSize(); ++i) {
            if (const auto panel{panels.getPanel(state.panelIndices[i], hookContext)}) {
                panel.setOpacity(0.0f);
            }
        }
    }

private:
    [[nodiscard]] bool shouldDrawOnPawn(auto&& playerPawn) noexcept
    {
        return playerPawn.isAlive().value_or(true)
            && playerPawn.health().greaterThan(0).valueOr(true)
            && !playerPawn.isControlledByLocalPlayer()
            && playerPawn.isTTorCT()
            && (!state.showOnlyEnemies || playerPawn.isEnemy().value_or(true));
    }

    [[nodiscard]] auto getPlayerPositionArrowColorCalculator(TeamNumber teamNumber) const noexcept
    {
        return PlayerPositionArrowColorCalculator{TeamColorCalculator{teamNumber}};
    }

    void setArrowColor(auto arrowPanel, auto&& playerController, TeamNumber teamNumber) const noexcept
    {
        if (!state.showPlayerPosition) {
            arrowPanel.setVisible(false);
            return;
        }

        arrowPanel.setVisible(true);
        arrowPanel.setWashColor(getPlayerPositionArrowColorCalculator(teamNumber).getArrowColor(playerController, state.playerPositionArrowColor));
    }

    [[nodiscard]] cs2::C_CSWeaponBase::m_iClip1 getActiveWeaponClip(auto&& playerPawn) const noexcept
    {
        const auto activeWeapon = playerPawn.getActiveWeapon();
        if (!activeWeapon)
            return -1;

        return hookContext.gameDependencies().weaponDeps.offsetToClipAmmo.of(activeWeapon).valueOr(-1);
    }

    [[nodiscard]] const char* getActiveWeaponName(auto&& playerPawn) const noexcept
    {
        const auto activeWeapon = playerPawn.getActiveWeapon();
        if (!activeWeapon)
            return nullptr;

        const auto vData = static_cast<cs2::CCSWeaponBaseVData*>(hookContext.template make<BaseEntity>(activeWeapon).vData().valueOr(nullptr));
        return hookContext.gameDependencies().weaponVDataDeps.offsetToWeaponName.of(vData).valueOr(nullptr);
    }

    void setHealth(auto&& healthPanel, auto&& playerPawn) const noexcept
    {
        if (!state.showPlayerHealth) {
            healthPanel.setVisible(false);
            return;
        }

        healthPanel.setVisible(true);
        const auto healthPanelChildren = healthPanel.children().vector;
        if (!healthPanelChildren || healthPanelChildren->size < 2)
            return;

        const auto healthText = static_cast<cs2::CLabel*>(healthPanelChildren->memory[1]->clientPanel);

        PanoramaUiPanel panel{PanoramaUiPanelContext{hookContext.gameDependencies(), healthText->uiPanel}};
        if (state.playerHealthTextColor == PlayerHealthTextColor::HealthBased)
            panel.setColor(playerPawn.healthColor().value_or(cs2::kColorWhite));
        else
            panel.setColor(cs2::kColorWhite);

        PanoramaLabel{hookContext, healthText}.setTextInternal(StringBuilderStorage<10>{}.builder().put(playerPawn.health().valueOr(0)).cstring(), 0, true);
    }

    void setPlayerStateIcons(auto&& playerStateIconsPanel, auto&& playerPawn) const noexcept
    {
        if (!state.playerStateIconsToShow) {
            playerStateIconsPanel.setVisible(false);
            return;
        }

        playerStateIconsPanel.setVisible(true);
        
        auto&& playerStateChildren = playerStateIconsPanel.children();
        playerStateChildren[0].setVisible(state.playerStateIconsToShow.has<DefuseIconPanel>() && playerPawn.isDefusing().valueOr(false));
        playerStateChildren[1].setVisible(state.playerStateIconsToShow.has<HostagePickupPanel>() && playerPawn.isPickingUpHostage().valueOr(false));
        playerStateChildren[2].setVisible(state.playerStateIconsToShow.has<HostageRescuePanel>() && playerPawn.isRescuingHostage());
        updateBlindedIconPanel(playerStateChildren[3], playerPawn);
    }

    void updateBlindedIconPanel(auto&& blindedIconPanel,auto&& playerPawn) const noexcept
    {
        if (!state.playerStateIconsToShow.has<BlindedIconPanel>()) {
            blindedIconPanel.setVisible(false);
            return;
        }

        const auto remainingFlashBangTime = playerPawn.getRemainingFlashBangTime();
        constexpr auto kFullBlindEnd{3.0f};
        constexpr auto kBlindEnd{1.0f};
        constexpr auto kPartiallyBlindDuration{kFullBlindEnd - kBlindEnd};
        if (remainingFlashBangTime <= kBlindEnd) {
            blindedIconPanel.setVisible(false);
            return;
        }

        blindedIconPanel.setVisible(true);
        const auto opacity = remainingFlashBangTime >= kFullBlindEnd ? 1.0f : (remainingFlashBangTime - kBlindEnd) / kPartiallyBlindDuration;
        blindedIconPanel.setOpacity(opacity);
    }

    void setActiveWeaponAmmo(auto&& weaponAmmoPanel, auto&& playerPawn) const noexcept
    {
        if (!state.showPlayerActiveWeaponAmmo) {
            weaponAmmoPanel.setVisible(false);
            return;
        }

        const auto ammo = getActiveWeaponClip(playerPawn);
        if (ammo < 0) {
            weaponAmmoPanel.setVisible(false);
            return;
        }

        const auto children = weaponAmmoPanel.children().vector;
        if (!children || children->size < 1)
            return;

        weaponAmmoPanel.setVisible(true);

        const auto ammoText = static_cast<cs2::CLabel*>(children->memory[0]->clientPanel);
        PanoramaLabel{hookContext, ammoText}.setTextInternal(StringBuilderStorage<10>{}.builder().put(ammo).cstring(), 0, true);
    }

    void setActiveWeapon(auto&& weaponIconPanel, auto&& playerPawn) const noexcept
    {
        if (!state.showPlayerActiveWeapon) {
            weaponIconPanel.setVisible(false);
            return;
        }

        auto weaponName = CString{getActiveWeaponName(playerPawn)};
        if (!weaponName.string)
            return;
        weaponName.skipPrefix("weapon_");

        weaponIconPanel.setVisible(true);

        StringBuilderStorage<100> weaponIconPathStorage;
        auto weaponIconPathBuilder = weaponIconPathStorage.builder();
        weaponIconPathBuilder.put("s2r://panorama/images/icons/equipment/", weaponName.string, ".svg");
        const auto weaponIconPath = weaponIconPathBuilder.cstring();

        auto&& weaponIconImagePanel = weaponIconPanel.clientPanel().template as<PanoramaImagePanel>();
        if (shouldUpdateImagePanel(weaponIconImagePanel, weaponIconPath))
            weaponIconImagePanel.setImageSvg(weaponIconPath, 24);
    }

    [[nodiscard]] bool shouldUpdateImagePanel(auto&& imagePanel, const char* newImagePath) const noexcept
    {
        return imagePanel.getImagePath() != newImagePath;
    }

    [[nodiscard]] bool requestCrucialDependencies() const noexcept
    {
        return hookContext.requestDependencies(kCrucialDependencies);
    }

    [[nodiscard]] float getFovScale() const noexcept
    {
        return ViewToProjectionMatrix{hookContext.gameDependencies().viewToProjectionMatrix}.getFovScale();
    }

    [[nodiscard]] PanoramaUiPanel<PanoramaUiPanelContext<HookContext>> getPanel(auto&& containerPanel, HudInWorldPanels inWorldPanels) const noexcept
    {
        if (currentIndex < state.panelIndices.getSize()) {
            if (const auto panel{inWorldPanels.getPanel(state.panelIndices[currentIndex], hookContext)})
                return panel;
            state.panelIndices.fastRemoveAt(currentIndex);
        }
        if (const auto panel{PlayerInformationThroughWallsPanelFactory{hookContext, *static_cast<cs2::CUIPanel*>(containerPanel)}.createPlayerInformationPanel()}) {
            state.panelIndices.pushBack(inWorldPanels.getIndexOfLastPanel());
            return panel;
        }
        return PanoramaUiPanel{PanoramaUiPanelContext{hookContext, nullptr}};
    }

    static constexpr auto kCrucialDependencies{
        HookDependenciesMask{}
            .set<WorldToClipSpaceConverter>()};

    PlayerInformationThroughWallsState& state;
    HookContext& hookContext;
    std::size_t currentIndex{0};
};
