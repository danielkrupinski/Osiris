#pragma once

#include <FeatureHelpers/ConVarAccessor.h>
#include <GameClasses/OffsetTypes/EntityOffset.h>
#include <GameClasses/OffsetTypes/GameSceneNodeOffset.h>
#include <GameClasses/OffsetTypes/PlayerControllerOffset.h>
#include <GameClasses/OffsetTypes/PlayerPawnOffset.h>
#include <Utils/TypeBitFlags.h>

struct EntitiesVMTs;
class EntityFromHandleFinder;
class EntityListWalker;
struct FileSystem;
struct FovScale;
class HudInWorldPanelContainer;
struct LocalPlayerController;
struct PanelConfigurator;
struct PanoramaLabelFactory;
struct PanoramaTransformFactory;
struct PlantedC4;
struct SoundChannels;
struct WorldToClipSpaceConverter;

using HookDependenciesMask = TypeBitFlags<
    OffsetToGameSceneNode,
    OffsetToPlayerPawnHandle,
    EntityListWalker,
    EntityFromHandleFinder,
    LocalPlayerController,
    EntitiesVMTs,
    OffsetToAbsOrigin,
    WorldToClipSpaceConverter,
    HudInWorldPanelContainer,
    PanelConfigurator,
    PanoramaTransformFactory,
    OffsetToHealth,
    OffsetToTeamNumber,
    FovScale,
    PlantedC4,
    SoundChannels,
    FileSystem,
    PanoramaLabelFactory,
    OffsetToPlayerPawnImmunity,
    ConVarAccessor
>;
