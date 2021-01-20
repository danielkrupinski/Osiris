#pragma once
#include <windows.h>
#include <string>

class Protobuffs
{
public:
	static void WritePacket(std::string packet, void* thisPtr, void* oldEBP, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
	void ReceiveMessage(void* thisPtr, void* oldEBP, uint32_t messageType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
	bool SendClientHello();
	bool SendMatchmakingClient2GCHello();
};

inline Protobuffs write;