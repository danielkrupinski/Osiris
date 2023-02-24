#include "GlobalContext.h"

#if IS_WIN32()
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>

#include "Platform/Windows/DynamicLibrarySection.h"
#else
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Platform/Linux/DynamicLibrarySection.h"
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
#include "Platform/DynamicLibraryWrapper.h"

GlobalContext::GlobalContext()
{
#if IS_WIN32()
    const windows_platform::DynamicLibrary clientDLL{ windows_platform::DynamicLibraryWrapper{}, csgo::CLIENT_DLL };
    const windows_platform::DynamicLibrary engineDLL{ windows_platform::DynamicLibraryWrapper{}, csgo::ENGINE_DLL };
#elif IS_LINUX()
    const linux_platform::SharedObject clientDLL{ linux_platform::DynamicLibraryWrapper{}, csgo::CLIENT_DLL };
    const linux_platform::SharedObject engineDLL{ linux_platform::DynamicLibraryWrapper{}, csgo::ENGINE_DLL };
#endif

    PatternNotFoundHandler patternNotFoundHandler;
    retSpoofGadgets.emplace(PatternFinder{ getCodeSection(clientDLL.getView()), patternNotFoundHandler }, PatternFinder{ getCodeSection(engineDLL.getView()), patternNotFoundHandler });
}

#if IS_WIN32()
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT GlobalContext::wndProcHook(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (state == GlobalContext::State::Initialized) {
        ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam);
        getOtherInterfaces().getInputSystem().enableInput(!gui->isOpen());
    } else if (state == GlobalContext::State::NotInitialized) {
        state = GlobalContext::State::Initializing;
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(window);
        initialize();
        state = GlobalContext::State::Initialized;
    }

    return CallWindowProcW(hooks->originalWndProc, window, msg, wParam, lParam);
}

HRESULT GlobalContext::presentHook(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion)
{
    [[maybe_unused]] static bool imguiInit{ ImGui_ImplDX9_Init(device) };

    if (config->loadScheduledFonts())
        ImGui_ImplDX9_DestroyFontsTexture();

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    renderFrame();

    if (device->BeginScene() == D3D_OK) {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    //
    GameData::clearUnusedAvatars();
    features->inventoryChanger.clearUnusedItemIconTextures();
    //

    return hooks->originalPresent(device, src, dest, windowOverride, dirtyRegion);
}

HRESULT GlobalContext::resetHook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params)
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    features->inventoryChanger.clearItemIconTextures();
    GameData::clearTextures();
    return hooks->originalReset(device, params);
}

#else
int GlobalContext::pollEventHook(SDL_Event* event)
{
    const auto result = hooks->pollEvent(event);

    if (state == GlobalContext::State::Initialized) {
        if (result && ImGui_ImplSDL2_ProcessEvent(event) && gui->isOpen())
            event->type = 0;
    } else if (state == GlobalContext::State::NotInitialized) {
        state = GlobalContext::State::Initializing;
        ImGui::CreateContext();
        initialize();
        state = GlobalContext::State::Initialized;
    }
    
    return result;
}

void GlobalContext::swapWindowHook(SDL_Window* window)
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

void GlobalContext::viewModelSequenceNetvarHook(csgo::recvProxyData* data, void* outStruct, void* arg3)
{
    const auto viewModel = csgo::Entity::from(retSpoofGadgets->client, static_cast<csgo::EntityPOD*>(outStruct));

    if (localPlayer && ClientInterfaces{ retSpoofGadgets->client, *clientInterfaces }.getEntityList().getEntityFromHandle(viewModel.owner()) == localPlayer.get().getPOD()) {
        if (const auto weapon = csgo::Entity::from(retSpoofGadgets->client, ClientInterfaces{ retSpoofGadgets->client, *clientInterfaces }.getEntityList().getEntityFromHandle(viewModel.weapon())); weapon.getPOD() != nullptr) {
            if (features->visuals.isDeagleSpinnerOn() && weapon.getNetworkable().getClientClass()->classId == ClassId::Deagle && data->value._int == 7)
                data->value._int = 8;

            features->inventoryChanger.fixKnifeAnimation(weapon, data->value._int, *randomGenerator);
        }
    }

    proxyHooks.viewModelSequence.originalProxy(data, outStruct, arg3);
}

