#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include <BuildConfig.h>
#include <CS2/Classes/CViewRender.h>
#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <CS2/Constants/DllNames.h>
#include <GameClient/Entities/BaseEntity.h>
#include <GameClient/Entities/BaseWeapon.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/Entities/PreviewPlayer.h>
#include <Features/Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVis.h>
#include <Features/Hud/DefusingAlert/DefusingAlert.h>
#include <Features/Hud/KillfeedPreserver/KillfeedPreserver.h>
#include <Features/Sound/SoundFeatures.h>
#include <Features/Visuals/ModelGlow/Preview/PlayerModelGlowPreview.h>
#include <Features/Visuals/ViewmodelMod/ViewmodelMod.h>
#include <MemorySearch/PatternNotFoundLogger.h>
#include <MemoryAllocation/FreeMemoryRegionList.h>
#include <MemorySearch/PatternFinder.h>
#include <Platform/DynamicLibrary.h>
#include <Platform/Macros/FunctionAttributes.h>
#include <SDL/SdlDll.h>

#include "DeferredCompleteObject.h"
#include "FullGlobalContext.h"
#include "PartialGlobalContext.h"
#include "HookContext/HookContext.h"
#include <Hooks/ClientModeHooks.h>

class GlobalContext {
public:
    explicit GlobalContext(std::span<std::byte> memoryStorage, DynamicLibrary clientDLL, DynamicLibrary panoramaDLL, SdlDll sdlDll) noexcept
        : _freeRegionList{memoryStorage}
        , deferredCompleteContext{clientDLL, panoramaDLL, sdlDll}
    {
    }

    using Complete = FullGlobalContext;

    static void initializeInstance() noexcept
    {
        if (!globalContext.isInitialized()) {
            alignas(FreeMemoryRegionList::minimumAlignment()) static constinit std::byte storage[build::MEMORY_CAPACITY];
            globalContext.initialize(storage, DynamicLibrary{cs2::CLIENT_DLL}, DynamicLibrary{cs2::PANORAMA_DLL}, SdlDll{});
            globalContext->deferredCompleteContext.partial().enableIfValid();
        }
    }

    [[nodiscard]] static GlobalContext& instance() noexcept
    {
        return *globalContext;
    }

    static void destroyInstance() noexcept
    {
        globalContext.destroy();
    }

    [[nodiscard]] FreeMemoryRegionList& freeRegionList() noexcept
    {
        return _freeRegionList;
    }

    [[nodiscard]] FullGlobalContext& fullContext() noexcept
    {
        return deferredCompleteContext.complete();
    }

    [[nodiscard]] bool isComplete() noexcept
    {
        return deferredCompleteContext.isComplete();
    }

    [[NOINLINE]] void initCompleteContextFromGameThread() noexcept
    {
        const auto partialContext = deferredCompleteContext.partial();
        deferredCompleteContext.makeComplete(
            partialContext.peepEventsHook,
            partialContext.clientDLL,
            partialContext.panoramaDLL,
            MemoryPatterns{partialContext.patternFinders},
            Tier0Dll{}
        );
    }

private:
    FreeMemoryRegionList _freeRegionList;
    DeferredCompleteObject<PartialGlobalContext, FullGlobalContext> deferredCompleteContext;

    static constinit ManuallyDestructible<GlobalContext> globalContext;
};
