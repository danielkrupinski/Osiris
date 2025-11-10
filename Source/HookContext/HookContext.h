#pragma once

#include <CS2/Classes/CCvar.h>
#include <Config/Config.h>
#include <GameClient/Entities/GameRules.h>
#include <GameClient/Entities/PlantedC4.h>
#include <GameClient/Entities/PlayerController.h>
#include <Features/Common/InWorldPanelsPerHookState.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCachePerHookState.h>
#include <GameClient/ConVars/CvarSystem.h>
#include <GameClient/Entities/PlayerResource.h>
#include <GameClient/FileSystem.h>
#include <GameClient/Hud/Hud.h>
#include <GameClient/Hud/HudContext.h>
#include <MemoryPatterns/PatternTypes/ClientPatternTypes.h>
#include <MemoryPatterns/PatternTypes/CvarPatternTypes.h>
#include <MemoryPatterns/PatternTypes/GameRulesPatternTypes.h>
#include <MemoryPatterns/PatternTypes/PlantedC4PatternTypes.h>
#include <GameClient/Panorama/PanelFactory.h>
#include <GameClient/GlobalVars.h>
#include <GameClient/Panorama/PanoramaTransformFactory.h>

struct BombStatusPanelState;
struct FeaturesStates;
struct PanoramaGuiState;
struct GlowSceneObjectsState;
struct Hooks;
class EntityClassifier;

template <typename GlobalContext>
struct HookContext {
    HookContext() noexcept
        : fullGlobalContext{GlobalContext::instance().fullContext()}
    {
    }

    [[nodiscard]] static bool isGlobalContextComplete() noexcept
    {
        return GlobalContext::instance().isComplete();
    }

    static void initCompleteGlobalContextFromGameThread() noexcept
    {
        GlobalContext::instance().initCompleteContextFromGameThread();
    }

    static void destroyGlobalContext() noexcept
    {
        GlobalContext::destroyInstance();
    }

    [[nodiscard]] SoundWatcherState& soundWatcherState() const noexcept
    {
        return fullGlobalContext.soundWatcherState;
    }

    [[nodiscard]] BombStatusPanelState& bombStatusPanelState() const noexcept
    {
        return fullGlobalContext.bombStatusPanelState;
    }

    [[nodiscard]] InWorldPanelsState& inWorldPanelsState() const noexcept
    {
        return fullGlobalContext.inWorldPanelsState;
    }

    [[nodiscard]] InWorldPanelsPerHookState& inWorldPanelsPerHookState() noexcept
    {
        return _inWorldPanelsPerHookState;
    }

    [[nodiscard]] PlayerInfoPanelCacheState& playerInfoPanelCacheState() const noexcept
    {
        return fullGlobalContext.playerInfoPanelCacheState;
    }

    [[nodiscard]] PlayerInfoPanelCachePerHookState& playerInfoPanelCachePerHookState() noexcept
    {
        return _playerInfoPanelCachePerHookState;
    }

    [[nodiscard]] PanoramaGuiState& panoramaGuiState() const noexcept
    {
        return fullGlobalContext.panoramaGuiState;
    }

    [[nodiscard]] FeaturesStates& featuresStates() const noexcept
    {
        return fullGlobalContext.featuresStates;
    }

    [[nodiscard]] GlowSceneObjectsState& glowSceneObjectsState() const noexcept
    {
        return fullGlobalContext.glowSceneObjectsState;
    }

    [[nodiscard]] EntityClassifier& entityClassifier() const noexcept
    {
        return fullGlobalContext.entityClassifier;
    }

    [[nodiscard]] auto& playerModelGlowPreviewState() const noexcept
    {
        return fullGlobalContext.playerModelGlowPreviewState;
    }

    [[nodiscard]] auto& weaponModelGlowPreviewState() const noexcept
    {
        return fullGlobalContext.weaponModelGlowPreviewState;
    }

    [[nodiscard]] Hooks& hooks() const noexcept
    {
        return fullGlobalContext.hooks;
    }

    [[nodiscard]] auto hud() noexcept
    {
        return Hud{HudContext{*this}};
    }

    [[nodiscard]] auto localPlayerController() noexcept
    {
        if (fullGlobalContext.patternSearchResults.template get<LocalPlayerControllerPointer>())
            return PlayerController{*this, *fullGlobalContext.patternSearchResults.template get<LocalPlayerControllerPointer>()};
        return PlayerController{*this, nullptr};
    }

    [[nodiscard]] auto globalVars() noexcept
    {
        if (fullGlobalContext.patternSearchResults.template get<GlobalVarsPointer>())
            return GlobalVars{*this, *fullGlobalContext.patternSearchResults.template get<GlobalVarsPointer>()};
        return GlobalVars{*this, nullptr};
    }

