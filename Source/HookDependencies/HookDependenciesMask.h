#pragma once

#include <FeatureHelpers/ConVarAccessor.h>
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
struct SoundChannels;
struct WorldToClipSpaceConverter;

using HookDependenciesMask = TypeBitFlags<
    EntityListWalker,
    EntityFromHandleFinder,
    WorldToClipSpaceConverter,
    PanoramaTransformFactory,
    SoundChannels,
    FileSystem,
    PanoramaLabelFactory
>;
