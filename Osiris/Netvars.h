#pragma once

#include <string_view>
#include <unordered_map>

struct RecvTable;

class Netvars final {
public:
    Netvars() noexcept;

    auto operator[](const char* netvar) const noexcept
    {
        return offsets[netvar];
    }

    void restore() noexcept;
private:
    void loadTable(RecvTable*, const std::size_t = 0) noexcept;
    void unloadTable(RecvTable*) noexcept;
    mutable std::unordered_map<std::string_view, std::size_t> offsets;
};

extern Netvars netvars;
