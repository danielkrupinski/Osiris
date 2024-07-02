#pragma once

#include <CS2/Classes/CGameSceneNode.h>
#include <CS2/Classes/ConVarTypes.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Constants/ColorConstants.h>
#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/LifeState.h>
#include <FeatureHelpers/PanoramaLabelFactory.h>
#include <FeatureHelpers/PanoramaTransformations.h>
#include <FeatureHelpers/FeatureToggle.h>
#include <FeatureHelpers/TeamNumber.h>
#include <GameClasses/PanoramaLabel.h>
#include <GameClasses/PanoramaImagePanel.h>
#include <Hooks/ViewRenderHook.h>
#include <Utils/ColorUtils.h>
#include <Utils/CString.h>

#include "PlayerPositionArrow/PlayerColorCalculator.h"
#include "PlayerPositionArrow/PlayerColorIndexAccessor.h"
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
    PlayerInformationThroughWallsToggle(PlayerInformationThroughWallsState& state, HookDependencies& dependencies, HudInWorldPanelContainer& hudInWorldPanelContainer, ViewRenderHook& viewRenderHook) noexcept
        : state{state}
        , dependencies{dependencies}
        , hudInWorldPanelContainer{hudInWorldPanelContainer}
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

        if (const auto containerPanel{hudInWorldPanelContainer.get(dependencies.hud(), dependencies.gameDependencies().panelConfigurator())}) {
            if (const auto containerPanelChildren{containerPanel.children().vector})
                hideRemainingPanels(HudInWorldPanels{*containerPanelChildren}, 0);
        }
    }

private:
    void hideRemainingPanels(HudInWorldPanels inWorldPanels, std::size_t firstIndexToHide) const noexcept
    {
        for (std::size_t i = firstIndexToHide; i < state.panelIndices.getSize(); ++i) {
            if (const auto panel{inWorldPanels.getPanel(state.panelIndices[i])}) {
                if (const auto style{panel.getStyle()})
                    dependencies.gameDependencies().panelConfigurator().panelStyle(*style).setOpacity(0.0f);
            }
        }
    }

    PlayerInformationThroughWallsState& state;
    HookDependencies& dependencies;
    HudInWorldPanelContainer& hudInWorldPanelContainer;
    ViewRenderHook& viewRenderHook;
};

class PlayerInformationThroughWalls {
public:
    PlayerInformationThroughWalls(PlayerInformationThroughWallsState& state, HookDependencies& dependencies) noexcept
        : state{state}
        , dependencies{dependencies}
    {
    }

