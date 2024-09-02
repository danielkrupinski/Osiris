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

#include <HookDependencies/HookDependencies.h>
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

struct PlayerInformationThroughWallsToggle : FeatureToggleOnOff<PlayerInformationThroughWallsToggle> {
    PlayerInformationThroughWallsToggle(PlayerInformationThroughWallsState& state, HookDependencies& dependencies, ViewRenderHook& viewRenderHook) noexcept
        : state{state}
        , dependencies{dependencies}
        , viewRenderHook{viewRenderHook}
    {
    }

    void update(char option) noexcept
    {
        switch (option) {
        case '0': enable(); state.showOnlyEnemies = true; break;
        case '1': enable(); state.showOnlyEnemies = false; break;
        case '2': disable(); break;
        }
    }

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.enabled;
    }

    void onEnable(ToggleMethod) noexcept
    {
        viewRenderHook.incrementReferenceCount();
    }

    void onDisable(ToggleMethod) noexcept
    {
        viewRenderHook.decrementReferenceCount();

        if (const auto containerPanel{dependencies.make<InWorldPanelContainer>().get()}) {
            if (const auto containerPanelChildren{containerPanel.children().vector})
                hideRemainingPanels(HudInWorldPanels{*containerPanelChildren}, 0);
        }
    }

private:
    void hideRemainingPanels(HudInWorldPanels inWorldPanels, std::size_t firstIndexToHide) const noexcept
    {
        for (std::size_t i = firstIndexToHide; i < state.panelIndices.getSize(); ++i) {
            if (const auto panel{inWorldPanels.getPanel(state.panelIndices[i], dependencies)}) {
                panel.setOpacity(0.0f);
            }
        }
    }

    PlayerInformationThroughWallsState& state;
    HookDependencies& dependencies;
    ViewRenderHook& viewRenderHook;
};

class PlayerInformationThroughWalls {
public:
    PlayerInformationThroughWalls(PlayerInformationThroughWallsState& state, HookDependencies& dependencies) noexcept
        : state{state}
        , dependencies{dependencies}
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
        if (!absOrigin)
            return;

        const auto positionInClipSpace = dependencies.getDependency<WorldToClipSpaceConverter>().toClipSpace(*absOrigin);
        if (!positionInClipSpace.onScreen())
            return;

        const auto containerPanel{dependencies.make<InWorldPanelContainer>().get()};
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

        setArrowColor(PanoramaUiPanel{PanoramaUiPanelContext{dependencies, playerInformationPanel.positionArrowPanel}}, playerPawn.playerController(), playerPawn.teamNumber());
        setHealth(PanoramaUiPanel{PanoramaUiPanelContext{dependencies, playerInformationPanel.healthPanel}}, playerPawn.health().value_or(0));
        setActiveWeapon(PanoramaUiPanel{PanoramaUiPanelContext{dependencies, playerInformationPanel.weaponIconPanel}}, playerPawn);
        setActiveWeaponAmmo(PanoramaUiPanel{PanoramaUiPanelContext{dependencies, playerInformationPanel.weaponAmmoPanel}}, playerPawn);
        setPlayerStateIcons(PanoramaUiPanel{PanoramaUiPanelContext{dependencies, playerInformationPanel.playerStateIconsPanel}}, playerPawn);

        panel.setOpacity(playerPawn.hasImmunity().value_or(false) ? 0.5f : 1.0f);
        panel.setZIndex(-positionInClipSpace.z);

        const auto deviceCoordinates = positionInClipSpace.toNormalizedDeviceCoordinates();

        constexpr auto kMaxScale{1.0f};
        const auto scale = std::clamp(500.0f / (positionInClipSpace.z / getFovScale() + 400.0f), 0.4f, kMaxScale);

        PanoramaTransformations{
            dependencies.panoramaTransformFactory().scale(scale),
            dependencies.panoramaTransformFactory().translate(deviceCoordinates.getX(), deviceCoordinates.getY())
        }.applyTo(panel);

        ++currentIndex;
    }

    void hideUnusedPanels() const noexcept
    {
        if (!requestCrucialDependencies())
            return;

        const auto containerPanel{dependencies.make<InWorldPanelContainer>().get()};
        if (!containerPanel)
            return;

        const auto containerPanelChildren{containerPanel.children().vector};
        if (!containerPanelChildren)
            return;

        const HudInWorldPanels panels{*containerPanelChildren};

        for (std::size_t i = currentIndex; i < state.panelIndices.getSize(); ++i) {
            if (const auto panel{panels.getPanel(state.panelIndices[i], dependencies)}) {
                panel.setOpacity(0.0f);
            }
        }
    }

