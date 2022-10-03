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

#include "SDK/Platform.h"

#include "Platform/DynamicLibraryView.h"
#include "Platform/RetSpoofInvoker.h"

#include "RetSpoofGadgets.h"

class BaseFileSystem;
class Client;
class Cvar;
class Engine;
class EngineSound;
class EngineTrace;
class EntityList;
class GameEventManager;
class GameMovement;
class GameUI;
class InputSystem;
class Localize;
class MaterialSystem;
class ModelInfo;
class ModelRender;
class NetworkStringTableContainer;
class PanoramaUIEngine;
class PhysicsSurfaceProps;
class Prediction;
class RenderView;
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
    explicit ClientInterfaces(InterfaceFinder<DynamicLibraryWrapper> clientInterfaceFinder)
        : client{ static_cast<Client*>(clientInterfaceFinder("VClient018")) },
          entityList{ static_cast<EntityList*>(clientInterfaceFinder("VClientEntityList003")) },
          gameMovement{ static_cast<GameMovement*>(clientInterfaceFinder("GameMovement001")) },
          prediction{ static_cast<Prediction*>(clientInterfaceFinder("VClientPrediction001")) }
    {
    }

    Client* client;
    EntityList* entityList;
    GameMovement* gameMovement;
    Prediction* prediction;
};

class EngineInterfaces {
public:
    template <typename DynamicLibraryWrapper>
    explicit EngineInterfaces(InterfaceFinder<DynamicLibraryWrapper> engineInterfaceFinder)
        : engine{ static_cast<Engine*>(engineInterfaceFinder("VEngineClient014")) },
          engineTrace{ static_cast<EngineTrace*>(engineInterfaceFinder("EngineTraceClient004")) },
          gameEventManager{ static_cast<GameEventManager*>(engineInterfaceFinder("GAMEEVENTSMANAGER002")) },
          modelInfo{ static_cast<ModelInfo*>(engineInterfaceFinder("VModelInfoClient004")) },
          modelRender{ static_cast<ModelRender*>(engineInterfaceFinder("VEngineModel016")) },
          networkStringTableContainer{ static_cast<NetworkStringTableContainer*>(engineInterfaceFinder("VEngineClientStringTable001")) },
          renderView{ static_cast<RenderView*>(engineInterfaceFinder("VEngineRenderView014")) },
          sound{ static_cast<EngineSound*>(engineInterfaceFinder("IEngineSoundClient003")) }
    {
    }

    Engine* engine;
    EngineTrace* engineTrace;
    GameEventManager* gameEventManager;
    ModelInfo* modelInfo;
    ModelRender* modelRender;
    NetworkStringTableContainer* networkStringTableContainer;
    RenderView* renderView;
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
