#pragma once

#include <MemoryPatterns/EntityPatterns.h>
#include <MemoryPatterns/GameSceneNodePatterns.h>
#include <MemoryPatterns/PlayerControllerPatterns.h>
#include <Utils/TypeBitFlags.h>

struct EntitiesVMTs;
class EntityFromHandleFinder;
class EntityListWalker;
class HudInWorldPanelContainer;
struct HudProvider;
struct LocalPlayerController;
struct PanelConfigurator;
struct PanoramaTransformFactory;
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
    OffsetToTeamNumber
>;
