#pragma once

#include "Utils.h"

struct ClientClass;

class Client final {
public:
    auto getAllClasses()
    {
        return callVirtualFunction<ClientClass*(__thiscall*)()>(this, 8)();
    }
};
