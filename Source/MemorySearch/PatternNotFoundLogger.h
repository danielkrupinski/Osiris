#pragma once

#include <array>
#include <cassert>
#include <cstddef>

#include <MemorySearch/BytePattern.h>
#include <Platform/Macros/IsPlatform.h>
#include <Platform/PlatformPath.h>
#include <Utils/StringBuilder.h>
#include <Platform/SimpleMessageBox.h>

#if IS_WIN64()
#include <Platform/Windows/FileSystem/WindowsFileSystem.h>
#endif

struct PatternNotFoundLogger {
    static void onPatternNotFound(BytePattern pattern) noexcept
    {
        StringBuilderStorage<500> storage;
        auto builder = storage.builder();

        assert(false && "Pattern needs to be updated!");

        builder.put("Failed to find pattern ");

        bool printedFirst = false;
        const auto wildcardChar{pattern.getWildcardChar()};
        for (const auto byte : pattern.raw()) {
            if (printedFirst)
                builder.put(' ');
            if (byte != wildcardChar) {
                if ((byte & 0xF0) == 0)
                    builder.put('0');
                builder.putHex(static_cast<unsigned char>(byte));
            } else {
                builder.put(byte);
            }

            printedFirst = true;
        }

        builder.put('\n');

        SimpleMessageBox{}.showWarning("Osiris", builder.cstring());
    }

    static void onPatternNotFound(const char* moduleName, std::size_t patternIndex, BytePattern pattern) noexcept
    {
        // write to persistent log file first (before MessageBox, in case MessageBox hangs in overlay)
        appendToLogBuffer(moduleName, patternIndex, pattern);

        // also show MessageBox for immediate visibility
        onPatternNotFound(pattern);
    }

    static void setLogDirectoryPath(const platform::PathCharType* path) noexcept
    {
        logDirectoryPath = path;
    }

    static void flushLogToFile() noexcept
    {
        if (!logDirectoryPath || logWritePos == 0)
            return;

#if IS_WIN64()
        // build full log file path: <directory>\pattern_scan.log
        constexpr std::wstring_view logFileName{L"\\pattern_scan.log"};
        wchar_t logFilePath[512]{};
        std::size_t writeIdx = 0;

        // copy directory path
        for (const wchar_t* p = logDirectoryPath; *p != L'\0' && writeIdx < 510; ++p)
            logFilePath[writeIdx++] = *p;

        // append log file name
        for (const wchar_t c : logFileName) {
            if (writeIdx < 510)
                logFilePath[writeIdx++] = c;
        }
        logFilePath[writeIdx] = L'\0';

        // ensure directory exists
        WindowsFileSystem::createDirectory(logDirectoryPath);

        // write buffer to file
        if (const auto handle = WindowsFileSystem::createFileForOverwrite(logFilePath); handle != INVALID_HANDLE_VALUE) {
            WindowsFileSystem::writeFile(handle, 0, logBuffer.data(), logWritePos);
            WindowsSyscalls::NtClose(handle);
        }
#endif
    }

private:
    static void appendToLogBuffer(const char* moduleName, std::size_t patternIndex, BytePattern pattern) noexcept
    {
        const auto remaining = std::span{logBuffer}.subspan(logWritePos);
        if (remaining.size() < 128) return; // buffer nearly full, skip

        StringBuilder builder{remaining};

        builder.put("[NOT FOUND] #", totalNotFound, " module=", moduleName, " idx=", patternIndex, " bytes=");

        const auto wildcardChar{pattern.getWildcardChar()};
        for (const auto byte : pattern.raw()) {
            if (byte != wildcardChar) {
                if ((byte & 0xF0) == 0)
                    builder.put('0');
                builder.putHex(static_cast<unsigned char>(byte));
            } else {
                builder.put('?');
            }
        }

        builder.put('\n');

        logWritePos += builder.string().size();
        ++totalNotFound;
    }

    static constexpr std::size_t kLogBufferSize{4096};
    inline static std::array<char, kLogBufferSize> logBuffer{};
    inline static std::size_t logWritePos{0};
    inline static std::size_t totalNotFound{0};
    inline static const platform::PathCharType* logDirectoryPath{nullptr};
};
