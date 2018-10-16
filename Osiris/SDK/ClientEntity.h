#pragma once

#include "ClientNetworkable.h"
#include "ClientRenderable.h"
#include "ClientThinkable.h"
#include "ClientUnknown.h"

class ClientEntity : public ClientUnknown, public ClientRenderable, public ClientNetworkable, public ClientThinkable {
public:
    virtual void Release(void) = 0;
};
