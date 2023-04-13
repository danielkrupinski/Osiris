#pragma once

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct LinuxPlatformApi {
    static void* dlopen(const char* file, int mode) noexcept
    {
        return ::dlopen(file, mode);
    }

    static void* dlsym(void* handle, const char* name) noexcept
    {
        return ::dlsym(handle, name);
    }

    static int dlclose(void* handle) noexcept
    {
        return ::dlclose(handle);
    }

    static int dlinfo(void* handle, int request, void* info) noexcept
    {
        return ::dlinfo(handle, request, info);
    }

    static int open(const char* pathname, int flags) noexcept
    {
        return ::open(pathname, flags);
    }

    static int close(int fd) noexcept
    {
        return ::close(fd);
    }

    static int fstat(int fd, struct stat* buf) noexcept
    {
        return ::fstat(fd, buf);
    }

    static void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) noexcept
    {
        return ::mmap(addr, length, prot, flags, fd, offset);
    }

    static int munmap(void* addr, size_t length) noexcept
    {
        return ::munmap(addr, length);
    }
};
