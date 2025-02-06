#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>

#include <dlfcn.h>
#include <elf.h>
#include <fcntl.h>
#include <link.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <Platform/Macros/FunctionAttributes.h>
#include <Utils/GenericPointer.h>
#include <Utils/MemorySection.h>

#include "LinuxPlatformApi.h"
#include "LinuxVmtFinderParams.h"

class LinuxDynamicLibrary {
public:
    LinuxDynamicLibrary(const char* libraryName)
        : handle{ getModuleHandle(libraryName) } {}

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return handle != nullptr;
    }

    [[nodiscard]] GenericPointer getFunctionAddress(const char* functionName) const noexcept
    {
        if (handle)
            return LinuxPlatformApi::dlsym(handle, functionName);
        return {};
    }

    [[nodiscard]] link_map* getLinkMap() const noexcept
    {
        link_map* map = nullptr;
        if (handle)
            LinuxPlatformApi::dlinfo(handle, RTLD_DI_LINKMAP, &map);
        return map;
    }

    [[nodiscard]] MemorySection getCodeSection() const noexcept
    {
        return getSection(".text");
    }

    [[nodiscard]] MemorySection getVmtSection() const noexcept
    {
        return getSection(".data.rel.ro");
    }

    [[nodiscard]] LinuxVmtFinderParams getVmtFinderParams() const noexcept
    {
        return {getSection(".rodata"), getSection(".data.rel.ro")};
    }

private:
    [[nodiscard]] [[NOINLINE]] MemorySection getSection(const char* sectionName) const noexcept
    {
        void* base = nullptr;
        std::size_t size = 0;

        const auto linkMap = getLinkMap();
        if (linkMap) {
            if (const auto fd = LinuxPlatformApi::open(linkMap->l_name, O_RDONLY); fd >= 0) {
                if (struct stat st; LinuxPlatformApi::fstat(fd, &st) == 0) {
                    if (const auto map = LinuxPlatformApi::mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0); map != MAP_FAILED) {
                        const auto ehdr = (ElfW(Ehdr)*)map;
                        const auto shdrs = (ElfW(Shdr)*)(std::uintptr_t(ehdr) + ehdr->e_shoff);
                        const auto strTab = (const char*)(std::uintptr_t(ehdr) + shdrs[ehdr->e_shstrndx].sh_offset);

                        for (auto i = 0; i < ehdr->e_shnum; ++i) {
                            const auto shdr = (ElfW(Shdr)*)(std::uintptr_t(shdrs) + i * ehdr->e_shentsize);

                            if (std::strcmp(strTab + shdr->sh_name, sectionName) != 0)
                                continue;

                            base = (void*)(linkMap->l_addr + shdr->sh_addr);
                            size = shdr->sh_size;
                            LinuxPlatformApi::munmap(map, st.st_size);
                            LinuxPlatformApi::close(fd);
                            break;
                        }
                        LinuxPlatformApi::munmap(map, st.st_size);
                    }
                }
                LinuxPlatformApi::close(fd);
            }
        }
        return MemorySection{ std::span{ reinterpret_cast<const std::byte*>(base), size } };
    }

    [[nodiscard]] void* getModuleHandle(const char* libraryName)
    {
        const auto handle = LinuxPlatformApi::dlopen(libraryName, RTLD_LAZY | RTLD_NOLOAD);
        if (handle)
            LinuxPlatformApi::dlclose(handle);
        return handle;
    }

    void* handle = nullptr;
};
