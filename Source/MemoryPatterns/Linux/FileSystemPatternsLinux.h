#pragma once

#include <MemoryPatterns/PatternTypes/FileSystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct FileSystemPatterns {
    [[nodiscard]] static consteval auto addSoundSystemPatterns(auto soundSystemPatterns) noexcept
    {
        return soundSystemPatterns
            .template addPattern<FileSystemPointer, CodePattern{"48 8D 05 ? ? ? ? 48 89 DA 4C 8B 00 49"}.add(3).abs()>();
    }

    [[nodiscard]] static consteval auto addFileSystemPatterns(auto fileSystemPatterns) noexcept
    {
        return fileSystemPatterns
            .template addPattern<FileNamesOffset, CodePattern{"48 8D BB ? ? ? ? E8 ? ? ? ? 8B 83 ? ? ? ? 89 C2"}.add(3).read()>();
    }
};