    void drawPlayerInformation(cs2::C_CSPlayerPawn& playerPawn) noexcept
    {
        if (!state.enabled)
            return;

        if (!requestCrucialDependencies())
            return;

        const auto teamNumber = getTeamNumber(playerPawn);
        if (teamNumber != TeamNumber::TT && teamNumber != TeamNumber::CT)
            return;

        if (state.showOnlyEnemies && !isEnemyTeam(teamNumber))
            return;

        if (!isAlive(playerPawn))
            return;

        const auto playerController = getPlayerController(playerPawn);
        if (!playerController || isLocalPlayerController(playerController))
            return;

        const auto health = dependencies.gameDependencies().entityDeps.offsetToHealth.of(&playerPawn);
        if (!health.get() || *health.get() <= 0)
            return;

        const auto gameSceneNode = *dependencies.gameDependencies().entityDeps.offsetToGameSceneNode.of(&playerPawn).get();
        if (!gameSceneNode)
            return;

        const auto absOrigin = *dependencies.gameDependencies().gameSceneNodeDeps.offsetToAbsOrigin.of(gameSceneNode).get();

        const auto positionInClipSpace = dependencies.getDependency<WorldToClipSpaceConverter>().toClipSpace(absOrigin);
        if (!positionInClipSpace.onScreen())
            return;

        const auto containerPanel{dependencies.getFeatureHelpers().hudInWorldPanelContainer.get(dependencies.hud(), dependencies.gameDependencies().panelConfigurator())};
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

        const auto panel = getPanel(containerPanel, panels, dependencies.gameDependencies().panelConfigurator());
        if (!panel)
            return;

        PlayerInformationPanel playerInformationPanel{panel};
        if (!playerInformationPanel.isValid())
            return;

        setArrowColor(PanoramaUiPanel{playerInformationPanel.positionArrowPanel}, *playerController, teamNumber);
        setHealth(PanoramaUiPanel{playerInformationPanel.healthPanel}, *health.get());
        setActiveWeapon(PanoramaUiPanel{playerInformationPanel.weaponIconPanel}, playerPawn);
        setActiveWeaponAmmo(PanoramaUiPanel{playerInformationPanel.weaponAmmoPanel}, playerPawn);
        setPlayerStateIcons(PanoramaUiPanel{playerInformationPanel.playerStateIconsPanel}, playerPawn);

        const auto style = panel.getStyle();
        if (!style)
            return;

        const auto styleSetter{dependencies.gameDependencies().panelConfigurator().panelStyle(*style)};
        styleSetter.setOpacity(hasImmunity(playerPawn) ? 0.5f : 1.0f);
        styleSetter.setZIndex(-positionInClipSpace.z);

        const auto deviceCoordinates = positionInClipSpace.toNormalizedDeviceCoordinates();

        constexpr auto kMaxScale{1.0f};
        const auto scale = std::clamp(500.0f / (positionInClipSpace.z / getFovScale() + 400.0f), 0.4f, kMaxScale);

        PanoramaTransformations{
            dependencies.getDependency<PanoramaTransformFactory>().scale(scale),
            dependencies.getDependency<PanoramaTransformFactory>().translate(deviceCoordinates.getX(), deviceCoordinates.getY())
        }.applyTo(styleSetter);

        ++currentIndex;
    }

    void hideUnusedPanels() const noexcept
    {
        if (!requestCrucialDependencies())
            return;

        const auto containerPanel{dependencies.getFeatureHelpers().hudInWorldPanelContainer.get(dependencies.hud(), dependencies.gameDependencies().panelConfigurator())};
        if (!containerPanel)
            return;

        const auto containerPanelChildren{containerPanel.children().vector};
        if (!containerPanelChildren)
            return;

        const HudInWorldPanels panels{*containerPanelChildren};

        for (std::size_t i = currentIndex; i < state.panelIndices.getSize(); ++i) {
            if (const auto panel{panels.getPanel(state.panelIndices[i])}) {
                if (const auto style{panel.getStyle()})
                    dependencies.gameDependencies().panelConfigurator().panelStyle(*style).setOpacity(0.0f);
            }
        }
    }

private:
    [[nodiscard]] cs2::CCSPlayerController* getPlayerController(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        if (!dependencies.gameDependencies().playerPawnDeps.offsetToPlayerController)
            return nullptr;

        if (!dependencies.requestDependency<EntityFromHandleFinder>())
            return nullptr;

        return static_cast<cs2::CCSPlayerController*>(dependencies.getDependency<EntityFromHandleFinder>().getEntityFromHandle(*dependencies.gameDependencies().playerPawnDeps.offsetToPlayerController.of(&playerPawn).get()));
    }

    [[nodiscard]] bool isLocalPlayerController(cs2::CCSPlayerController* playerController) const noexcept
    {
        return dependencies.localPlayerController() == playerController;
    }

    [[nodiscard]] bool isAlive(cs2::C_BaseEntity& entity) const noexcept
    {
        if (!dependencies.gameDependencies().entityDeps.offsetToLifeState)
            return true;
        return LifeState{*dependencies.gameDependencies().entityDeps.offsetToLifeState.of(&entity).get()} == LifeState::Alive;
    }

    [[nodiscard]] bool isEnemyTeam(TeamNumber team) const noexcept
    {
        return team != getLocalPlayerTeam() || teammatesAreEnemies();
    }

    [[nodiscard]] bool teammatesAreEnemies() const noexcept
    {
        auto conVarAccessor = dependencies.getConVarAccessor();
        if (!conVarAccessor.requestConVar<cs2::mp_teammates_are_enemies>())
            return true;
        return conVarAccessor.getConVarValue<cs2::mp_teammates_are_enemies>();
    }

