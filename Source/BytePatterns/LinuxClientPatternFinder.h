#pragma once

#include <CSGO/Functions.h>
#include <CSGO/Recv.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/PatternFinder.h>
#include <Utils/ReturnAddress.h>

namespace csgo
{
    struct GlowObjectManager;
    struct ViewRenderBeamsPOD;
}

struct LinuxClientPatternFinder {
    explicit LinuxClientPatternFinder(const PatternFinder& clientPatternFinder)
        : finder{ clientPatternFinder }
    {
    }

    [[nodiscard]] csgo::ViewRenderBeamsPOD* viewRenderBeams() const noexcept
    {
        return finder("C7 45 ? ? ? ? ? 4C 8D 25 ? ? ? ? 49 8B 3C 24"_pat).add(10).abs().deref().as<csgo::ViewRenderBeamsPOD*>();
    }

    [[nodiscard]] ReturnAddress scopeDust() const noexcept
    {
        return finder("41 FF 51 70 43 8D 14 3E"_pat).add(4).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress scopeArc() const noexcept
    {
        return finder("49 8B 3C 24 8B B3 ? ? ? ? 48 8B 07 FF 90 ? ? ? ? 49 8B 3C 24 66 0F EF E4"_pat).asReturnAddress();
    }

    [[nodiscard]] float* vignette() const noexcept
    {
        return finder("F3 0F 10 05 ? ? ? ? FF 50 20 48 8B BB ? ? ? ? 48 85 FF 74 17"_pat).add(4).abs().add(4).as<float*>();
    }

    [[nodiscard]] bool* disablePostProcessing() const noexcept
    {
        return finder("0F B6 05 ? ? ? ? 84 C0 0F 85 ? ? ? ? 85 D2"_pat).add(3).abs().as<bool*>();
    }

    [[nodiscard]] ReturnAddress cameraThink() const noexcept
    {
        return finder("0F 1F 80 ? ? ? ? 85 C0 75 5C"_pat).add(6).asReturnAddress();
    }

    [[nodiscard]] csgo::KeyValuesFindKey keyValuesFindKey() const noexcept
    {
        return finder("E8 ? ? ? ? 48 85 C0 75 1E"_pat).add(1).abs().as<csgo::KeyValuesFindKey>();
    }

    [[nodiscard]] csgo::KeyValuesSetString keyValuesSetString() const noexcept
    {
        return finder("E8 ? ? ? ? 48 89 DE 4C 89 FF E8 ? ? ? ? 49 8B 04 24"_pat).add(1).abs().as<csgo::KeyValuesSetString>();
    }

    [[nodiscard]] std::uintptr_t keyValuesFromString() const noexcept
    {
        return finder("E8 ? ? ? ? 48 89 DF 48 89 45 E0"_pat).add(1).abs().get();
    }

    [[nodiscard]] csgo::GlowObjectManager* glowObjectManager() const noexcept
    {
        return finder("E8 ? ? ? ? 4C 89 E7 8B 70 20"_pat).add(1).abs().add(12).abs().as<csgo::GlowObjectManager*>();
    }

    [[nodiscard]] csgo::SubmitReport submitReport() const noexcept
    {
        return finder("55 48 89 F7 48 89 E5 41 57 41 56 41 55 41 54 53 48 89 D3 48 83 EC 58"_pat).as<csgo::SubmitReport>();
    }

    [[nodiscard]] ReturnAddress demoOrHLTV() const noexcept
    {
        return finder("84 C0 75 0A 48 8D 05"_pat).asReturnAddress();
    }

    [[nodiscard]] std::uintptr_t money() const noexcept
    {
        return finder("84 C0 75 C6 B8 ? ? ? ? EB D9"_pat).get();
    }

    [[nodiscard]] ReturnAddress insertIntoTree() const noexcept
    {
        return finder("41 FF 52 30 89 85"_pat).add(4).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress demoFileEndReached() const noexcept
    {
        return finder("48 85 C0 74 76 80 78 10 00"_pat).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress setStickerToolSlotGetArgAsNumber() const noexcept
    {
        return finder("F2 44 0F 2C E8 45 85 ED"_pat, OffsetHint{ 11247132 }).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress wearItemStickerGetArgAsString() const noexcept
    {
        return finder("49 8B 47 18 BA"_pat, OffsetHint{ 11225783 }).add(-131).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress setNameToolStringGetArgAsString() const noexcept
    {
        return finder("BA ? ? ? ? 4C 89 F6 48 89 C7 49 89 C4"_pat, OffsetHint{ 11252637 }).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress clearCustomNameGetArgAsString() const noexcept
    {
        return finder("48 85 C0 74 E5 48 89 C7 E8 ? ? ? ? 49 89 C4"_pat, OffsetHint{ 11247366 }).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress deleteItemGetArgAsString() const noexcept
    {
        return finder("48 85 C0 74 DE 48 89 C7 E8 ? ? ? ? 48 89 C3 E8 ? ? ? ? 48 89 DE"_pat, OffsetHint{ 11248886 }).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress setStatTrakSwapToolItemsGetArgAsString() const noexcept
    {
        return finder("74 84 4C 89 EE 4C 89 F7 E8 ? ? ? ? 48 85 C0"_pat, OffsetHint{ 11246916 }).add(-86).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress acknowledgeNewItemByItemIDGetArgAsString() const noexcept
    {
        return finder("48 85 C0 74 DE 48 89 C7 E8 ? ? ? ? 4C 89 EF 48 89 C6 E8 ? ? ? ? 4C 89 E6 48 89 DF 0F B6 D0"_pat, OffsetHint{ 11221963 }).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress setItemAttributeValueAsyncGetArgAsString() const noexcept
    {
        return finder("FF 50 38 48 85 C0 74 C2"_pat, OffsetHint{ 11228494 }).add(3).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress setMyPredictionUsingItemIdGetNumArgs() const noexcept
    {
        return finder("83 F8 01 89 85 ? ? ? ? 7E 1A"_pat, OffsetHint{ 11613975 }).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress getMyPredictionTeamIDGetArgAsString() const noexcept
    {
        return finder("48 85 C0 48 89 C7 74 C2 E8 ? ? ? ? 41 BF"_pat, OffsetHint{ 11606770 }).add(-20).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress setInventorySortAndFiltersGetArgAsString() const noexcept
    {
        return finder("49 89 C5 E9 ? ? ? ? 0F 1F 44 00 00 BA"_pat, OffsetHint{ 11312157 }).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress getInventoryCountSetResultInt() const noexcept
    {
        return finder("48 8B 08 48 89 DE 48 89 C7 41 8B 96 38 02"_pat, OffsetHint{ 11404345 }).add(19).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress performItemCasketTransactionGetArgAsString() const noexcept
    {
        return finder("48 85 C0 0F 84 ? ? ? ? 48 89 C7 E8 ? ? ? ? F2 0F 10 85"_pat, OffsetHint{ 11300702 }).asReturnAddress();
    }

    [[nodiscard]] ReturnAddress useToolGetArgAsString() const noexcept
    {
        return finder("48 85 C0 74 DA 48 89 C7 E8 ? ? ? ? BA"_pat, OffsetHint{ 11246382 }).asReturnAddress();
    }

    [[nodiscard]] csgo::SetDynamicAttributeValue setDynamicAttributeValue() const noexcept
    {
        return finder("48 8B 7A 18 48 85 FF 0F 84 ? ? ? ? 55 48 89 E5 41 57 41 56 49 89 D6"_pat).as<csgo::SetDynamicAttributeValue>();
    }

    [[nodiscard]] csgo::RemoveDynamicAttribute removeDynamicAttribute() const noexcept
    {
        return finder("E8 ? ? ? ? 80 3D ? ? ? ? ? 75 14 48 8D 3D ? ? ? ? E8 ? ? ? ? 85 C0 0F 85 ? ? ? ? C7 45"_pat).add(1).abs().as<csgo::RemoveDynamicAttribute>();
    }

    [[nodiscard]] csgo::SetCustomName setCustomName() const noexcept
    {
        return finder("E8 ? ? ? ? 8B 83 ? ? ? ? E9 ? ? ? ? 48 8B 05 ? ? ? ? 44 8B A0"_pat).add(1).abs().as<csgo::SetCustomName>();
    }

    [[nodiscard]] csgo::ClearInventoryImageRGBA clearInventoryImageRGBA() const noexcept
    {
        return finder("E8 ? ? ? ? 83 C3 01 49 83 C4 08 41 39 5D 50"_pat).add(1).abs().as<csgo::ClearInventoryImageRGBA>();
    }

    [[nodiscard]] csgo::GetSOCData getSOCData() const noexcept
    {
        return finder("E8 ? ? ? ? 4D 85 E4 0F 94 C1"_pat).add(1).abs().as<csgo::GetSOCData>();
    }

    [[nodiscard]] csgo::CreateSharedObjectSubclass<csgo::EconItemPOD> createEconItemSharedObject() const noexcept
    {
        return finder("55 48 8D 05 ? ? ? ? 31 D2 BF"_pat).add(47).abs().as< csgo::CreateSharedObjectSubclass<csgo::EconItemPOD>>();
    }

    [[nodiscard]] csgo::UiComponentInventoryPOD** uiComponentInventory() const noexcept
    {
        return finder("4C 89 3D ? ? ? ? 4C 89 FF EB 9D 0F 1F 44 00 00"_pat).add(3).abs().as<csgo::UiComponentInventoryPOD**>();
    }

    [[nodiscard]] csgo::SetItemSessionPropertyValue setItemSessionPropertyValue() const noexcept
    {
        return finder("E8 ? ? ? ? 48 8B 85 ? ? ? ? 41 83 C4 01"_pat).add(1).abs().as<csgo::SetItemSessionPropertyValue>();
    }

private:
    const PatternFinder& finder;
};
