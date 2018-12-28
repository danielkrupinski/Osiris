#pragma once

#include <string_view>
#include <unordered_map>

#include "SDK/Recv.h"

class Netvars final {
public:
    Netvars();
    std::size_t getOffset(const std::string_view);
private:
    void loadTable(RecvTable*, std::size_t = 0);
    std::unordered_map<std::string_view, std::size_t> playerOffsets;
};

extern Netvars netvars;
