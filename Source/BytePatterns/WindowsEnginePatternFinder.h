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

struct WindowsEnginePatternFinder {
    explicit WindowsEnginePatternFinder(const PatternFinder& enginePatternFinder)
        : finder{ enginePatternFinder }
    {
    }

    [[nodiscard]] csgo::SendClanTag sendClanTag() const noexcept
    {
        return finder("53 56 57 8B DA 8B F9 FF 15"_pat).as<csgo::SendClanTag>();
    }

    [[nodiscard]] csgo::R_LoadNamedSkys loadNamedSkys() const noexcept
    {
        return finder("E8 ? ? ? ? 84 C0 74 2D A1"_pat).add(1).abs().as<csgo::R_LoadNamedSkys>();
    }

    [[nodiscard]] csgo::UtlRbTree<csgo::SoundInfo, int>* soundMessages() const noexcept
    {
        return finder("74 3D 8B 0D ? ? ? ? 56"_pat).add(4).deref().add(-4).as<csgo::UtlRbTree<csgo::SoundInfo, int>*>();
    }

    [[nodiscard]] csgo::SplitScreen* splitScreen() const noexcept
    {
        return finder("79 23 A1"_pat).add(3).deref().as<csgo::SplitScreen*>();
    }

    [[nodiscard]] csgo::GetEventDescriptor getEventDescriptor() const noexcept
    {
        return finder("E8 ? ? ? ? 8B D8 85 DB 75 27"_pat).add(1).abs().as<csgo::GetEventDescriptor>();
    }

    [[nodiscard]] csgo::ActiveChannels* activeChannels() const noexcept
    {
        return finder("8B 1D ? ? ? ? 89 5C 24 48"_pat).add(2).deref().as<csgo::ActiveChannels*>();
    }

    [[nodiscard]] csgo::Channel* channels() const noexcept
    {
        return finder("81 C2 ? ? ? ? 8B 72 54"_pat).add(2).deref().as<csgo::Channel*>();
    }

    [[nodiscard]] ReturnAddress keyValuesAlloc() const noexcept
    {
        return finder("E8 ? ? ? ? 83 C4 08 84 C0 75 10 FF 75 0C"_pat).add(1).abs().add(0x4A).as<ReturnAddress>();
    }

private:
    const PatternFinder& finder;
};
