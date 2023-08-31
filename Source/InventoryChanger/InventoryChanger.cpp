#include <algorithm>
#include <cassert>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <range/v3/algorithm/search.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/zip.hpp>

#define STBI_ONLY_PNG
#define STBI_NO_FAILURE_STRINGS
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <imgui/imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>
#include <imgui/imgui_stdlib.h>
#include "InventoryChanger.h"
#include "../ProtobufReader.h"
#include "../Texture.h"

#include <nlohmann/json.hpp>

#include <CSGO/PODs/ConVar.h>
#include <CSGO/Constants/ClassId.h>
#include <CSGO/Client.h>
#include <CSGO/ClientClass.h>
#include <CSGO/ConVar.h>
#include <CSGO/Cvar.h>
#include <CSGO/EconItemView.h>
#include <CSGO/Entity.h>
#include <CSGO/EntityList.h>
#include <CSGO/FileSystem.h>
#include <CSGO/Constants/ConVarNames.h>
#include <CSGO/Constants/FrameStage.h>
#include <CSGO/GameEvent.h>
#include <CSGO/GlobalVars.h>
#include <CSGO/ItemSchema.h>
#include <CSGO/LocalPlayer.h>
#include <CSGO/MemAlloc.h>
#include <CSGO/ModelInfo.h>
#include <CSGO/Panorama.h>
#include <CSGO/PlayerResource.h>
#include <CSGO/WeaponId.h>
#include <CSGO/PanoramaMarshallHelper.h>
#include <CSGO/CSPlayerInventory.h>
#include "../Helpers.h"

#include "GameItems/Lookup.h"
#include "Inventory/Item.h"
#include "Inventory/Structs.h"
#include "Backend/Loadout.h"
#include "Backend/BackendSimulator.h"
#include "ItemGenerator/ItemGenerator.h"
#include "Backend/Response/ResponseHandler.h"
#include "Backend/Request/RequestBuilder.h"
#include "GameIntegration/CrateLoot.h"
#include "GameIntegration/Inventory.h"
#include "GameIntegration/Items.h"
#include "../Hooks.h"
#include "WeaponNames.h"

#include <SortFilter.h>

#include <Interfaces/ClientInterfaces.h>
#include <Interfaces/OtherInterfaces.h>

#if IS_WIN32()
static csgo::EntityPOD* createGloves(const ClientInterfaces& clientInterfaces, csgo::MemAllocPOD* memAlloc) noexcept
{
    static const auto createWearable = [&clientInterfaces] {
        std::uintptr_t createWearableFn = 0;
        for (auto clientClass = clientInterfaces.getClient().getAllClasses(); clientClass; clientClass = clientClass->next) {
            if (clientClass->classId == ClassId::EconWearable) {
                createWearableFn = std::uintptr_t(clientClass->createFunction);
                break;
            }
        }
        return createWearableFn;
    }();

    const auto sizeOfEconWearable = *reinterpret_cast<std::uint32_t*>(std::uintptr_t(createWearable) + 39);

    const auto econWearable = csgo::MemAlloc::from(retSpoofGadgets->client, memAlloc).allocAligned(sizeOfEconWearable, 16);
    if (!econWearable)
        return nullptr;

    std::memset(econWearable, 0, sizeOfEconWearable);

    const auto econWearableConstructor = SafeAddress{ reinterpret_cast<std::byte*>(createWearable) + 61 }.abs().as<std::uintptr_t>();
    retSpoofGadgets->client.invokeThiscall<void>(std::uintptr_t(econWearable), econWearableConstructor);

    csgo::Entity::from(retSpoofGadgets->client, static_cast<csgo::EntityPOD*>(econWearable)).initializeAsClientEntity(nullptr, false);
    return static_cast<csgo::EntityPOD*>(econWearable);
}

#else
static csgo::EntityPOD* createGlove(const ClientInterfaces& clientInterfaces, int entry, int serial) noexcept
{
    static const auto createWearable = [&clientInterfaces]{
        std::add_pointer_t<csgo::EntityPOD* CDECL_CONV(int, int)> createWearableFn = nullptr;
        for (auto clientClass = clientInterfaces.getClient().getAllClasses(); clientClass; clientClass = clientClass->next) {
            if (clientClass->classId == ClassId::EconWearable) {
                createWearableFn = clientClass->createFunction;
                break;
            }
        }
        return createWearableFn;
    }();

    if (!createWearable)
        return nullptr;

    if (const auto wearable = createWearable(entry, serial))
        return reinterpret_cast<csgo::EntityPOD*>(std::uintptr_t(wearable) - 2 * sizeof(std::uintptr_t));
    return nullptr;
}
#endif

static std::optional<std::list<inventory_changer::inventory::Item>::const_iterator> getItemFromLoadout(const inventory_changer::backend::Loadout& loadout, csgo::Team team, std::uint8_t slot)
{
    switch (team) {
    case csgo::Team::None: return loadout.getItemInSlotNoTeam(slot);
    case csgo::Team::CT: return loadout.getItemInSlotCT(slot);
    case csgo::Team::TT: return loadout.getItemInSlotTT(slot);
    default: return {};
    }
}

static void applyGloves(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const inventory_changer::backend::BackendSimulator& backend, const csgo::CSPlayerInventory& localInventory, const csgo::Entity& local) noexcept
{
    const auto optionalItem = getItemFromLoadout(backend.getLoadout(), local.getTeamNumber(), 41);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;
    const auto itemID = backend.getItemID(item);
    if (!itemID.has_value())
        return;

    const auto wearables = local.wearables();
    static int gloveHandle = 0;

    auto glovePtr = clientInterfaces.getEntityList().getEntityFromHandle(wearables[0]);
    if (!glovePtr)
        glovePtr = clientInterfaces.getEntityList().getEntityFromHandle(gloveHandle);

    if (!glovePtr) {
#if IS_WIN32()
        glovePtr = createGloves(clientInterfaces, memory.memAlloc);
#else
        constexpr auto NUM_ENT_ENTRIES = 8192;
        glovePtr = createGlove(clientInterfaces, NUM_ENT_ENTRIES - 1, -1);
#endif
    }

    if (!glovePtr)
        return;

    const auto glove = csgo::Entity::from(retSpoofGadgets->client, glovePtr);
    wearables[0] = gloveHandle = glove.handle();
    glove.accountID() = localInventory.getAccountID();
    glove.entityQuality() = 3;
    local.body() = 1;

    bool dataUpdated = false;
    if (auto& definitionIndex = glove.itemDefinitionIndex(); definitionIndex != item->gameItem().getWeaponID()) {
        definitionIndex = item->gameItem().getWeaponID();

        if (const auto def = csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()).getItemDefinitionInterface(item->gameItem().getWeaponID()))
            glove.setModelIndex(engineInterfaces.getModelInfo().getModelIndex(csgo::EconItemDefinition::from(retSpoofGadgets->client, def).getWorldDisplayModel()));

        dataUpdated = true;
    }

    if (glove.itemID() != static_cast<csgo::ItemId>(*itemID)) {
        glove.itemIDHigh() = std::uint32_t(static_cast<csgo::ItemId>(*itemID) >> 32);
        glove.itemIDLow() = std::uint32_t(static_cast<csgo::ItemId>(*itemID) & 0xFFFFFFFF);
        dataUpdated = true;
    }

    glove.initialized() = true;
    memory.equipWearable(glove.getPOD(), local.getPOD());

    if (dataUpdated) {
        // FIXME: This leaks game memory
        glove.econItemView().visualDataProcessors().size = 0;
        glove.econItemView().customMaterials().size = 0;
        //

        glove.getNetworkable().postDataUpdate(0);
        glove.getNetworkable().onDataChanged(0);
    }
}

