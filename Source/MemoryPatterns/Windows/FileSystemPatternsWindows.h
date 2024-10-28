#pragma once

#include <MemoryPatterns/PatternTypes/FileSystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct FileSystemPatterns {
    [[nodiscard]] static consteval auto addSoundSystemPatterns(auto soundSystemPatterns) noexcept
    {
        return soundSystemPatterns
            .template addPattern<FileSystemPointer, CodePattern{"48 8B 57 10 48 83 C2 28 39 ? 74 ? 48 8B 0D ? ? ? ? 4C 8D 44 24 ? 48 8B 01 FF 90 ? ? ? ? 8B 44 24"}.add(15).abs()>();
    }

    [[nodiscard]] static consteval auto addFileSystemPatterns(auto fileSystemPatterns) noexcept
    {
        return fileSystemPatterns
            .template addPattern<FileNamesOffset, CodePattern{"E8 ? ? ? ? 48 8D 8B ? ? ? ? FF 15 ? ? ? ? 8B"}.add(8).read()>();
    }
};
