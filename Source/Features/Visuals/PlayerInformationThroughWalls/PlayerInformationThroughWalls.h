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
#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/TeamNumber.h>
#include <GameClasses/PanoramaLabel.h>
#include <GameClasses/PanoramaImagePanel.h>
#include <Hooks/ViewRenderHook.h>
#include <MemoryPatterns/EntityPatterns.h>
#include <MemoryPatterns/PlayerControllerPatterns.h>
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

struct PlayerPositionToggle : public TogglableFeature<PlayerPositionToggle> {
    explicit PlayerPositionToggle(PlayerInformationThroughWallsState& state) noexcept
        : TogglableFeature{state.showPlayerPosition}
    {
    }
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

struct PlayerHealthToggle : public TogglableFeature<PlayerHealthToggle> {
    explicit PlayerHealthToggle(PlayerInformationThroughWallsState& state) noexcept
        : TogglableFeature{state.showPlayerHealth}
    {
    }
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

struct PlayerActiveWeaponToggle : public TogglableFeature<PlayerActiveWeaponToggle> {
    explicit PlayerActiveWeaponToggle(PlayerInformationThroughWallsState& state) noexcept
        : TogglableFeature{state.showPlayerActiveWeapon}
    {
    }
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

struct PlayerInformationThroughWallsToggle : private TogglableFeature<PlayerInformationThroughWallsToggle> {
    PlayerInformationThroughWallsToggle(PlayerInformationThroughWallsState& state, HudInWorldPanelContainer& hudInWorldPanelContainer, ViewRenderHook& viewRenderHook, PanelConfigurator panelConfigurator, HudProvider hudProvider) noexcept
        : TogglableFeature{state.enabled}
        , state{state}
        , hudInWorldPanelContainer{hudInWorldPanelContainer}
        , viewRenderHook{viewRenderHook}
        , panelConfigurator{panelConfigurator}
        , hudProvider{hudProvider}
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

private:
    friend TogglableFeature;

    void onEnable() noexcept
    {
        viewRenderHook.incrementReferenceCount();
    }

    void onDisable() noexcept
    {
        viewRenderHook.decrementReferenceCount();

        if (const auto containerPanel{hudInWorldPanelContainer.get(hudProvider, panelConfigurator)}) {
            if (const auto containerPanelChildren{containerPanel.children()})
                hideRemainingPanels(HudInWorldPanels{*containerPanelChildren}, 0);
        }
    }

    void hideRemainingPanels(HudInWorldPanels inWorldPanels, std::size_t firstIndexToHide) const noexcept
    {
        for (std::size_t i = firstIndexToHide; i < state.panelIndices.getSize(); ++i) {
            if (const auto panel{inWorldPanels.getPanel(state.panelIndices[i])}) {
                if (const auto style{panel.getStyle()})
                    panelConfigurator.panelStyle(*style).setOpacity(0.0f);
            }
        }
    }

    PlayerInformationThroughWallsState& state;
    HudInWorldPanelContainer& hudInWorldPanelContainer;
    ViewRenderHook& viewRenderHook;
    PanelConfigurator panelConfigurator;
    HudProvider hudProvider;
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

        const auto hasHealth = dependencies.requestDependency<OffsetToHealth>();
        const auto health = hasHealth ? *dependencies.getDependency<OffsetToHealth>().of(&playerPawn).get() : 100;
        if (health <= 0)
            return;

        const auto gameSceneNode = *dependencies.getDependency<OffsetToGameSceneNode>().of(&playerPawn).get();
        if (!gameSceneNode)
            return;

        const auto absOrigin = *dependencies.getDependency<OffsetToAbsOrigin>().of(gameSceneNode).get();

        const auto positionInClipSpace = dependencies.getDependency<WorldToClipSpaceConverter>().toClipSpace(absOrigin);
        if (!positionInClipSpace.onScreen())
            return;

        const auto containerPanel{dependencies.getDependency<HudInWorldPanelContainer>().get(dependencies.getDependency<HudProvider>(), dependencies.getDependency<PanelConfigurator>())};
        if (!containerPanel)
            return;

        const auto containerPanelChildren{containerPanel.children()};
        if (!containerPanelChildren)
            return;

        const HudInWorldPanels panels{*containerPanelChildren};

        if (state.containerPanelHandle != PanoramaUiEngine::getPanelHandle(containerPanel)) {
            state.panelIndices.clear();
            state.containerPanelHandle = PanoramaUiEngine::getPanelHandle(containerPanel);
        }

        const auto panel = getPanel(containerPanel, panels, dependencies.getDependency<PanelConfigurator>());
        if (!panel)
            return;

        PlayerInformationPanel playerInformationPanel{panel};
        if (!playerInformationPanel.isValid())
            return;

