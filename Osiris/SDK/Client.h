#pragma once

#include "Utils.h"

struct ClientClass;

class Client final {
public:
    constexpr auto getAllClasses() noexcept
    {
        return callVirtualFunction<ClientClass*(__thiscall*)()>(this, 8)();
    }
};
