#pragma once

#include "VirtualMethod.h"

struct ClientClass;

class Client : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD2(ClientClass*, getAllClasses, 8, (), ())
    VIRTUAL_METHOD2(bool, dispatchUserMessage, 38, (int messageType, int passthroughFlags, int size, const void* data), (messageType, passthroughFlags, size, data))
};