private:
    [[nodiscard]] bool shouldDrawOnPawn(auto&& playerPawn) noexcept
    {
        return playerPawn.isAlive().value_or(true)
            && !(playerPawn.health() <= 0)
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

        return dependencies.gameDependencies().weaponDeps.offsetToClipAmmo.of(activeWeapon).valueOr(-1);
    }

    [[nodiscard]] const char* getActiveWeaponName(auto&& playerPawn) const noexcept
    {
        if (!dependencies.gameDependencies().entityDeps.offsetToVData
         || !dependencies.gameDependencies().weaponVDataDeps.offsetToWeaponName)
            return nullptr;

        const auto activeWeapon = playerPawn.getActiveWeapon();
        if (!activeWeapon)
            return nullptr;

        const auto vData = static_cast<cs2::CCSWeaponBaseVData*>(*dependencies.gameDependencies().entityDeps.offsetToVData.of(static_cast<cs2::C_BaseEntity*>(activeWeapon)).get());
        if (!vData)
            return nullptr;

        return *dependencies.gameDependencies().weaponVDataDeps.offsetToWeaponName.of(vData).get();
    }

    void setHealth(auto&& healthPanel, int health) const noexcept
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

        PanoramaUiPanel panel{PanoramaUiPanelContext{dependencies.gameDependencies(), healthText->uiPanel}};
        if (state.playerHealthTextColor == PlayerHealthTextColor::HealthBased)
            panel.setColor(getHealthBasedColor(health));
        else
            panel.setColor(cs2::kColorWhite);

        PanoramaLabel{dependencies, healthText}.setTextInternal(StringBuilderStorage<10>{}.builder().put(health).cstring(), 0, true);
    }

    [[nodiscard]] static cs2::Color getColorOfHealthFraction(float healthFraction) noexcept
    {
        return color::HSBtoRGB(color::kRedHue + (color::kGreenHue - color::kRedHue) * healthFraction, 0.7f, 1.0f);
    }

    [[nodiscard]] static cs2::Color getHealthBasedColor(int health) noexcept
    {
        return getColorOfHealthFraction(std::clamp(health, 0, 100) / 100.0f);
    }

    void setPlayerStateIcons(auto&& playerStateIconsPanel, auto&& playerPawn) const noexcept
    {
        if (!state.playerStateIconsToShow) {
            playerStateIconsPanel.setVisible(false);
            return;
        }

        playerStateIconsPanel.setVisible(true);
        
        auto&& playerStateChildren = playerStateIconsPanel.children();
        playerStateChildren[0].setVisible(state.playerStateIconsToShow.has<DefuseIconPanel>() && playerPawn.isDefusing().value_or(false));
        playerStateChildren[1].setVisible(state.playerStateIconsToShow.has<HostagePickupPanel>() && playerPawn.isPickingUpHostage().value_or(false));
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
        PanoramaLabel{dependencies, ammoText}.setTextInternal(StringBuilderStorage<10>{}.builder().put(ammo).cstring(), 0, true);
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

    [[nodiscard]] TeamNumber getTeamNumber(cs2::C_BaseEntity& entity) const noexcept
    {
        return TeamNumber{dependencies.gameDependencies().entityDeps.offsetToTeamNumber.of(&entity).valueOr({})};
    }

    [[nodiscard]] bool requestCrucialDependencies() const noexcept
    {
        return dependencies.requestDependencies(kCrucialDependencies);
    }

    [[nodiscard]] float getFovScale() const noexcept
    {
        return ViewToProjectionMatrix{dependencies.gameDependencies().viewToProjectionMatrix}.getFovScale();
    }

    [[nodiscard]] PanoramaUiPanel<PanoramaUiPanelContext<HookDependencies>> getPanel(auto&& containerPanel, HudInWorldPanels inWorldPanels) const noexcept
    {
        if (currentIndex < state.panelIndices.getSize()) {
            if (const auto panel{inWorldPanels.getPanel(state.panelIndices[currentIndex], dependencies)})
                return panel;
            state.panelIndices.fastRemoveAt(currentIndex);
        }
        if (const auto panel{PlayerInformationThroughWallsPanelFactory{dependencies, *static_cast<cs2::CUIPanel*>(containerPanel)}.createPlayerInformationPanel()}) {
            state.panelIndices.pushBack(inWorldPanels.getIndexOfLastPanel());
            return panel;
        }
        return PanoramaUiPanel{PanoramaUiPanelContext{dependencies, nullptr}};
    }

    static constexpr auto kCrucialDependencies{
        HookDependenciesMask{}
            .set<WorldToClipSpaceConverter>()};

    PlayerInformationThroughWallsState& state;
    HookDependencies& dependencies;
    std::size_t currentIndex{0};
};
