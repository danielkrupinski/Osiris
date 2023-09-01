#pragma once

#include <CSGO/Functions.h>
#include <CSGO/Recv.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/PatternFinder.h>
#include <Utils/ReturnAddress.h>

namespace csgo
{
    struct GlowObjectManager;
    struct InventoryManagerPOD;
    struct MoveData;
    struct MoveHelperPOD;
    struct PanoramaEventRegistration;
    struct PanoramaMarshallHelperPOD;
    struct PlantedC4POD;
    template <typename Key, typename Value> struct UtlMap;
    template <typename T> struct UtlVector;
    struct ViewRender;
    struct ViewRenderBeamsPOD;
    struct WeaponSystemPOD;
}

struct WindowsClientPatternFinder {
    explicit WindowsClientPatternFinder(const PatternFinder& clientPatternFinder)
        : finder{ clientPatternFinder }
    {
    }

    [[nodiscard]] csgo::ViewRenderBeamsPOD* viewRenderBeams() const noexcept
    {
        return finder("B9 ? ? ? ? 0F 11 44 24 ? C7 44 24 ? ? ? ? ? F3 0F 10 84 24"_pat).add(1).deref().as<csgo::ViewRenderBeamsPOD*>();
    }

    [[nodiscard]] ReturnAddress scopeDust() const noexcept
    {
        return finder("FF 50 3C 8B 4C 24 20"_pat).add(3).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress scopeArc() const noexcept
    {
        return finder("8B 0D ? ? ? ? FF B7 ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B 7C 24 1C"_pat).as<ReturnAddress>();
    }

    [[nodiscard]] float* vignette() const noexcept
    {
        return finder("0F 11 05 ? ? ? ? F3 0F 7E 87"_pat).add(3).deref().add(4).as<float*>();
    }

    [[nodiscard]] bool* disablePostProcessing() const noexcept
    {
        return finder("83 EC 4C 80 3D"_pat).add(5).deref().as<bool*>();
    }

    [[nodiscard]] csgo::RecvProxy maxFlashAlphaProxy() const noexcept
    {
        return finder("55 8B EC 8B 4D 0C 8B 45 08 81 C1"_pat).as<csgo::RecvProxy>();
    }

    [[nodiscard]] ReturnAddress cameraThink() const noexcept
    {
        return finder("85 C0 75 30 38 87"_pat).as<ReturnAddress>();
    }

    [[nodiscard]] csgo::KeyValuesFindKey keyValuesFindKey() const noexcept
    {
        return finder("E8 ? ? ? ? F7 45"_pat).add(1).abs().as<csgo::KeyValuesFindKey>();
    }

    [[nodiscard]] csgo::KeyValuesSetString keyValuesSetString() const noexcept
    {
        return finder("E8 ? ? ? ? 89 77 38"_pat).add(1).abs().as<csgo::KeyValuesSetString>();
    }

    [[nodiscard]] std::uintptr_t keyValuesFromString() const noexcept
    {
        return finder("E8 ? ? ? ? 83 C4 04 89 45 D8"_pat).add(1).abs().as<std::uintptr_t>();
    }

    [[nodiscard]] csgo::GlowObjectManager* glowObjectManager() const noexcept
    {
        return finder("0F 11 05 ? ? ? ? 83 C8 01"_pat).add(3).deref().as<csgo::GlowObjectManager*>();
    }

    [[nodiscard]] csgo::GlowObjectAntiCheatCheck glowObjectAntiCheatCheck() const noexcept
    {
        return finder("E8 ? ? ? ? 8B 75 FC 6A 04"_pat).add(1).abs().as< csgo::GlowObjectAntiCheatCheck>();
    }

    [[nodiscard]] csgo::SubmitReport submitReport() const noexcept
    {
        return finder("55 8B EC 83 E4 F8 83 EC 28 8B 4D 08"_pat).as<csgo::SubmitReport>();
    }

    [[nodiscard]] ReturnAddress demoOrHLTV() const noexcept
    {
        return finder("84 C0 75 09 38 05"_pat).as<ReturnAddress>();
    }

    [[nodiscard]] std::uintptr_t money() const noexcept
    {
        return finder("84 C0 75 0C 5B"_pat).as<std::uintptr_t>();
    }

    [[nodiscard]] ReturnAddress insertIntoTree() const noexcept
    {
        return finder("56 52 FF 50 18"_pat).add(5).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress demoFileEndReached() const noexcept
    {
        return finder("8B C8 85 C9 74 1F 80 79 10"_pat).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress setStickerToolSlotGetArgAsNumber() const noexcept
    {
        return finder("FF D2 DD 5C 24 10 F2 0F 2C 7C 24"_pat, OffsetHint{ 6120862 }).add(2).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress wearItemStickerGetArgAsString() const noexcept
    {
        return finder("DD 5C 24 18 F2 0F 2C 7C 24 ? 85 FF"_pat, OffsetHint{ 6095215 }).add(-80).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress setNameToolStringGetArgAsString() const noexcept
    {
        return finder("8B F8 C6 45 08 ? 33 C0"_pat, OffsetHint{ 6121111 }).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress clearCustomNameGetArgAsString() const noexcept
    {
        return finder("FF 50 1C 8B F0 85 F6 74 21"_pat, OffsetHint{ 6121269 }).add(3).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress deleteItemGetArgAsString() const noexcept
    {
        return finder("85 C0 74 22 51"_pat, OffsetHint{ 6122298 }).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress setStatTrakSwapToolItemsGetArgAsString() const noexcept
    {
        return finder("85 C0 74 7E 8B C8 E8 ? ? ? ? 8B 37"_pat, OffsetHint{ 6120623 }).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress acknowledgeNewItemByItemIDGetArgAsString() const noexcept
    {
        return finder("85 C0 74 33 8B C8 E8 ? ? ? ? B9"_pat, OffsetHint{ 6092121 }).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress setItemAttributeValueAsyncGetArgAsString() const noexcept
    {
        return finder("8B D8 83 C4 08 85 DB 0F 84 ? ? ? ? 8B 16 8B CE 57"_pat, OffsetHint{ 6098822 }).add(-22).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress setMyPredictionUsingItemIdGetNumArgs() const noexcept
    {
        return finder("8B F0 89 74 24 2C 83 FE 01"_pat, OffsetHint{ 6702914 }).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress getMyPredictionTeamIDGetArgAsString() const noexcept
    {
        return finder("85 C0 0F 84 ? ? ? ? 57 8B C8 E8 ? ? ? ? BF ? ? ? ? 89 45 E8"_pat, OffsetHint{ 6699829 }).add(-20).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress setInventorySortAndFiltersGetArgAsString() const noexcept
    {
        return finder("80 7D FF ? 8B F8 74 27"_pat, OffsetHint{ 6088445 }).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress getInventoryCountSetResultInt() const noexcept
    {
        return finder("B9 ? ? ? ? E8 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? C2 08"_pat, OffsetHint{ 6092048 }).add(-10).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress performItemCasketTransactionGetArgAsString() const noexcept
    {
        return finder("85 C0 0F 84 ? ? ? ? 8B C8 E8 ? ? ? ? 52 50 E8 ? ? ? ? 83 C4 08 89 44 24 0C 85 C0 0F 84 ? ? ? ? F2 0F 2C 44 24"_pat, OffsetHint{ 6098213 }).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress useToolGetArgAsString() const noexcept
    {
        return finder("85 C0 0F 84 ? ? ? ? 8B C8 E8 ? ? ? ? 8B 37"_pat, OffsetHint{ 6118723 }).as<ReturnAddress>();
    }

    [[nodiscard]] csgo::SetDynamicAttributeValue setDynamicAttributeValue() const noexcept
    {
        return finder("55 8B EC 83 E4 F8 83 EC 3C 53 8B 5D 08 56 57 6A"_pat).as<csgo::SetDynamicAttributeValue>();
    }

    [[nodiscard]] csgo::RemoveDynamicAttribute removeDynamicAttribute() const noexcept
    {
        return finder("55 8B EC 83 EC 08 8B C1 89 45 F8"_pat).as<csgo::RemoveDynamicAttribute>();
    }

    [[nodiscard]] csgo::SetCustomName setCustomName() const noexcept
    {
        return finder("E8 ? ? ? ? 8B 46 78 C1 E8 0A A8 01 74 13 8B 46 34"_pat).add(1).abs().as<csgo::SetCustomName>();
    }

    [[nodiscard]] csgo::ClearInventoryImageRGBA clearInventoryImageRGBA() const noexcept
    {
        return finder("55 8B EC 81 EC ? ? ? ? 57 8B F9 C7 47"_pat).as<csgo::ClearInventoryImageRGBA>();
    }

    [[nodiscard]] csgo::GetSOCData getSOCData() const noexcept
    {
        return finder("E8 ? ? ? ? 32 C9"_pat).add(1).abs().as<csgo::GetSOCData>();
    }

    [[nodiscard]] csgo::CreateSharedObjectSubclass<csgo::EconItemPOD> createEconItemSharedObject() const noexcept
    {
        return finder("55 8B EC 83 EC 1C 8D 45 E4 C7 45"_pat).add(20).deref().as< csgo::CreateSharedObjectSubclass<csgo::EconItemPOD>>();
    }

    [[nodiscard]] csgo::UiComponentInventoryPOD** uiComponentInventory() const noexcept
    {
        return finder("C6 44 24 ? ? 83 3D"_pat).add(7).deref().as<csgo::UiComponentInventoryPOD**>();
    }

    [[nodiscard]] csgo::SetItemSessionPropertyValue setItemSessionPropertyValue() const noexcept
    {
        return finder("E8 ? ? ? ? 8B 4C 24 2C 46"_pat).add(1).abs().as<csgo::SetItemSessionPropertyValue>();
    }

    [[nodiscard]] csgo::LineGoesThroughSmoke lineGoesThroughSmoke() const noexcept
    {
        return finder("E8 ? ? ? ? 8B 4C 24 30 33 D2"_pat).add(1).abs().as<csgo::LineGoesThroughSmoke>();
    }

    [[nodiscard]] csgo::WeaponSystemPOD* weaponSystem() const noexcept
    {
        return finder("8B 35 ? ? ? ? FF 10 0F B7 C0"_pat).add(2).deref().as<csgo::WeaponSystemPOD*>();
    }

    [[nodiscard]] csgo::InventoryManagerPOD* inventoryManager() const noexcept
    {
        return finder("8D 44 24 28 B9 ? ? ? ? 50"_pat).add(5).deref().as<csgo::InventoryManagerPOD*>();
    }

    [[nodiscard]] csgo::FindOrCreateEconItemViewForItemID findOrCreateEconItemViewForItemID() const noexcept
    {
        return finder("E8 ? ? ? ? 8B CE 83 C4 08"_pat).add(1).abs().as<csgo::FindOrCreateEconItemViewForItemID>();
    }

    [[nodiscard]] csgo::MakePanoramaSymbol makePanoramaSymbol() const noexcept
    {
        return finder("E8 ? ? ? ? 0F B7 45 0E 8D 4D 0E"_pat).add(1).abs().as<csgo::MakePanoramaSymbol>();
    }

    [[nodiscard]] csgo::IsOtherEnemy isOtherEnemy() const noexcept
    {
        return finder("8B CE E8 ? ? ? ? 02 C0"_pat).add(3).abs().as<csgo::IsOtherEnemy>();
    }

    [[nodiscard]] csgo::HudPOD* hud() const noexcept
    {
        return finder("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08"_pat).add(1).deref().as<csgo::HudPOD*>();
    }

    [[nodiscard]] csgo::FindHudElement findHudElement() const noexcept
    {
        return finder("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08"_pat).add(1).add(5).abs().as<csgo::FindHudElement>();
    }

    [[nodiscard]] csgo::ClearHudWeapon clearHudWeapon() const noexcept
    {
        return finder("E8 ? ? ? ? 8B F0 C6 44 24 ? ? C6 44 24"_pat).add(1).abs().as<csgo::ClearHudWeapon>();
    }

    [[nodiscard]] csgo::GetItemSystem getItemSystem() const noexcept
    {
        return finder("E8 ? ? ? ? 0F B7 0F"_pat).add(1).abs().as<csgo::GetItemSystem>();
    }

    [[nodiscard]] csgo::SetAbsOrigin setAbsOrigin() const noexcept
    {
        return finder("E8 ? ? ? ? EB 19 8B 07"_pat).add(1).abs().as<csgo::SetAbsOrigin>();
    }

    [[nodiscard]] std::uintptr_t traceToExit() const noexcept
    {
        return finder("55 8B EC 83 EC 4C F3 0F 10 75"_pat).as<std::uintptr_t>();
    }

    [[nodiscard]] csgo::ViewRender* viewRender() const noexcept
    {
        return finder("8B 0D ? ? ? ? FF 75 0C 8B 45 08"_pat).add(2).deref<2>().as<csgo::ViewRender*>();
    }

    [[nodiscard]] std::uintptr_t drawScreenEffectMaterial() const noexcept
    {
        return finder("E8 ? ? ? ? 83 C4 0C 8D 4D F8"_pat).add(1).abs().as<std::uintptr_t>();
    }

    [[nodiscard]] csgo::EquipWearable equipWearable() const noexcept
    {
        return finder("55 8B EC 83 EC 10 53 8B 5D 08 57 8B F9"_pat).as<csgo::EquipWearable>();
    }

    [[nodiscard]] int* predictionRandomSeed() const noexcept
    {
        return finder("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04"_pat).add(2).deref().as<int*>();
    }

    [[nodiscard]] csgo::MoveData* moveData() const noexcept
    {
        return finder("A1 ? ? ? ? F3 0F 59 CD"_pat).add(1).deref<2>().as<csgo::MoveData*>();
    }

    [[nodiscard]] csgo::MoveHelperPOD* moveHelper() const noexcept
    {
        return finder("8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01"_pat).add(2).deref<2>().as<csgo::MoveHelperPOD*>();
    }

    [[nodiscard]] csgo::PlayerResource** playerResource() const noexcept
    {
        return finder("74 30 8B 35 ? ? ? ? 85 F6"_pat).add(4).deref().as<csgo::PlayerResource**>();
    }

    [[nodiscard]] csgo::GetDecoratedPlayerName getDecoratedPlayerName() const noexcept
    {
        return finder("E8 ? ? ? ? 66 83 3E"_pat).add(1).abs().as<csgo::GetDecoratedPlayerName>();
    }

    [[nodiscard]] csgo::UtlVector<csgo::PlantedC4POD*>* plantedC4s() const noexcept
    {
        return finder("7E 2C 8B 15"_pat).add(4).deref().as<csgo::UtlVector<csgo::PlantedC4POD*>*>();
    }

    [[nodiscard]] csgo::EntityPOD** gameRules() const noexcept
    {
        return finder("8B EC 8B 0D ? ? ? ? 85 C9 74 07"_pat).add(4).deref().as<csgo::EntityPOD**>();
    }

    [[nodiscard]] csgo::UtlMap<short, csgo::PanoramaEventRegistration>* registeredPanoramaEvents() const noexcept
    {
        return finder("E8 ? ? ? ? A1 ? ? ? ? A8 01 75 21"_pat).add(6).deref().add(-36).as<csgo::UtlMap<short, csgo::PanoramaEventRegistration>*>();
    }

    [[nodiscard]] csgo::PanoramaMarshallHelperPOD* panoramaMarshallHelper() const noexcept
    {
        return finder("68 ? ? ? ? 8B C8 E8 ? ? ? ? 8D 4D F4 FF 15 ? ? ? ? 8B CF FF 15 ? ? ? ? 5F 5E 8B E5 5D C3"_pat).add(1).deref().as<csgo::PanoramaMarshallHelperPOD*>();
    }

    [[nodiscard]] csgo::CreateBaseTypeCache createBaseTypeCache() const noexcept
    {
        return finder("E8 ? ? ? ? 8D 4D 0F"_pat).add(1).abs().as<csgo::CreateBaseTypeCache>();
    }

    [[nodiscard]] csgo::EntityPOD** localPlayer() const noexcept
    {
        return finder("A1 ? ? ? ? 89 45 BC 85 C0"_pat).add(1).deref().as<csgo::EntityPOD**>();
    }

    [[nodiscard]] ReturnAddress keyValuesAlloc() const noexcept
    {
        return finder("E8 ? ? ? ? 83 C4 08 84 C0 75 10"_pat).add(1).abs().add(0x3E).as<ReturnAddress>();
    }

    [[nodiscard]] ReturnAddress shouldDrawFog() const noexcept
    {
        return finder("E8 ? ? ? ? 8B 0D ? ? ? ? 0F B6 D0"_pat).add(1).abs().add(82).as<ReturnAddress>();
    }

private:
    const PatternFinder& finder;
};
