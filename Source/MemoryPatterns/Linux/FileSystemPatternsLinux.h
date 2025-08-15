#pragma once

#include <MemoryPatterns/PatternTypes/FileSystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct FileSystemPatterns {
    [[nodiscard]] static consteval auto addSoundSystemPatterns(auto soundSystemPatterns) noexcept
    {
        return soundSystemPatterns
            .template addPattern<FileSystemPointer, CodePattern{"53 48 81 EC ? ? ? ? 48 ? ? ? ? ? ? 48 89 85 ? ? ? ? 48 8D"}.add(11).abs()>();
    }

    [[nodiscard]] static consteval auto addFileSystemPatterns(auto fileSystemPatterns) noexcept
    {
        return fileSystemPatterns
            .template addPattern<FileNamesOffset, CodePattern{"BC 24 ? ? ? ? E8 ? ? ? ? 41 8B"}.add(2).read()>();
    }
};
