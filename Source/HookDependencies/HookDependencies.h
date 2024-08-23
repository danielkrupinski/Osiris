#pragma once

#include "HookDependenciesBuilder.h"
#include "HookDependenciesMask.h"
#include <FeatureHelpers/EntityFromHandleFinder.h>
#include <FeatureHelpers/EntityListWalker.h>
#include <GameClasses/FileSystem.h>
#include <GameClasses/Hud/Hud.h>
#include <GameClasses/Hud/HudContext.h>
#include <GameClasses/PanelFactory.h>
#include <GameClasses/Panels.h>
#include <GameClasses/PlantedC4.h>
#include <GameClasses/PlayerController.h>
#include <GameClasses/GlobalVars.h>
#include <GameClasses/GameRules.h>

struct BombStatusPanelState;
struct FeaturesStates;

struct HookDependencies {
    HookDependencies(GameDependencies& gameDependencies, FeatureHelpers& featureHelpers, BombStatusPanelState& bombStatusPanelState, InWorldPanelContainerState& inWorldPanelContainerState, FeaturesStates& featuresStates) noexcept
        : _gameDependencies{gameDependencies}
        , featureHelpers{featureHelpers}
        , _bombStatusPanelState{bombStatusPanelState}
        , _inWorldPanelContainerState{inWorldPanelContainerState}
        , _featuresStates{featuresStates}
    {
        if (gameDependencies.worldToProjectionMatrix)
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

        if constexpr (std::is_same_v<Dependency, EntityListWalker>) {
            return EntityListWalker{*entityList, highestEntityIndex};
        } else if constexpr (std::is_same_v<Dependency, EntityFromHandleFinder>) {
            return EntityFromHandleFinder{*entityList};
        } else if constexpr (std::is_same_v<Dependency, WorldToClipSpaceConverter>) {
            return WorldToClipSpaceConverter{_gameDependencies.worldToProjectionMatrix};
        } else if constexpr (std::is_same_v<Dependency, SoundChannels>) {
            return (*soundChannels);
        } else if constexpr (std::is_same_v<Dependency, FileSystem>) {
            return FileSystem{*fileSystem, _gameDependencies.fileSystemDeps};
        } else {
            static_assert(!std::is_same_v<Dependency, Dependency>, "Unknown dependency");
        }
    }

    [[nodiscard]] GameDependencies& gameDependencies() const noexcept
    {
        return _gameDependencies;
    }

    [[nodiscard]] FeatureHelpers& getFeatureHelpers() const noexcept
    {
        return featureHelpers;
    }

    [[nodiscard]] BombStatusPanelState& bombStatusPanelState() const noexcept
    {
        return _bombStatusPanelState;
    }

    [[nodiscard]] InWorldPanelContainerState& inWorldPanelContainerState() const noexcept
    {
        return _inWorldPanelContainerState;
    }

    [[nodiscard]] FeaturesStates& featuresStates() const noexcept
    {
        return _featuresStates;
    }

    [[nodiscard]] auto hud() noexcept
    {
        return Hud{HudContext{*this}};
    }

    [[nodiscard]] cs2::CCSPlayerController* localPlayerController() const noexcept
    {
        if (_gameDependencies.localPlayerController)
            return *_gameDependencies.localPlayerController;
        return nullptr;
    }

    [[nodiscard]] auto localPlayerController2() const noexcept
    {
        if (_gameDependencies.localPlayerController)
            return PlayerController{*this, *_gameDependencies.localPlayerController};
        return PlayerController{*this, nullptr};
    }

    [[nodiscard]] GlobalVars globalVars() noexcept
    {
        if (_gameDependencies.globalVarsDeps.globalVars)
            return GlobalVars{*_gameDependencies.globalVarsDeps.globalVars};
        return GlobalVars{nullptr};
    }

    [[nodiscard]] auto gameRules() noexcept
    {
        if (_gameDependencies.gameRulesDeps.gameRules)
            return GameRules{*this, *_gameDependencies.gameRulesDeps.gameRules};
        return GameRules{*this, nullptr};
    }

    [[nodiscard]] auto plantedC4() noexcept
    {
        const auto base = PlantedC4Base{getPlantedC4()};
        if (base.thisptr)
            return std::optional<PlantedC4<HookDependencies&>>{PlantedC4<HookDependencies&>{base, *this}};
        return std::optional<PlantedC4<HookDependencies&>>{};
    }

    [[nodiscard]] ConVarAccessor getConVarAccessor() noexcept
    {
        if (!_gameDependencies.conVars.has_value()) {
            if (_gameDependencies.cvarDeps.cvar && _gameDependencies.cvarDeps.offsetToConVarList) {
                if (const auto cvar = *_gameDependencies.cvarDeps.cvar)
                    _gameDependencies.conVars.emplace(ConVarFinder{*_gameDependencies.cvarDeps.offsetToConVarList.of(cvar).get()});
            }
        }
        return ConVarAccessor{*_gameDependencies.conVars, _gameDependencies.conVarDeps, conVarAccessorState};
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

    [[nodiscard]] auto panels() noexcept
    {
        return Panels{*this};
    }

    [[nodiscard]] auto panelFactory() noexcept
    {
        return PanelFactory{*this};
    }

    [[nodiscard]] auto panoramaTransformFactory() noexcept
    {
        return PanoramaTransformFactory{_gameDependencies.transformTranslate3dVmt, _gameDependencies.transformScale3dVmt};
    }

private:
    [[nodiscard]] cs2::CPlantedC4* getPlantedC4() const noexcept
    {
        const auto* const plantedC4s = _gameDependencies.plantedC4Deps.plantedC4s;
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
        const HookDependenciesBuilder builder{requiredDependencies, _gameDependencies};

        presentDependencies |= builder.getEntityList(&entityList, &highestEntityIndex);
        presentDependencies |= builder.getSoundChannels(&soundChannels);
        presentDependencies |= builder.getFileSystem(&fileSystem);
    }

    GameDependencies& _gameDependencies;
    FeatureHelpers& featureHelpers;
    BombStatusPanelState& _bombStatusPanelState;
    InWorldPanelContainerState& _inWorldPanelContainerState;
    FeaturesStates& _featuresStates;

    const cs2::CConcreteEntityList* entityList;
    cs2::CEntityIndex highestEntityIndex{cs2::kMaxValidEntityIndex};
    cs2::SoundChannels* soundChannels;
    cs2::CBaseFileSystem* fileSystem;
    ConVarAccessorState conVarAccessorState;
    HookDependenciesMask presentDependencies;
};
