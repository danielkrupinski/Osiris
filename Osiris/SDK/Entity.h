#pragma once

#include "ClientClass.h"
#include "Vector.h"

class Entity {
public:
    int* getFlags();
    int getMoveType();
    int getTeamNumber();
    bool isScoped();
    int getCrosshairID();
    void drawModel(int flags, unsigned char alpha);
    bool isEnemy();
	bool isDormant();
	bool isWeapon();
    ClientClass* getClientClass();
    bool isAlive();
};