        setArrowColor(PanoramaUiPanel{playerInformationPanel.positionArrowPanel}, *playerController, teamNumber);
        if (hasHealth)
            setHealth(PanoramaUiPanel{playerInformationPanel.healthPanel}, health);
        setActiveWeapon(PanoramaUiPanel{playerInformationPanel.weaponIconPanel}, playerPawn);
        setPlayerStateIcons(PanoramaUiPanel{playerInformationPanel.playerStateIconsPanel}, playerPawn);

        const auto style = panel.getStyle();
        if (!style)
            return;

        const auto styleSetter{dependencies.getDependency<PanelConfigurator>().panelStyle(*style)};
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

        const auto containerPanel{dependencies.getDependency<HudInWorldPanelContainer>().get(dependencies.getDependency<HudProvider>(), dependencies.getDependency<PanelConfigurator>())};
        if (!containerPanel)
            return;

        const auto containerPanelChildren{containerPanel.children()};
        if (!containerPanelChildren)
            return;

        const HudInWorldPanels panels{*containerPanelChildren};

        for (std::size_t i = currentIndex; i < state.panelIndices.getSize(); ++i) {
            if (const auto panel{panels.getPanel(state.panelIndices[i])}) {
                if (const auto style{panel.getStyle()})
                    dependencies.getDependency<PanelConfigurator>().panelStyle(*style).setOpacity(0.0f);
            }
        }
    }

private:
    [[nodiscard]] cs2::CCSPlayerController* getPlayerController(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        if (!dependencies.offsets().playerPawn.offsetToPlayerController)
            return nullptr;

        if (!dependencies.requestDependency<EntityFromHandleFinder>())
            return nullptr;

        return static_cast<cs2::CCSPlayerController*>(dependencies.getDependency<EntityFromHandleFinder>().getEntityFromHandle(*dependencies.offsets().playerPawn.offsetToPlayerController.of(&playerPawn).get()));
    }

    [[nodiscard]] bool isLocalPlayerController(cs2::CCSPlayerController* playerController) const noexcept
    {
        return dependencies.requestDependency<LocalPlayerController>() && dependencies.getDependency<LocalPlayerController>() == playerController;
    }

    [[nodiscard]] bool isAlive(cs2::C_BaseEntity& entity) const noexcept
    {
        if (!dependencies.offsets().entity.offsetToLifeState)
            return true;
        return LifeState{*dependencies.offsets().entity.offsetToLifeState.of(&entity).get()} == LifeState::Alive;
    }

    [[nodiscard]] bool isEnemyTeam(TeamNumber team) const noexcept
    {
        return team != getLocalPlayerTeam() || teammatesAreEnemies();
    }

    [[nodiscard]] bool teammatesAreEnemies() const noexcept
    {
        if (!dependencies.requestDependency<ConVarAccessor>())
            return true;

        auto conVarAccessor = dependencies.getDependency<ConVarAccessor>();
        if (!conVarAccessor.requestConVar<cs2::mp_teammates_are_enemies>())
            return true;
        return conVarAccessor.getConVarValue<cs2::mp_teammates_are_enemies>();
    }

    [[nodiscard]] TeamNumber getLocalPlayerTeam() const noexcept
    {
        if (!dependencies.offsets().entity.offsetToTeamNumber)
            return {};

        if (!dependencies.requestDependency<LocalPlayerController>())
            return {};

        return TeamNumber{*dependencies.offsets().entity.offsetToTeamNumber.of(dependencies.getDependency<LocalPlayerController>()).get()};
    }

    [[nodiscard]] auto getPlayerColorCalculator(cs2::CCSPlayerController& playerController) const noexcept
    {
        return PlayerColorCalculator{PlayerColorIndexAccessor{playerController, dependencies.offsets().playerController.offsetToPlayerColor}};
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

        const auto styleSetter{dependencies.getDependency<PanelConfigurator>().panelStyle(*style)};
        styleSetter.setWashColor(getPlayerPositionArrowColorCalculator(playerController, teamNumber).getArrowColor(state.playerPositionArrowColor));
    }

