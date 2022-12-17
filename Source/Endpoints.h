#pragma once

#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/IsPlatform.h>

#include <CSGO/Constants/ItemId.h>

#if IS_WIN32()
#include <d3d9.h>
#include <Windows.h>
#elif IS_LINUX()
#include <SDL2/SDL.h>
#endif

void initializeGlobalContext();

#if IS_WIN32()

LRESULT __stdcall wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
HRESULT __stdcall reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept;
HRESULT __stdcall present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept;

#elif IS_LINUX()

int pollEvent(SDL_Event* event) noexcept;
void swapWindow(SDL_Window* window) noexcept;

#endif

namespace csgo
{

struct DemoPlaybackParameters;
class matrix3x4;
struct ModelRenderInfo;
struct SOID;
struct SoundInfo;
struct UserCmd;
struct Vector;
struct ViewSetup;
struct SharedObjectPOD;

enum class FrameStage;
enum class Team;
enum class UserMessageType;

}

bool STDCALL_CONV createMove(LINUX_ARGS(void* thisptr, ) float inputSampleTime, csgo::UserCmd* cmd) noexcept;
void STDCALL_CONV doPostScreenEffects(LINUX_ARGS(void* thisptr, ) void* param) noexcept;
float STDCALL_CONV getViewModelFov(LINUX_ARGS(void* thisptr)) noexcept;
void STDCALL_CONV drawModelExecute(LINUX_ARGS(void* thisptr, ) void* ctx, void* state, const csgo::ModelRenderInfo& info, csgo::matrix3x4* customBoneToWorld) noexcept;
int FASTCALL_CONV svCheatsGetInt(void* _this) noexcept;
void STDCALL_CONV frameStageNotify(LINUX_ARGS(void* thisptr, ) csgo::FrameStage stage) noexcept;
int STDCALL_CONV emitSound(LINUX_ARGS(void* thisptr, ) void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const csgo::Vector& origin, const csgo::Vector& direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams) noexcept;
bool STDCALL_CONV shouldDrawFog(LINUX_ARGS(void* thisptr)) noexcept;
bool STDCALL_CONV shouldDrawViewModel(LINUX_ARGS(void* thisptr)) noexcept;
void STDCALL_CONV lockCursor() noexcept;
void STDCALL_CONV setDrawColor(LINUX_ARGS(void* thisptr, ) int r, int g, int b, int a) noexcept;
void STDCALL_CONV overrideView(LINUX_ARGS(void* thisptr, ) csgo::ViewSetup* setup) noexcept;
int STDCALL_CONV listLeavesInBox(LINUX_ARGS(void* thisptr, ) const csgo::Vector& mins, const csgo::Vector& maxs, unsigned short* list, int listMax) noexcept;
int FASTCALL_CONV dispatchSound(csgo::SoundInfo& soundInfo) noexcept;
void STDCALL_CONV render2dEffectsPreHud(LINUX_ARGS(void* thisptr, ) void* viewSetup) noexcept;
const csgo::DemoPlaybackParameters* STDCALL_CONV getDemoPlaybackParameters(LINUX_ARGS(void* thisptr)) noexcept;
bool STDCALL_CONV isPlayingDemo(LINUX_ARGS(void* thisptr)) noexcept;
void STDCALL_CONV updateColorCorrectionWeights(LINUX_ARGS(void* thisptr)) noexcept;
float STDCALL_CONV getScreenAspectRatio(LINUX_ARGS(void* thisptr, ) int width, int height) noexcept;
void STDCALL_CONV renderSmokeOverlay(LINUX_ARGS(void* thisptr, ) bool update) noexcept;
double STDCALL_CONV getArgAsNumber(LINUX_ARGS(void* thisptr, ) void* params, int index) noexcept;
const char* STDCALL_CONV getArgAsString(LINUX_ARGS(void* thisptr, ) void* params, int index) noexcept;
void STDCALL_CONV setResultInt(LINUX_ARGS(void* thisptr, ) void* params, int result) noexcept;
unsigned STDCALL_CONV getNumArgs(LINUX_ARGS(void* thisptr, ) void* params) noexcept;
void STDCALL_CONV updateInventoryEquippedState(LINUX_ARGS(void* thisptr, ) std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap) noexcept;
void STDCALL_CONV soUpdated(LINUX_ARGS(void* thisptr, ) csgo::SOID owner, csgo::SharedObjectPOD* object, int event) noexcept;
bool STDCALL_CONV dispatchUserMessage(LINUX_ARGS(void* thisptr, ) csgo::UserMessageType type, int passthroughFlags, int size, const void* data) noexcept;

#if IS_WIN32()
void* STDCALL_CONV allocKeyValuesMemory(LINUX_ARGS(void* thisptr, ) int size) noexcept;
DWORD WINAPI unload(HMODULE moduleHandle) noexcept;
#endif