static void applyKnife(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const inventory_changer::backend::BackendSimulator& backend, const csgo::CSPlayerInventory& localInventory, const csgo::Entity& local) noexcept
{
    const auto localXuid = local.getSteamId(engineInterfaces.getEngine());

    const auto optionalItem = getItemFromLoadout(backend.getLoadout(), local.getTeamNumber(), 0);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;
    const auto itemID = backend.getItemID(item);
    if (!itemID.has_value())
        return;

    for (auto& weapons = local.weapons(); auto weaponHandle : weapons) {
        if (weaponHandle == -1)
            break;

        const auto weapon = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntityFromHandle(weaponHandle));
        if (weapon.getPOD() == nullptr)
            continue;

        auto& definitionIndex = weapon.itemDefinitionIndex();
        if (!Helpers::isKnife(definitionIndex))
            continue;

        if (weapon.originalOwnerXuid() != localXuid)
            continue;

        weapon.accountID() = localInventory.getAccountID();
        weapon.itemIDHigh() = std::uint32_t(static_cast<csgo::ItemId>(*itemID) >> 32);
        weapon.itemIDLow() = std::uint32_t(static_cast<csgo::ItemId>(*itemID) & 0xFFFFFFFF);
        weapon.entityQuality() = 3;

        if (definitionIndex != item->gameItem().getWeaponID()) {
            definitionIndex = item->gameItem().getWeaponID();

            if (const auto def = csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()).getItemDefinitionInterface(item->gameItem().getWeaponID())) {
                weapon.setModelIndex(engineInterfaces.getModelInfo().getModelIndex(csgo::EconItemDefinition::from(retSpoofGadgets->client, def).getPlayerDisplayModel()));
                weapon.getNetworkable().preDataUpdate(0);
            }
        }
    }

    const auto viewModel = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntityFromHandle(local.viewModel()));
    if (viewModel.getPOD() == nullptr)
        return;

    const auto viewModelWeapon = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntityFromHandle(viewModel.weapon()));
    if (viewModelWeapon.getPOD() == nullptr)
        return;

    const auto def = csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()).getItemDefinitionInterface(viewModelWeapon.itemDefinitionIndex());
    if (!def)
        return;

    viewModel.modelIndex() = engineInterfaces.getModelInfo().getModelIndex(csgo::EconItemDefinition::from(retSpoofGadgets->client, def).getPlayerDisplayModel());

    const auto worldModel = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntityFromHandle(viewModelWeapon.weaponWorldModel()));
    if (worldModel.getPOD() == nullptr)
        return;

    worldModel.modelIndex() = engineInterfaces.getModelInfo().getModelIndex(csgo::EconItemDefinition::from(retSpoofGadgets->client, def).getWorldDisplayModel());
}

static void applyWeapons(const inventory_changer::InventoryChanger& inventoryChanger, const csgo::Engine& engine, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const csgo::CSPlayerInventory& localInventory, const csgo::Entity& local) noexcept
{
    const auto localTeam = local.getTeamNumber();
    const auto localXuid = local.getSteamId(engine);
    const auto itemSchema = csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema());

    const auto highestEntityIndex = clientInterfaces.getEntityList().getHighestEntityIndex();
    for (int i = memory.globalVars->maxClients + 1; i <= highestEntityIndex; ++i) {
        const auto entity = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntity(i));
        if (entity.getPOD() == nullptr || !entity.isWeapon())
            continue;

        const auto weapon = entity;
        if (weapon.originalOwnerXuid() != localXuid)
            continue;

        const auto& definitionIndex = weapon.itemDefinitionIndex();
        if (Helpers::isKnife(definitionIndex))
            continue;

        const auto def = itemSchema.getItemDefinitionInterface(definitionIndex);
        if (!def)
            continue;

        const auto loadoutSlot = csgo::EconItemDefinition::from(retSpoofGadgets->client, def).getLoadoutSlot(localTeam);
        const auto optionalItem = getItemFromLoadout(inventoryChanger.getBackend().getLoadout(), localTeam, loadoutSlot);
        if (!optionalItem.has_value())
            continue;
        
        const auto& item = *optionalItem;
        if (definitionIndex != item->gameItem().getWeaponID())
            continue;

        const auto itemID = inventoryChanger.getBackend().getItemID(item);
        if (!itemID.has_value())
            continue;

        weapon.accountID() = localInventory.getAccountID();
        weapon.itemIDHigh() = std::uint32_t(static_cast<csgo::ItemId>(*itemID) >> 32);
        weapon.itemIDLow() = std::uint32_t(static_cast<csgo::ItemId>(*itemID) & 0xFFFFFFFF);
    }
}

static void onPostDataUpdateStart(const inventory_changer::InventoryChanger& inventoryChanger, const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, int localHandle) noexcept
{
    const auto local = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntityFromHandle(localHandle));
    if (local.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    applyKnife(engineInterfaces, clientInterfaces, interfaces, memory, inventoryChanger.getBackend(), localInventory, local);
    applyWeapons(inventoryChanger, engineInterfaces.getEngine(), clientInterfaces, interfaces, memory, localInventory, local);
}

static bool hudUpdateRequired{ false };

static void updateHud(const Memory& memory) noexcept
{
    if (auto hud_weapons = memory.findHudElement(memory.hud, "CCSGO_HudWeaponSelection") - WIN32_LINUX(0x28, 62)) {
        for (int i = 0; i < *(hud_weapons + WIN32_LINUX(32, 52)); i++)
            i = memory.clearHudWeapon(hud_weapons, i);
    }
    hudUpdateRequired = false;
}

static void applyMusicKit(const Memory& memory, const inventory_changer::backend::BackendSimulator& backend) noexcept
{
    if (!localPlayer)
        return;

    const auto pr = *memory.playerResource;
    if (pr == nullptr)
        return;

    const auto optionalItem = backend.getLoadout().getItemInSlotNoTeam(54);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;
    if (!item->gameItem().isMusic())
        return;

    pr->musicID()[localPlayer.get().getNetworkable().index()] = backend.getGameItemLookup().getStorage().getMusicKit(item->gameItem()).id;
}

static void applyPlayerAgent(const inventory_changer::InventoryChanger& inventoryChanger, const csgo::ModelInfo& modelInfo, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory) noexcept
{
    if (!localPlayer)
        return;

    const auto optionalItem = getItemFromLoadout(inventoryChanger.getBackend().getLoadout(), localPlayer.get().getTeamNumber(), 38);
    if (!optionalItem.has_value())
        return;

    const auto item = *optionalItem;
    if (!item->gameItem().isAgent())
        return;

    const auto def = csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()).getItemDefinitionInterface(item->gameItem().getWeaponID());
    if (!def)
        return;

    const auto model = csgo::EconItemDefinition::from(retSpoofGadgets->client, def).getPlayerDisplayModel();
    if (!model)
        return;

    if (const auto agent = get<inventory_changer::inventory::Agent>(*item)) {
        for (std::size_t i = 0; i < agent->patches.size(); ++i) {
            if (const auto& patch = agent->patches[i]; patch.patchID != 0)
                localPlayer.get().playerPatchIndices()[i] = patch.patchID;
        }
    }

    const auto idx = modelInfo.getModelIndex(model);
    localPlayer.get().setModelIndex(idx);

    if (const auto ragdoll = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntityFromHandle(localPlayer.get().ragdoll())); ragdoll.getPOD() != nullptr)
        ragdoll.setModelIndex(idx);
}

static void applyMedal(const Memory& memory, const inventory_changer::backend::Loadout& loadout) noexcept
{
    if (!localPlayer)
        return;

    const auto pr = *memory.playerResource;
    if (!pr)
        return;

    const auto optionalItem = loadout.getItemInSlotNoTeam(55);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;
    if (!item->gameItem().isCollectible() && !item->gameItem().isServiceMedal() && !item->gameItem().isTournamentCoin())
        return;

    pr->activeCoinRank()[localPlayer.get().getNetworkable().index()] = static_cast<int>(item->gameItem().getWeaponID());
}

