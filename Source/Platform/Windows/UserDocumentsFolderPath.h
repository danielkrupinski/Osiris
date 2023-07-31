#pragma once

#include <memory>
#include <ShlObj_core.h>

#include "CoTaskMemDeleter.h"
#include "DLLs/Shell32Dll.h"

struct UserDocumentsFolderPath {
    [[nodiscard]] const wchar_t* get() const noexcept
    {
        return path.get() ? path.get() : L"";
    }

private:
    using PathPointer = std::unique_ptr<wchar_t[], CoTaskMemDeleter>;

    [[nodiscard]] static PathPointer getUserDocumentsFolderPath() noexcept
    {
        wchar_t* path = nullptr;
        if (const auto getKnownFolderPath = Shell32Dll{}.SHGetKnownFolderPath())
            getKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path);
        return PathPointer{ path };
    }

    PathPointer path = getUserDocumentsFolderPath();
};
