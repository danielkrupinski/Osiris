#pragma once

#include <CS2/Classes/CGameSceneNode.h>
#include <CS2/Classes/ConVarTypes.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
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

#include "States/PlayerInformationThroughWallsState.h"

#include <HookDependencies/HookDependencies.h>
#include <HookDependencies/HookDependenciesMask.h>

class PlayerInformationThroughWallsPanelFactory {
public:
    PlayerInformationThroughWallsPanelFactory(cs2::CUIPanel& parentPanel, HookDependencies& dependencies, PanelConfigurator panelConfigurator) noexcept
        : parentPanel{parentPanel}
        , dependencies{dependencies}
        , panelConfigurator{panelConfigurator}
    {
    }

    [[nodiscard]] PanoramaUiPanel createPanel(PanoramaTransformFactory panoramaTransformFactory) const noexcept
    {
        const auto containerPanel{Panel::create("", &parentPanel)};
        if (!containerPanel)
            return PanoramaUiPanel{nullptr};

        if (const auto style{PanoramaUiPanel{containerPanel->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setWidth(cs2::CUILength::pixels(kWidth));
            styler.setHeight(cs2::CUILength::pixels(kHeight));
            styler.setPosition(cs2::CUILength::pixels(-kWidth * 0.5f), cs2::CUILength::pixels(0.0f));
            styler.setTransformOrigin(cs2::CUILength::percent(50), cs2::CUILength::percent(0));
            styler.setFlowChildren(cs2::k_EFlowDown);
        }

        createPositionArrowPanel(containerPanel->uiPanel, panoramaTransformFactory);
        createHealthPanel(containerPanel->uiPanel);
        createActiveWeaponIconPanel(containerPanel->uiPanel);
        return PanoramaUiPanel{containerPanel->uiPanel};
    }

private:
    void createActiveWeaponIconPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        const auto weaponIconPanel = PanoramaImagePanel::create("", containerPanel);
        if (!weaponIconPanel)
            return;

        if (const auto style{PanoramaUiPanel{weaponIconPanel->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
            styler.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(3), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
            styler.setImageShadow(shadowParams());
        }
    }

    void createHealthPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        const auto healthPanel{Panel::create("", containerPanel)};
        if (!healthPanel)
            return;

        if (const auto style{PanoramaUiPanel{healthPanel->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
            styler.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(1), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
            styler.setFlowChildren(cs2::k_EFlowRight);
        }

        createHealthIconPanel(healthPanel->uiPanel);
        createHealthTextPanel(healthPanel->uiPanel);
    }

    void createHealthIconPanel(cs2::CUIPanel* containerPanel) const
    {
        const auto healthIconPanel = PanoramaImagePanel::create("", containerPanel);
        if (!healthIconPanel)
            return;

        PanoramaImagePanel{healthIconPanel}.setImageSvg("s2r://panorama/images/hud/health_cross.vsvg", 24);
        if (const auto style{PanoramaUiPanel{healthIconPanel->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
            styler.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(0), cs2::CUILength::pixels(5), cs2::CUILength::pixels(0));
            styler.setImageShadow(shadowParams());
        }
    }

    void createHealthTextPanel(cs2::CUIPanel* containerPanel) const
    {
        if (!dependencies.requestDependency<PanoramaLabelFactory>())
            return;

        const auto label = dependencies.getDependency<PanoramaLabelFactory>().createLabelPanel(containerPanel);
        if (!label)
            return;

        if (const auto style{PanoramaUiPanel{label->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setFont("Stratum2, 'Arial Unicode MS'", 24.0f, cs2::k_EFontWeightBlack);
            styler.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
            styler.setTextShadow(shadowParams());
        }
    }

    void createPositionArrowPanel(cs2::CUIPanel* containerPanel, PanoramaTransformFactory panoramaTransformFactory) const noexcept
    {
        const auto imagePanel = PanoramaImagePanel::create("", containerPanel);
        if (!imagePanel)
            return;

        PanoramaImagePanel{imagePanel}.setImageSvg("s2r://panorama/images/hud/reticle/playerid_arrow.vsvg", 24);
        if (const auto style{PanoramaUiPanel{imagePanel->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
            styler.setImageShadow(shadowParams());
            PanoramaTransformations{
                panoramaTransformFactory.scale(1.0f, -1.0f),
            }.applyTo(styler);
        }
    }

    [[nodiscard]] static PanelShadowParams shadowParams() noexcept
    {
        return PanelShadowParams{
            .horizontalOffset{cs2::CUILength::pixels(0)},
            .verticalOffset{cs2::CUILength::pixels(0)},
            .blurRadius{cs2::CUILength::pixels(3)},
            .strength = 3,
            .color{0, 0, 0}
        };
    }

    static constexpr auto kWidth{256};
    static constexpr auto kHeight{256};

    cs2::CUIPanel& parentPanel;
    HookDependencies& dependencies;
    PanelConfigurator panelConfigurator;
};

struct PlayerPositionToggle : public TogglableFeature<PlayerPositionToggle> {
    explicit PlayerPositionToggle(PlayerInformationThroughWallsState& state) noexcept
        : TogglableFeature{state.showPlayerPosition}
    {
    }
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

    void run(cs2::C_CSPlayerPawn& nonLocalPlayerPawn) noexcept
    {
        if (!state.enabled)
            return;

        if (!requestCrucialDependencies())
            return;

        const auto teamNumber = getTeamNumber(nonLocalPlayerPawn);
        if (teamNumber != TeamNumber::TT && teamNumber != TeamNumber::CT)
            return;

        if (state.showOnlyEnemies && !isEnemyTeam(teamNumber))
            return;

        if (!isAlive(nonLocalPlayerPawn))
            return;

        const auto hasHealth = dependencies.requestDependency<OffsetToHealth>();
        const auto health = hasHealth ? *dependencies.getDependency<OffsetToHealth>().of(&nonLocalPlayerPawn).get() : 100;
        if (health <= 0)
            return;

        const auto gameSceneNode = *dependencies.getDependency<OffsetToGameSceneNode>().of(&nonLocalPlayerPawn).get();
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

        setArrowColor(panel, getArrowColor(teamNumber));
        if (hasHealth)
            setHealth(panel, health);
        setActiveWeapon(panel, nonLocalPlayerPawn);

        const auto style = panel.getStyle();
        if (!style)
            return;

        const auto styleSetter{dependencies.getDependency<PanelConfigurator>().panelStyle(*style)};
        styleSetter.setOpacity(hasImmunity(nonLocalPlayerPawn) ? 0.5f : 1.0f);
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

    void setArrowColor(PanoramaUiPanel parentPanel, cs2::Color color) const noexcept
    {
        const auto children = parentPanel.children();
        if (!children || children->size < 1)
            return;

        const PanoramaUiPanel arrowPanel{children->memory[0]};
        if (!state.showPlayerPosition) {
            arrowPanel.setVisible(false);
            return;
        }

        arrowPanel.setVisible(true);
        const auto style = arrowPanel.getStyle();
        if (!style)
            return;

        const auto styleSetter{dependencies.getDependency<PanelConfigurator>().panelStyle(*style)};
        styleSetter.setWashColor(color);
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

    void setHealth(PanoramaUiPanel parentPanel, int health) const noexcept
    {
        const auto children = parentPanel.children();
        if (!children || children->size < 2)
            return;

        const PanoramaUiPanel healthPanel{children->memory[1]};
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
                styler.setSimpleForegroundColor(cs2::Color{255, 255, 255});
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

    void setActiveWeapon(PanoramaUiPanel parentPanel, cs2::C_CSPlayerPawn& playerPawn) const noexcept
    {
        const auto children = parentPanel.children();
        if (!children || children->size < 3)
            return;

        const PanoramaUiPanel weaponIconPanel{children->memory[2]};
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

    [[nodiscard]] static cs2::Color getArrowColor(TeamNumber teamNumber) noexcept
    {
        switch (teamNumber) {
            using enum TeamNumber;
            case TT: return cs2::Color{234, 190, 84};
            case CT: return cs2::Color{150, 200, 250};
            default: return cs2::Color{255, 255, 255};
        }
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
        if (const auto panel{PlayerInformationThroughWallsPanelFactory{*static_cast<cs2::CUIPanel*>(containerPanel), dependencies, panelConfigurator}.createPanel(dependencies.getDependency<PanoramaTransformFactory>())}) {
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
