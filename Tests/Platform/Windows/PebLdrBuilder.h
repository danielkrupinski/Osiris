#pragma once

#include <cwchar>
#include <iterator>
#include <list>

#include <Platform/Windows/PebLdr.h>
#include <Platform/Windows/Win.h>

class PebLdrBuilder {
public:
    void addModule(void* dllBase, const wchar_t* baseDllName)
    {
        win::LdrDataTableEntry entry{};
        entry.dllBase = dllBase;
        entry.baseDllName.buffer = const_cast<wchar_t*>(baseDllName);
        entry.baseDllName.length = std::wcslen(baseDllName);
        entry.baseDllName.maximumLength = entry.baseDllName.length;
        entries.push_back(entry);
    }

    [[nodiscard]] PebLdr pebLdr()
    {
        buildModuleList();
        return PebLdr{ &pebLdrData };
    }

private:
    void buildModuleList()
    {
        for (auto it = entries.begin(); it != entries.end(); ++it) {
            it->inLoadOrderLinks.bLink = (it != entries.begin()) ? &std::prev(it)->inLoadOrderLinks : &pebLdrData.inLoadOrderModuleList;
            it->inLoadOrderLinks.fLink = (it != std::prev(entries.end())) ? &std::next(it)->inLoadOrderLinks : &pebLdrData.inLoadOrderModuleList;
        }

        pebLdrData.inLoadOrderModuleList.bLink = &std::prev(entries.end())->inLoadOrderLinks;
        pebLdrData.inLoadOrderModuleList.fLink = &entries.begin()->inLoadOrderLinks;
    }
    
    std::list<win::LdrDataTableEntry> entries;
    win::PebLdrData pebLdrData{};
};
