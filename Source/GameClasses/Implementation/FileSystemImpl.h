#pragma once

#include <GameClasses/OffsetTypes/FileSystemOffset.h>

struct FileSystemImpl {
    template <typename FileSystemPatterns>
    explicit FileSystemImpl(const FileSystemPatterns& fileSystemPatterns) noexcept
        : fileNamesOffset{fileSystemPatterns.fileNamesOffset()}
    {
    }

    FileNamesOffset fileNamesOffset;
};
