#pragma once

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32()
#include "Platform/Windows/DynamicLibraryWrapper.h"
#include "Platform/Windows/DynamicLibrary.h"
#elif IS_LINUX()
#include "Platform/Linux/DynamicLibraryWrapper.h"
#include "Platform/Linux/SharedObject.h"
#endif

#include <RetSpoofGadgets.h>
#include <CSGO/Constants/DllNames.h>

#include "Interfaces/InterfaceFinder.h"
#include "Interfaces/InterfaceFinderWithLog.h"

namespace csgo { struct BaseFileSystemPOD; }
namespace csgo { struct CvarPOD; }
namespace csgo { struct InputSystemPOD; }
namespace csgo { struct LocalizePOD; }
namespace csgo { struct MaterialSystemPOD; }
namespace csgo { struct PanoramaUIEnginePOD; }
namespace csgo { struct PhysicsSurfacePropsPOD; }
namespace csgo { struct SoundEmitterPOD; }
namespace csgo { struct StudioRenderPOD; }
namespace csgo { struct SurfacePOD; }

struct OtherInterfacesPODs {
    OtherInterfacesPODs()
        : baseFileSystem{ static_cast<csgo::BaseFileSystemPOD*>(find(csgo::FILESYSTEM_DLL, "VBaseFileSystem011")) },
          cvar{ static_cast<csgo::CvarPOD*>(find(csgo::VSTDLIB_DLL, "VEngineCvar007")) },
          inputSystem{ static_cast<csgo::InputSystemPOD*>(find(csgo::INPUTSYSTEM_DLL, "InputSystemVersion001")) },
          localize{ static_cast<csgo::LocalizePOD*>(find(csgo::LOCALIZE_DLL, "Localize_001")) },
          materialSystem{ static_cast<csgo::MaterialSystemPOD*>(find(csgo::MATERIALSYSTEM_DLL, "VMaterialSystem080")) },
          panoramaUIEngine{ static_cast<csgo::PanoramaUIEnginePOD*>(find(csgo::PANORAMA_DLL, "PanoramaUIEngine001")) },
          physicsSurfaceProps{ static_cast<csgo::PhysicsSurfacePropsPOD*>(find(csgo::VPHYSICS_DLL, "VPhysicsSurfaceProps001")) },
          soundEmitter{ static_cast<csgo::SoundEmitterPOD*>(find(csgo::SOUNDEMITTERSYSTEM_DLL, "VSoundEmitter003")) },
          studioRender{ static_cast<csgo::StudioRenderPOD*>(find(csgo::STUDIORENDER_DLL, "VStudioRender026")) },
          surface{ static_cast<csgo::SurfacePOD*>(find(csgo::VGUIMATSURFACE_DLL, "VGUI_Surface031")) }
    {
    }

    csgo::BaseFileSystemPOD* baseFileSystem;
    csgo::CvarPOD* cvar;
    csgo::InputSystemPOD* inputSystem;
    csgo::LocalizePOD* localize;
    csgo::MaterialSystemPOD* materialSystem;
    csgo::PanoramaUIEnginePOD* panoramaUIEngine;
    csgo::PhysicsSurfacePropsPOD* physicsSurfaceProps;
    csgo::SoundEmitterPOD* soundEmitter;
    csgo::StudioRenderPOD* studioRender;
    csgo::SurfacePOD* surface;

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
