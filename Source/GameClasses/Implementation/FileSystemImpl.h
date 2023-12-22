#pragma once

#include <MemoryPatterns/FileSystemPatterns.h>

struct FileSystemImpl {
    explicit FileSystemImpl(const FileSystemPatterns& fileSystemPatterns) noexcept
        : fileNamesOffset{fileSystemPatterns.fileNamesOffset()}
    {
    }

    [[nodiscard]] static const FileSystemImpl& instance() noexcept;

    FileNamesOffset fileNamesOffset;
};
