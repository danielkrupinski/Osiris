#include "GlobalContext.h"

#if IS_WIN32()
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>
#else
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#endif

#include "EventListener.h"
#include "GameData.h"
#include "GUI.h"
#include "Hooks.h"
#include "InventoryChanger/InventoryChanger.h"
#include "Memory.h"
#include "Hacks/Aimbot.h"
#include "Hacks/Backtrack.h"
#include "Hacks/Chams.h"
#include "Hacks/EnginePrediction.h"
#include "Hacks/Glow.h"
#include "Hacks/Misc.h"
#include "Hacks/Sound.h"
#include "Hacks/StreamProofESP.h"
#include "Hacks/Triggerbot.h"
#include "Hacks/Visuals.h"
#include "CSGO/ClientClass.h"
#include "CSGO/Constants/ClassId.h"
#include "CSGO/Constants/FrameStage.h"
#include "CSGO/Constants/GameEventNames.h"
#include "CSGO/Constants/UserMessages.h"
#include "CSGO/Engine.h"
#include "CSGO/Entity.h"
#include "CSGO/EntityList.h"
#include "CSGO/GlobalVars.h"
#include "CSGO/InputSystem.h"
#include "CSGO/LocalPlayer.h"
#include "CSGO/ModelRender.h"
#include "CSGO/Recv.h"
#include "CSGO/SoundEmitter.h"
#include "CSGO/SoundInfo.h"
#include "CSGO/StudioRender.h"
#include "CSGO/Surface.h"
#include "CSGO/UserCmd.h"
#include "CSGO/ViewSetup.h"
#include "CSGO/PODs/RenderableInfo.h"

#include "Interfaces/ClientInterfaces.h"

#include "Platform/DynamicLibrary.h"
#include "Platform/PlatformApi.h"

template <typename PlatformApi>
GlobalContext<PlatformApi>::GlobalContext(PlatformApi platformApi)
    : platformApi{ platformApi }
{
    const DynamicLibrary<PlatformApi> clientDLL{ platformApi, csgo::CLIENT_DLL };
    const DynamicLibrary<PlatformApi> engineDLL{ platformApi, csgo::ENGINE_DLL };

    PatternNotFoundHandler patternNotFoundHandler;
    retSpoofGadgets.emplace(PatternFinder{ clientDLL.getCodeSection(), patternNotFoundHandler }, PatternFinder{ clientDLL.getCodeSection(), patternNotFoundHandler });
}

#if IS_LINUX()
template <typename PlatformApi>
int GlobalContext<PlatformApi>::pollEventHook(SDL_Event* event)
{
    const auto result = hooks->pollEvent(event);

    if (state == GlobalContextState::Initialized) {
        if (result && ImGui_ImplSDL2_ProcessEvent(event) && gui->isOpen())
            event->type = 0;
    } else if (state == GlobalContextState::NotInitialized) {
        state = GlobalContextState::Initializing;
        ImGui::CreateContext();
        initialize();
        state = GlobalContextState::Initialized;
    }
    
    return result;
}

template <typename PlatformApi>
void GlobalContext<PlatformApi>::swapWindowHook(SDL_Window* window)
{
    [[maybe_unused]] static const auto _ = ImGui_ImplSDL2_InitForOpenGL(window, nullptr);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);

    renderFrame();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    GameData::clearUnusedAvatars();
    features->inventoryChanger.clearUnusedItemIconTextures();

    hooks->swapWindow(window);
}

#endif

template <typename PlatformApi>
void GlobalContext<PlatformApi>::renderFrame()
{
    ImGui::NewFrame();

    if (const auto& displaySize = ImGui::GetIO().DisplaySize; displaySize.x > 0.0f && displaySize.y > 0.0f) {
        StreamProofESP::render(*memory, *config);
        features->misc.purchaseList();
        features->misc.noscopeCrosshair(ImGui::GetBackgroundDrawList());
        features->misc.recoilCrosshair(ImGui::GetBackgroundDrawList());
        features->misc.drawOffscreenEnemies(ImGui::GetBackgroundDrawList());
        features->misc.drawBombTimer();
        features->misc.spectatorList();
        features->visuals.hitMarker(nullptr, ImGui::GetBackgroundDrawList());
        features->visuals.drawMolotovHull(ImGui::GetBackgroundDrawList());
        features->misc.watermark();

        features->aimbot.updateInput(*config);
        features->visuals.updateInput();
        StreamProofESP::updateInput(*config);
        features->misc.updateInput();
        Triggerbot::updateInput(*config);
        features->chams.updateInput();
        features->glow.updateInput();

        gui->handleToggle(features->misc, getOtherInterfaces());

        if (gui->isOpen())
            gui->render(getEngineInterfaces(), ClientInterfaces{ retSpoofGadgets->client, *clientInterfaces }, getOtherInterfaces(), *memory, *config);
    }

    ImGui::EndFrame();
    ImGui::Render();
}

template <typename PlatformApi>
void GlobalContext<PlatformApi>::initialize()
{
    const DynamicLibrary<PlatformApi> clientSo{ platformApi, csgo::CLIENT_DLL };
    clientInterfaces = createClientInterfacesPODs(InterfaceFinderWithLog{ InterfaceFinder{ clientSo, retSpoofGadgets->client } });
    const DynamicLibrary<PlatformApi> engineSo{ platformApi, csgo::ENGINE_DLL };
    engineInterfacesPODs = createEngineInterfacesPODs(InterfaceFinderWithLog{ InterfaceFinder{ engineSo, retSpoofGadgets->client } });

    interfaces.emplace();
    PatternNotFoundHandler patternNotFoundHandler;
    const PatternFinder clientPatternFinder{ clientSo.getCodeSection(), patternNotFoundHandler };
    const PatternFinder enginePatternFinder{ engineSo.getCodeSection(), patternNotFoundHandler };

    memory.emplace(clientPatternFinder, enginePatternFinder, std::get<csgo::ClientPOD*>(*clientInterfaces), *retSpoofGadgets);

    Netvars::init(ClientInterfaces{ retSpoofGadgets->client, *clientInterfaces }.getClient());
    gameEventListener.emplace(getEngineInterfaces().getGameEventManager(memory->getEventDescriptor));

    randomGenerator.emplace();
    features.emplace(createFeatures(*memory, ClientInterfaces{ retSpoofGadgets->client, *clientInterfaces }, getEngineInterfaces(), getOtherInterfaces(), clientPatternFinder, enginePatternFinder, *randomGenerator));
    config.emplace(*features, getOtherInterfaces(), *memory);
    
    gui.emplace();
    hooks->install(std::get<csgo::ClientPOD*>(*clientInterfaces), getEngineInterfaces(), getOtherInterfaces(), *memory);
}
