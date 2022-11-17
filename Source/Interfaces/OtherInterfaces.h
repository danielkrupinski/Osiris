#pragma once

#include "OtherInterfacesPODs.h"
#include <Platform/RetSpoofInvoker.h>
#include <SDK/Cvar.h>
#include <SDK/FileSystem.h>
#include <SDK/InputSystem.h>
#include <SDK/Localize.h>
#include <SDK/MaterialSystem.h>
#include <SDK/Panorama.h>
#include <SDK/PhysicsSurfaceProps.h>
#include <SDK/SoundEmitter.h>
#include <SDK/StudioRender.h>
#include <SDK/Surface.h>

class OtherInterfaces {
public:
    OtherInterfaces(RetSpoofInvoker retSpoofInvoker, const OtherInterfacesPODs& pods)
        : retSpoofInvoker{ retSpoofInvoker }, pods{ pods }
    {
    }

    [[nodiscard]] auto getLocalize() const noexcept
    {
        return Localize::from(retSpoofInvoker, pods.localize);
    }

    [[nodiscard]] auto getBaseFileSystem() const noexcept
    {
        return BaseFileSystem::from(retSpoofInvoker, pods.baseFileSystem);
    }

    [[nodiscard]] auto getMaterialSystem() const noexcept
    {
        return MaterialSystem::from(retSpoofInvoker, pods.materialSystem);
    }

    [[nodiscard]] auto getCvar() const noexcept
    {
        return Cvar::from(retSpoofInvoker, pods.cvar);
    }

    [[nodiscard]] auto getInputSystem() const noexcept
    {
        return InputSystem::from(retSpoofInvoker, pods.inputSystem);
    }

    [[nodiscard]] auto getSoundEmitter() const noexcept
    {
        return SoundEmitter::from(retSpoofInvoker, pods.soundEmitter);
    }

    [[nodiscard]] auto getStudioRender() const noexcept
    {
        return StudioRender::from(retSpoofInvoker, pods.studioRender);
    }

    [[nodiscard]] auto getPanoramaUIEngine() const noexcept
    {
        return PanoramaUIEngine::from(retSpoofInvoker, pods.panoramaUIEngine);
    }

    [[nodiscard]] auto getPhysicsSurfaceProps() const noexcept
    {
        return PhysicsSurfaceProps::from(retSpoofInvoker, pods.physicsSurfaceProps);
    }

    [[nodiscard]] auto getSurface() const noexcept
    {
        return Surface::from(retSpoofInvoker, pods.surface);
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    const OtherInterfacesPODs& pods;
};
