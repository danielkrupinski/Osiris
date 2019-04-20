#pragma once

#include "Utils.h"

struct ClientClass;

class Client final {
public:
    constexpr auto getAllClasses() noexcept
    {
        return callVirtualFunction<ClientClass*>(this, 8);
    }

    constexpr bool dispatchUserMessage(int messageType, int arg, int arg1, void* data) noexcept
    {
        return callVirtualFunction<bool, int, int, int, void*>(this, 38, messageType, arg, arg1, data);
    }
};
