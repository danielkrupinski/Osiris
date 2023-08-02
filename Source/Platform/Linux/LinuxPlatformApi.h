#pragma once

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct LinuxPlatformApi {
    static void* dlopen(const char* file, int mode) noexcept;
    static void* dlsym(void* handle, const char* name) noexcept;
    static int dlclose(void* handle) noexcept;
    static int dlinfo(void* handle, int request, void* info) noexcept;

    static int open(const char* pathname, int flags) noexcept;
    static int close(int fd) noexcept;
    static int fstat(int fd, struct stat* buf) noexcept;

    static void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) noexcept;
    static int munmap(void* addr, size_t length) noexcept;

    static void debugBreak() noexcept;

    static char* getenv(const char* name) noexcept;
};
