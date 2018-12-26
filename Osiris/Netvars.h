#pragma once

#include <unordered_map>

#include "SDK/Recv.h"

class Netvars final {
public:
    Netvars();
    int getOffset(const std::string_view, const std::string_view);
private:
    std::unordered_map<std::string, RecvTable*> recvTables;
};
