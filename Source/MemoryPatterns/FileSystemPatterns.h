#pragma once

#include <CS2/Classes/FileSystem.h>
#include <GameClasses/OffsetTypes/FileSystemOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct FileSystemPatterns {
    [[nodiscard]] cs2::CBaseFileSystem** fileSystem() const noexcept;
    [[nodiscard]] FileNamesOffset fileNamesOffset() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& soundSystemPatternFinder;
    const PatternFinder<PatternNotFoundLogger>& fileSystemPatternFinder;
};
