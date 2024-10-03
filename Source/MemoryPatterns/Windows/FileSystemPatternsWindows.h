#pragma once

#include <GameClasses/OffsetTypes/FileSystemOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct FileSystemPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CBaseFileSystem** fileSystem() const noexcept
    {
        // todo: find better pattern (from filesystem dll?)
        return patternFinders.soundSystemPatternFinder("48 8B 57 10 48 83 C2 28 39 ? 74 ? 48 8B 0D ? ? ? ? 4C 8D 44 24 ? 48 8B 01 FF 90 ? ? ? ? 8B 44 24"_pat).add(15).abs().template as<cs2::CBaseFileSystem**>();
    }

    [[nodiscard]] FileNamesOffset fileNamesOffset() const noexcept
    {
        return patternFinders.fileSystemPatternFinder("E8 ? ? ? ? 48 8D 8B ? ? ? ? FF 15 ? ? ? ? 8B"_pat).add(8).template readOffset<FileNamesOffset>();
    }
};
