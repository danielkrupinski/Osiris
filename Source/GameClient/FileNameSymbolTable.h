#pragma once

#include <span>

#include <CS2/Classes/CUtlFilenameSymbolTable.h>

template <typename HookContext>
class FileNameSymbolTable {
public:
    explicit FileNameSymbolTable(HookContext& hookContext, cs2::CUtlFilenameSymbolTable* thisptr) noexcept
        : hookContext{hookContext}
        , thisptr{thisptr}
    {
    }

    explicit operator bool() const noexcept
    {
        return thisptr != nullptr;
    }

    void getString(cs2::FileNameHandle_t handle, std::span<char> buffer) const noexcept
    {
        if (thisptr && deps().string)
            deps().string(thisptr, &handle, buffer.data(), static_cast<int>(buffer.size()));
    }

private:
    [[nodiscard]] const auto& deps() const noexcept
    {
        return hookContext.fileNameSymbolTableState();
    }

    HookContext& hookContext;
    cs2::CUtlFilenameSymbolTable* thisptr;
};
