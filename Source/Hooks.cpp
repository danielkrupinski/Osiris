#include <charconv>
#include <functional>
#include <string>

#include "imgui/imgui.h"

#ifdef _WIN32
#include <intrin.h>
#include <Windows.h>
#include <Psapi.h>

#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "MinHook/MinHook.h"
#elif __linux__
#include <sys/mman.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#endif

#include "Config.h"
#include "EventListener.h"
#include "GameData.h"
#include "GUI.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "Memory.h"

#include "Hacks/Aimbot.h"
#include "Hacks/AntiAim.h"
#include "Hacks/Backtrack.h"
#include "Hacks/Chams.h"
#include "Hacks/EnginePrediction.h"
#include "Hacks/StreamProofESP.h"
#include "Hacks/Glow.h"
#include "Hacks/Misc.h"
#include "Hacks/Sound.h"
#include "Hacks/Triggerbot.h"
#include "Hacks/Visuals.h"

#include "InventoryChanger/InventoryChanger.h"

#include "SDK/ClientClass.h"
#include "SDK/Cvar.h"
#include "SDK/Engine.h"
#include "SDK/Entity.h"
#include "SDK/EntityList.h"
#include "SDK/Constants/FrameStage.h"
#include "SDK/GameEvent.h"
#include "SDK/GameUI.h"
#include "SDK/GlobalVars.h"
#include "SDK/InputSystem.h"
#include "SDK/ItemSchema.h"
#include "SDK/LocalPlayer.h"
#include "SDK/MaterialSystem.h"
#include "SDK/ModelRender.h"
#include "SDK/Platform.h"
#include "SDK/RenderContext.h"
#include "SDK/SoundInfo.h"
#include "SDK/SoundEmitter.h"
#include "SDK/StudioRender.h"
#include "SDK/Surface.h"
#include "SDK/UserCmd.h"
#include "SDK/Constants/UserMessages.h"

#include "GlobalContext.h"

#ifdef _WIN32

static LRESULT __stdcall wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    return globalContext->wndProcHook(window, msg, wParam, lParam);
}

static HRESULT __stdcall reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    InventoryChanger::clearItemIconTextures();
    GameData::clearTextures();
    return hooks->originalReset(device, params);
}

#endif

