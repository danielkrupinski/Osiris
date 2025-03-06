#pragma once

#include <Config/Config.h>
#include <GameClient/Entities/GameRules.h>
#include <GameClient/Entities/PlantedC4.h>
#include <GameClient/Entities/PlayerController.h>
#include <Features/Common/InWorldPanelsPerHookState.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCachePerHookState.h>
#include <GameClient/ConVars/ConVarAccessor.h>
#include <GameClient/ConVars/ConVarFinder.h>
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

template <typename FullGlobalContext>
struct HookContext {
    HookContext(FullGlobalContext& fullGlobalContext) noexcept
        : fullGlobalContext{fullGlobalContext}
    {
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

    [[nodiscard]] auto features() const noexcept
    {
        return Features{fullGlobalContext.featuresStates, fullGlobalContext.hooks, *this};
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
        if (fullGlobalContext.clientPatternSearchResults.template get<LocalPlayerControllerPointer>())
            return PlayerController{*this, *fullGlobalContext.clientPatternSearchResults.template get<LocalPlayerControllerPointer>()};
        return PlayerController{*this, nullptr};
    }

    [[nodiscard]] GlobalVars globalVars() noexcept
    {
        if (fullGlobalContext.clientPatternSearchResults.template get<GlobalVarsPointer>())
            return GlobalVars{*fullGlobalContext.clientPatternSearchResults.template get<GlobalVarsPointer>()};
        return GlobalVars{nullptr};
    }

    [[nodiscard]] auto gameRules() noexcept
    {
        if (fullGlobalContext.clientPatternSearchResults.template get<GameRulesPointer>())
            return GameRules{*this, *fullGlobalContext.clientPatternSearchResults.template get<GameRulesPointer>()};
        return GameRules{*this, nullptr};
    }

    [[nodiscard]] auto plantedC4() noexcept
    {
        return std::optional{make<PlantedC4<HookContext>>(getPlantedC4())};
    }

    [[nodiscard]] auto getConVarAccessor() noexcept
    {
        if (!fullGlobalContext.conVars.has_value()) {
            const auto cvar = fullGlobalContext.clientPatternSearchResults.template get<CvarPointer>();
            if (cvar && *cvar && fullGlobalContext.tier0PatternSearchResults.template get<OffsetToConVarList>()) {
                fullGlobalContext.conVars.emplace(ConVarFinder{*fullGlobalContext.tier0PatternSearchResults.template get<OffsetToConVarList>().of(*cvar).get()});
            }
        }
        return ConVarAccessor{*this, *fullGlobalContext.conVars, conVarAccessorState};
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
        return PanoramaTransformFactory{*this, fullGlobalContext.clientPatternSearchResults.template get<TransformTranslate3dVMT>(), fullGlobalContext.clientPatternSearchResults.template get<TransformScale3dVMT>()};
    }

    [[nodiscard]] const auto& panoramaSymbols() noexcept
    {
        auto& symbols = fullGlobalContext.panoramaSymbols;
        if (!symbols.has_value())
            symbols.emplace(*this);
        return *symbols;
    }

    [[nodiscard]] const auto& clientPatternSearchResults() noexcept
    {
        return fullGlobalContext.clientPatternSearchResults;
    }

    [[nodiscard]] const auto& sceneSystemPatternSearchResults() noexcept
    {
        return fullGlobalContext.sceneSystemPatternSearchResults;
    }

    [[nodiscard]] const auto& tier0PatternSearchResults() noexcept
    {
        return fullGlobalContext.tier0PatternSearchResults;
    }

    [[nodiscard]] const auto& fileSystemPatternSearchResults() noexcept
    {
        return fullGlobalContext.fileSystemPatternSearchResults;
    }

    [[nodiscard]] const auto& soundSystemPatternSearchResults() noexcept
    {
        return fullGlobalContext.soundSystemPatternSearchResults;
    }

    [[nodiscard]] const auto& panoramaPatternSearchResults() noexcept
    {
        return fullGlobalContext.panoramaPatternSearchResults;
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

private:
    [[nodiscard]] cs2::CPlantedC4* getPlantedC4() const noexcept
    {
        const auto* const plantedC4s = fullGlobalContext.clientPatternSearchResults.template get<PlantedC4sPointer>();
        if (plantedC4s && plantedC4s->size > 0)
            return plantedC4s->memory[0];
        return nullptr;
    }

    FullGlobalContext& fullGlobalContext;
    ConVarAccessorState conVarAccessorState;
    InWorldPanelsPerHookState _inWorldPanelsPerHookState;
    PlayerInfoPanelCachePerHookState _playerInfoPanelCachePerHookState;
};
