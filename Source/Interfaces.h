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

class Interfaces {
public:
    Interfaces(RetSpoofInvoker retSpoofInvoker)
        : baseFileSystem{ std::uintptr_t(find(FILESYSTEM_DLL, "VBaseFileSystem011")) },
          cvar{ std::uintptr_t(find(VSTDLIB_DLL, "VEngineCvar007")) },
          inputSystem{ std::uintptr_t(find(INPUTSYSTEM_DLL, "InputSystemVersion001")) },
          materialSystem{ std::uintptr_t(find(MATERIALSYSTEM_DLL, "VMaterialSystem080")) },
          panoramaUIEngine{ std::uintptr_t(find(PANORAMA_DLL, "PanoramaUIEngine001")) },
          physicsSurfaceProps{ std::uintptr_t(find(VPHYSICS_DLL, "VPhysicsSurfaceProps001")) },
          surface{ std::uintptr_t(find(VGUIMATSURFACE_DLL, "VGUI_Surface031")) },
          soundEmitter{ std::uintptr_t(find(SOUNDEMITTERSYSTEM_DLL, "VSoundEmitter003")) },
          studioRender{ std::uintptr_t(find(STUDIORENDER_DLL, "VStudioRender026")) },
          retSpoofInvoker{ retSpoofInvoker },
          localize{ std::uintptr_t(find(LOCALIZE_DLL, "Localize_001")) }
    {
    }

    [[nodiscard]] auto getLocalize() const noexcept
    {
        return Localize::from(retSpoofInvoker, (csgo::pod::Localize*)localize);
    }

    [[nodiscard]] auto getBaseFileSystem() const noexcept
    {
        return BaseFileSystem::from(retSpoofInvoker, (csgo::pod::BaseFileSystem*)baseFileSystem);
    }

    [[nodiscard]] auto getMaterialSystem() const noexcept
    {
        return MaterialSystem::from(retSpoofInvoker, (csgo::pod::MaterialSystem*)materialSystem);
    }

    [[nodiscard]] auto getCvar() const noexcept
    {
        return Cvar::from(retSpoofInvoker, (csgo::pod::Cvar*)cvar);
    }

    [[nodiscard]] auto getInputSystem() const noexcept
    {
        return InputSystem::from(retSpoofInvoker, (csgo::pod::InputSystem*)inputSystem);
    }

    [[nodiscard]] auto getSoundEmitter() const noexcept
    {
        return SoundEmitter::from(retSpoofInvoker, (csgo::pod::SoundEmitter*)soundEmitter);
    }

    [[nodiscard]] auto getStudioRender() const noexcept
    {
        return StudioRender::from(retSpoofInvoker, (csgo::pod::StudioRender*)studioRender);
    }

    [[nodiscard]] auto getPanoramaUIEngine() const noexcept
    {
        return PanoramaUIEngine::from(retSpoofInvoker, (csgo::pod::PanoramaUIEngine*)panoramaUIEngine);
    }

    [[nodiscard]] auto getPhysicsSurfaceProps() const noexcept
    {
        return PhysicsSurfaceProps::from(retSpoofInvoker, (csgo::pod::PhysicsSurfaceProps*)physicsSurfaceProps);
    }

    [[nodiscard]] auto getSurface() const noexcept
    {
        return Surface::from(retSpoofInvoker, (csgo::pod::Surface*)surface);
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    std::uintptr_t localize;
    std::uintptr_t baseFileSystem;
    std::uintptr_t materialSystem;
    std::uintptr_t cvar;
    std::uintptr_t inputSystem;
    std::uintptr_t soundEmitter;
    std::uintptr_t studioRender;
    std::uintptr_t panoramaUIEngine;
    std::uintptr_t physicsSurfaceProps;
    std::uintptr_t surface;

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

inline std::optional<const Interfaces> interfaces;
