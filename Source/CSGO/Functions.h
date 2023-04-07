#pragma once

#include "Constants/ItemId.h"
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>

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

struct GameEventDescriptor;
struct GameEventManagerPOD;
using GetEventDescriptor = GameEventDescriptor* (THISCALL_CONV*)(GameEventManagerPOD* thisptr, const char* name, int* cookie);

using GlowObjectAntiCheatCheck = void (THISCALL_CONV*)(void*);

struct EconItemPOD;
using SetCustomName = void (THISCALL_CONV*)(EconItemPOD* thisptr, const char* name);

struct EconItemAttributeDefinition;
using RemoveDynamicAttribute = void (THISCALL_CONV*)(EconItemPOD* thisptr, EconItemAttributeDefinition* attribute);

using SetDynamicAttributeValue = void (THISCALL_CONV*)(LINUX_ARGS(void*,) EconItemPOD* thisptr, EconItemAttributeDefinition* attribute, void* value);

using ClearInventoryImageRGBA = void (THISCALL_CONV*)(EconItemViewPOD* thisptr);
using GetSOCData = EconItemPOD* (THISCALL_CONV*)(EconItemViewPOD* thisptr);

struct SharedObjectTypeCachePOD;
struct ClientSharedObjectCachePOD;
using CreateBaseTypeCache = SharedObjectTypeCachePOD* (THISCALL_CONV*)(ClientSharedObjectCachePOD* thisptr, int classID);

struct UiComponentInventoryPOD;
using SetItemSessionPropertyValue = void (THISCALL_CONV*)(UiComponentInventoryPOD* thisptr, ItemId itemID, const char* type, const char* value);

using MakePanoramaSymbol = void (THISCALL_CONV*)(short* symbol, const char* name);

struct ModelRenderPOD;
struct ModelRenderInfo;
struct matrix3x4;
using DrawModelExecute = void (THISCALL_CONV*)(csgo::ModelRenderPOD* thisptr, void* ctx, void* state, const csgo::ModelRenderInfo* info, csgo::matrix3x4* customBoneToWorld);

struct KeyValuesPOD;
using KeyValuesFindKey = KeyValuesPOD* (THISCALL_CONV*)(KeyValuesPOD* thisptr, const char* keyName, bool create);
using KeyValuesSetString = void (THISCALL_CONV*)(KeyValuesPOD* thisptr, const char* value);

using CreateInterface = void* (CDECL_CONV*)(const char* name, int* returnCode);

}
