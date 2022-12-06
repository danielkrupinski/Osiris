#pragma once

#include <Platform/IsPlatform.h>

#if IS_WIN32()
#include "Platform/Windows/DynamicLibraryWrapper.h"
#include "Platform/Windows/DynamicLibrary.h"
#elif IS_LINUX()
#include "Platform/Linux/DynamicLibraryWrapper.h"
#include "Platform/Linux/SharedObject.h"
#endif

#include <RetSpoofGadgets.h>
#include <SDK/Constants/DllNames.h>

#include "Interfaces/InterfaceFinder.h"
#include "Interfaces/InterfaceFinderWithLog.h"

namespace csgo::pod { struct BaseFileSystem; }
namespace csgo::pod { struct Cvar; }
namespace csgo::pod { struct InputSystem; }
namespace csgo::pod { struct Localize; }
namespace csgo::pod { struct MaterialSystem; }
namespace csgo::pod { struct PanoramaUIEngine; }
namespace csgo::pod { struct PhysicsSurfaceProps; }
namespace csgo::pod { struct SoundEmitter; }
namespace csgo::pod { struct StudioRender; }
namespace csgo::pod { struct Surface; }

struct OtherInterfacesPODs {
    OtherInterfacesPODs()
        : baseFileSystem{ static_cast<csgo::pod::BaseFileSystem*>(find(csgo::FILESYSTEM_DLL, "VBaseFileSystem011")) },
          cvar{ static_cast<csgo::pod::Cvar*>(find(csgo::VSTDLIB_DLL, "VEngineCvar007")) },
          inputSystem{ static_cast<csgo::pod::InputSystem*>(find(csgo::INPUTSYSTEM_DLL, "InputSystemVersion001")) },
          localize{ static_cast<csgo::pod::Localize*>(find(csgo::LOCALIZE_DLL, "Localize_001")) },
          materialSystem{ static_cast<csgo::pod::MaterialSystem*>(find(csgo::MATERIALSYSTEM_DLL, "VMaterialSystem080")) },
          panoramaUIEngine{ static_cast<csgo::pod::PanoramaUIEngine*>(find(csgo::PANORAMA_DLL, "PanoramaUIEngine001")) },
          physicsSurfaceProps{ static_cast<csgo::pod::PhysicsSurfaceProps*>(find(csgo::VPHYSICS_DLL, "VPhysicsSurfaceProps001")) },
          soundEmitter{ static_cast<csgo::pod::SoundEmitter*>(find(csgo::SOUNDEMITTERSYSTEM_DLL, "VSoundEmitter003")) },
          studioRender{ static_cast<csgo::pod::StudioRender*>(find(csgo::STUDIORENDER_DLL, "VStudioRender026")) },
          surface{ static_cast<csgo::pod::Surface*>(find(csgo::VGUIMATSURFACE_DLL, "VGUI_Surface031")) }
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
