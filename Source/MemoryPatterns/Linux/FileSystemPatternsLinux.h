#pragma once

#include <GameClasses/OffsetTypes/FileSystemOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct FileSystemPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CBaseFileSystem** fileSystem() const noexcept
    {
        return patternFinders.soundSystemPatternFinder("48 8D 05 ? ? ? ? 48 89 DA 4C 8B 00 49"_pat).add(3).abs().template as<cs2::CBaseFileSystem**>();
    }

    [[nodiscard]] FileNamesOffset fileNamesOffset() const noexcept
    {
        return patternFinders.fileSystemPatternFinder("48 8D BB ? ? ? ? E8 ? ? ? ? 8B 83 ? ? ? ? 89 C2"_pat).add(3).template readOffset<FileNamesOffset>();
    }
};
