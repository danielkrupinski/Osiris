#include "inventorychanger.h"


std::string Inventory::Changer(void* pubDest, uint32_t* pcubMsgSize)
{
	ProtoWriter msg((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8, 11);
	if (msg.getAll(CMsgClientWelcome::outofdate_subscribed_caches).empty())
		return msg.serialize();

	ProtoWriter cache(msg.get(CMsgClientWelcome::outofdate_subscribed_caches).String(), 4);
	// If not have items in inventory, Create null inventory
	FixNullInventory(cache);
	// Add custom items
	auto objects = cache.getAll(CMsgSOCacheSubscribed::objects);
	for (size_t i = 0; i < objects.size(); i++)
	{
		ProtoWriter object(objects[i].String(), 2);

		if (!object.has(SubscribedType::type_id))
			continue;

		if (object.get(SubscribedType::type_id).Int32() == 1)
		{
			object.clear(SubscribedType::object_data);

			ClearEquipState(object);
			AddAllItems(object);
			ApplyMedals(object);
			cache.replace(Field(CMsgSOCacheSubscribed::objects, TYPE_STRING, object.serialize()), i);
		}
	}
	msg.replace(Field(CMsgClientWelcome::outofdate_subscribed_caches, TYPE_STRING, cache.serialize()), 0);

	return msg.serialize();
}

void Inventory::ApplyMedals(ProtoWriter& object)
{
	uint32_t MedalIndex = config->medalChanger.medals;
	uint32_t steamid = memory->SteamUser->GetSteamID().GetAccountID();
	ProtoWriter Medal(19);
	Medal.add(Field(CSOEconItem::account_id, TYPE_UINT32, (int64_t)steamid));
	Medal.add(Field(CSOEconItem::origin, TYPE_UINT32, (int64_t)9));
	Medal.add(Field(CSOEconItem::rarity, TYPE_UINT32, (int64_t)4));
	Medal.add(Field(CSOEconItem::quantity, TYPE_UINT32, (int64_t)1));
	Medal.add(Field(CSOEconItem::quality, TYPE_UINT32, (int64_t)4));
	Medal.add(Field(CSOEconItem::level, TYPE_UINT32, (int64_t)1));

	uint32_t TimeAcquiredAttributeValue = 0;
	ProtoWriter TimeAcquiredAttribute(3);
	TimeAcquiredAttribute.add(Field(CSOEconItemAttribute::def_index, TYPE_UINT32, (int64_t)222));
	TimeAcquiredAttribute.add(Field(CSOEconItemAttribute::value_bytes, TYPE_UINT32, (int64_t)4));

	Medal.add(Field(CSOEconItemAttribute::def_index, TYPE_UINT32, (int64_t)4));
	Medal.add(Field(CSOEconItemAttribute::value, TYPE_UINT32, (int64_t)4));
	Medal.add(Field(CSOEconItem::def_index, TYPE_UINT32, (int64_t)MedalIndex));
	Medal.add(Field(CSOEconItem::inventory, TYPE_UINT32, (int64_t)10001));
	Medal.add(Field(CSOEconItem::id, TYPE_UINT32, (int64_t)10001));


	object.add(Field(SubscribedType::object_data, TYPE_STRING, Medal.serialize()));

}

void Inventory::FixNullInventory(ProtoWriter& cache)
{
	bool inventory_exist = false;
	auto objects = cache.getAll(CMsgSOCacheSubscribed::objects);
	for (size_t i = 0; i < objects.size(); i++)
	{
		ProtoWriter object(objects[i].String(), 2);
		if (!object.has(SubscribedType::type_id))
			continue;
		if (object.get(SubscribedType::type_id).Int32() != 1)
			continue;
		inventory_exist = true;
		break;
	}
	if (!inventory_exist)
	{
		ProtoWriter null_object(2);
		null_object.add(Field(SubscribedType::type_id, TYPE_INT32, (int64_t)1));

		cache.add(Field(CMsgSOCacheSubscribed::objects, TYPE_STRING, null_object.serialize()));
	}
}

void Inventory::ClearEquipState(ProtoWriter& object)
{
	auto object_data = object.getAll(SubscribedType::object_data);
	for (size_t j = 0; j < object_data.size(); j++)
	{
		ProtoWriter item(object_data[j].String(), 19);

		if (item.getAll(CSOEconItem::equipped_state).empty())
			continue;

		// create NOT equiped state for item 
		ProtoWriter null_equipped_state(2);
		null_equipped_state.replace(Field(CSOEconItemEquipped::new_class, TYPE_UINT32, (int64_t)0));
		null_equipped_state.replace(Field(CSOEconItemEquipped::new_slot, TYPE_UINT32, (int64_t)0));
		// unequip all 
		auto equipped_state = item.getAll(CSOEconItem::equipped_state);
		for (size_t k = 0; k < equipped_state.size(); k++)
			item.replace(Field(CSOEconItem::equipped_state, TYPE_STRING, null_equipped_state.serialize()), k);

		object.replace(Field(SubscribedType::object_data, TYPE_STRING, item.serialize()), j);
	}
}

static auto is_knife(const int i) -> bool
{
	return (i >= 500 && i < 5027) || i == 59 || i == 42;
}

static auto is_glove(const int i) -> bool
{
	return (i >= 5027 && i <= 5035);
}

void Inventory::AddAllItems(ProtoWriter& object)
{
	for (auto& x : g_InventorySkins)
		AddItem(object, x.first, x.second.wId, config->inventory.skinInfo[x.second.paintKit].rarity, x.second.paintKit, x.second.seed, x.second.wear, x.second.name);
}

bool is_uncommon(int index)
{
	switch (index)
	{
	case WEAPON_DEAGLE:
	case WEAPON_GLOCK:
	case WEAPON_AK47:
	case WEAPON_AWP:
	case WEAPON_M4A1:
	case WEAPON_M4A1_SILENCER:
	case WEAPON_HKP2000:
	case WEAPON_USP_SILENCER:
		return true;
	default:
		return false;
	}
}

void Inventory::AddItem(ProtoWriter& object, int index, int itemIndex, int rarity, int paintKit, int seed, float wear, std::string name)
{
	uint32_t steamid = memory->SteamUser->GetSteamID().GetAccountID();
	if (!steamid)
		return;
	ProtoWriter item(19);
	item.add(Field(CSOEconItem::id, TYPE_UINT64, (int64_t)index));
	item.add(Field(CSOEconItem::account_id, TYPE_UINT32, (int64_t)steamid));
	item.add(Field(CSOEconItem::def_index, TYPE_UINT32, (int64_t)itemIndex));
	item.add(Field(CSOEconItem::inventory, TYPE_UINT32, (int64_t)index));
	item.add(Field(CSOEconItem::origin, TYPE_UINT32, (int64_t)24));
	item.add(Field(CSOEconItem::quantity, TYPE_UINT32, (int64_t)1));
	item.add(Field(CSOEconItem::level, TYPE_UINT32, (int64_t)1));
	item.add(Field(CSOEconItem::style, TYPE_UINT32, (int64_t)0));
	item.add(Field(CSOEconItem::flags, TYPE_UINT32, (int64_t)0));
	item.add(Field(CSOEconItem::in_use, TYPE_BOOL, (int64_t)false));
	item.add(Field(CSOEconItem::original_id, TYPE_UINT64, (int64_t)itemIndex));
	if (is_uncommon(itemIndex))
		rarity++;
	if (is_knife(itemIndex) || is_glove(itemIndex))
	{
		item.add(Field(CSOEconItem::quality, TYPE_UINT32, (int64_t)3));
		rarity = 6;
	}
	item.add(Field(CSOEconItem::rarity, TYPE_UINT32, (int64_t)rarity));

	if (name.length() > 0)
		item.add(Field(CSOEconItem::custom_name, TYPE_STRING, name));
	// Paint Kit
	float _PaintKitAttributeValue = (float)paintKit;
	auto PaintKitAttributeValue = std::string{ reinterpret_cast<const char*>((void*)&_PaintKitAttributeValue), 4 };
	ProtoWriter PaintKitAttribute(3);
	PaintKitAttribute.add(Field(CSOEconItemAttribute::def_index, TYPE_UINT32, (int64_t)6));
	PaintKitAttribute.add(Field(CSOEconItemAttribute::value_bytes, TYPE_STRING, PaintKitAttributeValue));
	item.add(Field(CSOEconItem::attribute, TYPE_STRING, PaintKitAttribute.serialize()));

	// Paint Seed
	float _SeedAttributeValue = (float)seed;
	auto SeedAttributeValue = std::string{ reinterpret_cast<const char*>((void*)&_SeedAttributeValue), 4 };
	ProtoWriter SeedAttribute(3);
	SeedAttribute.add(Field(CSOEconItemAttribute::def_index, TYPE_UINT32, (int64_t)7));
	SeedAttribute.add(Field(CSOEconItemAttribute::value_bytes, TYPE_STRING, SeedAttributeValue));
	item.add(Field(CSOEconItem::attribute, TYPE_STRING, SeedAttribute.serialize()));

	// Paint Wear
	float _WearAttributeValue = wear;
	auto WearAttributeValue = std::string{ reinterpret_cast<const char*>((void*)&_WearAttributeValue), 4 };
	ProtoWriter WearAttribute(3);
	WearAttribute.add(Field(CSOEconItemAttribute::def_index, TYPE_UINT32, (int64_t)8));
	WearAttribute.add(Field(CSOEconItemAttribute::value_bytes, TYPE_STRING, WearAttributeValue));
	item.add(Field(CSOEconItem::attribute, TYPE_STRING, WearAttribute.serialize()));

	object.add(Field(SubscribedType::object_data, TYPE_STRING, item.serialize()));
}

bool Inventory::Presend(uint32_t& unMsgType, void* pubData, uint32_t& cubData)
{
	uint32_t MessageType = unMsgType & 0x7FFFFFFF;

	if (MessageType == k_EMsgGCAdjustItemEquippedState) {

		ProtoWriter msg((void*)((DWORD)pubData + 8), cubData - 8, 19);

		if (!msg.has(CMsgAdjustItemEquippedState::item_id)
			|| !msg.has(CMsgAdjustItemEquippedState::new_class)
			|| !msg.has(CMsgAdjustItemEquippedState::new_slot))
			return true;

		uint32_t item_id = msg.get(CMsgAdjustItemEquippedState::item_id).UInt32();
		uint32_t new_class = msg.get(CMsgAdjustItemEquippedState::new_class).UInt32();

		auto weapon = g_InventorySkins[item_id];

		if (new_class == 2)
		{
			for (auto& skins : g_InventorySkins)
			{
				if (Protobuffs::GetSlotID(skins.second.wId) == Protobuffs::GetSlotID(weapon.wId))
					skins.second.in_use_t = false;
			}

			g_InventorySkins[item_id].in_use_t = true;
			config->weaponsInv[weapon.wId].skins.t.enabled = true;
			config->weaponsInv[weapon.wId].skins.t.paintKit = weapon.paintKit;
			config->weaponsInv[weapon.wId].skins.t.wear = weapon.wear;
			config->weaponsInv[weapon.wId].skins.t.seed = weapon.seed;

			if (is_knife(weapon.wId))
			{
				config->weaponsInv[WEAPON_KNIFE].skins.t.enabled = true;
				config->weaponsInv[WEAPON_KNIFE].skins.t.definitionIndex = weapon.wId;
				config->weaponsInv[WEAPON_KNIFE].skins.t.paintKit = weapon.paintKit;
				config->weaponsInv[WEAPON_KNIFE].skins.t.wear = weapon.wear;
				config->weaponsInv[WEAPON_KNIFE].skins.t.seed = weapon.seed;
			}
			else if (is_glove(weapon.wId))
			{
				config->weaponsInv[GLOVE_T_SIDE].skins.t.enabled = true;
				config->weaponsInv[GLOVE_T_SIDE].skins.t.definitionIndex = weapon.wId;
				config->weaponsInv[GLOVE_T_SIDE].skins.t.paintKit = weapon.paintKit;
				config->weaponsInv[GLOVE_T_SIDE].skins.t.wear = weapon.wear;
				config->weaponsInv[GLOVE_T_SIDE].skins.t.seed = weapon.seed;
			}
		}
		else
		{
			for (auto& skins : g_InventorySkins)
			{
				if (Protobuffs::GetSlotID(skins.second.wId) == Protobuffs::GetSlotID(weapon.wId))
					skins.second.in_use_ct = false;
			}

			config->weaponsInv[weapon.wId].skins.ct.enabled = true;
			config->weaponsInv[weapon.wId].skins.ct.paintKit = weapon.paintKit;
			config->weaponsInv[weapon.wId].skins.ct.wear = weapon.wear;
			config->weaponsInv[weapon.wId].skins.ct.seed = weapon.seed;

			if (is_knife(weapon.wId))
			{
				config->weaponsInv[WEAPON_KNIFE].skins.ct.enabled = true;
				config->weaponsInv[WEAPON_KNIFE].skins.ct.definitionIndex = weapon.wId;
				config->weaponsInv[WEAPON_KNIFE].skins.ct.paintKit = weapon.paintKit;
				config->weaponsInv[WEAPON_KNIFE].skins.ct.wear = weapon.wear;
				config->weaponsInv[WEAPON_KNIFE].skins.ct.seed = weapon.seed;
			}
			else if (is_glove(weapon.wId))
			{
				config->weaponsInv[GLOVE_T_SIDE].skins.ct.enabled = true;
				config->weaponsInv[GLOVE_T_SIDE].skins.ct.definitionIndex = weapon.wId;
				config->weaponsInv[GLOVE_T_SIDE].skins.ct.paintKit = weapon.paintKit;
				config->weaponsInv[GLOVE_T_SIDE].skins.ct.wear = weapon.wear;
				config->weaponsInv[GLOVE_T_SIDE].skins.ct.seed = weapon.seed;
			}
		}

		memory->clientState->ForceFullUpdate();

		return false;
	}
	return true;
}
