#pragma once

#include <GameClasses/OffsetTypes/FileSystemOffset.h>

struct FileSystemDeps {
    FileSystemDeps() = default;

    template <typename FileSystemPatterns>
    explicit FileSystemDeps(const FileSystemPatterns& fileSystemPatterns) noexcept
        : fileNamesOffset{fileSystemPatterns.fileNamesOffset()}
    {
    }

    FileNamesOffset fileNamesOffset;
};
