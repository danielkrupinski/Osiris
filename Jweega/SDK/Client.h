#pragma once

#include "Utils.h"

struct ClientClass;

class Client {
public:
    constexpr auto getAllClasses() noexcept
    {
        return callVirtualMethod<ClientClass*>(this, 8);
    }

    constexpr bool dispatchUserMessage(int messageType, int arg, int arg1, void* data) noexcept
    {
        return callVirtualMethod<bool, int, int, int, void*>(this, 38, messageType, arg, arg1, data);
    }
};
