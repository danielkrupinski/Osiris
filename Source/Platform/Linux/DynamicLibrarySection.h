#pragma once

#include <cstddef>
#include <span>

#include <elf.h>
#include <fcntl.h>
#include <link.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "DynamicLibraryView.h"

namespace linux_platform
{

template <typename DynamicLibraryWrapper>
[[nodiscard]] std::span<const std::byte> getCodeSection(DynamicLibraryView<DynamicLibraryWrapper> library)
{
    void* base = nullptr;
    std::size_t size = 0;

    const auto linkMap = library.getLinkMap();
    if (linkMap) {
        if (const auto fd = open(linkMap->l_name, O_RDONLY); fd >= 0) {
            if (struct stat st; fstat(fd, &st) == 0) {
                if (const auto map = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0); map != MAP_FAILED) {
                    const auto ehdr = (ElfW(Ehdr)*)map;
                    const auto shdrs = (ElfW(Shdr)*)(std::uintptr_t(ehdr) + ehdr->e_shoff);
                    const auto strTab = (const char*)(std::uintptr_t(ehdr) + shdrs[ehdr->e_shstrndx].sh_offset);

                    for (auto i = 0; i < ehdr->e_shnum; ++i) {
                        const auto shdr = (ElfW(Shdr)*)(std::uintptr_t(shdrs) + i * ehdr->e_shentsize);

                        if (std::strcmp(strTab + shdr->sh_name, ".text") != 0)
                            continue;

                        base = (void*)(linkMap->l_addr + shdr->sh_offset);
                        size = shdr->sh_size;
                        munmap(map, st.st_size);
                        close(fd);
                        break;
                    }
                    munmap(map, st.st_size);
                }
            }
            close(fd);
        }
    }
    return { reinterpret_cast<const std::byte*>(base), size };
}

}