void GlobalContext::spottedHook(csgo::recvProxyData* data, void* outStruct, void* arg3)
{
    if (features->misc.isRadarHackOn()) {
        data->value._int = 1;

        if (localPlayer) {
            const auto entity = csgo::Entity::from(retSpoofGadgets->client, static_cast<csgo::EntityPOD*>(outStruct));
            if (const auto index = localPlayer.get().getNetworkable().index(); index > 0 && index <= 32)
                entity.spottedByMask() |= 1 << (index - 1);
        }
    }

    proxyHooks.spotted.originalProxy(data, outStruct, arg3);
}

void GlobalContext::fireGameEventCallback(csgo::GameEventPOD* eventPointer)
{
    const auto event = csgo::GameEvent::from(retSpoofGadgets->client, eventPointer);

    switch (fnv::hashRuntime(event.getName())) {
    case fnv::hash(csgo::round_start):
        GameData::clearProjectileList();
        features->misc.preserveKillfeed(true);
        [[fallthrough]];
    case fnv::hash(csgo::round_freeze_end):
        features->misc.purchaseList(&event);
        break;
    case fnv::hash(csgo::player_death):
        features->inventoryChanger.updateStatTrak(event);
        features->inventoryChanger.overrideHudIcon(*memory, event);
        features->misc.killMessage(event);
        features->misc.killSound(event);
        break;
    case fnv::hash(csgo::player_hurt):
        features->misc.playHitSound(event);
        features->visuals.hitEffect(&event);
        features->visuals.hitMarker(&event);
        break;
    case fnv::hash(csgo::vote_cast):
        features->misc.voteRevealer(event);
        break;
    case fnv::hash(csgo::round_mvp):
        features->inventoryChanger.onRoundMVP(event);
        break;
    case fnv::hash(csgo::item_purchase):
        features->misc.purchaseList(&event);
        break;
    case fnv::hash(csgo::bullet_impact):
        features->visuals.bulletTracer(event);
        break;
    }
}

void GlobalContext::renderFrame()
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

void GlobalContext::initialize()
{
    const DynamicLibrary<DynamicLibraryWrapper> clientSo{ DynamicLibraryWrapper{}, csgo::CLIENT_DLL };
    clientInterfaces = createClientInterfacesPODs(InterfaceFinderWithLog{ InterfaceFinder{ clientSo.getView(), retSpoofGadgets->client } });
    const DynamicLibrary<DynamicLibraryWrapper> engineSo{ DynamicLibraryWrapper{}, csgo::ENGINE_DLL };
    engineInterfacesPODs = createEngineInterfacesPODs(InterfaceFinderWithLog{ InterfaceFinder{ engineSo.getView(), retSpoofGadgets->client } });

    interfaces.emplace();
    PatternNotFoundHandler patternNotFoundHandler;
    const PatternFinder clientPatternFinder{ getCodeSection(clientSo.getView()), patternNotFoundHandler };
    const PatternFinder enginePatternFinder{ getCodeSection(engineSo.getView()), patternNotFoundHandler };

    memory.emplace(clientPatternFinder, enginePatternFinder, clientInterfaces->client, *retSpoofGadgets);

    Netvars::init(ClientInterfaces{ retSpoofGadgets->client, *clientInterfaces }.getClient());
    gameEventListener.emplace(getEngineInterfaces().getGameEventManager(memory->getEventDescriptor));

    randomGenerator.emplace();
    features.emplace(createFeatures(*memory, ClientInterfaces{ retSpoofGadgets->client, *clientInterfaces }, getEngineInterfaces(), getOtherInterfaces(), clientPatternFinder, enginePatternFinder, *randomGenerator));
    config.emplace(*features, getOtherInterfaces(), *memory);
    
    gui.emplace();
    hooks->install(clientInterfaces->client, getEngineInterfaces(), getOtherInterfaces(), *memory);
}
