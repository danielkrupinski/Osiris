#pragma once

#include <MemoryPatterns/PatternTypes/FileSystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct FileSystemPatterns {
    [[nodiscard]] static consteval auto addSoundSystemPatterns(auto soundSystemPatterns) noexcept
    {
        return soundSystemPatterns
            .template addPattern<FileSystemPointer, CodePattern{"83 3D ? ? ? ? 00 75 ? 48 8D"}.add(2).abs(5)>();
    }

    [[nodiscard]] static consteval auto addFileSystemPatterns(auto fileSystemPatterns) noexcept
    {
        return fileSystemPatterns
            .template addPattern<FileNamesOffset, CodePattern{"? ? ? ? FF 15 ? ? ? ? 8B ? ? ? ? ? 83 E8"}.read()>();
    }
};
