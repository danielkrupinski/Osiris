#pragma once

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace linux_platform
{

struct PlatformApi {
    void* dlopen(const char* file, int mode) const noexcept
    {
        return ::dlopen(file, mode);
    }

    void* dlsym(void* handle, const char* name) const noexcept
    {
        return ::dlsym(handle, name);
    }

    int dlclose(void* handle) const noexcept
    {
        return ::dlclose(handle);
    }

    int dlinfo(void* handle, int request, void* info) const noexcept
    {
        return ::dlinfo(handle, request, info);
    }

    int open(const char* pathname, int flags) const noexcept
    {
        return ::open(pathname, flags);
    }

    int close(int fd) const noexcept
    {
        return ::close(fd);
    }

    int fstat(int fd, struct stat* buf) const noexcept
    {
        return ::fstat(fd, buf);
    }

    void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) const noexcept
    {
        return ::mmap(addr, length, prot, flags, fd, offset);
    }

    int munmap(void* addr, size_t length) const noexcept
    {
        return ::munmap(addr, length);
    }
};

}