    [[nodiscard]] const char* getActiveWeaponName(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        if (!dependencies.offsets().playerPawn.offsetToWeaponServices
         || !dependencies.offsets().weaponServices.offsetToActiveWeapon
         || !dependencies.offsets().entity.offsetToVData
         || !dependencies.offsets().weaponVData.offsetToWeaponName)
            return nullptr;

        if (!dependencies.requestDependency<EntityFromHandleFinder>())
            return nullptr;

        const auto weaponServices = *dependencies.offsets().playerPawn.offsetToWeaponServices.of(&playerPawn).get();
        if (!weaponServices)
            return nullptr;

        const auto activeWeapon = dependencies.getDependency<EntityFromHandleFinder>().getEntityFromHandle(*dependencies.offsets().weaponServices.offsetToActiveWeapon.of(weaponServices).get());
        if (!activeWeapon)
            return nullptr;

        const auto vData = static_cast<cs2::CCSWeaponBaseVData*>(*dependencies.offsets().entity.offsetToVData.of(static_cast<cs2::C_BaseEntity*>(activeWeapon)).get());
        if (!vData)
            return nullptr;

        return *dependencies.offsets().weaponVData.offsetToWeaponName.of(vData).get();
    }

    void setHealth(PanoramaUiPanel healthPanel, int health) const noexcept
    {
        if (!state.showPlayerHealth) {
            healthPanel.setVisible(false);
            return;
        }

        healthPanel.setVisible(true);
        const auto healthPanelChildren = healthPanel.children();
        if (!healthPanelChildren || healthPanelChildren->size < 2)
            return;

        const auto healthText = static_cast<cs2::CLabel*>(healthPanelChildren->memory[1]->clientPanel);

        if (const auto style{PanoramaUiPanel{healthText->uiPanel}.getStyle()}) {
            const auto styler{dependencies.getDependency<PanelConfigurator>().panelStyle(*style)};
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
        
        const auto playerStateChildren = playerStateIconsPanel.children();
        if (!playerStateChildren || playerStateChildren->size != 3)
            return;

        PanoramaUiPanel{playerStateChildren->memory[0]}.setVisible(state.playerStateIconsToShow.has<DefuseIconPanel>() && isDefusing(playerPawn));
        PanoramaUiPanel{playerStateChildren->memory[1]}.setVisible(state.playerStateIconsToShow.has<HostagePickupPanel>() && isPickingUpHostage(playerPawn));
        PanoramaUiPanel{playerStateChildren->memory[2]}.setVisible(state.playerStateIconsToShow.has<HostageRescuePanel>() && isRescuingHostage(playerPawn));
    }

    [[nodiscard]] bool isRescuingHostage(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        const auto hostageServices = dependencies.offsets().playerPawn.offsetToHostageServices.of(&playerPawn).valueOr(nullptr);
        if (!hostageServices)
            return false;

        if (!dependencies.requestDependency<EntityFromHandleFinder>())
            return false;

        return dependencies.getDependency<EntityFromHandleFinder>().getEntityFromHandle(dependencies.offsets().hostageServicesImpl.offsetToCarriedHostage.of(hostageServices).valueOr(cs2::CEntityHandle{cs2::INVALID_EHANDLE_INDEX})) != nullptr;
    }

    [[nodiscard]] bool isPickingUpHostage(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        return dependencies.offsets().playerPawn.offsetToIsPickingUpHostage.of(&playerPawn).valueOr(false);
    }

    [[nodiscard]] bool isDefusing(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        return dependencies.offsets().playerPawn.offsetToIsDefusing.of(&playerPawn).valueOr(false);
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
        if (!dependencies.offsets().imagePanel.offsetToImagePath)
            return true;

        const auto existingImagePath = *dependencies.offsets().imagePanel.offsetToImagePath.of(imagePanel).get();
        return !existingImagePath.m_pString || std::strcmp(existingImagePath.m_pString, newImagePath) != 0;
    }

    [[nodiscard]] TeamNumber getTeamNumber(cs2::C_BaseEntity& entity) const noexcept
    {
        if (dependencies.requestDependency<OffsetToTeamNumber>())
            return TeamNumber{*dependencies.getDependency<OffsetToTeamNumber>().of(&entity).get()};
        return {};
    }

    [[nodiscard]] bool requestCrucialDependencies() const noexcept
    {
        return dependencies.requestDependencies(kCrucialDependencies);
    }

    [[nodiscard]] bool hasImmunity(cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        if (dependencies.requestDependency<OffsetToPlayerPawnImmunity>())
            return *dependencies.getDependency<OffsetToPlayerPawnImmunity>().of(&playerPawn).get();
        return false;
    }

    [[nodiscard]] float getFovScale() const noexcept
    {
        if (dependencies.requestDependency<FovScale>())
            return dependencies.getDependency<FovScale>();
        return 1.0f;
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
            .set<OffsetToGameSceneNode>()
            .set<OffsetToAbsOrigin>()
            .set<WorldToClipSpaceConverter>()
            .set<HudInWorldPanelContainer>()
            .set<HudProvider>()
            .set<PanelConfigurator>()
            .set<PanoramaTransformFactory>()};

    PlayerInformationThroughWallsState& state;
    HookDependencies& dependencies;
    std::size_t currentIndex{0};
};