#ifdef _WIN32
static HRESULT __stdcall present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
{
    [[maybe_unused]] static bool imguiInit{ ImGui_ImplDX9_Init(device) };

    if (config->loadScheduledFonts())
        ImGui_ImplDX9_DestroyFontsTexture();

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
#else
static void swapWindow(SDL_Window * window) noexcept
{
    [[maybe_unused]] static const auto _ = ImGui_ImplSDL2_InitForOpenGL(window, nullptr);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
#endif
    ImGui::NewFrame();

    if (const auto& displaySize = ImGui::GetIO().DisplaySize; displaySize.x > 0.0f && displaySize.y > 0.0f) {
        StreamProofESP::render(*memory, *config);
        Misc::purchaseList(*interfaces, *memory);
        Misc::noscopeCrosshair(*memory, ImGui::GetBackgroundDrawList());
        Misc::recoilCrosshair(*memory, ImGui::GetBackgroundDrawList());
        Misc::drawOffscreenEnemies(*interfaces, *memory, ImGui::GetBackgroundDrawList());
        Misc::drawBombTimer(*memory);
        Misc::spectatorList();
        Visuals::hitMarker(*interfaces, *memory, nullptr, ImGui::GetBackgroundDrawList());
        Visuals::drawMolotovHull(*memory, ImGui::GetBackgroundDrawList());
        Misc::watermark(*memory);

        Aimbot::updateInput(*config);
        Visuals::updateInput();
        StreamProofESP::updateInput(*config);
        Misc::updateInput();
        Triggerbot::updateInput(*config);
        Chams::updateInput(*config);
        Glow::updateInput();

        gui->handleToggle(*interfaces);

        if (gui->isOpen())
            gui->render(*interfaces, *memory, *config);
    }

    ImGui::EndFrame();
    ImGui::Render();

#ifdef _WIN32
    if (device->BeginScene() == D3D_OK) {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }
#else
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

    GameData::clearUnusedAvatars();
    InventoryChanger::clearUnusedItemIconTextures();

#ifdef _WIN32
    return hooks->originalPresent(device, src, dest, windowOverride, dirtyRegion);
#else
    hooks->swapWindow(window);
#endif
}

static bool STDCALL_CONV createMove(LINUX_ARGS(void* thisptr,) float inputSampleTime, UserCmd* cmd) noexcept
{
    return globalContext->createMoveHook(inputSampleTime, cmd);
}

static void STDCALL_CONV doPostScreenEffects(LINUX_ARGS(void* thisptr,) void* param) noexcept
{
    globalContext->doPostScreenEffectsHook(param);
}

static float STDCALL_CONV getViewModelFov(LINUX_ARGS(void* thisptr)) noexcept
{
    return globalContext->getViewModelFovHook();
}

static void STDCALL_CONV drawModelExecute(LINUX_ARGS(void* thisptr,) void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    globalContext->drawModelExecuteHook(ctx, state, info, customBoneToWorld);
}

static bool FASTCALL_CONV svCheatsGetBool(void* _this) noexcept
{
    return globalContext->svCheatsGetBoolHook(_this, RETURN_ADDRESS());
}

static void STDCALL_CONV frameStageNotify(LINUX_ARGS(void* thisptr,) csgo::FrameStage stage) noexcept
{
    globalContext->frameStageNotifyHook(stage);
}

static int STDCALL_CONV emitSound(LINUX_ARGS(void* thisptr,) void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const Vector& origin, const Vector& direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams) noexcept
{
    Sound::modulateSound(*interfaces, *memory, soundEntry, entityIndex, volume);
    Misc::autoAccept(*interfaces, *memory, soundEntry);

    volume = std::clamp(volume, 0.0f, 1.0f);
    return hooks->sound.callOriginal<int, WIN32_LINUX(5, 6)>(filter, entityIndex, channel, soundEntry, soundEntryHash, sample, volume, seed, soundLevel, flags, pitch, std::cref(origin), std::cref(direction), utlVecOrigins, updatePositions, soundtime, speakerentity, soundParams);
}

static bool STDCALL_CONV shouldDrawFog(LINUX_ARGS(void* thisptr)) noexcept
{
#ifdef _WIN32
    if constexpr (std::is_same_v<HookType, MinHook>) {
        if (RETURN_ADDRESS() != memory->shouldDrawFogReturnAddress)
            return hooks->clientMode.callOriginal<bool, 17>();
    }
#endif
    
    return !Visuals::shouldRemoveFog();
}

static bool STDCALL_CONV shouldDrawViewModel(LINUX_ARGS(void* thisptr)) noexcept
{
    if (Visuals::isZoomOn() && localPlayer && localPlayer->fov() < 45 && localPlayer->fovStart() < 45)
        return false;
    return hooks->clientMode.callOriginal<bool, WIN32_LINUX(27, 28)>();
}

static void STDCALL_CONV lockCursor() noexcept
{
    if (gui->isOpen())
        return interfaces->surface->unlockCursor();
    return hooks->surface.callOriginal<void, 67>();
}

static void STDCALL_CONV setDrawColor(LINUX_ARGS(void* thisptr,) int r, int g, int b, int a) noexcept
{
    if (Visuals::shouldRemoveScopeOverlay() && (RETURN_ADDRESS() == memory->scopeDust || RETURN_ADDRESS() == memory->scopeArc))
        a = 0;
    hooks->surface.callOriginal<void, WIN32_LINUX(15, 14)>(r, g, b, a);
}

struct ViewSetup {
    PAD(172);
    void* csm;
    float fov;
    PAD(32);
    float farZ;
};

static void STDCALL_CONV overrideView(LINUX_ARGS(void* thisptr,) ViewSetup* setup) noexcept
{
    if (localPlayer && !localPlayer->isScoped())
        setup->fov += Visuals::fov();
    setup->farZ += Visuals::farZ() * 10;
    hooks->clientMode.callOriginal<void, WIN32_LINUX(18, 19)>(setup);
}

struct RenderableInfo {
    Entity* renderable;
    std::byte pad[18];
    uint16_t flags;
    uint16_t flags2;
};

static int STDCALL_CONV listLeavesInBox(LINUX_ARGS(void* thisptr, ) const Vector& mins, const Vector& maxs, unsigned short* list, int listMax) noexcept
{
    if (Misc::shouldDisableModelOcclusion() && RETURN_ADDRESS() == memory->insertIntoTree) {
        if (const auto info = *reinterpret_cast<RenderableInfo**>(FRAME_ADDRESS() + WIN32_LINUX(0x18, 0x10 + 0x948)); info && info->renderable) {
            if (const auto ent = VirtualMethod::call<Entity*, WIN32_LINUX(7, 8)>(info->renderable - sizeof(std::uintptr_t)); ent && ent->isPlayer()) {
                constexpr float maxCoord = 16384.0f;
                constexpr float minCoord = -maxCoord;
                constexpr Vector min{ minCoord, minCoord, minCoord };
                constexpr Vector max{ maxCoord, maxCoord, maxCoord };
                return hooks->bspQuery.callOriginal<int, 6>(std::cref(min), std::cref(max), list, listMax);
            }
        }
    }

    return hooks->bspQuery.callOriginal<int, 6>(std::cref(mins), std::cref(maxs), list, listMax);
}

static int FASTCALL_CONV dispatchSound(SoundInfo& soundInfo) noexcept
{
    if (const char* soundName = interfaces->soundEmitter->getSoundName(soundInfo.soundIndex)) {
        Sound::modulateSound(*interfaces, *memory, soundName, soundInfo.entityIndex, soundInfo.volume);
        soundInfo.volume = std::clamp(soundInfo.volume, 0.0f, 1.0f);
    }
    return hooks->originalDispatchSound(soundInfo);
}

static void STDCALL_CONV render2dEffectsPreHud(LINUX_ARGS(void* thisptr,) void* viewSetup) noexcept
{
    Visuals::applyScreenEffects(*interfaces, *memory);
    Visuals::hitEffect(*interfaces, *memory);
    hooks->viewRender.callOriginal<void, WIN32_LINUX(39, 40)>(viewSetup);
}

static const DemoPlaybackParameters* STDCALL_CONV getDemoPlaybackParameters(LINUX_ARGS(void* thisptr)) noexcept
{
    const auto params = hooks->engine.callOriginal<const DemoPlaybackParameters*, WIN32_LINUX(218, 219)>();

    if (params && Misc::shouldRevealSuspect() && RETURN_ADDRESS() != memory->demoFileEndReached) {
        static DemoPlaybackParameters customParams;
        customParams = *params;
        customParams.anonymousPlayerIdentity = false;
        return &customParams;
    }

    return params;
}

static bool STDCALL_CONV isPlayingDemo(LINUX_ARGS(void* thisptr)) noexcept
{
    if (Misc::shouldRevealMoney() && RETURN_ADDRESS() == memory->demoOrHLTV && *reinterpret_cast<std::uintptr_t*>(FRAME_ADDRESS() + WIN32_LINUX(8, 24)) == memory->money)
        return true;

    return hooks->engine.callOriginal<bool, 82>();
}

static void STDCALL_CONV updateColorCorrectionWeights(LINUX_ARGS(void* thisptr)) noexcept
{
    hooks->clientMode.callOriginal<void, WIN32_LINUX(58, 61)>();

    Visuals::performColorCorrection(*memory);
    if (Visuals::shouldRemoveScopeOverlay())
        *memory->vignette = 0.0f;
}

static float STDCALL_CONV getScreenAspectRatio(LINUX_ARGS(void* thisptr,) int width, int height) noexcept
{
    if (Misc::aspectRatio() != 0.0f)
        return Misc::aspectRatio();
    return hooks->engine.callOriginal<float, 101>(width, height);
}

static void STDCALL_CONV renderSmokeOverlay(LINUX_ARGS(void* thisptr,) bool update) noexcept
{
    if (Visuals::shouldRemoveSmoke() || Visuals::isSmokeWireframe())
        *reinterpret_cast<float*>(std::uintptr_t(memory->viewRender) + WIN32_LINUX(0x588, 0x648)) = 0.0f;
    else
        hooks->viewRender.callOriginal<void, WIN32_LINUX(41, 42)>(update);
}

static double STDCALL_CONV getArgAsNumber(LINUX_ARGS(void* thisptr,) void* params, int index) noexcept
{
    const auto result = hooks->panoramaMarshallHelper.callOriginal<double, 5>(params, index);
    inventory_changer::InventoryChanger::instance(*interfaces, *memory).getArgAsNumberHook(*memory, static_cast<int>(result), RETURN_ADDRESS());
    return result;
}

static const char* STDCALL_CONV getArgAsString(LINUX_ARGS(void* thisptr,) void* params, int index) noexcept
{
    const auto result = hooks->panoramaMarshallHelper.callOriginal<const char*, 7>(params, index);

    if (result)
        inventory_changer::InventoryChanger::instance(*interfaces, *memory).getArgAsStringHook(*memory, result, RETURN_ADDRESS(), params);

    return result;
}

static void STDCALL_CONV setResultInt(LINUX_ARGS(void* thisptr, ) void* params, int result) noexcept
{
    result = inventory_changer::InventoryChanger::instance(*interfaces, *memory).setResultIntHook(*memory, RETURN_ADDRESS(), params, result);
    hooks->panoramaMarshallHelper.callOriginal<void, WIN32_LINUX(14, 11)>(params, result);
}

static unsigned STDCALL_CONV getNumArgs(LINUX_ARGS(void* thisptr, ) void* params) noexcept
{
    const auto result = hooks->panoramaMarshallHelper.callOriginal<unsigned, 1>(params);
    inventory_changer::InventoryChanger::instance(*interfaces, *memory).getNumArgsHook(*memory, result, RETURN_ADDRESS(), params);
    return result;
}

static void STDCALL_CONV updateInventoryEquippedState(LINUX_ARGS(void* thisptr, ) CSPlayerInventory* inventory, std::uint64_t itemID, csgo::Team team, int slot, bool swap) noexcept
{
    inventory_changer::InventoryChanger::instance(*interfaces, *memory).onItemEquip(team, slot, itemID);
    return hooks->inventoryManager.callOriginal<void, WIN32_LINUX(29, 30)>(inventory, itemID, team, slot, swap);
}

static void STDCALL_CONV soUpdated(LINUX_ARGS(void* thisptr, ) SOID owner, SharedObject* object, int event) noexcept
{
    inventory_changer::InventoryChanger::instance(*interfaces, *memory).onSoUpdated(object);
    hooks->inventory.callOriginal<void, 1>(owner, object, event);
}

static bool STDCALL_CONV dispatchUserMessage(LINUX_ARGS(void* thisptr, ) csgo::UserMessageType type, int passthroughFlags, int size, const void* data) noexcept
{
    if (type == csgo::UserMessageType::Text)
        inventory_changer::InventoryChanger::instance(*interfaces, *memory).onUserTextMsg(*memory, data, size);
    else if (type == csgo::UserMessageType::VoteStart)
        Misc::onVoteStart(*interfaces, *memory, data, size);
    else if (type == csgo::UserMessageType::VotePass)
        Misc::onVotePass(*memory);
    else if (type == csgo::UserMessageType::VoteFailed)
        Misc::onVoteFailed(*memory);
    
    return hooks->client.callOriginal<bool, 38>(type, passthroughFlags, size, data);
}

#ifdef _WIN32

static void* STDCALL_CONV allocKeyValuesMemory(LINUX_ARGS(void* thisptr, ) int size) noexcept
{
    if (const auto returnAddress = RETURN_ADDRESS(); returnAddress == memory->keyValuesAllocEngine || returnAddress == memory->keyValuesAllocClient)
        return nullptr;
    return hooks->keyValuesSystem.callOriginal<void*, 2>(size);
}

Hooks::Hooks(HMODULE moduleHandle) noexcept : moduleHandle{ moduleHandle }
{
#ifndef __MINGW32__
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#endif

    window = FindWindowW(L"Valve001", nullptr);
    originalWndProc = WNDPROC(SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(&wndProc)));
}

