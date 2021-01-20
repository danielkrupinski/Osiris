#include "../Memory.h"
#include "../Config.h"
#include "../MemAlloc.h"
#include "../SDK/SteamAPI.h"
#include "../Hacks/ProfileChanger.h"
#include "Protobuffs.h"
#include "Messages.h"
#include "ProtoWriter.h"
#include "valve_parser.h"
#include <array>
#define CAST(cast, address, add) reinterpret_cast<cast>((uint32_t)address + (uint32_t)add)

#define _gc2ch MatchmakingGC2ClientHello
#define _pci PlayerCommendationInfo
#define _pri PlayerRankingInfo
static std::string ProfileChanger(void* pubDest, uint32_t* pcubMsgSize)
{
	ProtoWriter msg((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8, 19);
	auto _commendation = msg.has(_gc2ch::commendation) ? msg.get(_gc2ch::commendation).String() : std::string("");
	ProtoWriter commendation(_commendation, 4);
	commendation.replace(Field(_pci::cmd_friendly, TYPE_UINT32, (int64_t)config->profileChanger.friendly));
	commendation.replace(Field(_pci::cmd_teaching, TYPE_UINT32, (int64_t)config->profileChanger.teach));
	commendation.replace(Field(_pci::cmd_leader, TYPE_UINT32, (int64_t)config->profileChanger.leader));
	msg.replace(Field(_gc2ch::commendation, TYPE_STRING, commendation.serialize()));
	auto _ranking = msg.has(_gc2ch::ranking) ? msg.get(_gc2ch::ranking).String() : std::string("");
	ProtoWriter ranking(_ranking, 6);
	ranking.replace(Field(_pri::rank_id, TYPE_UINT32, (int64_t)config->profileChanger.rank));
	ranking.replace(Field(_pri::wins, TYPE_UINT32, (int64_t)config->profileChanger.wins));
	msg.replace(Field(_gc2ch::ranking, TYPE_STRING, ranking.serialize()));
	msg.replace(Field(_gc2ch::player_level, TYPE_INT32, (int64_t)config->profileChanger.level));
	msg.replace(Field(_gc2ch::player_cur_xp, TYPE_INT32, (int64_t)config->profileChanger.exp));

	if (config->profileChanger.ban_type != 0 && config->profileChanger.ban_time != 0)
	{
		msg.replace(Field(_gc2ch::penalty_reason, TYPE_INT32, (int64_t)config->profileChanger.ban_type));
		msg.replace(Field(_gc2ch::penalty_seconds, TYPE_INT32, (int64_t)config->profileChanger.ban_time));
	}

	return msg.serialize();
}



void Protobuffs::WritePacket(std::string packet, void* thisPtr, void* oldEBP, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize)
{
	auto g_MemAlloc = memory->memalloc;
	if ((uint32_t)packet.size() <= cubDest - 8)
	{
		memcpy((void*)((DWORD)pubDest + 8), (void*)packet.data(), packet.size());
		*pcubMsgSize = packet.size() + 8;
	}
	else if (g_MemAlloc)
	{
		auto memPtr = *CAST(void**, thisPtr, 0x14);
		auto memPtrSize = *CAST(uint32_t*, thisPtr, 0x18);
		auto newSize = (memPtrSize - cubDest) + packet.size() + 8;

		auto memory = g_MemAlloc->Realloc(memPtr, newSize + 4);

		*CAST(void**, thisPtr, 0x14) = memory;
		*CAST(uint32_t*, thisPtr, 0x18) = newSize;
		*CAST(void**, oldEBP, -0x14) = memory;

		memcpy(CAST(void*, memory, 24), (void*)packet.data(), packet.size());

		*pcubMsgSize = packet.size() + 8;
	}
}

void Protobuffs::ReceiveMessage(void* thisPtr, void* oldEBP, uint32_t messageType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize)
{
	if (messageType == k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello && config->profileChanger.enabled)
	{
		auto packet = ProfileChanger(pubDest, pcubMsgSize);
		WritePacket(packet, thisPtr, oldEBP, pubDest, cubDest, pcubMsgSize);
	}
}

bool Protobuffs::PreSendMessage(uint32_t& unMsgType, void* pubData, uint32_t& cubData)
{
	g_Inventory->Presend(unMsgType, pubData, cubData);

	if (unMsgType == k_EMsgGCAdjustItemEquippedState)
	{
		return g_Inventory->Presend(unMsgType, pubData, cubData);
	}

	return true;
}

bool Protobuffs::SendClientHello()
{
	ProtoWriter msg(7);
	msg.add(Field(3, TYPE_UINT32, (int64_t)1));
	auto packet = msg.serialize();

	void* ptr = malloc(packet.size() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCClientHello | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	bool result = memory->SteamGameCoordinator->GCSendMessage(k_EMsgGCClientHello | ((DWORD)1 << 31), ptr, packet.size() + 8) == EGCResult::k_EGCResultOK;
	free(ptr);

	return result;
}


bool Protobuffs::SendMatchmakingClient2GCHello()
{
	ProtoWriter msg(0);
	auto packet = msg.serialize();
	void* ptr = malloc(packet.size() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	bool result = memory->SteamGameCoordinator->GCSendMessage(k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31), ptr, packet.size() + 8) == EGCResult::k_EGCResultOK;
	free(ptr);

	return result;
}
