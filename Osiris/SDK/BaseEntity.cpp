#include "BaseEntity.h"
#include "../Interfaces.h"
#include "../Memory.h"

int* BaseEntity::getFlags()
{
    return reinterpret_cast<int*>(this + 0x104);
}

int BaseEntity::getMoveType()
{
    return *reinterpret_cast<int*>(this + 0x25C);
}

int BaseEntity::getTeamNumber()
{
    return *reinterpret_cast<int*>(this + 0xF4);
}

bool BaseEntity::isScoped()
{
    return *reinterpret_cast<bool*>(this + 0x390A);
}

int BaseEntity::getCrosshairID()
{
    return *reinterpret_cast<int*>(this + 0xB390);
}

bool BaseEntity::isAlive()
{
    return (*reinterpret_cast<int*>(this + 0x25F)) == 0;
}

/*
ClientRenderable* BaseEntity::getRenderable()
{
    return reinterpret_cast<ClientRenderable*>(this + 0x4);
}
*/
void BaseEntity::drawModel(int flags, unsigned char alpha)
{
    return callVirtualFunction<void(__thiscall*)(void*, int, unsigned char)>(this + 4, 9)(this + 4, flags, alpha);
}

bool BaseEntity::isEnemy()
{
    static auto gameMode = interfaces.cvar->findVar("game_type");

    if (gameMode->getInt() == 6 && getTeamNumber() == (*memory.localPlayer)->getTeamNumber() || getTeamNumber() != (*memory.localPlayer)->getTeamNumber())
        return true;
    else
        return false;
}

bool BaseEntity::isDormant()
{
	return callVirtualFunction<bool(__thiscall*)(void*)>(this + 8, 9)(this + 8);
}
