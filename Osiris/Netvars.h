#pragma once

#include <unordered_map>

#include "SDK/Recv.h"

class Netvars final {
public:
    Netvars();
private:
    void loadTable(RecvTable*, std::size_t = 0);
    std::unordered_map<std::string_view, std::size_t> playerOffsets;
};

