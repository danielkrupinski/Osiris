#pragma once

#include "VirtualMethod.h"

struct ClientClass;

namespace csgo::pod { struct Client; }

class Client : public VirtualCallableFromPOD<Client, csgo::pod::Client> {
public:
    VIRTUAL_METHOD(ClientClass*, getAllClasses, 8, (), ())
    VIRTUAL_METHOD(bool, dispatchUserMessage, 38, (int messageType, int passthroughFlags, int size, const void* data), (messageType, passthroughFlags, size, data))
};
