#include "BaseEntity.h"

int* BaseEntity::getFlags()
{
    return reinterpret_cast<int*>(this + 0x100);
}

int BaseEntity::getHealth()
{
    return *reinterpret_cast<int*>(this + 0xFC);
}

Vector BaseEntity::getVelocity()
{
    return *reinterpret_cast<Vector*>(this + 0x110);
}

int BaseEntity::getMoveType()
{
    return *reinterpret_cast<int*>(this + 0x258);
}

int BaseEntity::getTeamNumber()
{
    return *reinterpret_cast<int*>(this + 0xF0);
}

bool BaseEntity::isScoped()
{
    return *reinterpret_cast<bool*>(this + 0x38AA);
}

int BaseEntity::getCrosshairID()
{
    return *reinterpret_cast<int*>(this + 0xB2DC);
}

bool BaseEntity::isAlive()
{
    return (*reinterpret_cast<int*>(this + 0x25B)) == 0;
}

ClientRenderable* BaseEntity::getRenderable()
{
    return reinterpret_cast<ClientRenderable*>(this + 0x4);
}

void BaseEntity::drawModel(int flags, char alpha)
{
    return callVirtualFunction<void(__thiscall*)(void*, int, char)>(getRenderable(), 9)(getRenderable(), flags, alpha);
}