#endif

void Hooks::install(const Interfaces& interfaces, const Memory& memory) noexcept
{
#ifdef _WIN32
    originalPresent = **reinterpret_cast<decltype(originalPresent)**>(memory.present);
    **reinterpret_cast<decltype(present)***>(memory.present) = present;
    originalReset = **reinterpret_cast<decltype(originalReset)**>(memory.reset);
    **reinterpret_cast<decltype(reset)***>(memory.reset) = reset;

    if constexpr (std::is_same_v<HookType, MinHook>)
        MH_Initialize();
#else
    ImGui_ImplOpenGL3_Init();

    swapWindow = *reinterpret_cast<decltype(swapWindow)*>(sdlFunctions.swapWindow);
    *reinterpret_cast<decltype(::swapWindow)**>(sdlFunctions.swapWindow) = ::swapWindow;

#endif
    
    bspQuery.init(interfaces.engine->getBSPTreeQuery());
    bspQuery.hookAt(6, &listLeavesInBox);

    client.init(interfaces.client);
    client.hookAt(37, &frameStageNotify);
    client.hookAt(38, &dispatchUserMessage);

    clientMode.init(memory.clientMode);
    clientMode.hookAt(WIN32_LINUX(17, 18), &shouldDrawFog);
    clientMode.hookAt(WIN32_LINUX(18, 19), &overrideView);
    clientMode.hookAt(WIN32_LINUX(24, 25), &createMove);
    clientMode.hookAt(WIN32_LINUX(27, 28), &shouldDrawViewModel);
    clientMode.hookAt(WIN32_LINUX(35, 36), &getViewModelFov);
    clientMode.hookAt(WIN32_LINUX(44, 45), &doPostScreenEffects);
    clientMode.hookAt(WIN32_LINUX(58, 61), &updateColorCorrectionWeights);

    engine.init(interfaces.engine);
    engine.hookAt(82, &isPlayingDemo);
    engine.hookAt(101, &getScreenAspectRatio);
#ifdef _WIN32
    keyValuesSystem.init(memory.keyValuesSystem);
    keyValuesSystem.hookAt(2, &allocKeyValuesMemory);
#endif
    engine.hookAt(WIN32_LINUX(218, 219), &getDemoPlaybackParameters);

    inventory.init(memory.inventoryManager->getLocalInventory());
    inventory.hookAt(1, &soUpdated);

    inventoryManager.init(memory.inventoryManager);
    inventoryManager.hookAt(WIN32_LINUX(29, 30), &updateInventoryEquippedState);

    modelRender.init(interfaces.modelRender);
    modelRender.hookAt(21, &drawModelExecute);

    panoramaMarshallHelper.init(memory.panoramaMarshallHelper);
    panoramaMarshallHelper.hookAt(1, &getNumArgs);
    panoramaMarshallHelper.hookAt(5, &getArgAsNumber);
    panoramaMarshallHelper.hookAt(7, &getArgAsString);
    panoramaMarshallHelper.hookAt(WIN32_LINUX(14, 11), &setResultInt);

    sound.init(interfaces.sound);
    sound.hookAt(WIN32_LINUX(5, 6), &emitSound);

    surface.init(interfaces.surface);
    surface.hookAt(WIN32_LINUX(15, 14), &setDrawColor);
    
    svCheats.init(interfaces.cvar->findVar("sv_cheats"));
    svCheats.hookAt(WIN32_LINUX(13, 16), &svCheatsGetBool);

    viewRender.init(memory.viewRender);
    viewRender.hookAt(WIN32_LINUX(39, 40), &render2dEffectsPreHud);
    viewRender.hookAt(WIN32_LINUX(41, 42), &renderSmokeOverlay);

#ifdef _WIN32
    if (DWORD oldProtection; VirtualProtect(memory.dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection)) {
#else
    if (const auto addressPageAligned = std::uintptr_t(memory.dispatchSound) - std::uintptr_t(memory.dispatchSound) % sysconf(_SC_PAGESIZE);
        mprotect((void*)addressPageAligned, 4, PROT_READ | PROT_WRITE | PROT_EXEC) == 0) {
#endif
        originalDispatchSound = decltype(originalDispatchSound)(uintptr_t(memory.dispatchSound + 1) + *memory.dispatchSound);
        *memory.dispatchSound = uintptr_t(&dispatchSound) - uintptr_t(memory.dispatchSound + 1);
#ifdef _WIN32
        VirtualProtect(memory.dispatchSound, 4, oldProtection, nullptr);
#endif
    }

#ifdef _WIN32
    surface.hookAt(67, &lockCursor);

    if constexpr (std::is_same_v<HookType, MinHook>)
        MH_EnableHook(MH_ALL_HOOKS);
#endif
}

#ifdef _WIN32

extern "C" BOOL WINAPI _CRT_INIT(HMODULE moduleHandle, DWORD reason, LPVOID reserved);

static DWORD WINAPI unload(HMODULE moduleHandle) noexcept
{
    Sleep(100);

    interfaces->inputSystem->enableInput(true);
    globalContext->gameEventListener->remove();

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    _CRT_INIT(moduleHandle, DLL_PROCESS_DETACH, nullptr);

    FreeLibraryAndExitThread(moduleHandle, 0);
}

#endif

void Hooks::uninstall(const Interfaces& interfaces, const Memory& memory) noexcept
{
    Misc::updateEventListeners(interfaces, memory, true);
    Visuals::updateEventListeners(interfaces, memory, true);

#ifdef _WIN32
    if constexpr (std::is_same_v<HookType, MinHook>) {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }
#endif

    bspQuery.restore();
    client.restore();
    clientMode.restore();
    engine.restore();
    inventory.restore();
    inventoryManager.restore();
    modelRender.restore();
    panoramaMarshallHelper.restore();
    sound.restore();
    surface.restore();
    svCheats.restore();
    viewRender.restore();

    Netvars::restore();

    Glow::clearCustomObjects(memory);
    inventory_changer::InventoryChanger::instance(interfaces, memory).reset(interfaces, memory);

#ifdef _WIN32
    keyValuesSystem.restore();

    SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(originalWndProc));
    **reinterpret_cast<void***>(memory.present) = originalPresent;
    **reinterpret_cast<void***>(memory.reset) = originalReset;

    if (DWORD oldProtection; VirtualProtect(memory.dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        *memory.dispatchSound = uintptr_t(originalDispatchSound) - uintptr_t(memory.dispatchSound + 1);
        VirtualProtect(memory.dispatchSound, 4, oldProtection, nullptr);
    }

    if (HANDLE thread = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(unload), moduleHandle, 0, nullptr))
        CloseHandle(thread);
#else
    *reinterpret_cast<decltype(pollEvent)*>(sdlFunctions.pollEvent) = pollEvent;
    *reinterpret_cast<decltype(swapWindow)*>(sdlFunctions.swapWindow) = swapWindow;
#endif
}

void Hooks::callOriginalDrawModelExecute(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
}

#ifndef _WIN32

static int pollEvent(SDL_Event* event) noexcept
{
    return globalContext->pollEventHook(event);
}

Hooks::Hooks() noexcept
    : sdlFunctions{ linux_platform::SharedObject{ linux_platform::DynamicLibraryWrapper{}, "libSDL2-2.0.so.0" } }
{
    pollEvent = *reinterpret_cast<decltype(pollEvent)*>(sdlFunctions.pollEvent);
    *reinterpret_cast<decltype(::pollEvent)**>(sdlFunctions.pollEvent) = ::pollEvent;
}

#endif
