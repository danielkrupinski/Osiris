#pragma once

#include "OtherInterfacesPODs.h"
#include <RetSpoof/RetSpoofInvoker.h>
#include <CSGO/Cvar.h>
#include <CSGO/FileSystem.h>
#include <CSGO/InputSystem.h>
#include <CSGO/Localize.h>
#include <CSGO/MaterialSystem.h>
#include <CSGO/Panorama.h>
#include <CSGO/PhysicsSurfaceProps.h>
#include <CSGO/SoundEmitter.h>
#include <CSGO/StudioRender.h>
#include <CSGO/Surface.h>

class OtherInterfaces {
public:
    OtherInterfaces(RetSpoofInvoker retSpoofInvoker, const OtherInterfacesPODs& pods)
        : retSpoofInvoker{ retSpoofInvoker }, pods{ pods }
    {
    }

    [[nodiscard]] auto getLocalize() const noexcept
    {
        return csgo::Localize::from(retSpoofInvoker, pods.localize);
    }

    [[nodiscard]] auto getBaseFileSystem() const noexcept
    {
        return csgo::BaseFileSystem::from(retSpoofInvoker, pods.baseFileSystem);
    }

    [[nodiscard]] auto getMaterialSystem() const noexcept
    {
        return csgo::MaterialSystem::from(retSpoofInvoker, pods.materialSystem);
    }

    [[nodiscard]] auto getCvar() const noexcept
    {
        return csgo::Cvar::from(retSpoofInvoker, pods.cvar);
    }

    [[nodiscard]] auto getInputSystem() const noexcept
    {
        return csgo::InputSystem::from(retSpoofInvoker, pods.inputSystem);
    }

    [[nodiscard]] auto getSoundEmitter() const noexcept
    {
        return csgo::SoundEmitter::from(retSpoofInvoker, pods.soundEmitter);
    }

    [[nodiscard]] auto getStudioRender() const noexcept
    {
        return csgo::StudioRender::from(retSpoofInvoker, pods.studioRender);
    }

    [[nodiscard]] auto getPanoramaUIEngine() const noexcept
    {
        return csgo::PanoramaUIEngine::from(retSpoofInvoker, pods.panoramaUIEngine);
    }

    [[nodiscard]] auto getPhysicsSurfaceProps() const noexcept
    {
        return csgo::PhysicsSurfaceProps::from(retSpoofInvoker, pods.physicsSurfaceProps);
    }

    [[nodiscard]] auto getSurface() const noexcept
    {
        return csgo::Surface::from(retSpoofInvoker, pods.surface);
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    OtherInterfacesPODs pods;
};
