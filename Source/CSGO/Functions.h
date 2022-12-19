#pragma once

#include "Constants/ItemId.h"
#include <Platform/Macros/CallingConventions.h>

namespace csgo
{

using R_LoadNamedSkys = bool (FASTCALL_CONV*)(const char* skyname);
using SendClanTag = void (FASTCALL_CONV*)(const char* tag, const char* name);
using SubmitReport = bool (STDCALL_CONV*)(LINUX_ARGS(void*, ) const char* xuid, const char* report);

template <typename T>
using CreateSharedObjectSubclass = T* (STDCALL_CONV*)();

struct EconItemViewPOD;
using FindOrCreateEconItemViewForItemID = EconItemViewPOD* (CDECL_CONV*)(ItemId itemID);

struct Vector;
using LineGoesThroughSmoke = bool (CDECL_CONV*)(Vector from, Vector to, short);

class GameEventDescriptor;
struct GameEventManagerPOD;
using GetEventDescriptor = GameEventDescriptor* (THISCALL_CONV*)(GameEventManagerPOD* thisptr, const char* name, int* cookie);

using GlowObjectAntiCheatCheck = void (THISCALL_CONV*)(void*);

}