struct EquipRequest {
    std::chrono::steady_clock::time_point time;
    std::uint64_t itemID;
    WeaponId weaponID;
    std::uint8_t counter = 0;
};
static std::vector<EquipRequest> equipRequests;

static void simulateItemUpdate(const Memory& memory, const EconItemViewFunctions& econItemViewFunctions, std::uint64_t itemID)
{
    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    if (const auto view = csgo::EconItemView::from(retSpoofGadgets->client, memory.findOrCreateEconItemViewForItemID(itemID), econItemViewFunctions); view.getPOD() != nullptr) {
        if (const auto soc = view.getSOCData())
            localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)soc, 4);
    }
}

static void processEquipRequests(const Memory& memory, const EconItemViewFunctions& econItemViewFunctions)
{
    const auto now = std::chrono::steady_clock::now();
    for (auto it = equipRequests.begin(); it != equipRequests.end();) {
        if (now - it->time >= std::chrono::milliseconds{ 500 }) {
            if (it->counter == 0)
                simulateItemUpdate(memory, econItemViewFunctions, it->itemID);
            it = equipRequests.erase(it);
        } else {
            ++it;
        }
    }
}

[[nodiscard]] static bool isLocalPlayerMVP(const csgo::Engine& engine, const csgo::GameEvent& event)
{
    return localPlayer && localPlayer.get().getUserId(engine) == event.getInt("userid");
}

static bool windowOpen = false;

void inventory_changer::InventoryChanger::menuBarItem() noexcept
{
    if (ImGui::MenuItem("Inventory Changer")) {
        windowOpen = true;
        ImGui::SetWindowFocus("Inventory Changer");
        ImGui::SetWindowPos("Inventory Changer", { 100.0f, 100.0f });
    }
}

void inventory_changer::InventoryChanger::tabItem(const Memory& memory) noexcept
{
    if (ImGui::BeginTabItem("Inventory Changer")) {
        drawGUI(memory, true);
        ImGui::EndTabItem();
    }
}

static ImTextureID getItemIconTexture(const OtherInterfaces& interfaces, std::string_view iconpath) noexcept;

namespace ImGui
{
    static bool SkinSelectable(const inventory_changer::InventoryChanger& inventoryChanger, const OtherInterfaces& interfaces, const Memory& memory, const inventory_changer::game_items::Item& item, const ImVec2& iconSizeSmall, const ImVec2& iconSizeLarge, ImU32 rarityColor, int* toAddCount = nullptr) noexcept
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const auto itemName = inventory_changer::WeaponNames::instance(interfaces, memory).getWeaponName(item.getWeaponID()).data();
        const auto itemNameSize = CalcTextSize(itemName, nullptr);

        const auto paintKitName = getItemName(inventoryChanger.getGameItemLookup().getStorage(), item).forDisplay.data();
        const auto paintKitNameSize = CalcTextSize(paintKitName, nullptr);

        PushID(itemName);
        PushID(paintKitName);
        const auto id = window->GetID(0);
        PopID();
        PopID();

        const auto height = ImMax(paintKitNameSize.y, ImMax(itemNameSize.y, iconSizeSmall.y));
        const auto rarityBulletRadius = IM_FLOOR(height * 0.2f);
        const auto size = ImVec2{ iconSizeSmall.x + rarityBulletRadius * 2.0f + itemNameSize.x + paintKitNameSize.x, height };
        
        ImVec2 pos = window->DC.CursorPos;
        pos.y += window->DC.CurrLineTextBaseOffset;
        ItemSize(size, 0.0f);

        const auto smallIconMin = pos;
        const auto smallIconMax = smallIconMin + iconSizeSmall;

        const auto rarityBulletPos = ImVec2{ pos.x + iconSizeSmall.x + 5.0f + rarityBulletRadius, pos.y + IM_FLOOR(size.y * 0.5f) };

        const auto itemNameMin = ImVec2{ rarityBulletPos.x + rarityBulletRadius + 5.0f, pos.y };
        const auto itemNameMax = itemNameMin + ImVec2{ itemNameSize.x, size.y };
        
        const auto separatorHeightInv = IM_FLOOR(height * 0.2f);
        const auto separatorMin = ImVec2{ itemNameMax.x + 5.0f, pos.y + separatorHeightInv };
        const auto separatorMax = separatorMin + ImVec2{ 1.0f, height - 2.0f * separatorHeightInv };

        const auto paintKitNameMin = ImVec2{ separatorMax.x + 5.0f, pos.y };
        const auto paintKitNameMax = paintKitNameMin + ImVec2{ paintKitNameSize.x, size.y };

        // Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
        ImRect bb(pos, pos + ImVec2{ ImMax(size.x, window->WorkRect.Max.x - pos.x), size.y });
        const float spacingX = style.ItemSpacing.x;
        const float spacingY = style.ItemSpacing.y;
        const float spacingL = IM_FLOOR(spacingX * 0.50f);
        const float spacingU = IM_FLOOR(spacingY * 0.50f);
        bb.Min.x -= spacingL;
        bb.Min.y -= spacingU;
        bb.Max.x += (spacingX - spacingL);
        bb.Max.y += (spacingY - spacingU);

        if (!ItemAdd(bb, id))
            return false;

        const ImRect selectableBB{ bb.Min, ImVec2{ bb.Max.x - 130.0f, bb.Max.y} };
        // We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
        ImGuiButtonFlags buttonFlags = 0;
        bool hovered, held;
        bool pressed = false;
        ButtonBehavior(selectableBB, id, &hovered, &held, buttonFlags);

        // Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
        if (pressed) {
            if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent) {
                SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent, ImRect(bb.Min - window->Pos, bb.Max - window->Pos));
                g.NavDisableHighlight = true;
            }
            MarkItemEdited(id);
        }

        if (hovered) {
            const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
            RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
            RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);
        }

        if (const auto icon = getItemIconTexture(interfaces, item.getIconPath())) {
            window->DrawList->AddImage(icon, smallIconMin, smallIconMax);
            if (g.HoveredWindow == window && IsMouseHoveringRect(bb.Min, ImVec2{ bb.Min.x + iconSizeSmall.x, bb.Max.y })) {
                BeginTooltip();
                Image(icon, iconSizeLarge);
                EndTooltip();
            }
        }

        window->DrawList->AddCircleFilled(rarityBulletPos, rarityBulletRadius + 1.0f, IM_COL32(0, 0, 0, (std::min)(120u, (rarityColor & IM_COL32_A_MASK))), 12);
        window->DrawList->AddCircleFilled(rarityBulletPos, rarityBulletRadius, rarityColor, 12);

        RenderTextClipped(itemNameMin, itemNameMax, itemName, nullptr, &itemNameSize, { 0.0f, 0.5f }, &bb);
        if (paintKitName[0] != '\0')
            window->DrawList->AddRectFilled(separatorMin, separatorMax, GetColorU32(ImGuiCol_Text));
        RenderTextClipped(paintKitNameMin, paintKitNameMax, paintKitName, nullptr, &paintKitNameSize, { 0.0f, 0.5f }, &bb);

        if (toAddCount) {
            const auto cursorPosNext = window->DC.CursorPos.y;
            SameLine(window->WorkRect.Max.x - pos.x - 130.0f);
            const auto cursorPosBackup = window->DC.CursorPos.y;

            window->DC.CursorPos.y += (size.y - GetFrameHeight()) * 0.5f;
            SetNextItemWidth(80.0f);
            InputInt("", toAddCount);
            *toAddCount = (std::max)(*toAddCount, 1);

            window->DC.CursorPosPrevLine.y = cursorPosBackup;
            window->DC.CursorPos.y = cursorPosNext;
        }

        {
            const auto cursorPosNext = window->DC.CursorPos.y;
            SameLine(window->WorkRect.Max.x - pos.x - 40.0f);
            const auto cursorPosBackup = window->DC.CursorPos.y;

            window->DC.CursorPos.y += (size.y - GetFrameHeight()) * 0.5f;
            pressed = Button("Add");

            window->DC.CursorPosPrevLine.y = cursorPosBackup;
            window->DC.CursorPos.y = cursorPosNext;
        }

        if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
            CloseCurrentPopup();

        return pressed;
    }

    static void SkinItem(const inventory_changer::InventoryChanger& inventoryChanger, const OtherInterfaces& interfaces, const Memory& memory, const inventory_changer::game_items::Item& item, const ImVec2& iconSizeSmall, const ImVec2& iconSizeLarge, ImU32 rarityColor, bool& shouldDelete) noexcept
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return;

        const ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const auto itemName = inventory_changer::WeaponNames::instance(interfaces, memory).getWeaponName(item.getWeaponID()).data();
        const auto itemNameSize = CalcTextSize(itemName, nullptr);

        const auto paintKitName = getItemName(inventoryChanger.getGameItemLookup().getStorage(), item).forDisplay.data();
        const auto paintKitNameSize = CalcTextSize(paintKitName, nullptr);

        PushID(itemName);
        PushID(paintKitName);
        const auto id = window->GetID(0);
        PopID();
        PopID();

        const auto height = ImMax(paintKitNameSize.y, ImMax(itemNameSize.y, iconSizeSmall.y));
        const auto rarityBulletRadius = IM_FLOOR(height * 0.2f);
        const auto size = ImVec2{ iconSizeSmall.x + rarityBulletRadius * 2.0f + itemNameSize.x + paintKitNameSize.x, height };

        ImVec2 pos = window->DC.CursorPos;
        pos.y += window->DC.CurrLineTextBaseOffset;
        ItemSize(size, 0.0f);

        const auto smallIconMin = pos;
        const auto smallIconMax = smallIconMin + iconSizeSmall;

        const auto rarityBulletPos = ImVec2{ pos.x + iconSizeSmall.x + 5.0f + rarityBulletRadius, pos.y + IM_FLOOR(size.y * 0.5f) };

        const auto itemNameMin = ImVec2{ rarityBulletPos.x + rarityBulletRadius + 5.0f, pos.y };
        const auto itemNameMax = itemNameMin + ImVec2{ itemNameSize.x, size.y };

        const auto separatorHeightInv = IM_FLOOR(height * 0.2f);
        const auto separatorMin = ImVec2{ itemNameMax.x + 5.0f, pos.y + separatorHeightInv };
        const auto separatorMax = separatorMin + ImVec2{ 1.0f, height - 2.0f * separatorHeightInv };

        const auto paintKitNameMin = ImVec2{ separatorMax.x + 5.0f, pos.y };
        const auto paintKitNameMax = paintKitNameMin + ImVec2{ paintKitNameSize.x, size.y };

        // Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
        ImRect bb(pos, pos + ImVec2{ ImMax(size.x, window->WorkRect.Max.x - pos.x), size.y });
        const float spacingX = style.ItemSpacing.x;
        const float spacingY = style.ItemSpacing.y;
        const float spacingL = IM_FLOOR(spacingX * 0.50f);
        const float spacingU = IM_FLOOR(spacingY * 0.50f);
        bb.Min.x -= spacingL;
        bb.Min.y -= spacingU;
        bb.Max.x += (spacingX - spacingL);
        bb.Max.y += (spacingY - spacingU);

        if (!ItemAdd(bb, id))
            return;

        if (const bool hovered = (g.HoveredWindow == window && IsMouseHoveringRect(bb.Min, bb.Max))) {
            const ImU32 col = GetColorU32(ImGuiCol_HeaderHovered);
            RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
            RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);
        }

        if (const auto icon = getItemIconTexture(interfaces, item.getIconPath())) {
            window->DrawList->AddImage(icon, smallIconMin, smallIconMax);
            if (g.HoveredWindow == window && IsMouseHoveringRect(bb.Min, ImVec2{ bb.Min.x + iconSizeSmall.x, bb.Max.y })) {
                BeginTooltip();
                Image(icon, iconSizeLarge);
                EndTooltip();
            }
        }

        window->DrawList->AddCircleFilled(rarityBulletPos, rarityBulletRadius + 1.0f, IM_COL32(0, 0, 0, (std::min)(120u, (rarityColor & IM_COL32_A_MASK))), 12);
        window->DrawList->AddCircleFilled(rarityBulletPos, rarityBulletRadius, rarityColor, 12);

        RenderTextClipped(itemNameMin, itemNameMax, itemName, nullptr, &itemNameSize, { 0.0f, 0.5f }, &bb);
        if (paintKitName[0] != '\0')
            window->DrawList->AddRectFilled(separatorMin, separatorMax, GetColorU32(ImGuiCol_Text));
        RenderTextClipped(paintKitNameMin, paintKitNameMax, paintKitName, nullptr, &paintKitNameSize, { 0.0f, 0.5f }, &bb);

        const auto removeButtonSize = CalcTextSize("Delete", nullptr) + style.FramePadding * 2.0f;
        const auto cursorPosNext = window->DC.CursorPos.y;
        SameLine(window->WorkRect.Max.x - pos.x - removeButtonSize.x - 7.0f);
        const auto cursorPosBackup = window->DC.CursorPos.y;

        window->DC.CursorPos.y += (size.y - GetFrameHeight()) * 0.5f;
        if (Button("Delete"))
            shouldDelete = true;

        window->DC.CursorPosPrevLine.y = cursorPosBackup;
        window->DC.CursorPos.y = cursorPosNext;
    }
}

namespace inventory_changer
{

[[nodiscard]] InventoryChangerReturnAddresses createInventoryChangerReturnAddresses(const ClientPatternFinder& clientPatternFinder)
{
    return InventoryChangerReturnAddresses{
        .setStickerToolSlotGetArgAsNumber = clientPatternFinder.setStickerToolSlotGetArgAsNumber(),
        .wearItemStickerGetArgAsString = clientPatternFinder.wearItemStickerGetArgAsString(),
        .setNameToolStringGetArgAsString = clientPatternFinder.setNameToolStringGetArgAsString(),
        .clearCustomNameGetArgAsString = clientPatternFinder.clearCustomNameGetArgAsString(),
        .deleteItemGetArgAsString = clientPatternFinder.deleteItemGetArgAsString(),
        .setStatTrakSwapToolItemsGetArgAsString = clientPatternFinder.setStatTrakSwapToolItemsGetArgAsString(),
        .acknowledgeNewItemByItemIDGetArgAsString = clientPatternFinder.acknowledgeNewItemByItemIDGetArgAsString(),
        .setItemAttributeValueAsyncGetArgAsString = clientPatternFinder.setItemAttributeValueAsyncGetArgAsString(),
        .setMyPredictionUsingItemIdGetNumArgs = clientPatternFinder.setMyPredictionUsingItemIdGetNumArgs(),
        .getMyPredictionTeamIDGetArgAsString = clientPatternFinder.getMyPredictionTeamIDGetArgAsString(),
        .setInventorySortAndFiltersGetArgAsString = clientPatternFinder.setInventorySortAndFiltersGetArgAsString(),
        .getInventoryCountSetResultInt = clientPatternFinder.getInventoryCountSetResultInt(),
        .performItemCasketTransactionGetArgAsString = clientPatternFinder.performItemCasketTransactionGetArgAsString(),
        .useToolGetArgAsString = clientPatternFinder.useToolGetArgAsString()
    };
}

struct NameComparator {
    NameComparator(const game_items::Storage& gameItemStorage, const WeaponNames& weaponNames)
        : gameItemStorage{ gameItemStorage }, weaponNames{ weaponNames } {}

