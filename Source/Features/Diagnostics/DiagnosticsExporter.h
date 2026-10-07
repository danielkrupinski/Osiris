#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <cwchar>
#include <string_view>

#include <BuildConfig.h>
#include <Features/Diagnostics/DiagnosticsConfigVariables.h>
#include <MemoryPatterns/AllMemoryPatternSearchResults.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Platform/PlatformPath.h>
#include <Utils/StringBuilder.h>

#if IS_WIN64()
#include <Platform/Windows/FileSystem/WindowsFileSystem.h>
#elif IS_LINUX()
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Platform/Linux/LinuxPlatformApi.h>
#endif

namespace diagnostics
{

[[nodiscard]] constexpr const char* status(bool value) noexcept
{
    return value ? "OK" : "FAILED";
}

template <typename HookContext>
class DiagnosticsExporter {
public:
    explicit DiagnosticsExporter(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool exportToFile() const noexcept
    {
        StringBuilderStorage<2048> storage;
        auto diagnostics = storage.builder();
        appendText(diagnostics);
        const auto path = buildPath();
        if (!path)
            return false;

#if IS_WIN64()
        WindowsFileSystem::createDirectory(hookContext.osirisDirectoryPath().get());
        const auto handle = WindowsFileSystem::createFileForOverwrite(path);
        if (handle == INVALID_HANDLE_VALUE)
            return false;
        const auto* data = diagnostics.cstring();
        const auto size = std::strlen(data);
        const auto written = WindowsFileSystem::writeFile(handle, 0, const_cast<char*>(data), size);
        WindowsSyscalls::NtClose(handle);
        return written == size;
#elif IS_LINUX()
        const auto fd = LinuxPlatformApi::open(path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC | O_NOFOLLOW, 0600);
        if (fd < 0)
            return false;
        const auto* data = diagnostics.cstring();
        const auto size = std::strlen(data);
        const auto written = LinuxPlatformApi::write(fd, data, size);
        LinuxPlatformApi::close(fd);
        return written == static_cast<ssize_t>(size);
#endif
    }

    void appendText(StringBuilder& builder) const noexcept
    {
        const auto& patterns = hookContext.patternSearchResults();

        builder.put("Osiris version: ", build::kVersion, '\n');
        builder.put("CS2 client: runtime module loaded\n");
        builder.put("CS2 build: exact game build is not exposed by the client module\n");
        builder.put("Safe mode: ", status(hookContext.config().template getVariable<diagnostics_vars::SafeModeEnabled>()), '\n');
        builder.put("Pattern groups:\n");
        builder.put("  client: ", status(patterns.clientPatternsValid()), '\n');
        builder.put("  scene_system: ", status(patterns.sceneSystemPatternsValid()), '\n');
        builder.put("  tier0: ", status(patterns.tier0PatternsValid()), '\n');
        builder.put("  filesystem: ", status(patterns.fileSystemPatternsValid()), '\n');
        builder.put("  soundsystem: ", status(patterns.soundSystemPatternsValid()), '\n');
        builder.put("  panorama: ", status(patterns.panoramaPatternsValid()), '\n');
        builder.put("  all: ", status(patterns.isValid()), '\n');
        builder.put("Known schema offsets:\n");
        builder.put("  radar entity spotted state: 0x1E88\n");
        builder.put("  radar spotted flag: 0x08\n");
        builder.put("Note: dynamic offsets are validated through the pattern groups above.\n");
    }

private:
    [[nodiscard]] auto buildPath() const noexcept
    {
        constexpr auto suffix = WIN64_LINUX(std::wstring_view{L"\\diagnostics.txt"}, std::string_view{"/diagnostics.txt"});
        constexpr std::size_t maxPathLength{512};
        auto basePath = hookContext.osirisDirectoryPath().get();
        if (!basePath)
            return static_cast<platform::PathCharType*>(nullptr);

        const auto baseLength = WIN64_LINUX(std::wcslen, std::strlen)(basePath);
        if (baseLength + suffix.length() + 1 > maxPathLength)
            return static_cast<platform::PathCharType*>(nullptr);

        static platform::PathCharType path[maxPathLength];
        std::ranges::copy_n(basePath, baseLength, path);
        std::ranges::copy(suffix, path + baseLength);
        path[baseLength + suffix.length()] = 0;
        return path;
    }

    HookContext& hookContext;
};

}
