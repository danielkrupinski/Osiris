#pragma once

#include <FeatureHelpers/ConVarAccessor.h>
#include <GameClasses/OffsetTypes/EntityOffset.h>
#include <GameClasses/OffsetTypes/GameSceneNodeOffset.h>
#include <GameClasses/OffsetTypes/PlayerControllerOffset.h>
#include <GameClasses/OffsetTypes/PlayerPawnOffset.h>
#include <Utils/TypeBitFlags.h>

struct CurTime;
struct EntitiesVMTs;
class EntityFromHandleFinder;
class EntityListWalker;
struct FileSystem;
struct FovScale;
class HudInWorldPanelContainer;
struct HudProvider;
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
    HudProvider,
    PanoramaTransformFactory,
    OffsetToHealth,
    OffsetToTeamNumber,
    FovScale,
    CurTime,
    PlantedC4,
    SoundChannels,
    FileSystem,
    PanoramaLabelFactory,
    OffsetToPlayerPawnImmunity,
    ConVarAccessor
>;
