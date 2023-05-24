#pragma once

#include <CSGO/Functions.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/PatternFinder.h>

namespace csgo
{
    struct SoundInfo;
    template <typename ElementType, typename IndexType> struct UtlRbTree;
    struct SplitScreen;
    struct ActiveChannels;
    struct Channel;
}

struct LinuxEnginePatternFinder {
    explicit LinuxEnginePatternFinder(const PatternFinder& enginePatternFinder)
        : finder{ enginePatternFinder }
    {
    }

    [[nodiscard]] csgo::SendClanTag sendClanTag() const noexcept
    {
        return finder("E8 ? ? ? ? E9 ? ? ? ? 0F 1F 44 00 00 48 8B 7D B0"_pat).add(1).abs().as<csgo::SendClanTag>();
    }

    [[nodiscard]] csgo::R_LoadNamedSkys loadNamedSkys() const noexcept
    {
        return finder("55 4C 8D 05 ? ? ? ? 48 89 E5 41 57"_pat).as<csgo::R_LoadNamedSkys>();
    }

    [[nodiscard]] csgo::UtlRbTree<csgo::SoundInfo, int>* soundMessages() const noexcept
    {
        return finder("41 5C 5D E9 ? ? ? ? 8B 48 08"_pat).add(-4).abs().as<csgo::UtlRbTree<csgo::SoundInfo, int>*>();
    }

    [[nodiscard]] csgo::SplitScreen* splitScreen() const noexcept
    {
        return finder("C6 05 ? ? ? ? ? 48 89 05 ? ? ? ? 0F 84"_pat).add(10).abs().as<csgo::SplitScreen*>();
    }

    [[nodiscard]] csgo::GetEventDescriptor getEventDescriptor() const noexcept
    {
        return finder("E8 ? ? ? ? 4D 85 F6 74 09"_pat).add(1).abs().as<csgo::GetEventDescriptor>();
    }

    [[nodiscard]] csgo::ActiveChannels* activeChannels() const noexcept
    {
        return finder("48 8D 3D ? ? ? ? 48 89 DE E8 ? ? ? ? 8B BD"_pat).add(3).abs().as<csgo::ActiveChannels*>();
    }

    [[nodiscard]] csgo::Channel* channels() const noexcept
    {
        return finder("48 8D 3D ? ? ? ? 48 0F BF 10"_pat).add(3).abs().as<csgo::Channel*>();
    }

private:
    const PatternFinder& finder;
};