    [[nodiscard]] auto gameRules() noexcept
    {
        if (fullGlobalContext.patternSearchResults.template get<GameRulesPointer>())
            return GameRules{*this, *fullGlobalContext.patternSearchResults.template get<GameRulesPointer>()};
        return GameRules{*this, nullptr};
    }

    [[nodiscard]] auto plantedC4() noexcept
    {
        return std::optional{make<PlantedC4<HookContext>>(getPlantedC4())};
    }

    [[nodiscard]] auto cvarSystem() noexcept
    {
        return CvarSystem{*this};
    }

    [[nodiscard]] const auto& getConVarsBase() noexcept
    {
        if (!fullGlobalContext.conVars.has_value())
            fullGlobalContext.conVars.emplace(CvarSystem{*this});
        return *fullGlobalContext.conVars;
    }

    template <typename T, typename... Args>
    [[nodiscard]] auto make(Args&&... args) noexcept
    {
        return T{*this, std::forward<Args>(args)...};
    }

    template <template <typename...> typename T, typename... Args>
    [[nodiscard]] auto make(Args&&... args) noexcept
    {
        return T<HookContext>{*this, std::forward<Args>(args)...};
    }

    [[nodiscard]] auto panelFactory() noexcept
    {
        return PanelFactory{*this};
    }

    [[nodiscard]] auto panoramaTransformFactory() noexcept
    {
        return PanoramaTransformFactory{*this, fullGlobalContext.patternSearchResults.template get<TransformTranslate3dVMT>(), fullGlobalContext.patternSearchResults.template get<TransformScale3dVMT>()};
    }

    [[nodiscard]] const auto& panoramaSymbols() noexcept
    {
        auto& symbols = fullGlobalContext.panoramaSymbols;
        if (!symbols.has_value())
            symbols.emplace(*this);
        return *symbols;
    }

    [[nodiscard]] const auto& patternSearchResults() noexcept
    {
        return fullGlobalContext.patternSearchResults;
    }

    [[nodiscard]] auto& hudState() noexcept
    {
        return fullGlobalContext.hudState;
    }

    [[nodiscard]] auto& fileNameSymbolTableState() noexcept
    {
        return fullGlobalContext.fileNameSymbolTableState;
    }

    [[nodiscard]] auto& memAllocState() noexcept
    {
        return fullGlobalContext.memAllocState;
    }

    [[nodiscard]] auto& glowSceneObjectState() noexcept
    {
        return fullGlobalContext.glowSceneObjectState;
    }

    [[nodiscard]] auto& stylePropertySymbolsAndVMTs() noexcept
    {
        return fullGlobalContext.stylePropertySymbolsAndVMTs;
    }

    [[nodiscard]] auto config() noexcept
    {
        return Config{*this};
    }

    [[nodiscard]] auto& configState() noexcept
    {
        return fullGlobalContext.configState;
    }

    [[nodiscard]] auto gui() noexcept
    {
        return PanoramaGUI{*this};
    }

    [[nodiscard]] const auto& osirisDirectoryPath() noexcept
    {
        return fullGlobalContext.osirisDirectoryPath;
    }

    [[nodiscard]] auto soundWatcher() noexcept
    {
        return SoundWatcher<HookContext>{fullGlobalContext.soundWatcherState, *this};
    }

    [[nodiscard]] auto uiPanel(cs2::CUIPanel* panel) noexcept
    {
        return PanoramaUiPanel<HookContext>{*this, panel};
    }

    [[nodiscard]] decltype(auto) activeLocalPlayerPawn() noexcept
    {
        return localPlayerController().pawn().template cast<PlayerPawn>();
    }

    [[nodiscard]] decltype(auto) localPlayerBulletInaccuracy() noexcept
    {
        return activeLocalPlayerPawn().getActiveWeapon().bulletInaccuracy();
    }

    [[nodiscard]] decltype(auto) playerResource()
    {
        if (fullGlobalContext.patternSearchResults.template get<PointerToPlayerResource>())
            return PlayerResource{*this, *fullGlobalContext.patternSearchResults.template get<PointerToPlayerResource>()};
        return PlayerResource{*this, nullptr};
    }

private:
    [[nodiscard]] cs2::CPlantedC4* getPlantedC4() const noexcept
    {
        const auto* const plantedC4s = fullGlobalContext.patternSearchResults.template get<PlantedC4sPointer>();
        if (plantedC4s && plantedC4s->size > 0)
            return plantedC4s->memory[0];
        return nullptr;
    }

    GlobalContext::Complete& fullGlobalContext;
    InWorldPanelsPerHookState _inWorldPanelsPerHookState;
    PlayerInfoPanelCachePerHookState _playerInfoPanelCachePerHookState;
};