    [[nodiscard]] TeamNumber getLocalPlayerTeam() const noexcept
    {
        if (!dependencies.gameDependencies().entityDeps.offsetToTeamNumber)
            return {};

        if (!dependencies.localPlayerController())
            return {};

        return TeamNumber{*dependencies.gameDependencies().entityDeps.offsetToTeamNumber.of(dependencies.localPlayerController()).get()};
    }

    [[nodiscard]] auto getPlayerColorCalculator(cs2::CCSPlayerController& playerController) const noexcept
    {
        return PlayerColorCalculator{PlayerColorIndexAccessor{playerController, dependencies.gameDependencies().playerControllerDeps.offsetToPlayerColor}};
    }

    [[nodiscard]] auto getPlayerPositionArrowColorCalculator(cs2::CCSPlayerController& playerController, TeamNumber teamNumber) const noexcept
    {
        return PlayerPositionArrowColorCalculator{getPlayerColorCalculator(playerController), TeamColorCalculator{teamNumber}};
    }

    void setArrowColor(PanoramaUiPanel arrowPanel, cs2::CCSPlayerController& playerController, TeamNumber teamNumber) const noexcept
    {
        if (!state.showPlayerPosition) {
            arrowPanel.setVisible(false);
            return;
        }

        arrowPanel.setVisible(true);
        const auto style = arrowPanel.getStyle();
        if (!style)
            return;

        const auto styleSetter{dependencies.gameDependencies().panelConfigurator().panelStyle(*style)};
        styleSetter.setWashColor(getPlayerPositionArrowColorCalculator(playerController, teamNumber).getArrowColor(state.playerPositionArrowColor));
    }

    [[nodiscard]] cs2::C_CSWeaponBase* getActiveWeapon(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        if (!dependencies.requestDependency<EntityFromHandleFinder>())
            return nullptr;

        const auto weaponServices = dependencies.gameDependencies().playerPawnDeps.offsetToWeaponServices.of(&playerPawn).valueOr(nullptr);
        if (!weaponServices)
            return nullptr;

        return static_cast<cs2::C_CSWeaponBase*>(dependencies.getDependency<EntityFromHandleFinder>().getEntityFromHandle(dependencies.gameDependencies().weaponServicesDeps.offsetToActiveWeapon.of(weaponServices).valueOr(cs2::CEntityHandle{cs2::INVALID_EHANDLE_INDEX})));
    }

    [[nodiscard]] cs2::C_CSWeaponBase::m_iClip1 getActiveWeaponClip(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        const auto activeWeapon = getActiveWeapon(playerPawn);
        if (!activeWeapon)
            return -1;

        return dependencies.gameDependencies().weaponDeps.offsetToClipAmmo.of(activeWeapon).valueOr(-1);
    }

    [[nodiscard]] const char* getActiveWeaponName(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        if (!dependencies.gameDependencies().entityDeps.offsetToVData
         || !dependencies.gameDependencies().weaponVDataDeps.offsetToWeaponName)
            return nullptr;

        const auto activeWeapon = getActiveWeapon(playerPawn);
        if (!activeWeapon)
            return nullptr;

        const auto vData = static_cast<cs2::CCSWeaponBaseVData*>(*dependencies.gameDependencies().entityDeps.offsetToVData.of(static_cast<cs2::C_BaseEntity*>(activeWeapon)).get());
        if (!vData)
            return nullptr;

        return *dependencies.gameDependencies().weaponVDataDeps.offsetToWeaponName.of(vData).get();
    }

    void setHealth(PanoramaUiPanel healthPanel, int health) const noexcept
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

        if (const auto style{PanoramaUiPanel{healthText->uiPanel}.getStyle()}) {
            const auto styler{dependencies.gameDependencies().panelConfigurator().panelStyle(*style)};
            if (state.playerHealthTextColor == PlayerHealthTextColor::HealthBased)
                styler.setSimpleForegroundColor(getHealthBasedColor(health));
            else
                styler.setSimpleForegroundColor(cs2::kColorWhite);
        }

