#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct ClientClass;
struct ClientPOD;

struct Client : GameClass<Client, ClientPOD> {
    VIRTUAL_METHOD(ClientClass*, getAllClasses, 8, (), ())
    VIRTUAL_METHOD(bool, dispatchUserMessage, 38, (int messageType, int passthroughFlags, int size, const void* data), (messageType, passthroughFlags, size, data))
};

}
