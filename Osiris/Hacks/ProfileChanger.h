#pragma once
#include "../ProfileChanger/ProtoWriter.h"
#include "../ProfileChanger/Messages.h"
#include "../SDK/SteamAPI.h"
#include "../ProfileChanger/Protobuffs.h"
#include "../Config.h"
#include "../Memory.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/NetworkChannel.h"
#define START_MUSICKIT_INDEX 1500000
#define START_ITEM_INDEX     2000000



struct wskin
{
	int wId;
	int paintKit;
	int quality;
	float wear;
	int seed;
	std::string name;
	bool in_use_t;
	bool in_use_ct;
	int slot;
};

inline std::unordered_map<int, wskin> g_InventorySkins;

class Inventory
{
public:
	void FixNullInventory(ProtoWriter& cache);
	void ClearEquipState(ProtoWriter& object);
	void AddAllItems(ProtoWriter& object);
	void AddItem(ProtoWriter& object, int index, int itemIndex, int rarity, int paintKit, int seed, float wear, std::string name);
	bool Presend(uint32_t& unMsgType, void* pubData, uint32_t& cubData);
	void ApplyMedals(ProtoWriter& object);
	static int GetAvailableClassID(int definition_index);
	std::string Changer(void* pubDest, uint32_t* pcubMsgSize);
};

inline Inventory* g_Inventory;