        PanoramaLabel{healthText}.setTextInternal(StringBuilderStorage<10>{}.builder().put(health).cstring(), 0, true);
    }

    [[nodiscard]] static cs2::Color getColorOfHealthFraction(float healthFraction) noexcept
    {
        return color::HSBtoRGB(color::kRedHue + (color::kGreenHue - color::kRedHue) * healthFraction, 0.7f, 1.0f);
    }

    [[nodiscard]] static cs2::Color getHealthBasedColor(int health) noexcept
    {
        return getColorOfHealthFraction(std::clamp(health, 0, 100) / 100.0f);
    }

    void setPlayerStateIcons(PanoramaUiPanel playerStateIconsPanel, cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        if (!state.playerStateIconsToShow) {
            playerStateIconsPanel.setVisible(false);
            return;
        }

        playerStateIconsPanel.setVisible(true);
        
        const auto playerStateChildren = playerStateIconsPanel.children().vector;
        if (!playerStateChildren || playerStateChildren->size != 4)
            return;

        PanoramaUiPanel{playerStateChildren->memory[0]}.setVisible(state.playerStateIconsToShow.has<DefuseIconPanel>() && isDefusing(playerPawn));
        PanoramaUiPanel{playerStateChildren->memory[1]}.setVisible(state.playerStateIconsToShow.has<HostagePickupPanel>() && isPickingUpHostage(playerPawn));
        PanoramaUiPanel{playerStateChildren->memory[2]}.setVisible(state.playerStateIconsToShow.has<HostageRescuePanel>() && isRescuingHostage(playerPawn));
        updateBlindedIconPanel(PanoramaUiPanel{playerStateChildren->memory[3]}, playerPawn);
    }

    void updateBlindedIconPanel(PanoramaUiPanel blindedIconPanel, cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        if (!state.playerStateIconsToShow.has<BlindedIconPanel>()) {
            blindedIconPanel.setVisible(false);
            return;
        }

        const auto remainingFlashBangTime = getRemainingFlashBangTime(playerPawn);
        constexpr auto kFullBlindEnd{3.0f};
        constexpr auto kBlindEnd{1.0f};
        constexpr auto kPartiallyBlindDuration{kFullBlindEnd - kBlindEnd};
        if (remainingFlashBangTime <= kBlindEnd) {
            blindedIconPanel.setVisible(false);
            return;
        }

        blindedIconPanel.setVisible(true);
        const auto opacity = remainingFlashBangTime >= kFullBlindEnd ? 1.0f : (remainingFlashBangTime - kBlindEnd) / kPartiallyBlindDuration;
        if (const auto style{blindedIconPanel.getStyle()}) {
            const auto styler{dependencies.gameDependencies().panelConfigurator().panelStyle(*style)};
            styler.setOpacity(opacity);
        }
    }

    [[nodiscard]] float getRemainingFlashBangTime(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        if (!dependencies.gameDependencies().playerPawnDeps.offsetToFlashBangEndTime)
            return 0.0f;

        const auto curTime = dependencies.globalVars().curtime();
        if (!curTime)
            return 0.0f;
        const auto flashBangEndTime = *dependencies.gameDependencies().playerPawnDeps.offsetToFlashBangEndTime.of(&playerPawn).get();
        if (flashBangEndTime <= *curTime)
            return 0.0f;
        return flashBangEndTime - *curTime;
    }

    [[nodiscard]] bool isRescuingHostage(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        const auto hostageServices = dependencies.gameDependencies().playerPawnDeps.offsetToHostageServices.of(&playerPawn).valueOr(nullptr);
        if (!hostageServices)
            return false;

        if (!dependencies.requestDependency<EntityFromHandleFinder>())
            return false;

        return dependencies.getDependency<EntityFromHandleFinder>().getEntityFromHandle(dependencies.gameDependencies().hostageServicesDeps.offsetToCarriedHostage.of(hostageServices).valueOr(cs2::CEntityHandle{cs2::INVALID_EHANDLE_INDEX})) != nullptr;
    }

    [[nodiscard]] bool isPickingUpHostage(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        return dependencies.gameDependencies().playerPawnDeps.offsetToIsPickingUpHostage.of(&playerPawn).valueOr(false);
    }

    [[nodiscard]] bool isDefusing(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        return dependencies.gameDependencies().playerPawnDeps.offsetToIsDefusing.of(&playerPawn).valueOr(false);
    }

    void setActiveWeaponAmmo(PanoramaUiPanel weaponAmmoPanel, cs2::C_CSPlayerPawn& playerPawn) const noexcept
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
        PanoramaLabel{ammoText}.setTextInternal(StringBuilderStorage<10>{}.builder().put(ammo).cstring(), 0, true);
    }

    void setActiveWeapon(PanoramaUiPanel weaponIconPanel, cs2::C_CSPlayerPawn& playerPawn) const noexcept
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

        const auto weaponIconImagePanel = static_cast<cs2::CImagePanel*>(weaponIconPanel.getClientPanel());

        StringBuilderStorage<100> weaponIconPathStorage;
        auto weaponIconPathBuilder = weaponIconPathStorage.builder();
        weaponIconPathBuilder.put("s2r://panorama/images/icons/equipment/", weaponName.string, ".svg");
        const auto weaponIconPath = weaponIconPathBuilder.cstring();

        if (shouldUpdateImagePanel(weaponIconImagePanel, weaponIconPath))
            PanoramaImagePanel{weaponIconImagePanel}.setImageSvg(weaponIconPath, 24);
    }

    [[nodiscard]] bool shouldUpdateImagePanel(cs2::CImagePanel* imagePanel, const char* newImagePath) const noexcept
    {
        if (!dependencies.gameDependencies().imagePanelDeps.offsetToImagePath)
            return true;

        const auto existingImagePath = *dependencies.gameDependencies().imagePanelDeps.offsetToImagePath.of(imagePanel).get();
        return !existingImagePath.m_pString || std::strcmp(existingImagePath.m_pString, newImagePath) != 0;
    }

    [[nodiscard]] TeamNumber getTeamNumber(cs2::C_BaseEntity& entity) const noexcept
    {
        return TeamNumber{dependencies.gameDependencies().entityDeps.offsetToTeamNumber.of(&entity).valueOr({})};
    }

    [[nodiscard]] bool requestCrucialDependencies() const noexcept
    {
        return dependencies.requestDependencies(kCrucialDependencies);
    }

    [[nodiscard]] bool hasImmunity(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        return dependencies.gameDependencies().playerPawnDeps.offsetToPlayerPawnImmunity.of(&playerPawn).valueOr(false);
    }

    [[nodiscard]] float getFovScale() const noexcept
    {
        return ViewToProjectionMatrix{dependencies.gameDependencies().viewToProjectionMatrix}.getFovScale();
    }

    [[nodiscard]] PanoramaUiPanel getPanel(PanoramaUiPanel containerPanel, HudInWorldPanels inWorldPanels, PanelConfigurator panelConfigurator) const noexcept
    {
        if (currentIndex < state.panelIndices.getSize()) {
            if (const auto panel{inWorldPanels.getPanel(state.panelIndices[currentIndex])})
                return panel;
            state.panelIndices.fastRemoveAt(currentIndex);
        }
        if (const auto panel{PlayerInformationThroughWallsPanelFactory{*static_cast<cs2::CUIPanel*>(containerPanel), dependencies, panelConfigurator}.createPlayerInformationPanel(dependencies.getDependency<PanoramaTransformFactory>())}) {
            state.panelIndices.pushBack(inWorldPanels.getIndexOfLastPanel());
            return panel;
        }
        return PanoramaUiPanel{nullptr};
    }

    static constexpr auto kCrucialDependencies{
        HookDependenciesMask{}
            .set<WorldToClipSpaceConverter>()
            .set<PanoramaTransformFactory>()};

    PlayerInformationThroughWallsState& state;
    HookDependencies& dependencies;
    std::size_t currentIndex{0};
};
