#pragma once

#include "ClientEntity.h"
#include "Vector.h"

class BaseEntity : public ClientEntity {
public:
    int* getFlags();
    int getHealth();
    Vector getVelocity();
    int getMoveType();
};
