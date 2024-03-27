#pragma once

#include <MemoryPatterns/EntityPatterns.h>
#include <MemoryPatterns/GameSceneNodePatterns.h>
#include <MemoryPatterns/PlayerControllerPatterns.h>
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
    FileSystem
>;
