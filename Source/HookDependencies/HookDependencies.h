#pragma once

#include <Entities/GameRules.h>
#include <Entities/PlantedC4.h>
#include <Entities/PlayerController.h>
#include <FeatureHelpers/ConVarAccessor.h>
#include <FeatureHelpers/ConVarFinder.h>
#include <GameClasses/FileSystem.h>
#include <GameClasses/Hud/Hud.h>
#include <GameClasses/Hud/HudContext.h>
#include <GameClasses/PanelFactory.h>
#include <GameClasses/GlobalVars.h>
#include <Helpers/PanoramaTransformFactory.h>

struct BombStatusPanelState;
struct FeaturesStates;
struct PanoramaGuiState;
struct GlowSceneObjectsState;
struct Hooks;
class EntityClassifier;

template <typename FullGlobalContext>
struct HookDependencies {
    HookDependencies(FullGlobalContext& fullGlobalContext) noexcept
        : fullGlobalContext{fullGlobalContext}
    {
    }

    [[nodiscard]] GameDependencies& gameDependencies() const noexcept
    {
        return fullGlobalContext.gameDependencies;
    }

    [[nodiscard]] SoundWatcherState& soundWatcherState() const noexcept
    {
        return fullGlobalContext.soundWatcherState;
    }

    [[nodiscard]] BombStatusPanelState& bombStatusPanelState() const noexcept
    {
        return fullGlobalContext.bombStatusPanelState;
    }

    [[nodiscard]] InWorldPanelContainerState& inWorldPanelContainerState() const noexcept
    {
        return fullGlobalContext.inWorldPanelContainerState;
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
        return std::optional{make<PlantedC4<HookDependencies>>(getPlantedC4())};
    }

    [[nodiscard]] auto getConVarAccessor() noexcept
    {
        if (!fullGlobalContext.gameDependencies.conVars.has_value()) {
            const auto cvar = fullGlobalContext.clientPatternSearchResults.template get<CvarPointer>();
            if (cvar && *cvar && fullGlobalContext.tier0PatternSearchResults.template get<OffsetToConVarList>()) {
                fullGlobalContext.gameDependencies.conVars.emplace(ConVarFinder{*fullGlobalContext.tier0PatternSearchResults.template get<OffsetToConVarList>().of(*cvar).get()});
            }
        }
        return ConVarAccessor{*this, *fullGlobalContext.gameDependencies.conVars, conVarAccessorState};
    }

    template <typename T, typename... Args>
    [[nodiscard]] auto make(Args&&... args) noexcept
    {
        return T{*this, std::forward<Args>(args)...};
    }

    template <template <typename> typename T, typename... Args>
    [[nodiscard]] auto make(Args&&... args) noexcept
    {
        return T{*this, std::forward<Args>(args)...};
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
        auto& symbols = fullGlobalContext.gameDependencies.panoramaSymbols;
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
};
