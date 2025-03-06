#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string_view>

#include <MemoryAllocation/UniquePtr.h>
#include <Platform/PlatformPath.h>

#if IS_WIN64()
#include <Platform/Windows/DLLs/Shell32Dll.h>
#include <Platform/Windows/CoTaskMemDeleter.h>
#include <Utils/Wcslen.h>
#elif IS_LINUX()
#include <Platform/Linux/LinuxPlatformApi.h>
#endif

class OsirisDirectoryPath {
public:
    OsirisDirectoryPath() noexcept
    {
#if IS_WIN64()
        wchar_t* appDataPathRaw = nullptr;
        const auto getKnownFolderPath = Shell32Dll{}.SHGetKnownFolderPath();
        if (!getKnownFolderPath)
            return;
        const auto gotAppDataPath = getKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appDataPathRaw);
        std::unique_ptr<wchar_t[], CoTaskMemDeleter> appDataPathMemory{appDataPathRaw};
        if (gotAppDataPath != S_OK)
            return;

        const std::wstring_view appDataPath{appDataPathRaw, utils::wcslen(appDataPathRaw)};
        constexpr std::wstring_view ntPathPrefix{L"\\??\\"};
        constexpr auto kPathSeparatorLength{1};
        constexpr auto kNullTerminatorLength{1};

        pathString = mem::makeUniqueForOverwrite<wchar_t[]>(ntPathPrefix.length() + appDataPath.length() + kPathSeparatorLength + build::kOsirisDirectoryName.length() + kNullTerminatorLength);
        if (!pathString)
            return;

        std::size_t writeIndex{0};
        std::ranges::copy(ntPathPrefix, pathString.get() + writeIndex);
        writeIndex += ntPathPrefix.length();
        std::ranges::copy(appDataPath, pathString.get() + writeIndex);
        writeIndex += appDataPath.length();
        pathString.get()[writeIndex++] = L'\\';
        std::ranges::copy(build::kOsirisDirectoryName, pathString.get() + writeIndex);
        writeIndex += build::kOsirisDirectoryName.length();
        pathString.get()[writeIndex++] = L'\0';
#elif IS_LINUX()
        const auto home = LinuxPlatformApi::getenv("HOME");
        if (!home)
            return;

        const std::string_view homePath{home};
        constexpr auto kPathSeparatorLength{1};
        constexpr auto kNullTerminatorLength{1};
        pathString = mem::makeUniqueForOverwrite<char[]>(homePath.length() + kPathSeparatorLength + build::kOsirisDirectoryName.length() + kNullTerminatorLength);
        if (!pathString)
            return;

        std::size_t writeIndex{0};
        std::ranges::copy(homePath, pathString.get());
        writeIndex += homePath.length();
        pathString.get()[writeIndex++] = '/';
        std::ranges::copy(build::kOsirisDirectoryName, pathString.get() + writeIndex);
        writeIndex += build::kOsirisDirectoryName.length();
        pathString.get()[writeIndex++] = '\0';
#endif
    }

    [[nodiscard]] platform::PathCharType* get() const noexcept
    {
        return pathString.get();
    }

private:
    UniquePtr<platform::PathCharType[]> pathString;
};
