#pragma once

#include "Utils.h"

struct ClientClass;

class Client final {
public:
    constexpr auto getAllClasses() noexcept
    {
        return callVirtualFunction<ClientClass*(__thiscall*)(void*)>(this, 8)(this);
    }

    constexpr bool dispatchUserMessage(int messageType, int arg, int arg1, void* data) noexcept
    {
        return callVirtualFunction<bool(__thiscall*)(void*, int, int, int, void*)>(this, 38)(this, messageType, arg, arg1, data);
    }
};
