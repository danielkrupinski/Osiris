#pragma once

#include <cstdint>
#include <MemoryPatterns/PatternTypes/SoundSystemPatternTypes.h>
#include <Platform/PlatformMemory.h>

#include "MemoryValidationStatus.h"

template <typename HookContext>
class GameMemoryValidator {
public:
    explicit GameMemoryValidator(HookContext& hookContext)
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] MemoryValidationStatus checkSoundChannelInfo1SizeValid()
    {
        auto& status = hookContext.gameMemoryValidatorState().soundChannelInfo1SizeStatus;
        if (status != MemoryValidationStatus::Unvalidated)
            return status;

        auto soundChannels = hookContext.patternSearchResults().template get<SoundChannelsPointer>();
        if (!soundChannels || !*soundChannels)
            return status;

        int checkedChannelInfos = 0;

        const auto& channelInfo1 = (*soundChannels)->channelInfo1;
        for (int i = 0; i < channelInfo1.size; ++i) {
            const auto& channel = channelInfo1.memory[i];
            if (!channel.sfx)
                continue;

            if (!PlatformMemory::isMemoryReadable(reinterpret_cast<std::uintptr_t>(channel.sfx))) {
                status = MemoryValidationStatus::Nok;
                return status;
            }

            ++checkedChannelInfos;
            if (checkedChannelInfos > 1) {
                status = MemoryValidationStatus::Ok;
                return status;
            }
        }

        return status;
    }

private:
    HookContext& hookContext;
};
