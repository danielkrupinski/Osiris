#pragma once

#include "VirtualMethod.h"

class NetworkChannel {
public:
    VIRTUAL_METHOD(float, getLatency, 9, (int flow), (this, flow))

    std::byte pad[44];
    int chokedPackets;
};

class ClientState
{
public:
	void ForceFullUpdate()
	{
		deltaTick = -1;
	}
	std::byte		pad0[0x9C];
	NetworkChannel* netChannel;
	int				challengeNr;
	std::byte		pad1[0x64];
	int				signonState;
	std::byte		pad2[0x8];
	float			nextCmdTime;
	int				serverCount;
	int				currentSequence;
	std::byte		pad3[0x54];
	int				deltaTick;
	bool			paused;
	std::byte		pad4[0x7];
	int				viewEntity;
	int				playerSlot;
	char			levelName[260];
	char			levelNameShort[80];
	char			groupName[80];
	std::byte		pad5[0x5C];
	int				maxClients;
	std::byte		pad6[0x4984];
	float			lastServerTickTime;
	bool			InSimulation;
	std::byte		pad7[0x3];
	int				oldTickcount;
	float			tickRemainder;
	float			frameTime;
	int				lastOutgoingCommand;
	int				chokedCommands;
	int				lastCommandAck;
	int				commandAck;
	int				soundSequence;
	std::byte		pad8[0x50];
	Vector			angViewPoint;
	std::byte		pad9[0xD0];
	EventInfo* pEvents;
};