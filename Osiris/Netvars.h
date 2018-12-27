#pragma once

#include <unordered_map>

#include "SDK/Recv.h"

class Netvars final {
public:
    Netvars();
    //int getOffset(const std::string_view);
private:
    void loadTable(RecvTable* recvTable, std::size_t offset = 0);
    //std::pair<std::string_view, std::size_t> getProperties(RecvTable*);
    std::unordered_map<std::string_view, std::size_t> playerOffsets;
};

