#pragma once

struct NetworkChannel {
    std::byte pad[44];
    int chokedPackets;
};
