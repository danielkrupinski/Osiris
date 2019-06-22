#pragma once

struct SoundInfo {
    std::byte pad[40];
    float volume;
    std::byte pad1[12];
    int	entityIndex;
    std::byte pad2[12];
    int soundIndex;
};
