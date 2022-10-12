#pragma once

#include <optional>
#include <string>
#include <type_traits>

#ifdef _WIN32
#include <Windows.h>

#include "Platform/Windows/DynamicLibraryWrapper.h"
#else
#include <dlfcn.h>

#include "Platform/Linux/DynamicLibraryWrapper.h"
#include "Platform/Linux/SharedObject.h"
#endif

#include "SDK/Client.h"
#include "SDK/Engine.h"
#include "SDK/EngineTrace.h"
#include "SDK/EntityList.h"
#include "SDK/GameEvent.h"
#include "SDK/GameMovement.h"
#include "SDK/ModelInfo.h"
#include "SDK/Platform.h"
#include "SDK/Prediction.h"

#include "Platform/DynamicLibraryView.h"
#include "Platform/RetSpoofInvoker.h"

#include "RetSpoofGadgets.h"

class BaseFileSystem;
class Cvar;
class EngineSound;
class GameUI;
class InputSystem;
class Localize;
class MaterialSystem;
class ModelRender;
class NetworkStringTableContainer;
class PanoramaUIEngine;
class PhysicsSurfaceProps;
class Surface;
class SoundEmitter;
class StudioRender;

template <typename DynamicLibraryWrapper>
struct InterfaceFinder {
    explicit InterfaceFinder(DynamicLibraryView<DynamicLibraryWrapper> library, RetSpoofInvoker invoker)
        : library{ library }, invoker{ invoker }
    {
    }

    void* operator()(const char* name) const noexcept
    {
        if (const auto createInterface = library.getFunctionAddress("CreateInterface")) {
            if (void* foundInterface = invoker.invokeCdecl<void*>(std::uintptr_t(createInterface), name, nullptr))
                return foundInterface;
        }

#ifdef _WIN32
        MessageBoxA(nullptr, ("Failed to find " + std::string{ name } + " interface!").c_str(), "Osiris", MB_OK | MB_ICONERROR);
#endif
        std::exit(EXIT_FAILURE);
    }

private:
    DynamicLibraryView<DynamicLibraryWrapper> library;
    RetSpoofInvoker invoker;
};

class ClientInterfaces {
public:
    template <typename DynamicLibraryWrapper>
    explicit ClientInterfaces(InterfaceFinder<DynamicLibraryWrapper> clientInterfaceFinder, RetSpoofInvoker retSpoofInvoker)
        : retSpoofInvoker{ retSpoofInvoker },
          client{ std::uintptr_t(clientInterfaceFinder("VClient018")) },
          entityList{ std::uintptr_t(clientInterfaceFinder("VClientEntityList003")) },
          gameMovement{ std::uintptr_t(clientInterfaceFinder("GameMovement001")) },
          prediction{ std::uintptr_t(clientInterfaceFinder("VClientPrediction001")) }
    {
    }

    [[nodiscard]] auto getClient() const noexcept
    {
        return Client{ retSpoofInvoker, client };
    }

    [[nodiscard]] std::uintptr_t getClientAddress() const noexcept
    {
        return client;
    }

    [[nodiscard]] auto getEntityList() const noexcept
    {
        return EntityList{ retSpoofInvoker, entityList };
    }

    [[nodiscard]] auto getGameMovement() const noexcept
    {
        return GameMovement{ retSpoofInvoker, gameMovement };
    }

    [[nodiscard]] auto getPrediction() const noexcept
    {
        return Prediction{ retSpoofInvoker, prediction };
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    std::uintptr_t client;
    std::uintptr_t entityList;
    std::uintptr_t gameMovement;
    std::uintptr_t prediction;
};

class EngineInterfaces {
public:
    template <typename DynamicLibraryWrapper>
    explicit EngineInterfaces(InterfaceFinder<DynamicLibraryWrapper> engineInterfaceFinder, RetSpoofInvoker retSpoofInvoker)
        : retSpoofInvoker{ retSpoofInvoker },
          engine{ std::uintptr_t(engineInterfaceFinder("VEngineClient014")) },
          engineTrace{ retSpoofInvoker, std::uintptr_t(engineInterfaceFinder("EngineTraceClient004")) },
          gameEventManager{ std::uintptr_t(engineInterfaceFinder("GAMEEVENTSMANAGER002")) },
          modelInfo{ std::uintptr_t(engineInterfaceFinder("VModelInfoClient004")) },
          modelRender{ static_cast<ModelRender*>(engineInterfaceFinder("VEngineModel016")) },
          sound{ static_cast<EngineSound*>(engineInterfaceFinder("IEngineSoundClient003")) }
    {
    }

    [[nodiscard]] auto getEngine() const noexcept
    {
        return Engine{ retSpoofInvoker, engine };
    }

    [[nodiscard]] std::uintptr_t getEngineAddress() const noexcept
    {
        return engine;
    }

    [[nodiscard]] auto getGameEventManager() const noexcept
    {
        return GameEventManager{ retSpoofInvoker, gameEventManager };
    }

    [[nodiscard]] std::uintptr_t getGameEventManagerAddress() const noexcept
    {
        return gameEventManager;
    }

    [[nodiscard]] auto getModelInfo() const noexcept
    {
        return ModelInfo{ retSpoofInvoker, modelInfo };
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    std::uintptr_t engine;
    std::uintptr_t gameEventManager;
    std::uintptr_t modelInfo;
public:
    EngineTrace engineTrace;
    ModelRender* modelRender;
    EngineSound* sound;
};

class Interfaces {
public:
#define GAME_INTERFACE(type, name, moduleName, version) \
type* name = reinterpret_cast<type*>(find(moduleName, version));

    GAME_INTERFACE(BaseFileSystem, baseFileSystem, FILESYSTEM_DLL, "VBaseFileSystem011")
    GAME_INTERFACE(Cvar, cvar, VSTDLIB_DLL, "VEngineCvar007")
    GAME_INTERFACE(InputSystem, inputSystem, INPUTSYSTEM_DLL, "InputSystemVersion001")
    GAME_INTERFACE(Localize, localize, LOCALIZE_DLL, "Localize_001")
    GAME_INTERFACE(MaterialSystem, materialSystem, MATERIALSYSTEM_DLL, "VMaterialSystem080")
    GAME_INTERFACE(PanoramaUIEngine, panoramaUIEngine, PANORAMA_DLL, "PanoramaUIEngine001")
    GAME_INTERFACE(PhysicsSurfaceProps, physicsSurfaceProps, VPHYSICS_DLL, "VPhysicsSurfaceProps001")
    GAME_INTERFACE(Surface, surface, VGUIMATSURFACE_DLL, "VGUI_Surface031")
    GAME_INTERFACE(SoundEmitter, soundEmitter, SOUNDEMITTERSYSTEM_DLL, "VSoundEmitter003")
    GAME_INTERFACE(StudioRender, studioRender, STUDIORENDER_DLL, "VStudioRender026")

#undef GAME_INTERFACE
private:
    static void* find(const char* moduleName, const char* name) noexcept
    {
#ifdef _WIN32
        const InterfaceFinder finder{ DynamicLibraryView<windows_platform::DynamicLibraryWrapper>{ windows_platform::DynamicLibraryWrapper{}, moduleName }, retSpoofGadgets.jmpEbxInClient };
#else
        const linux_platform::SharedObject so{ linux_platform::DynamicLibraryWrapper{}, moduleName };
        const InterfaceFinder finder{ so.getView(), retSpoofGadgets.jmpEbxInClient };
#endif
        return finder(name);
    }
};

inline std::optional<const Interfaces> interfaces;
