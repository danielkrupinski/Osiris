#pragma once

#include <cstddef>
#include <cstring>
#include <span>

#include <dlfcn.h>
#include <elf.h>
#include <fcntl.h>
#include <link.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <Utils/SafeAddress.h>

namespace linux_platform
{

template <typename PlatformApi>
class SharedObject {
public:
    SharedObject(const char* libraryName)
        : handle{ getModuleHandle(libraryName) } {}

    [[nodiscard]] bool isValid() const noexcept
    {
        return handle != nullptr;
    }

    [[nodiscard]] SafeAddress getFunctionAddress(const char* functionName) const noexcept
    {
        return SafeAddress{ std::uintptr_t(PlatformApi::dlsym(handle, functionName)) };
    }

    [[nodiscard]] link_map* getLinkMap() const noexcept
    {
        link_map* map = nullptr;
        PlatformApi::dlinfo(handle, RTLD_DI_LINKMAP, &map);
        return map;
    }

    [[nodiscard]] std::span<const std::byte> getCodeSection() const noexcept
    {
        void* base = nullptr;
        std::size_t size = 0;

        const auto linkMap = getLinkMap();
        if (linkMap) {
            if (const auto fd = PlatformApi::open(linkMap->l_name, O_RDONLY); fd >= 0) {
                if (struct stat st; PlatformApi::fstat(fd, &st) == 0) {
                    if (const auto map = PlatformApi::mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0); map != MAP_FAILED) {
                        const auto ehdr = (ElfW(Ehdr)*)map;
                        const auto shdrs = (ElfW(Shdr)*)(std::uintptr_t(ehdr) + ehdr->e_shoff);
                        const auto strTab = (const char*)(std::uintptr_t(ehdr) + shdrs[ehdr->e_shstrndx].sh_offset);

                        for (auto i = 0; i < ehdr->e_shnum; ++i) {
                            const auto shdr = (ElfW(Shdr)*)(std::uintptr_t(shdrs) + i * ehdr->e_shentsize);

                            if (std::strcmp(strTab + shdr->sh_name, ".text") != 0)
                                continue;

                            base = (void*)(linkMap->l_addr + shdr->sh_offset);
                            size = shdr->sh_size;
                            PlatformApi::munmap(map, st.st_size);
                            PlatformApi::close(fd);
                            break;
                        }
                        PlatformApi::munmap(map, st.st_size);
                    }
                }
                PlatformApi::close(fd);
            }
        }
        return { reinterpret_cast<const std::byte*>(base), size };
    }

private:
    [[nodiscard]] void* getModuleHandle(const char* libraryName)
    {
        const auto handle = PlatformApi::dlopen(libraryName, RTLD_LAZY | RTLD_NOLOAD);
        if (handle)
            PlatformApi::dlclose(handle);
        return handle;
    }

    void* handle = nullptr;
};

}
