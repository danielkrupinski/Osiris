#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

struct ClientClass;

class Client {
public:
    INCONSTRUCTIBLE(Client)

    VIRTUAL_METHOD(ClientClass*, getAllClasses, 8, (), (this))
    VIRTUAL_METHOD(bool, dispatchUserMessage, 38, (int messageType, int arg, int arg1, void* data), (this, messageType, arg, arg1, data))
};
