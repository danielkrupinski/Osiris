#pragma once

#include "VirtualMethod.h"

class NetworkChannel {
public:
    VIRTUAL_METHOD(float, getLatency, 9, (int flow), (this, flow))

    std::byte pad[24];
    int OutSequenceNr;
    int InSequenceNr;
    int OutSequenceNrAck;
    int OutReliableState;
    int InReliableState;
    int chokedPackets;
};

class NetworkMessage
{
public:
    VIRTUAL_METHOD(int, getType, 7, (), (this))
};

class EventInfo {
public:
	enum {
		EVENT_INDEX_BITS = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA = 192,  // ( 1<<8 bits == 256, but only using 192 below )
	};
	short class_id;
	float fire_delay;
	const void* m_send_table;
	//const ClientClass* m_client_class; //ez fix cause lazy
	int bits;
	uint8_t* data;
	int flags;
	PAD(0x1C);
	EventInfo* m_next;
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
	EventInfo*      pEvents;
};