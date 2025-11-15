#pragma once

#include <cassert>
#include <BuildConfig.h>
#include <MemoryAllocation/UniquePtr.h>
#include <Platform/PlatformPath.h>

#include "ConfigFileOperation.h"
#include "ConfigFromString.h"
#include "ConfigSchema.h"
#include "ConfigStringConversionState.h"
#include "ConfigToString.h"
#include "ConfigVariableChangeHandler.h"

#if IS_WIN64()
#include <Platform/Windows/FileSystem/WindowsFileSystem.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Utils/Wcslen.h>
#elif IS_LINUX()
#include <Platform/Linux/LinuxPlatformApi.h>
#endif

template <typename HookContext>
class Config {
public:
    explicit Config(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void init() noexcept
    {
        buildConfigDirectoryPath();
        buildConfigFilePath(WIN64_LINUX(L"default.cfg", "default.cfg"));
        buildConfigTempFilePath();
        static constinit char8_t fileOperationBuffer[build::kConfigFileBufferSize];
        state().fileOperationBuffer = fileOperationBuffer;
    }

    template <typename ConfigVariable>
    [[nodiscard]] auto getVariable() noexcept
    {
        return state().configVariables.template getVariableValue<ConfigVariable>();
    }

    template <typename ConfigVariable>
    bool setVariable(ConfigVariable::ValueType newValue) noexcept
    {
        if (changeVariableValue<ConfigVariable>(newValue)) {
            scheduleAutoSave();
            return true;
        }
        return false;
    }

    template <typename ConfigVariable>
    void setVariableWithoutAutoSave(ConfigVariable::ValueType newValue) noexcept
    {
        changeVariableValue<ConfigVariable>(newValue);
    }

    void restoreDefaults() noexcept
    {
        ConfigVariableTypes::forEach([this] <typename ConfigVariable> (std::type_identity<ConfigVariable>) {
            this->setVariableWithoutAutoSave<ConfigVariable>(ConfigVariable::kDefaultValue);
        });
        scheduleAutoSave();
        hookContext.gui().updateFromConfig();
    }

    void scheduleLoad() noexcept
    {
        state().loadScheduled = true;
    }

    void update()
    {
        switch (state().currentFileOperation) {
        case ConfigFileOperation::None:
            if (state().autoSaveScheduled) {
                state().currentFileOperation = ConfigFileOperation::Save;
                prepareSaveToFile();
                state().autoSaveScheduled = false;
                break;
            }
            if (state().loadScheduled) {
                state().currentFileOperation = ConfigFileOperation::Load;
                state().bufferUsedBytes = 0;
                state().loadScheduled = false;
                break;
            }
            break;
        case ConfigFileOperation::Load:
            finishLoadFromFile();
            break;
        default:
            break;
        }
    }

    void performFileOperation() noexcept
    {
        switch (state().currentFileOperation) {
        case ConfigFileOperation::Load:
            loadFromFile();
            break;
        case ConfigFileOperation::Save:
            saveToFile();
            break;
        default:
            break;
        }
    }

private:
    template <typename ConfigVariable>
    bool changeVariableValue(ConfigVariable::ValueType newValue) noexcept
    {
        if (getVariable<ConfigVariable>() == newValue)
            return false;

        ConfigVariableChangeHandler{hookContext}.template onConfigVariableValueChanged<ConfigVariable>(newValue);
        state().configVariables.template storeVariableValue<ConfigVariable>(newValue);
        return true;
    }

    void scheduleAutoSave() noexcept
    {
        state().autoSaveScheduled = true;
    }

    [[nodiscard]] auto& state()
    {
        return hookContext.configState();
    }

    void loadFromFile() noexcept
    {
        if (!state().pathToConfigFile)
            return;

#if IS_WIN64()
        const std::basic_string_view path{state().pathToConfigFile.get(), utils::wcslen(state().pathToConfigFile.get())};
        UNICODE_STRING pathStr{.Length = static_cast<USHORT>(path.length() * sizeof(wchar_t)), .MaximumLength = static_cast<USHORT>(path.length() * sizeof(wchar_t)), .Buffer = const_cast<wchar_t*>(path.data())};
        if (const auto handle = WindowsFileSystem::openFileForReading(pathStr); handle != INVALID_HANDLE_VALUE) {
            state().bufferUsedBytes = WindowsFileSystem::readFile(handle, 0, state().fileOperationBuffer, build::kConfigFileBufferSize);
            WindowsSyscalls::NtClose(handle);
        }
#elif IS_LINUX()
        if (const auto fd = LinuxPlatformApi::open(state().pathToConfigFile.get(), O_RDONLY); fd >= 0) {
            if (const auto read = LinuxPlatformApi::pread(fd, state().fileOperationBuffer, build::kConfigFileBufferSize, 0); read > 0)
                state().bufferUsedBytes = static_cast<std::size_t>(read);
            LinuxPlatformApi::close(fd);
        }
#endif
    }

    void finishLoadFromFile()
    {
        assert(state().currentFileOperation == ConfigFileOperation::Load);
        state().currentFileOperation = ConfigFileOperation::None;

        const auto readBytes = state().bufferUsedBytes;
        assert(readBytes < build::kConfigFileBufferSize && "Currently file must fit into a buffer");
        ConfigStringConversionState conversionState;
        std::size_t parsedBytes{0};
        do {
            assert(conversionState.offset <= readBytes);
            ConfigFromString configFromString{std::span{state().fileOperationBuffer + conversionState.offset, readBytes - conversionState.offset}, conversionState};
            parsedBytes = ConfigSchema{hookContext}.performConversion(configFromString);
        } while (parsedBytes != 0 && (conversionState.nestingLevel != 0 || conversionState.indexInNestingLevel[0] != 1));
        
        assert(readBytes == 0 || (conversionState.nestingLevel == 0 && conversionState.indexInNestingLevel[0] == 1));
        hookContext.gui().updateFromConfig();
    }

    void prepareSaveToFile()
    {
        ConfigStringConversionState conversionState;
        ConfigToString configToString{std::span{state().fileOperationBuffer, build::kConfigFileBufferSize}, conversionState};
        state().bufferUsedBytes = ConfigSchema{hookContext}.performConversion(configToString);
        assert(conversionState.nestingLevel == 0 && conversionState.indexInNestingLevel[0] == 1);
    }

    void saveToFile() noexcept
    {
        assert(state().currentFileOperation == ConfigFileOperation::Save);
        state().currentFileOperation = ConfigFileOperation::None;

        if (!hookContext.osirisDirectoryPath().get() || !state().pathToConfigDirectory || !state().pathToConfigFile || !state().pathToConfigTempFile)
            return;

        const auto numberOfBytesToWrite = state().bufferUsedBytes;
#if IS_WIN64()
        WindowsFileSystem::createDirectory(hookContext.osirisDirectoryPath().get());
        WindowsFileSystem::createDirectory(state().pathToConfigDirectory.get());

        if (const auto handle = WindowsFileSystem::createFileForOverwrite(state().pathToConfigTempFile.get()); handle != INVALID_HANDLE_VALUE) {
            if (WindowsFileSystem::writeFile(handle, 0, state().fileOperationBuffer, numberOfBytesToWrite) == numberOfBytesToWrite)
                WindowsFileSystem::renameFile(handle, state().pathToConfigFile.get());
            WindowsSyscalls::NtClose(handle);
        }
#elif IS_LINUX()
        mkdir(hookContext.osirisDirectoryPath().get(), 0777);
        mkdir(state().pathToConfigDirectory.get(), 0777);

        if (const auto fd = LinuxPlatformApi::open(state().pathToConfigTempFile.get(), O_CREAT | O_WRONLY, 0666); fd >= 0) {
            if (std::cmp_equal(LinuxPlatformApi::write(fd, state().fileOperationBuffer, numberOfBytesToWrite), numberOfBytesToWrite))
                rename(state().pathToConfigTempFile.get(), state().pathToConfigFile.get());
            LinuxPlatformApi::close(fd);
        }
#endif
    }

    void buildConfigDirectoryPath() noexcept
    {
        if (!hookContext.osirisDirectoryPath().get())
            return;

        const std::basic_string_view osirisDirectoryPath{hookContext.osirisDirectoryPath().get(), WIN64_LINUX(utils::wcslen, std::strlen)(hookContext.osirisDirectoryPath().get())};
        constexpr auto kPathSeparatorLength{1};
        constexpr auto kNullTerminatorLength{1};
        const auto length = osirisDirectoryPath.length() + kPathSeparatorLength + build::kConfigDirectoryName.length() + kNullTerminatorLength;
        state().pathToConfigDirectory = mem::makeUniqueForOverwrite<platform::PathCharType[]>(length);
        if (!state().pathToConfigDirectory)
            return;
        std::size_t writeIndex{0};
        std::ranges::copy(osirisDirectoryPath, state().pathToConfigDirectory.get() + writeIndex);
        writeIndex += osirisDirectoryPath.length();
        state().pathToConfigDirectory.get()[writeIndex++] = platform::kPathSeparator;
        std::ranges::copy(build::kConfigDirectoryName, state().pathToConfigDirectory.get() + writeIndex);
        writeIndex += build::kConfigDirectoryName.length();
        state().pathToConfigDirectory.get()[writeIndex++] = 0;
    }

    void buildConfigFilePath(std::basic_string_view<platform::PathCharType> configFileName) noexcept
    {
        if (!state().pathToConfigDirectory)
            return;

        const std::basic_string_view pathToConfigDirectory{state().pathToConfigDirectory.get(), WIN64_LINUX(utils::wcslen, std::strlen)(state().pathToConfigDirectory.get())};
        constexpr auto kPathSeparatorLength{1};
        constexpr auto kNullTerminatorLength{1};
        const auto length = pathToConfigDirectory.length() + kPathSeparatorLength + configFileName.length() + kNullTerminatorLength;
        state().pathToConfigFile = mem::makeUniqueForOverwrite<platform::PathCharType[]>(length);
        if (!state().pathToConfigFile)
            return;
        std::size_t writeIndex{0};
        std::ranges::copy(pathToConfigDirectory, state().pathToConfigFile.get() + writeIndex);
        writeIndex += pathToConfigDirectory.length();
        state().pathToConfigFile.get()[writeIndex++] = platform::kPathSeparator;
        std::ranges::copy(configFileName, state().pathToConfigFile.get() + writeIndex);
        writeIndex += configFileName.length();
        state().pathToConfigFile.get()[writeIndex++] = 0;
    }

    void buildConfigTempFilePath() noexcept
    {
        if (!state().pathToConfigFile)
            return;

        const std::basic_string_view pathToConfigFile{state().pathToConfigFile.get(), WIN64_LINUX(utils::wcslen, std::strlen)(state().pathToConfigFile.get())};
        const std::basic_string_view configTempFileSuffix{WIN64_LINUX(L".new", ".new")};
        constexpr auto kNullTerminatorLength{1};
        const auto length = pathToConfigFile.length() + configTempFileSuffix.length() + kNullTerminatorLength;
        state().pathToConfigTempFile = mem::makeUniqueForOverwrite<platform::PathCharType[]>(length);
        if (!state().pathToConfigTempFile)
            return;
        std::size_t writeIndex{0};
        std::ranges::copy(pathToConfigFile, state().pathToConfigTempFile.get() + writeIndex);
        writeIndex += pathToConfigFile.length();
        std::ranges::copy(configTempFileSuffix, state().pathToConfigTempFile.get() + writeIndex);
        writeIndex += configTempFileSuffix.length();
        state().pathToConfigTempFile.get()[writeIndex++] = 0;
    }

    HookContext& hookContext;
};
