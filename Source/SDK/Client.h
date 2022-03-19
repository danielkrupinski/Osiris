#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

struct ClientClass;

class Client {
public:
    INCONSTRUCTIBLE(Client)

    VIRTUAL_METHOD(ClientClass*, getAllClasses, 8, (), (this))
    VIRTUAL_METHOD(bool, dispatchUserMessage, 38, (int messageType, int passthroughFlags, int size, const void* data), (this, messageType, passthroughFlags, size, data))
};
