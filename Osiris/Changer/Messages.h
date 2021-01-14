#pragma once

#define k_EMsgGCCStrike15_v2_MatchmakingGC2ClientReserve 9107
#define k_EMsgGCClientWelcome 4004
#define k_EMsgGCClientHello 4006
#define k_EMsgGCAdjustItemEquippedState 1059
#define k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello 9109
#define k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello 9110

// ProfileChanger
struct MatchmakingGC2ClientHello {
	static const uint32_t commendation = 8;
	static const uint32_t ranking = 7;
	static const uint32_t player_level = 17;
	static const uint32_t penalty_seconds = 4;
	static const uint32_t penalty_reason = 5;
	static const uint32_t player_cur_xp = 18;
};
struct PlayerCommendationInfo {
	static const uint32_t cmd_friendly = 1;
	static const uint32_t cmd_teaching = 2;
	static const uint32_t cmd_leader = 4;
};
struct PlayerRankingInfo {
	static const uint32_t rank_id = 2;
	static const uint32_t wins = 3;
};

// InvChanger
struct SubscribedType {
	static const uint32_t type_id = 1;
	static const uint32_t object_data = 2;
};

struct CMsgSOCacheSubscribed {
	static const uint32_t objects = 2;
};

struct CMsgClientWelcome {
	static const uint32_t outofdate_subscribed_caches = 3;
};

struct CSOEconItem {
	static const uint32_t id = 1;
	static const uint32_t account_id = 2;
	static const uint32_t inventory = 3;
	static const uint32_t def_index = 4;
	static const uint32_t quantity = 5;
	static const uint32_t level = 6;
	static const uint32_t quality = 7;
	static const uint32_t flags = 8;
	static const uint32_t origin = 9;
	static const uint32_t custom_name = 10;
	static const uint32_t attribute = 12;
	static const uint32_t in_use = 14;
	static const uint32_t style = 15;
	static const uint32_t original_id = 16;
	static const uint32_t equipped_state = 18;
	static const uint32_t rarity = 19;
};

struct CMsgAdjustItemEquippedState {
	static const uint32_t item_id = 1;
	static const uint32_t new_class = 2;
	static const uint32_t new_slot = 3;
};

struct CSOEconItemEquipped {
	static const uint32_t new_class = 1;
	static const uint32_t new_slot = 2;
};

struct CSOEconItemAttribute {
	static const uint32_t def_index = 1;
	static const uint32_t value = 2;
	static const uint32_t value_bytes = 3;
};