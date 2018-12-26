#pragma once

#include "ClientClass.h"
#include "Utils.h"

class Client final {
public:
    ClientClass* getAllClasses()
    {
        return callVirtualFunction<ClientClass*(__thiscall*)()>(this, 8)();
    }
};
