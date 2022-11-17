#pragma once

#include <optional>
#include <string>
#include <type_traits>

#include "Platform/IsPlatform.h"

#if IS_WIN32()
#include <Windows.h>

#include "Platform/Windows/DynamicLibraryWrapper.h"
#include "Platform/Windows/DynamicLibrary.h"
#else
#include <dlfcn.h>

#include "Platform/Linux/DynamicLibraryWrapper.h"
#include "Platform/Linux/SharedObject.h"
#endif

#include "SDK/Client.h"
#include "SDK/Cvar.h"
#include "SDK/Engine.h"
#include "SDK/EngineTrace.h"
#include "SDK/EntityList.h"
#include "SDK/FileSystem.h"
#include "SDK/GameEvent.h"
#include "SDK/GameMovement.h"
#include "SDK/InputSystem.h"
#include "SDK/Localize.h"
#include "SDK/MaterialSystem.h"
#include "SDK/ModelInfo.h"
#include "SDK/Panorama.h"
#include "SDK/PhysicsSurfaceProps.h"
#include "SDK/Platform.h"
#include "SDK/Prediction.h"
#include "SDK/SoundEmitter.h"
#include "SDK/StudioRender.h"
#include "SDK/Surface.h"

#include "Platform/DynamicLibraryView.h"
#include "Platform/RetSpoofInvoker.h"

#include "RetSpoofGadgets.h"

#include "Interfaces/InterfaceFinder.h"
#include "Interfaces/InterfaceFinderWithLog.h"

class GameUI;
class NetworkStringTableContainer;

struct OtherInterfacesPODs {
    OtherInterfacesPODs()
        : baseFileSystem{ static_cast<csgo::pod::BaseFileSystem*>(find(FILESYSTEM_DLL, "VBaseFileSystem011")) },
          cvar{ static_cast<csgo::pod::Cvar*>(find(VSTDLIB_DLL, "VEngineCvar007")) },
          inputSystem{ static_cast<csgo::pod::InputSystem*>(find(INPUTSYSTEM_DLL, "InputSystemVersion001")) },
          localize{ static_cast<csgo::pod::Localize*>(find(LOCALIZE_DLL, "Localize_001")) },
          materialSystem{ static_cast<csgo::pod::MaterialSystem*>(find(MATERIALSYSTEM_DLL, "VMaterialSystem080")) },
          panoramaUIEngine{ static_cast<csgo::pod::PanoramaUIEngine*>(find(PANORAMA_DLL, "PanoramaUIEngine001")) },
          physicsSurfaceProps{ static_cast<csgo::pod::PhysicsSurfaceProps*>(find(VPHYSICS_DLL, "VPhysicsSurfaceProps001")) },
          soundEmitter{ static_cast<csgo::pod::SoundEmitter*>(find(SOUNDEMITTERSYSTEM_DLL, "VSoundEmitter003")) },
          studioRender{ static_cast<csgo::pod::StudioRender*>(find(STUDIORENDER_DLL, "VStudioRender026")) },
          surface{ static_cast<csgo::pod::Surface*>(find(VGUIMATSURFACE_DLL, "VGUI_Surface031")) }
    {
    }

    csgo::pod::BaseFileSystem* baseFileSystem;
    csgo::pod::Cvar* cvar;
    csgo::pod::InputSystem* inputSystem;
    csgo::pod::Localize* localize;
    csgo::pod::MaterialSystem* materialSystem;
    csgo::pod::PanoramaUIEngine* panoramaUIEngine;
    csgo::pod::PhysicsSurfaceProps* physicsSurfaceProps;
    csgo::pod::SoundEmitter* soundEmitter;
    csgo::pod::StudioRender* studioRender;
    csgo::pod::Surface* surface;

private:
    static void* find(const char* moduleName, const char* name) noexcept
    {
#if IS_WIN32()
        const windows_platform::DynamicLibrary dll{ windows_platform::DynamicLibraryWrapper{}, moduleName };
        const InterfaceFinderWithLog finder{ InterfaceFinder { dll.getView(), retSpoofGadgets->client } };
#else
        const linux_platform::SharedObject so{ linux_platform::DynamicLibraryWrapper{}, moduleName };
        const InterfaceFinderWithLog finder{ InterfaceFinder{ so.getView(), retSpoofGadgets->client } };
#endif
        return finder(name);
    }
};

class Interfaces {
public:
    Interfaces(RetSpoofInvoker retSpoofInvoker, const OtherInterfacesPODs& pods)
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