    [[nodiscard]] bool operator()(const game_items::Item& a, const game_items::Item& b) const
    {
         if (a.getWeaponID() == b.getWeaponID())
            return getItemName(gameItemStorage, a).forSearch < getItemName(gameItemStorage, b).forSearch;
        const auto comp = weaponNames.getWeaponNameUpper(a.getWeaponID()).compare(weaponNames.getWeaponNameUpper(b.getWeaponID()));
        if (comp == 0)
            return a.getWeaponID() < b.getWeaponID();
        return comp < 0;
    }

private:
    const game_items::Storage& gameItemStorage;
    const WeaponNames& weaponNames;
};

void InventoryChanger::scheduleHudUpdate() noexcept
{
    otherInterfaces.getCvar().findVar(csgo::cl_fullupdate)->changeCallback();
    hudUpdateRequired = true;
}

void InventoryChanger::drawGUI(const Memory& memory, bool contentOnly)
{
    if (!contentOnly) {
        if (!windowOpen)
            return;
        ImGui::SetNextWindowSize({ 700.0f, 400.0f });
        if (!ImGui::Begin("Inventory Changer", &windowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
            ImGui::End();
            return;
        }
    }

    static std::string filter;

    static bool isInAddMode = false;

    if (!isInAddMode && ImGui::Button("Add items.."))
        isInAddMode = true;

    if (!isInAddMode) {
        ImGui::SameLine();
        if (ImGui::Button("Force Update"))
            scheduleHudUpdate();
    }

    constexpr auto rarityColor = [](EconRarity rarity) noexcept {
        constexpr auto rarityColors = std::to_array<ImU32>({
            IM_COL32(106,  97,  85, 255),
            IM_COL32(176, 195, 217, 255),
            IM_COL32( 94, 152, 217, 255),
            IM_COL32( 75, 105, 255, 255),
            IM_COL32(136,  71, 255, 255),
            IM_COL32(211,  44, 230, 255),
            IM_COL32(235,  75,  75, 255),
            IM_COL32(228, 174,  57, 255)
            });
        return rarityColors[static_cast<std::size_t>(rarity) < rarityColors.size() ? static_cast<std::size_t>(rarity) : 0];
    };

    if (isInAddMode) {
        if (ImGui::Button("Back")) {
            isInAddMode = false;
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(550.0f);
        const bool filterChanged = ImGui::InputTextWithHint("##search", "Search weapon skins, stickers, knives, gloves, music kits..", &filter);
        ImGui::SameLine();
        const bool addingAll = ImGui::Button("Add all in list");

        constexpr auto passesFilter = []<typename... Strings>(std::wstring_view filter, Strings&&... strings) {
            for (const auto filterWord : ranges::views::split(filter, L' ')) {
                if ((ranges::search(strings, filterWord).empty() && ...))
                    return false;
            }
            return true;
        };

        static SortFilter gameItemList{ getGameItemLookup().getStorage().getItems() };

        if (filterChanged) {
            const std::wstring filterWide{ Helpers::ToUpperConverter{}.toUpper(Helpers::toWideString(filter)) };

            gameItemList.filter([&passesFilter, &filterWide, &weaponNames = inventory_changer::WeaponNames::instance(otherInterfaces, memory), &gameItemStorage = getGameItemLookup().getStorage()](const inventory_changer::game_items::Item& item) {
                return filterWide.empty() || passesFilter(filterWide, weaponNames.getWeaponNameUpper(item.getWeaponID()), getItemName(gameItemStorage, item).forSearch);
            });
        }

        if (ImGui::BeginChild("##scrollarea", ImVec2{ 0.0f, contentOnly ? 400.0f : 0.0f })) {
            static std::vector<int> toAddCount(gameItemList.totalItemCount(), 1);

            if (static bool sorted = false; !sorted) {
                gameItemList.sort(inventory_changer::NameComparator{ getGameItemLookup().getStorage(), inventory_changer::WeaponNames::instance(otherInterfaces, memory) });
                sorted = true;
            }

            for (const auto& [i, gameItem] : gameItemList.getItems()) {
                if (addingAll) {
                    backend.getInventoryHandler().addItem(inventory::Item{ gameItem, backend.getItemGenerator().createDefaultItemProperties(gameItem) }, true);
                }

                ImGui::PushID(i);

                if (ImGui::SkinSelectable(*this, otherInterfaces, memory, gameItem, { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(gameItem.getRarity()), &toAddCount[i])) {
                    for (int j = 0; j < toAddCount[i]; ++j)
                        backend.getInventoryHandler().addItem(inventory::Item{ gameItem, backend.getItemGenerator().createDefaultItemProperties(gameItem) }, true);
                    toAddCount[i] = 1;
                }
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    } else {
        if (ImGui::BeginChild("##scrollarea2", ImVec2{ 0.0f, contentOnly ? 400.0f : 0.0f })) {
            const auto& inventory = backend.getInventory();
            std::size_t i = 0;
            for (auto it = inventory.rbegin(); it != inventory.rend();) {
                if (it->getState() != inventory::Item::State::Default) {
                    ++it;
                    continue;
                }

                ImGui::PushID(i);
                bool shouldDelete = false;
                ImGui::SkinItem(*this, otherInterfaces, memory, it->gameItem(), { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(it->gameItem().getRarity()), shouldDelete);
                if (shouldDelete) {
                    it = std::make_reverse_iterator(backend.getItemRemovalHandler()(std::next(it).base()));
                } else {
                    ++it;
                }
                ImGui::PopID();
                ++i;
            }
        }
        ImGui::EndChild();
    }

    if (!contentOnly)
        ImGui::End();
}

}

[[nodiscard]] static bool isDefaultKnifeNameLocalizationString(std::string_view string) noexcept
{
    return string == "#SFUI_WPNHUD_Knife" || string == "#SFUI_WPNHUD_Knife_T";
}

static void appendProtobufString(std::string_view string, std::vector<char>& buffer) noexcept
{
    assert(string.length() < 128);
    buffer.push_back(0x1A);
    buffer.push_back(static_cast<char>(string.length()));
    std::ranges::copy(string, std::back_inserter(buffer));
}

[[nodiscard]] static std::vector<char> buildTextUserMessage(int destination, std::string_view string1, std::string_view string2, std::string_view string3 = {}) noexcept
{
    std::vector<char> buffer{ 0x8, static_cast<char>(destination) };
    appendProtobufString(string1, buffer);
    appendProtobufString(string2, buffer);
    appendProtobufString(string3, buffer);
    // game client expects text protobuf to contain 5 strings
    appendProtobufString("", buffer);
    appendProtobufString("", buffer);
    return buffer;
}

static std::uint64_t stringToUint64(std::string_view str) noexcept
{
    std::uint64_t result = 0;
    std::from_chars(str.data(), str.data() + str.size(), result);
    return result;
}

struct Icon {
    Texture texture;
    int lastReferencedFrame = 0;
};

static std::unordered_map<std::string, Icon> iconTextures;

static ImTextureID getItemIconTexture(const OtherInterfaces& interfaces, std::string_view iconpath) noexcept
{
    if (iconpath.empty())
        return 0;

    auto& icon = iconTextures[std::string{ iconpath }];
    if (!icon.texture.get()) {
        static int frameCount = 0;
        static float timeSpentThisFrame = 0.0f;
        static int loadedThisFrame = 0;

        if (frameCount != ImGui::GetFrameCount()) {
            frameCount = ImGui::GetFrameCount();
            timeSpentThisFrame = 0.0f;
            // memory->debugMsg("LOADED %d ICONS\n", loadedThisFrame);
            loadedThisFrame = 0;
        }

        if (timeSpentThisFrame > 0.01f)
            return 0;

        ++loadedThisFrame;

        const auto start = std::chrono::steady_clock::now();

        auto handle = interfaces.getBaseFileSystem().open(("resource/flash/" + std::string{ iconpath } + (iconpath.find("status_icons") != std::string_view::npos ? "" : "_large") + ".png").c_str(), "r", "GAME");
        if (!handle)
            handle = interfaces.getBaseFileSystem().open(("resource/flash/" + std::string{ iconpath } + ".png").c_str(), "r", "GAME");

        assert(handle);
        if (handle) {
            if (const auto size = interfaces.getBaseFileSystem().size(handle); size > 0) {
                const auto buffer = std::make_unique<std::uint8_t[]>(size);
                if (interfaces.getBaseFileSystem().read(buffer.get(), size, handle) > 0) {
                    int width, height;
                    stbi_set_flip_vertically_on_load_thread(false);

                    if (const auto data = stbi_load_from_memory((const stbi_uc*)buffer.get(), size, &width, &height, nullptr, STBI_rgb_alpha)) {
                        icon.texture.init(width, height, data);
                        stbi_image_free(data);
                    } else {
                        assert(false);
                    }
                }
            }
            interfaces.getBaseFileSystem().close(handle);
        }

        const auto end = std::chrono::steady_clock::now();
        timeSpentThisFrame += std::chrono::duration<float>(end - start).count();
    }
    icon.lastReferencedFrame = ImGui::GetFrameCount();
    return icon.texture.get();
}

void inventory_changer::InventoryChanger::clearItemIconTextures() noexcept
{
    iconTextures.clear();
}

void inventory_changer::InventoryChanger::clearUnusedItemIconTextures() noexcept
{
    constexpr auto maxIcons = 30;
    const auto frameCount = ImGui::GetFrameCount();
    while (iconTextures.size() > maxIcons) {
        const auto oldestIcon = std::ranges::min_element(iconTextures, {}, [](const auto& icon) { return icon.second.lastReferencedFrame; });
        if (oldestIcon->second.lastReferencedFrame == frameCount)
            break;

        iconTextures.erase(oldestIcon);
    }
}

static int remapKnifeAnim(WeaponId weaponID, const int sequence, Helpers::RandomGenerator& randomGenerator) noexcept
{
    enum Sequence
    {
        SEQUENCE_DEFAULT_DRAW = 0,
        SEQUENCE_DEFAULT_IDLE1 = 1,
        SEQUENCE_DEFAULT_IDLE2 = 2,
        SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
        SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
        SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
        SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
        SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
        SEQUENCE_DEFAULT_LOOKAT01 = 12,

        SEQUENCE_BUTTERFLY_DRAW = 0,
        SEQUENCE_BUTTERFLY_DRAW2 = 1,
        SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
        SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

        SEQUENCE_FALCHION_IDLE1 = 1,
        SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
        SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
        SEQUENCE_FALCHION_LOOKAT01 = 12,
        SEQUENCE_FALCHION_LOOKAT02 = 13,

        SEQUENCE_DAGGERS_IDLE1 = 1,
        SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
        SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
        SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
        SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

        SEQUENCE_BOWIE_IDLE1 = 1,
    };

    switch (weaponID) {
    case WeaponId::Butterfly:
        switch (sequence) {
        case SEQUENCE_DEFAULT_DRAW:
            return randomGenerator(std::uniform_int_distribution<>{ SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2 });
        case SEQUENCE_DEFAULT_LOOKAT01:
            return randomGenerator(std::uniform_int_distribution<>{ SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03 });
        default:
            return sequence + 1;
        }
    case WeaponId::Falchion:
        switch (sequence) {
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
        case SEQUENCE_DEFAULT_HEAVY_HIT1:
        case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
            return sequence;
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return randomGenerator(std::uniform_int_distribution<>{ SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP });
        case SEQUENCE_DEFAULT_LOOKAT01:
            return randomGenerator(std::uniform_int_distribution<>{ SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02 });
        default:
            return sequence - 1;
        }
    case WeaponId::Daggers:
        switch (sequence) {
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_DAGGERS_IDLE1;
        case SEQUENCE_DEFAULT_LIGHT_MISS1:
        case SEQUENCE_DEFAULT_LIGHT_MISS2:
            return randomGenerator(std::uniform_int_distribution<>{ SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5 });
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return randomGenerator(std::uniform_int_distribution<>{ SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1 });
        case SEQUENCE_DEFAULT_HEAVY_HIT1:
        case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
        case SEQUENCE_DEFAULT_LOOKAT01:
            return sequence + 3;
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        default:
            return sequence + 2;
        }
    case WeaponId::Bowie:
        switch (sequence) {
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_BOWIE_IDLE1;
        default:
            return sequence - 1;
        }
    case WeaponId::Ursus:
    case WeaponId::SkeletonKnife:
    case WeaponId::NomadKnife:
    case WeaponId::Paracord:
    case WeaponId::SurvivalKnife:
        switch (sequence) {
        case SEQUENCE_DEFAULT_DRAW:
            return randomGenerator(std::uniform_int_distribution<>{ SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2 });
        case SEQUENCE_DEFAULT_LOOKAT01:
            return randomGenerator(std::uniform_int_distribution<>{ SEQUENCE_BUTTERFLY_LOOKAT01, Sequence(14) });
        default:
            return sequence + 1;
        }
    case WeaponId::Stiletto:
        switch (sequence) {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return randomGenerator(std::uniform_int_distribution<>{ 12, 13 });
        }
    case WeaponId::Talon:
        switch (sequence) {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return randomGenerator(std::uniform_int_distribution<>{ 14, 15 });
        }
    default:
        return sequence;
    }
}

namespace inventory_changer
{

void InventoryChanger::onSoUpdated(const csgo::SharedObject& object) noexcept
{
    if (object.getTypeID() == 43 /* = k_EEconTypeDefaultEquippedDefinitionInstanceClient */) {
        WeaponId& weaponID = *reinterpret_cast<WeaponId*>(std::uintptr_t(object.getPOD()) + WIN32_LINUX(0x10, 0x1C));
        if (const auto it = std::ranges::find(equipRequests, weaponID, &EquipRequest::weaponID); it != equipRequests.end()) {
            ++it->counter;
            weaponID = WeaponId{};
        }
    }
}

void InventoryChanger::run(const Memory& memory, csgo::FrameStage stage) noexcept
{
    static int localPlayerHandle = -1;

    if (localPlayer)
        localPlayerHandle = localPlayer.get().handle();

    if (stage == csgo::FrameStage::NET_UPDATE_POSTDATAUPDATE_START) {
        onPostDataUpdateStart(*this, engineInterfaces, clientInterfaces, otherInterfaces, memory, localPlayerHandle);
        if (hudUpdateRequired && localPlayer && !localPlayer.get().getNetworkable().isDormant())
            updateHud(memory);
    }

    if (stage != csgo::FrameStage::RENDER_START)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    if (localPlayer)
        applyGloves(engineInterfaces, clientInterfaces, otherInterfaces, memory, backend, localInventory, localPlayer.get());

    applyMusicKit(memory, backend);
    applyPlayerAgent(*this, engineInterfaces.getModelInfo(), clientInterfaces, otherInterfaces, memory);
    applyMedal(memory, backend.getLoadout());

    processEquipRequests(memory, gameInventory.getEconItemViewFunctions());
    backend.run(gameInventory, std::chrono::milliseconds{ 300 });
}

InventoryChanger createInventoryChanger(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const ClientPatternFinder& clientPatternFinder, Helpers::RandomGenerator& randomGenerator)
{
    auto itemSchema = csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema());
    game_integration::Items items{ itemSchema, interfaces.getLocalize() };
    auto storage = game_integration::createGameItemStorage(interfaces, items);
    storage.compress();
    auto gameItemLookup = game_items::Lookup{ std::move(storage) };

    game_integration::CrateLoot gameLoot{ itemSchema, gameItemLookup };
    game_items::CrateLoot crateLoot;
    gameLoot.getLoot(crateLoot);
    crateLoot.compress();
    auto crateLootLookup = game_items::CrateLootLookup{ std::move(crateLoot) };

    return InventoryChanger{ engineInterfaces, clientInterfaces, interfaces, memory, std::move(gameItemLookup), std::move(crateLootLookup), clientPatternFinder, randomGenerator };
}

void InventoryChanger::getArgAsNumberHook(int number, ReturnAddress returnAddress)
{
    if (returnAddress == returnAddresses.setStickerToolSlotGetArgAsNumber)
        requestBuilderParams.stickerSlot = static_cast<std::uint8_t>(number);
}

void InventoryChanger::onRoundMVP(const csgo::GameEvent& event)
{
    if (!isLocalPlayerMVP(engineInterfaces.getEngine(), event))
        return;

    const auto optionalItem = backend.getLoadout().getItemInSlotNoTeam(54);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;
    const auto music = get<inventory_changer::inventory::Music>(*item);
    if (!music)
        return;

    if (music->statTrak > -1) {
        event.setInt("musickitmvps", music->statTrak + 1);
        backend.getItemModificationHandler().updateStatTrak(item, music->statTrak + 1);
    }
}

void InventoryChanger::updateStatTrak(const csgo::GameEvent& event)
{
    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer.get().getUserId(engineInterfaces.getEngine()); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    const auto weapon = csgo::Entity::from(retSpoofGadgets->client, localPlayer.get().getActiveWeapon());
    if (weapon.getPOD() == nullptr)
        return;

    const auto optionalItem = backend.itemFromID(ItemId{ weapon.itemID() });
    if (!optionalItem.has_value())
        return;

    const auto item = *optionalItem;
    const auto skin = get<inventory::Skin>(*item);
    if (!skin)
        return;

    if (skin->statTrak > -1)
        backend.getItemModificationHandler().updateStatTrak(item, skin->statTrak + 1);
}

void InventoryChanger::overrideHudIcon(const Memory& memory, const csgo::GameEvent& event)
{
    if (!localPlayer)
        return;

    if (event.getInt("attacker") != localPlayer.get().getUserId(engineInterfaces.getEngine()))
        return;

    if (const auto weapon = std::string_view{ event.getString("weapon") }; weapon != "knife" && weapon != "knife_t")
        return;

    const auto optionalItem = getItemFromLoadout(backend.getLoadout(), localPlayer.get().getTeamNumber(), 0);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;

    if (const auto def = csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()).getItemDefinitionInterface(item->gameItem().getWeaponID())) {
        if (const auto defName = csgo::EconItemDefinition::from(retSpoofGadgets->client, def).getDefinitionName(); defName && std::string_view{ defName }.starts_with("weapon_"))
            event.setString("weapon", defName + 7);
    }
}

[[nodiscard]] constexpr std::string_view removePrefix(std::string_view str, std::string_view prefix)
{
    assert(str.starts_with(prefix));
    str.remove_prefix(prefix.length());
    return str;
}

[[nodiscard]] csgo::Tournament extractTournamentFromString(std::string_view s)
{
    if (!s.starts_with("tournament:"))
        return csgo::Tournament{};

    return static_cast<csgo::Tournament>(stringToUint64(removePrefix(s, "tournament:")));
}

void InventoryChanger::getArgAsStringHook(const PanoramaMarshallHelperHooks& panoramaMarshallHelperHooks, const Memory& memory, const char* string, ReturnAddress returnAddress, void* params)
{
    if (returnAddress == returnAddresses.useToolGetArgAsString) {
        const auto toolItemID = stringToUint64(string);
        const auto destItemIdString = panoramaMarshallHelperHooks.getOriginalGetArgAsString()(memory.panoramaMarshallHelper, params, 1);
        if (destItemIdString)
            getRequestBuilder().useToolOn(ItemId{ toolItemID }, ItemId{ stringToUint64(destItemIdString) });
    } else if (returnAddress == returnAddresses.wearItemStickerGetArgAsString) {
        const auto slot = (std::uint8_t)panoramaMarshallHelperHooks.getOriginalGetArgAsNumber()(memory.panoramaMarshallHelper, params, 1);
        getRequestBuilder().wearStickerOf(ItemId{ stringToUint64(string) }, slot);
    } else if (returnAddress == returnAddresses.setNameToolStringGetArgAsString) {
        requestBuilderParams.nameTag = string;
    } else if (returnAddress == returnAddresses.clearCustomNameGetArgAsString) {
        getRequestBuilder().removeNameTagFrom(ItemId{ stringToUint64(string) });
    } else if (returnAddress == returnAddresses.deleteItemGetArgAsString) {
        if (const auto itOptional = backend.itemFromID(ItemId{ stringToUint64(string) }); itOptional.has_value())
            backend.getItemRemovalHandler()(*itOptional);
    } else if (returnAddress == returnAddresses.acknowledgeNewItemByItemIDGetArgAsString) {
        acknowledgeItem(memory, stringToUint64(string));
    } else if (returnAddress == returnAddresses.setStatTrakSwapToolItemsGetArgAsString) {
        const auto swapItem1 = ItemId{ stringToUint64(string) };
        const auto swapItem2String = panoramaMarshallHelperHooks.getOriginalGetArgAsString()(memory.panoramaMarshallHelper, params, 1);
        if (swapItem2String) {
            requestBuilderParams.statTrakSwapItemID1 = swapItem1;
            requestBuilderParams.statTrakSwapItemID2 = ItemId{ stringToUint64(swapItem2String) };
        }
    } else if (returnAddress == returnAddresses.setItemAttributeValueAsyncGetArgAsString) {
        if (const auto itOptional = backend.itemFromID(ItemId{ stringToUint64(string) }); itOptional.has_value() && (*itOptional)->gameItem().isTournamentCoin()) {
            const auto attribute = panoramaMarshallHelperHooks.getOriginalGetArgAsString()(memory.panoramaMarshallHelper, params, 1);
            if (attribute && std::strcmp(attribute, "sticker slot 0 id") == 0) {
                const auto graffitiID = (int)panoramaMarshallHelperHooks.getOriginalGetArgAsNumber()(memory.panoramaMarshallHelper, params, 2);
                backend.getItemModificationHandler().selectTeamGraffiti(*itOptional, static_cast<std::uint16_t>(graffitiID));
            }
        }
    } else if (returnAddress == returnAddresses.getMyPredictionTeamIDGetArgAsString) {
        const auto tournament = extractTournamentFromString(string);
        if (tournament == csgo::Tournament{})
            return;

        const auto groupId = (std::uint16_t)panoramaMarshallHelperHooks.getOriginalGetArgAsNumber()(memory.panoramaMarshallHelper, params, 1);
        const auto pickInGroupIndex = (std::uint8_t)panoramaMarshallHelperHooks.getOriginalGetArgAsNumber()(memory.panoramaMarshallHelper, params, 2);
        panoramaMarshallHelperHooks.getOriginalSetResultInt()(memory.panoramaMarshallHelper, params, static_cast<int>(backend.getPickEm().getPickedTeam({ tournament, groupId, pickInGroupIndex })));
    } else if (returnAddress == returnAddresses.setInventorySortAndFiltersGetArgAsString) {
        panoramaCodeInXrayScanner = (std::strcmp(string, "xraymachine") == 0);
    } else if (returnAddress == returnAddresses.performItemCasketTransactionGetArgAsString) {
        const auto operation = (int)panoramaMarshallHelperHooks.getOriginalGetArgAsNumber()(memory.panoramaMarshallHelper, params, 0);
        const auto storageUnitItemIdString = panoramaMarshallHelperHooks.getOriginalGetArgAsString()(memory.panoramaMarshallHelper, params, 1);

        if (operation == 1) {
            getRequestBuilder().addToStorageUnit(ItemId{ stringToUint64(string) }, ItemId{ stringToUint64(storageUnitItemIdString) });
        } else if (operation == -1) {
            getRequestBuilder().removeFromStorageUnit(ItemId{ stringToUint64(string) }, ItemId{ stringToUint64(storageUnitItemIdString) });
        } else if (operation == 0) {
            getRequestBuilder().loadStorageUnitContents(ItemId{ stringToUint64(storageUnitItemIdString) });
        }
    }
}

void InventoryChanger::getNumArgsHook(const PanoramaMarshallHelperHooks& panoramaMarshallHelperHooks, csgo::PanoramaMarshallHelperPOD* panoramaMarshallHelper, unsigned numberOfArgs, ReturnAddress returnAddress, void* params)
{
    if (returnAddress != returnAddresses.setMyPredictionUsingItemIdGetNumArgs)
        return;

    if (numberOfArgs <= 1 || (numberOfArgs - 1) % 3 != 0)
        return;

    const char* tournamentStr = panoramaMarshallHelperHooks.getOriginalGetArgAsString()(panoramaMarshallHelper, params, 0);
    if (!tournamentStr)
        return;

    const auto tournament = extractTournamentFromString(tournamentStr);
    if (tournament == csgo::Tournament{})
        return;

    for (unsigned i = 1; i < numberOfArgs; i += 3) {
        const auto groupId = (std::uint16_t)panoramaMarshallHelperHooks.getOriginalGetArgAsNumber()(panoramaMarshallHelper, params, i);
        const auto pickInGroupIndex = (std::uint8_t)panoramaMarshallHelperHooks.getOriginalGetArgAsNumber()(panoramaMarshallHelper, params, i + 1);
        const char* stickerItemID = panoramaMarshallHelperHooks.getOriginalGetArgAsString()(panoramaMarshallHelper, params, i + 2);

        if (!stickerItemID)
            continue;

        placePickEmPick(tournament, groupId, pickInGroupIndex, static_cast<csgo::StickerId>((stringToUint64(stickerItemID) >> 16) & 0xFFFF));
    }
}

int InventoryChanger::setResultIntHook(ReturnAddress returnAddress, [[maybe_unused]] void* params, int result)
{
    if (returnAddress == returnAddresses.getInventoryCountSetResultInt && panoramaCodeInXrayScanner && !backend.isInXRayScan()) {
        return 0;
    }
    return result;
}

[[nodiscard]] constexpr bool isWeaponDropNoticeString(std::string_view string) noexcept
{
    return string == "#SFUI_Notice_CannotDropWeapon" || string == "#SFUI_Notice_YouDroppedWeapon";
}

[[nodiscard]] constexpr bool isKillAwardString(std::string_view string) noexcept
{
    return string == "#Player_Cash_Award_Killed_Enemy" ||
           string == "#Player_Point_Award_Killed_Enemy" ||
           string == "#Player_Point_Award_Killed_Enemy_Plural";
}

void InventoryChanger::onUserTextMsg(const Memory& memory, const void*& data, int& size)
{
    if (!localPlayer)
        return;

    const auto optionalItem = getItemFromLoadout(backend.getLoadout(), localPlayer.get().getTeamNumber(), 0);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;

    constexpr auto HUD_PRINTTALK = 3;
    constexpr auto HUD_PRINTCENTER = 4;
    // https://github.com/SteamDatabase/Protobufs/blob/017f1710737b7026cdd6d7e602f96a66dddb7b2e/csgo/cstrike15_usermessages.proto#L128-L131

    const auto reader = ProtobufReader{ static_cast<const std::uint8_t*>(data), size };

    if (reader.readInt32(1) == HUD_PRINTCENTER) {
        const auto strings = reader.readRepeatedString(3);
        if (strings.size() < 2)
            return;

        if (!isWeaponDropNoticeString(strings[0]))
            return;

        if (!isDefaultKnifeNameLocalizationString(strings[1]))
            return;

        const auto itemSchema = memory.itemSystem().getItemSchema();
        if (!itemSchema)
            return;

        const auto def = csgo::ItemSchema::from(retSpoofGadgets->client, itemSchema).getItemDefinitionInterface(item->gameItem().getWeaponID());
        if (!def)
            return;

        userTextMsgBuffer = buildTextUserMessage(HUD_PRINTCENTER, strings[0], csgo::EconItemDefinition::from(retSpoofGadgets->client, def).getItemBaseName());
        data = userTextMsgBuffer.data();
        size = static_cast<int>(userTextMsgBuffer.size());
    } else if (reader.readInt32(1) == HUD_PRINTTALK) {
        const auto strings = reader.readRepeatedString(3);
        if (strings.size() < 3)
            return;

        if (!isKillAwardString(strings[0]))
            return;

        if (!isDefaultKnifeNameLocalizationString(strings[2]))
            return;

        const auto itemSchema = memory.itemSystem().getItemSchema();
        if (!itemSchema)
            return;

        const auto def = csgo::ItemSchema::from(retSpoofGadgets->client, itemSchema).getItemDefinitionInterface(item->gameItem().getWeaponID());
        if (!def)
            return;

        userTextMsgBuffer = buildTextUserMessage(HUD_PRINTTALK, strings[0], strings[1], csgo::EconItemDefinition::from(retSpoofGadgets->client, def).getItemBaseName());
        data = userTextMsgBuffer.data();
        size = static_cast<int>(userTextMsgBuffer.size());
    }
}

void InventoryChanger::onItemEquip(csgo::Team team, int slot, std::uint64_t& itemID)
{
    if (const auto itemOptional = backend.itemFromID(ItemId{ itemID }); itemOptional.has_value()) {
        const auto& itemIterator = *itemOptional;

        if (slot != 0xFFFF) {
            backend.getLoadoutHandler().markItemEquipped(itemIterator, static_cast<backend::Loadout::Slot>(slot), team);
            equipRequests.push_back({ std::chrono::steady_clock::now(), itemID, itemIterator->gameItem().getWeaponID() });
        } else {
            // unequip
        }

        itemID = (std::uint64_t(0xF) << 60) | static_cast<short>(itemIterator->gameItem().getWeaponID());
    }
}

void InventoryChanger::acknowledgeItem(const Memory& memory, std::uint64_t itemID)
{
    if (!backend.itemFromID(ItemId{ itemID }).has_value())
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    if (const auto view = csgo::EconItemView::from(retSpoofGadgets->client, memory.findOrCreateEconItemViewForItemID(itemID), gameInventory.getEconItemViewFunctions()); view.getPOD() != nullptr) {
        if (const auto soc = view.getSOCData()) {
            if (const auto baseTypeCache = getItemBaseTypeCache(localInventory, memory.createBaseTypeCache)) {
                soc->inventory = baseTypeCache->getHighestIDs().second + 1;
                localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)soc, 4);
            }
        }
    }
}

void InventoryChanger::fixKnifeAnimation(const csgo::Entity& viewModelWeapon, long& sequence, Helpers::RandomGenerator& randomGenerator)
{
    if (!localPlayer)
        return;

    if (!Helpers::isKnife(viewModelWeapon.itemDefinitionIndex()))
        return;

    if (const auto optionalItem = getItemFromLoadout(backend.getLoadout(), localPlayer.get().getTeamNumber(), 0); !optionalItem.has_value())
        return;

    sequence = remapKnifeAnim(viewModelWeapon.itemDefinitionIndex(), sequence, randomGenerator);
}

void InventoryChanger::reset(const Memory& memory)
{
    clearInventory(backend);
    backend.getPickEmHandler().clearPicks();
    backend.run(gameInventory, std::chrono::milliseconds{ 0 });
}

void InventoryChanger::placePickEmPick(csgo::Tournament tournament, std::uint16_t group, std::uint8_t indexInGroup, csgo::StickerId stickerID)
{
    const auto sticker = getGameItemLookup().findSticker(stickerID);
    if (!sticker || !sticker->isSticker())
        return;

    const auto tournamentTeam = getGameItemLookup().getStorage().getStickerKit(*sticker).tournamentTeam;
    backend.getPickEmHandler().pickSticker(backend::PickEm::PickPosition{ tournament, group, indexInGroup }, tournamentTeam);
}

}
