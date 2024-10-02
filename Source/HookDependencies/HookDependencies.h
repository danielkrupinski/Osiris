#pragma once

#include "HookDependenciesBuilder.h"
#include "HookDependenciesMask.h"
#include <FeatureHelpers/ConVarAccessor.h>
#include <GameClasses/FileSystem.h>
#include <GameClasses/Hud/Hud.h>
#include <GameClasses/Hud/HudContext.h>
#include <GameClasses/PanelFactory.h>
#include <GameClasses/PlantedC4.h>
#include <GameClasses/PlayerController.h>
#include <GameClasses/GlobalVars.h>
#include <GameClasses/GameRules.h>

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
        if (fullGlobalContext.gameDependencies.worldToProjectionMatrix)
            presentDependencies |= HookDependenciesMask{}.set<WorldToClipSpaceConverter>();
    }

    [[nodiscard]] bool requestDependencies(HookDependenciesMask requiredDependencies) noexcept
    {
        prepareDependencies(requiredDependencies.difference(presentDependencies));
        return presentDependencies.contains(requiredDependencies);
    }

    template <typename Dependency>
    [[nodiscard]] bool requestDependency() noexcept
    {
        if (!hasDependency<Dependency>())
            prepareDependencies(HookDependenciesMask{}.set<Dependency>());
        return hasDependency<Dependency>();
    }

    template <typename Dependency>
    [[nodiscard]] decltype(auto) getDependency() noexcept
    {
        assert(hasDependency<Dependency>());

        if constexpr (std::is_same_v<Dependency, WorldToClipSpaceConverter>) {
            return WorldToClipSpaceConverter{fullGlobalContext.gameDependencies.worldToProjectionMatrix};
        } else if constexpr (std::is_same_v<Dependency, SoundChannels>) {
            return (*soundChannels);
        } else {
            static_assert(!std::is_same_v<Dependency, Dependency>, "Unknown dependency");
        }
    }

    [[nodiscard]] GameDependencies& gameDependencies() const noexcept
    {
        return fullGlobalContext.gameDependencies;
    }

    [[nodiscard]] FeatureHelpers& getFeatureHelpers() const noexcept
    {
        return fullGlobalContext.featureHelpers;
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
        if (fullGlobalContext.gameDependencies.localPlayerController)
            return PlayerController{*this, *fullGlobalContext.gameDependencies.localPlayerController};
        return PlayerController{*this, nullptr};
    }

    [[nodiscard]] GlobalVars globalVars() noexcept
    {
        if (fullGlobalContext.gameDependencies.globalVarsDeps.globalVars)
            return GlobalVars{*fullGlobalContext.gameDependencies.globalVarsDeps.globalVars};
        return GlobalVars{nullptr};
    }

    [[nodiscard]] auto gameRules() noexcept
    {
        if (fullGlobalContext.gameDependencies.gameRulesDeps.gameRules)
            return GameRules{*this, *fullGlobalContext.gameDependencies.gameRulesDeps.gameRules};
        return GameRules{*this, nullptr};
    }

    [[nodiscard]] auto plantedC4() noexcept
    {
        return std::optional{make<PlantedC4<HookDependencies>>(getPlantedC4())};
    }

    [[nodiscard]] ConVarAccessor getConVarAccessor() noexcept
    {
        if (!fullGlobalContext.gameDependencies.conVars.has_value()) {
            if (fullGlobalContext.gameDependencies.cvarDeps.cvar && fullGlobalContext.gameDependencies.cvarDeps.offsetToConVarList) {
                if (const auto cvar = *fullGlobalContext.gameDependencies.cvarDeps.cvar)
                    fullGlobalContext.gameDependencies.conVars.emplace(ConVarFinder{*fullGlobalContext.gameDependencies.cvarDeps.offsetToConVarList.of(cvar).get()});
            }
        }
        return ConVarAccessor{*fullGlobalContext.gameDependencies.conVars, fullGlobalContext.gameDependencies.conVarDeps, conVarAccessorState};
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
        return PanoramaTransformFactory{*this, fullGlobalContext.gameDependencies.transformTranslate3dVmt, fullGlobalContext.gameDependencies.transformScale3dVmt};
    }

    [[nodiscard]] const auto& panoramaSymbols() noexcept
    {
        auto& symbols = fullGlobalContext.gameDependencies.panoramaSymbols;
        if (!symbols.has_value())
            symbols.emplace(*this);
        return *symbols;
    }

private:
    [[nodiscard]] cs2::CPlantedC4* getPlantedC4() const noexcept
    {
        const auto* const plantedC4s = fullGlobalContext.gameDependencies.plantedC4Deps.plantedC4s;
        if (plantedC4s && plantedC4s->size > 0)
            return plantedC4s->memory[0];
        return nullptr;
    }

    template <typename Dependency>
    [[nodiscard]] bool hasDependency() const noexcept
    {
        return presentDependencies.has<Dependency>();
    }

    void prepareDependencies(HookDependenciesMask requiredDependencies) noexcept
    {
        const HookDependenciesBuilder builder{requiredDependencies, fullGlobalContext.gameDependencies};

        presentDependencies |= builder.getSoundChannels(&soundChannels);
    }

    FullGlobalContext& fullGlobalContext;

    cs2::SoundChannels* soundChannels;
    cs2::CBaseFileSystem* fileSystem;
    ConVarAccessorState conVarAccessorState;
    HookDependenciesMask presentDependencies;
};
