#pragma once
#include <windows.h>
#include <string>

class Protobuffs
{
public:
	static void WritePacket(std::string packet, void* thisPtr, void* oldEBP, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
	void ReceiveMessage(void* thisPtr, void* oldEBP, uint32_t messageType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
	bool PreSendMessage(uint32_t& unMsgType, void* pubData, uint32_t& cubData);
	bool SendClientHello();
	bool SendMatchmakingClient2GCHello();
	bool EquipWeapon(int weaponid, int classid, int slotid);
	void EquipAll();
	static int GetSlotID(int definition_index);
	static void ParseSkins();
};

inline Protobuffs write;