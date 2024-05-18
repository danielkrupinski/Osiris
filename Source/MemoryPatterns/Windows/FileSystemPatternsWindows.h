#pragma once

#include <GameClasses/OffsetTypes/FileSystemOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct FileSystemPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CBaseFileSystem** fileSystem() const noexcept
    {
        return patternFinders.soundSystemPatternFinder("22 74 ? 48 8B 0D ? ? ? ? 4C 8D 44"_pat).add(6).abs().template as<cs2::CBaseFileSystem**>();
    }

    [[nodiscard]] FileNamesOffset fileNamesOffset() const noexcept
    {
        return patternFinders.fileSystemPatternFinder("E8 ? ? ? ? 48 8D 8B ? ? ? ? FF 15"_pat).add(8).template readOffset<FileNamesOffset>();
    }
};
