#pragma once 

#include <cmath>
#include <cstdint>

#include "IClientNetworkable.h"
#include "IClientRenderable.h"
#include "IClientThinkable.h"
#include "IClientUnknown.h"

struct SpatializationInfo_t;

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
    virtual void Release(void) = 0;
};

#pragma pack(push, 1)
class CCSWeaponInfo {
public:
    std::int8_t pad0[20];
    std::int32_t iMaxClip1;
    std::int8_t pad1[12];
    std::int32_t iMaxReservedAmmo;
    std::int8_t pad2[96];
    char* szHudName;
    char* szWeaponName;
    std::int8_t pad3[56];
    std::int32_t iWeaponType;
    std::int8_t pad4[4];
    std::int32_t iWeaponPrice;
    std::int32_t iKillAward;
    std::int8_t pad5[20];
    std::uint8_t bFullAuto;
    std::int8_t pad6[3];
    std::int32_t iDamage;
    std::float_t flArmorRatio;
    std::int32_t iBullets;
    std::float_t flPenetration;
    std::int8_t pad7[8];
    std::float_t flRange;
    std::float_t flRangeModifier;
    std::int8_t pad8[16];
    std::uint8_t bHasSilencer;
    std::int8_t pad9[15];
    std::float_t flSpread;
    std::float_t flSpreadAlt;
    std::int8_t pad10[76];
    std::int32_t iRecoilSeed;
    std::int8_t pad11[32];
};
#pragma pack(pop)

class IWeaponSystem
{
    virtual void unused0() = 0;
    virtual void unused1() = 0;
public:
    virtual const CCSWeaponInfo* GetWpnData(unsigned ItemDefinitionIndex) = 0;
};
