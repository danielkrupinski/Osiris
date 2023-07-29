#pragma once

#include <Windows.h>

#include "Win.h"

class PebLdr {
public:
    explicit PebLdr(win::PebLdrData* ldr)
        : ldr{ ldr }
    {
    }

    [[nodiscard]] HMODULE getModuleHandle(const char* moduleName) const noexcept
    {
        const auto head = getInLoadOrderModuleListHead();
        for (auto currentLink = head->fLink; currentLink != head; currentLink = currentLink->fLink) {
            const auto& entry = *CONTAINING_RECORD(currentLink, win::LdrDataTableEntry, inLoadOrderLinks);

            if (entry.baseDllName.equalsCaseInsensitive(moduleName))
                return HMODULE(entry.dllBase);
        }
        return {};
    }

private:
    [[nodiscard]] win::ListEntry* getInLoadOrderModuleListHead() const noexcept
    {
        return &ldr->inLoadOrderModuleList;
    }

    win::PebLdrData* ldr;
};
