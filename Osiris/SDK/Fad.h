#pragma once
#include <minwindef.h>

template <class T>
static T GetFunction(void* instance, int index)
{
	const auto vtable = *static_cast<void***>(instance);
	return reinterpret_cast<T>(vtable[index]);
}

class CSWeaponInfo
{
public:
	char _0x0000[20];
	__int32 max_clip;			//0x0014 
	char _0x0018[12];
	__int32 max_reserved_ammo;	//0x0024 
	char _0x0028[96];
	char* hud_name;				//0x0088 
	char* weapon_name;			//0x008C 
	char _0x0090[60];
	__int32 m_WeaponType;				//0x00CC 
	__int32 price;				//0x00D0 
	__int32 reward;				//0x00D4 
	char _0x00D8[20];
	BYTE full_auto;				//0x00EC 
	char _0x00ED[3];
	__int32 m_Damage;				//0x00F0 
	float m_ArmorRatio;			//0x00F4 
	__int32 bullets;			//0x00F8 
	float m_Penetration;			//0x00FC 
	char _0x0100[8];
	float m_Range;				//0x0108 
	float m_RangeModifier;		//0x010C 
	char _0x0110[16];
	BYTE m_HasSilencer;				//0x0120 
	char _0x0121[15];
	float max_speed;			//0x0130 
	float max_speed_alt;		//0x0134 
	char _0x0138[76];
	__int32 recoil_seed;		//0x0184 
	char _0x0188[32];
};