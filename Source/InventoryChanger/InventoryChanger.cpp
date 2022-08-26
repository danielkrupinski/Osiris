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
#include "../Interfaces.h"
#include "InventoryChanger.h"
#include "../ProtobufReader.h"
#include "../Texture.h"

#include <nlohmann/json.hpp>

#include <SDK/Constants/ClassId.h>
#include "../SDK/Client.h"
#include "../SDK/ClientClass.h"
#include "../SDK/ConVar.h"
#include "../SDK/Cvar.h"
#include "../SDK/EconItemView.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/FileSystem.h"
#include <SDK/Constants/FrameStage.h>
#include "../SDK/GameEvent.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/Panorama.h"
#include "../SDK/PlayerResource.h"
#include "../SDK/Platform.h"
#include "../SDK/WeaponId.h"
#include <SDK/PanoramaMarshallHelper.h>
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

static Entity* createGlove(int entry, int serial) noexcept
{
    static const auto createWearable = []{
        std::add_pointer_t<Entity* CDECL_CONV(int, int)> createWearableFn = nullptr;
        for (auto clientClass = interfaces->client->getAllClasses(); clientClass; clientClass = clientClass->next) {
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
        return reinterpret_cast<Entity*>(std::uintptr_t(wearable) - 2 * sizeof(std::uintptr_t));
    return nullptr;
}

static std::optional<std::list<inventory_changer::inventory::Item>::const_iterator> getItemFromLoadout(const inventory_changer::backend::Loadout& loadout, csgo::Team team, std::uint8_t slot)
{
    switch (team) {
    case csgo::Team::None: return loadout.getItemInSlotNoTeam(slot);
    case csgo::Team::CT: return loadout.getItemInSlotCT(slot);
    case csgo::Team::TT: return loadout.getItemInSlotTT(slot);
    default: return {};
    }
}

static void applyGloves(const inventory_changer::backend::BackendSimulator& backend, CSPlayerInventory& localInventory, Entity* local) noexcept
{
    const auto optionalItem = getItemFromLoadout(backend.getLoadout(), localPlayer->getTeamNumber(), 41);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;
    const auto itemID = backend.getItemID(item);
    if (!itemID.has_value())
        return;

    const auto wearables = local->wearables();
    static int gloveHandle = 0;

    auto glove = interfaces->entityList->getEntityFromHandle(wearables[0]);
    if (!glove)
        glove = interfaces->entityList->getEntityFromHandle(gloveHandle);

    constexpr auto NUM_ENT_ENTRIES = 8192;
    if (!glove)
        glove = createGlove(NUM_ENT_ENTRIES - 1, -1);

    if (!glove)
        return;

    wearables[0] = gloveHandle = glove->handle();
    glove->accountID() = localInventory.getAccountID();
    glove->entityQuality() = 3;
    local->body() = 1;

    bool dataUpdated = false;
    if (auto& definitionIndex = glove->itemDefinitionIndex(); definitionIndex != item->gameItem().getWeaponID()) {
        definitionIndex = item->gameItem().getWeaponID();

        if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item->gameItem().getWeaponID()))
            glove->setModelIndex(interfaces->modelInfo->getModelIndex(def->getWorldDisplayModel()));

        dataUpdated = true;
    }

    if (glove->itemID() != *itemID) {
        glove->itemIDHigh() = std::uint32_t(*itemID >> 32);
        glove->itemIDLow() = std::uint32_t(*itemID & 0xFFFFFFFF);
        dataUpdated = true;
    }

    glove->initialized() = true;
    memory->equipWearable(glove, local);

    if (dataUpdated) {
        // FIXME: This leaks memory
        glove->econItemView().visualDataProcessors().size = 0;
        glove->econItemView().customMaterials().size = 0;
        //

        glove->postDataUpdate(0);
        glove->onDataChanged(0);
    }
}

static void applyKnife(const inventory_changer::backend::BackendSimulator& backend, CSPlayerInventory& localInventory, Entity* local) noexcept
{
    const auto localXuid = local->getSteamId();

    const auto optionalItem = getItemFromLoadout(backend.getLoadout(), local->getTeamNumber(), 0);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;
    const auto itemID = backend.getItemID(item);
    if (!itemID.has_value())
        return;

    for (auto& weapons = local->weapons(); auto weaponHandle : weapons) {
        if (weaponHandle == -1)
            break;

        const auto weapon = interfaces->entityList->getEntityFromHandle(weaponHandle);
        if (!weapon)
            continue;

        auto& definitionIndex = weapon->itemDefinitionIndex();
        if (!Helpers::isKnife(definitionIndex))
            continue;

        if (weapon->originalOwnerXuid() != localXuid)
            continue;

        weapon->accountID() = localInventory.getAccountID();
        weapon->itemIDHigh() = std::uint32_t(*itemID >> 32);
        weapon->itemIDLow() = std::uint32_t(*itemID & 0xFFFFFFFF);
        weapon->entityQuality() = 3;

        if (definitionIndex != item->gameItem().getWeaponID()) {
            definitionIndex = item->gameItem().getWeaponID();

            if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item->gameItem().getWeaponID())) {
                weapon->setModelIndex(interfaces->modelInfo->getModelIndex(def->getPlayerDisplayModel()));
                weapon->preDataUpdate(0);
            }
        }
    }

    const auto viewModel = interfaces->entityList->getEntityFromHandle(local->viewModel());
    if (!viewModel)
        return;

    const auto viewModelWeapon = interfaces->entityList->getEntityFromHandle(viewModel->weapon());
    if (!viewModelWeapon)
        return;

    const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(viewModelWeapon->itemDefinitionIndex());
    if (!def)
        return;

    viewModel->modelIndex() = interfaces->modelInfo->getModelIndex(def->getPlayerDisplayModel());

    const auto worldModel = interfaces->entityList->getEntityFromHandle(viewModelWeapon->weaponWorldModel());
    if (!worldModel)
        return;

    worldModel->modelIndex() = interfaces->modelInfo->getModelIndex(def->getWorldDisplayModel());
}

static void applyWeapons(CSPlayerInventory& localInventory, Entity* local) noexcept
{
    const auto localTeam = local->getTeamNumber();
    const auto localXuid = local->getSteamId();
    const auto itemSchema = memory->itemSystem()->getItemSchema();

    const auto highestEntityIndex = interfaces->entityList->getHighestEntityIndex();
    for (int i = memory->globalVars->maxClients + 1; i <= highestEntityIndex; ++i) {
        const auto entity = interfaces->entityList->getEntity(i);
        if (!entity || !entity->isWeapon())
            continue;

        const auto weapon = entity;
        if (weapon->originalOwnerXuid() != localXuid)
            continue;

        const auto& definitionIndex = weapon->itemDefinitionIndex();
        if (Helpers::isKnife(definitionIndex))
            continue;

        const auto def = itemSchema->getItemDefinitionInterface(definitionIndex);
        if (!def)
            continue;

        const auto loadoutSlot = def->getLoadoutSlot(localTeam);
        const auto optionalItem = getItemFromLoadout(inventory_changer::InventoryChanger::instance().getBackend().getLoadout(), localTeam, loadoutSlot);
        if (!optionalItem.has_value())
            continue;
        
        const auto& item = *optionalItem;
        if (definitionIndex != item->gameItem().getWeaponID())
            continue;

        const auto itemID = inventory_changer::InventoryChanger::instance().getBackend().getItemID(item);
        if (!itemID.has_value())
            continue;

        weapon->accountID() = localInventory.getAccountID();
        weapon->itemIDHigh() = std::uint32_t(*itemID >> 32);
        weapon->itemIDLow() = std::uint32_t(*itemID & 0xFFFFFFFF);
    }
}

static void onPostDataUpdateStart(int localHandle) noexcept
{
    const auto local = interfaces->entityList->getEntityFromHandle(localHandle);
    if (!local)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    applyKnife(inventory_changer::InventoryChanger::instance().getBackend(), *localInventory, local);
    applyWeapons(*localInventory, local);
}

static bool hudUpdateRequired{ false };

static void updateHud() noexcept
{
    if (auto hud_weapons = memory->findHudElement(memory->hud, "CCSGO_HudWeaponSelection") - WIN32_LINUX(0x28, 62)) {
        for (int i = 0; i < *(hud_weapons + WIN32_LINUX(32, 52)); i++)
            i = memory->clearHudWeapon(hud_weapons, i);
    }
    hudUpdateRequired = false;
}

static void applyMusicKit(const inventory_changer::backend::BackendSimulator& backend) noexcept
{
    if (!localPlayer)
        return;

    const auto pr = *memory->playerResource;
    if (pr == nullptr)
        return;

    const auto optionalItem = backend.getLoadout().getItemInSlotNoTeam(54);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;
    if (!item->gameItem().isMusic())
        return;

    pr->musicID()[localPlayer->index()] = backend.getGameItemLookup().getStorage().getMusicKit(item->gameItem()).id;
}

static void applyPlayerAgent() noexcept
{
    if (!localPlayer)
        return;

    const auto optionalItem = getItemFromLoadout(inventory_changer::InventoryChanger::instance().getBackend().getLoadout(), localPlayer->getTeamNumber(), 38);
    if (!optionalItem.has_value())
        return;

    const auto item = *optionalItem;
    if (!item->gameItem().isAgent())
        return;

    const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item->gameItem().getWeaponID());
    if (!def)
        return;

    const auto model = def->getPlayerDisplayModel();
    if (!model)
        return;

    if (const auto agent = get<inventory_changer::inventory::Agent>(*item)) {
        for (std::size_t i = 0; i < agent->patches.size(); ++i) {
            if (const auto& patch = agent->patches[i]; patch.patchID != 0)
                localPlayer->playerPatchIndices()[i] = patch.patchID;
        }
    }

    const auto idx = interfaces->modelInfo->getModelIndex(model);
    localPlayer->setModelIndex(idx);

    if (const auto ragdoll = interfaces->entityList->getEntityFromHandle(localPlayer->ragdoll()))
        ragdoll->setModelIndex(idx);
}

static void applyMedal(const inventory_changer::backend::Loadout& loadout) noexcept
{
    if (!localPlayer)
        return;

    const auto pr = *memory->playerResource;
    if (!pr)
        return;

    const auto optionalItem = loadout.getItemInSlotNoTeam(55);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;
    if (!item->gameItem().isCollectible() && !item->gameItem().isServiceMedal() && !item->gameItem().isTournamentCoin())
        return;

    pr->activeCoinRank()[localPlayer->index()] = static_cast<int>(item->gameItem().getWeaponID());
}

struct EquipRequest {
    std::chrono::steady_clock::time_point time;
    std::uint64_t itemID;
    WeaponId weaponID;
    std::uint8_t counter = 0;
};
static std::vector<EquipRequest> equipRequests;

static void simulateItemUpdate(std::uint64_t itemID)
{
    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    if (const auto view = memory->findOrCreateEconItemViewForItemID(itemID)) {
        if (const auto soc = memory->getSOCData(view))
            localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)soc, 4);
    }
}

static void processEquipRequests()
{
    const auto now = std::chrono::steady_clock::now();
    for (auto it = equipRequests.begin(); it != equipRequests.end();) {
        if (now - it->time >= std::chrono::milliseconds{ 500 }) {
            if (it->counter == 0)
                simulateItemUpdate(it->itemID);
            it = equipRequests.erase(it);
        } else {
            ++it;
        }
    }
}

void InventoryChanger::run(csgo::FrameStage stage) noexcept
{
    static int localPlayerHandle = -1;

    if (localPlayer)
        localPlayerHandle = localPlayer->handle();

    if (stage == csgo::FrameStage::NET_UPDATE_POSTDATAUPDATE_START) {
        onPostDataUpdateStart(localPlayerHandle);
        if (hudUpdateRequired && localPlayer && !localPlayer->isDormant())
            updateHud();
    }

    if (stage != csgo::FrameStage::RENDER_START)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    using namespace inventory_changer::backend;

    auto& backend = inventory_changer::InventoryChanger::instance().getBackend();

    if (localPlayer)
        applyGloves(backend, *localInventory, localPlayer.get());

    applyMusicKit(backend);
    applyPlayerAgent();
    applyMedal(backend.getLoadout());

    processEquipRequests();
    static inventory_changer::game_integration::Inventory gameInventory{};
    backend.run(gameInventory, std::chrono::milliseconds{ 300 });
}

void InventoryChanger::scheduleHudUpdate() noexcept
{
    interfaces->cvar->findVar("cl_fullupdate")->changeCallback();
    hudUpdateRequired = true;
}

[[nodiscard]] static bool isLocalPlayerMVP(GameEvent& event)
{
    return localPlayer && localPlayer->getUserId() == event.getInt("userid");
}

static bool windowOpen = false;

void InventoryChanger::menuBarItem() noexcept
{
    if (ImGui::MenuItem("Inventory Changer")) {
        windowOpen = true;
        ImGui::SetWindowFocus("Inventory Changer");
        ImGui::SetWindowPos("Inventory Changer", { 100.0f, 100.0f });
    }
}

void InventoryChanger::tabItem() noexcept
{
    if (ImGui::BeginTabItem("Inventory Changer")) {
        inventory_changer::InventoryChanger::instance().drawGUI(true);
        ImGui::EndTabItem();
    }
}

static ImTextureID getItemIconTexture(std::string_view iconpath) noexcept;

namespace ImGui
{
    static bool SkinSelectable(const inventory_changer::game_items::Item& item, const ImVec2& iconSizeSmall, const ImVec2& iconSizeLarge, ImU32 rarityColor, int* toAddCount = nullptr) noexcept
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const auto itemName = inventory_changer::WeaponNames::instance().getWeaponName(item.getWeaponID()).data();
        const auto itemNameSize = CalcTextSize(itemName, nullptr);

        const auto paintKitName = getItemName(inventory_changer::InventoryChanger::instance().getGameItemLookup().getStorage(), item).forDisplay.data();
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

        if (const auto icon = getItemIconTexture(item.getIconPath())) {
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

    static void SkinItem(const inventory_changer::game_items::Item& item, const ImVec2& iconSizeSmall, const ImVec2& iconSizeLarge, ImU32 rarityColor, bool& shouldDelete) noexcept
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return;

        const ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const auto itemName = inventory_changer::WeaponNames::instance().getWeaponName(item.getWeaponID()).data();
        const auto itemNameSize = CalcTextSize(itemName, nullptr);

        const auto paintKitName = getItemName(inventory_changer::InventoryChanger::instance().getGameItemLookup().getStorage(), item).forDisplay.data();
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

        if (const auto icon = getItemIconTexture(item.getIconPath())) {
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

void InventoryChanger::drawGUI(bool contentOnly)
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
            ::InventoryChanger::scheduleHudUpdate();
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

        static SortFilter gameItemList{ gameItemLookup.getStorage().getItems() };

        if (filterChanged) {
            const std::wstring filterWide{ Helpers::ToUpperConverter{}.toUpper(Helpers::toWideString(filter)) };

            gameItemList.filter([&passesFilter, &filterWide, &weaponNames = inventory_changer::WeaponNames::instance(), &gameItemStorage = gameItemLookup.getStorage()](const inventory_changer::game_items::Item& item) {
                return filterWide.empty() || passesFilter(filterWide, weaponNames.getWeaponNameUpper(item.getWeaponID()), getItemName(gameItemStorage, item).forSearch);
            });
        }

        if (ImGui::BeginChild("##scrollarea", ImVec2{ 0.0f, contentOnly ? 400.0f : 0.0f })) {
            static std::vector<int> toAddCount(gameItemList.totalItemCount(), 1);

            if (static bool sorted = false; !sorted) {
                gameItemList.sort(inventory_changer::NameComparator{ inventory_changer::InventoryChanger::instance().getGameItemLookup().getStorage(), inventory_changer::WeaponNames::instance() });
                sorted = true;
            }

            for (const auto& [i, gameItem] : gameItemList.getItems()) {
                if (addingAll) {
                    backend.getInventoryHandler().addItem(inventory::Item{ gameItem, item_generator::createDefaultDynamicData(gameItemLookup.getStorage(), gameItem) }, true);
                }

                ImGui::PushID(i);

                if (ImGui::SkinSelectable(gameItem, { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(gameItem.getRarity()), &toAddCount[i])) {
                    for (int j = 0; j < toAddCount[i]; ++j)
                        backend.getInventoryHandler().addItem(inventory::Item{ gameItem, item_generator::createDefaultDynamicData(gameItemLookup.getStorage(), gameItem) }, true);
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
                ImGui::SkinItem(it->gameItem(), { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(it->gameItem().getRarity()), shouldDelete);
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

void InventoryChanger::onSoUpdated(SharedObject* object) noexcept
{
    if (object->getTypeID() == 43 /* = k_EEconTypeDefaultEquippedDefinitionInstanceClient */) {
        WeaponId& weaponID = *reinterpret_cast<WeaponId*>(std::uintptr_t(object) + WIN32_LINUX(0x10, 0x1C));
        if (const auto it = std::ranges::find(equipRequests, weaponID, &EquipRequest::weaponID); it != equipRequests.end()) {
            ++it->counter;
            weaponID = WeaponId::None;
        }
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

static std::uint64_t stringToUint64(const char* str) noexcept
{
    std::uint64_t result = 0;
    std::from_chars(str, str + std::strlen(str), result);
    return result;
}

struct Icon {
    Texture texture;
    int lastReferencedFrame = 0;
};

static std::unordered_map<std::string, Icon> iconTextures;

static ImTextureID getItemIconTexture(std::string_view iconpath) noexcept
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

        auto handle = interfaces->baseFileSystem->open(("resource/flash/" + std::string{ iconpath } + (iconpath.find("status_icons") != std::string_view::npos ? "" : "_large") + ".png").c_str(), "r", "GAME");
        if (!handle)
            handle = interfaces->baseFileSystem->open(("resource/flash/" + std::string{ iconpath } + ".png").c_str(), "r", "GAME");

        assert(handle);
        if (handle) {
            if (const auto size = interfaces->baseFileSystem->size(handle); size > 0) {
                const auto buffer = std::make_unique<std::uint8_t[]>(size);
                if (interfaces->baseFileSystem->read(buffer.get(), size, handle) > 0) {
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
            interfaces->baseFileSystem->close(handle);
        }

        const auto end = std::chrono::steady_clock::now();
        timeSpentThisFrame += std::chrono::duration<float>(end - start).count();
    }
    icon.lastReferencedFrame = ImGui::GetFrameCount();
    return icon.texture.get();
}

void InventoryChanger::clearItemIconTextures() noexcept
{
    iconTextures.clear();
}

void InventoryChanger::clearUnusedItemIconTextures() noexcept
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

static int remapKnifeAnim(WeaponId weaponID, const int sequence) noexcept
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
            return Helpers::random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return Helpers::random(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
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
            return Helpers::random(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return Helpers::random(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
        default:
            return sequence - 1;
        }
    case WeaponId::Daggers:
        switch (sequence) {
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_DAGGERS_IDLE1;
        case SEQUENCE_DEFAULT_LIGHT_MISS1:
        case SEQUENCE_DEFAULT_LIGHT_MISS2:
            return Helpers::random(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return Helpers::random(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
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
            return Helpers::random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return Helpers::random(SEQUENCE_BUTTERFLY_LOOKAT01, Sequence(14));
        default:
            return sequence + 1;
        }
    case WeaponId::Stiletto:
        switch (sequence) {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return Helpers::random(12, 13);
        }
    case WeaponId::Talon:
        switch (sequence) {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return Helpers::random(14, 15);
        }
    default:
        return sequence;
    }
}

namespace inventory_changer
{

InventoryChanger createInventoryChanger()
{
    assert(memory && interfaces);

    const auto itemSchema = memory->itemSystem()->getItemSchema();
    game_integration::Items items{ *itemSchema, *interfaces->localize };
    auto storage = game_integration::createGameItemStorage(items);
    storage.compress();
    auto gameItemLookup = game_items::Lookup{ std::move(storage) };

    game_integration::CrateLoot gameLoot{ *itemSchema, gameItemLookup };
    game_items::CrateLoot crateLoot;
    gameLoot.getLoot(crateLoot);
    crateLoot.compress();
    auto crateLootLookup = game_items::CrateLootLookup{ std::move(crateLoot) };

    return InventoryChanger{ std::move(gameItemLookup), std::move(crateLootLookup) };
}

InventoryChanger& InventoryChanger::instance()
{
    static InventoryChanger inventoryChanger{ createInventoryChanger() };
    return inventoryChanger;
}

void InventoryChanger::getArgAsNumberHook(int number, std::uintptr_t returnAddress)
{
    if (returnAddress == memory->setStickerToolSlotGetArgAsNumberReturnAddress)
        requestBuilderParams.stickerSlot = static_cast<std::uint8_t>(number);
}

void InventoryChanger::onRoundMVP(GameEvent& event)
{
    if (!isLocalPlayerMVP(event))
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

void InventoryChanger::updateStatTrak(GameEvent& event)
{
    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    const auto weapon = localPlayer->getActiveWeapon();
    if (!weapon)
        return;

    const auto itemID = weapon->itemID();

    const auto optionalItem = backend.itemFromID(itemID);
    if (!optionalItem.has_value())
        return;

    const auto item = *optionalItem;
    const auto skin = get<inventory::Skin>(*item);
    if (!skin)
        return;

    if (skin->statTrak > -1)
        backend.getItemModificationHandler().updateStatTrak(item, skin->statTrak + 1);
}

void InventoryChanger::overrideHudIcon(GameEvent& event)
{
    if (!localPlayer)
        return;

    if (event.getInt("attacker") != localPlayer->getUserId())
        return;

    if (const auto weapon = std::string_view{ event.getString("weapon") }; weapon != "knife" && weapon != "knife_t")
        return;

    const auto optionalItem = getItemFromLoadout(backend.getLoadout(), localPlayer->getTeamNumber(), 0);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;

    if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item->gameItem().getWeaponID())) {
        if (const auto defName = def->getDefinitionName(); defName && std::string_view{ defName }.starts_with("weapon_"))
            event.setString("weapon", defName + 7);
    }
}

void InventoryChanger::getArgAsStringHook(const char* string, std::uintptr_t returnAddress, void* params)
{
    if (returnAddress == memory->useToolGetArgAsStringReturnAddress) {
        const auto toolItemID = stringToUint64(string);
        const auto destItemIdString = hooks->panoramaMarshallHelper.callOriginal<const char*, 7>(params, 1);
        if (destItemIdString)
            getRequestBuilder().useToolOn(toolItemID, stringToUint64(destItemIdString));
    } else if (returnAddress == memory->wearItemStickerGetArgAsStringReturnAddress) {
        const auto slot = (std::uint8_t)hooks->panoramaMarshallHelper.callOriginal<double, 5>(params, 1);
        getRequestBuilder().wearStickerOf(stringToUint64(string), slot);
    } else if (returnAddress == memory->setNameToolStringGetArgAsStringReturnAddress) {
        requestBuilderParams.nameTag = string;
    } else if (returnAddress == memory->clearCustomNameGetArgAsStringReturnAddress) {
        getRequestBuilder().removeNameTagFrom(stringToUint64(string));
    } else if (returnAddress == memory->deleteItemGetArgAsStringReturnAddress) {
        if (const auto itOptional = backend.itemFromID(stringToUint64(string)); itOptional.has_value())
            backend.getItemRemovalHandler()(*itOptional);
    } else if (returnAddress == memory->acknowledgeNewItemByItemIDGetArgAsStringReturnAddress) {
        acknowledgeItem(stringToUint64(string));
    } else if (returnAddress == memory->setStatTrakSwapToolItemsGetArgAsStringReturnAddress1) {
        const auto swapItem1 = stringToUint64(string);
        const auto swapItem2String = hooks->panoramaMarshallHelper.callOriginal<const char*, 7>(params, 1);
        if (swapItem2String) {
            requestBuilderParams.statTrakSwapItemID1 = swapItem1;
            requestBuilderParams.statTrakSwapItemID2 = stringToUint64(swapItem2String);
        }
    } else if (returnAddress == memory->setItemAttributeValueAsyncGetArgAsStringReturnAddress) {
        if (const auto itOptional = backend.itemFromID(stringToUint64(string)); itOptional.has_value() && (*itOptional)->gameItem().isTournamentCoin()) {
            const auto attribute = hooks->panoramaMarshallHelper.callOriginal<const char*, 7>(params, 1);
            if (attribute && std::strcmp(attribute, "sticker slot 0 id") == 0) {
                const auto graffitiID = (int)hooks->panoramaMarshallHelper.callOriginal<double, 5>(params, 2);
                backend.getItemModificationHandler().selectTeamGraffiti(*itOptional, static_cast<std::uint16_t>(graffitiID));
            }
        }
    } else if (returnAddress == memory->getMyPredictionTeamIDGetArgAsStringReturnAddress) {
        if (std::strcmp(string, "tournament:19") != 0) // PGL Antwerp 2022, TODO: Support other tournaments
            return;

        const auto groupId = (std::uint16_t)hooks->panoramaMarshallHelper.callOriginal<double, 5>(params, 1);
        const auto pickInGroupIndex = (std::uint8_t)hooks->panoramaMarshallHelper.callOriginal<double, 5>(params, 2);

        memory->panoramaMarshallHelper->setResult(params, static_cast<int>(backend.getPickEm().getPickedTeam({ csgo::Tournament::PglAntwerp2022, groupId, pickInGroupIndex })));
    } else if (returnAddress == memory->setInventorySortAndFiltersGetArgAsStringReturnAddress) {
        panoramaCodeInXrayScanner = (std::strcmp(string, "xraymachine") == 0);
    } else if (returnAddress == memory->performItemCasketTransactionGetArgAsStringReturnAddress) {
        const auto operation = (int)hooks->panoramaMarshallHelper.callOriginal<double, 5>(params, 0);
        const auto storageUnitItemIdString = hooks->panoramaMarshallHelper.callOriginal<const char*, 7>(params, 1);

        if (operation == 1) {
            getRequestBuilder().addToStorageUnit(stringToUint64(string), stringToUint64(storageUnitItemIdString));
        } else if (operation == -1) {
            getRequestBuilder().removeFromStorageUnit(stringToUint64(string), stringToUint64(storageUnitItemIdString));
        }
    }
}

void InventoryChanger::getNumArgsHook(unsigned numberOfArgs, std::uintptr_t returnAddress, void* params)
{
    if (returnAddress != memory->setMyPredictionUsingItemIdGetNumArgsReturnAddress)
        return;

    if (numberOfArgs <= 1 || (numberOfArgs - 1) % 3 != 0)
        return;

    const char* tournament = hooks->panoramaMarshallHelper.callOriginal<const char*, 7>(params, 0);
    if (!tournament || std::strcmp(tournament, "tournament:19") != 0) // PGL Antwerp 2022, TODO: Support other tournaments
        return;

    for (unsigned i = 1; i < numberOfArgs; i += 3) {
        const auto groupId = (std::uint16_t)hooks->panoramaMarshallHelper.callOriginal<double, 5>(params, i);
        const auto pickInGroupIndex = (std::uint8_t)hooks->panoramaMarshallHelper.callOriginal<double, 5>(params, i + 1);
        const char* stickerItemID = hooks->panoramaMarshallHelper.callOriginal<const char*, 7>(params, i + 2);

        if (!stickerItemID)
            continue;

        placePickEmPick(groupId, pickInGroupIndex, static_cast<int>((stringToUint64(stickerItemID) >> 16) & 0xFFFF));
    }
}

int InventoryChanger::setResultIntHook(std::uintptr_t returnAddress, [[maybe_unused]] void* params, int result)
{
    if (returnAddress == memory->getInventoryCountSetResultIntReturnAddress && panoramaCodeInXrayScanner && !backend.isInXRayScan()) {
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

void InventoryChanger::onUserTextMsg(const void*& data, int& size)
{
    if (!localPlayer)
        return;

    const auto optionalItem = getItemFromLoadout(backend.getLoadout(), localPlayer->getTeamNumber(), 0);
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

        const auto itemSchema = memory->itemSystem()->getItemSchema();
        if (!itemSchema)
            return;

        const auto def = itemSchema->getItemDefinitionInterface(item->gameItem().getWeaponID());
        if (!def)
            return;

        userTextMsgBuffer = buildTextUserMessage(HUD_PRINTCENTER, strings[0], def->getItemBaseName());
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

        const auto itemSchema = memory->itemSystem()->getItemSchema();
        if (!itemSchema)
            return;

        const auto def = itemSchema->getItemDefinitionInterface(item->gameItem().getWeaponID());
        if (!def)
            return;

        userTextMsgBuffer = buildTextUserMessage(HUD_PRINTTALK, strings[0], strings[1], def->getItemBaseName());
        data = userTextMsgBuffer.data();
        size = static_cast<int>(userTextMsgBuffer.size());
    }
}

void InventoryChanger::onItemEquip(csgo::Team team, int slot, std::uint64_t& itemID)
{
    if (const auto itemOptional = backend.itemFromID(itemID); itemOptional.has_value()) {
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

void InventoryChanger::acknowledgeItem(std::uint64_t itemID)
{
    if (!backend.itemFromID(itemID).has_value())
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    if (const auto view = memory->findOrCreateEconItemViewForItemID(itemID)) {
        if (const auto soc = memory->getSOCData(view)) {
            soc->inventory = localInventory->getHighestIDs().second + 1;
            localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)soc, 4);
        }
    }
}

void InventoryChanger::fixKnifeAnimation(Entity* viewModelWeapon, long& sequence)
{
    if (!localPlayer)
        return;

    if (!Helpers::isKnife(viewModelWeapon->itemDefinitionIndex()))
        return;

    if (const auto optionalItem = getItemFromLoadout(backend.getLoadout(), localPlayer->getTeamNumber(), 0); !optionalItem.has_value())
        return;

    sequence = remapKnifeAnim(viewModelWeapon->itemDefinitionIndex(), sequence);
}

void InventoryChanger::reset()
{
    clearInventory(backend);
    backend.getPickEmHandler().clearPicks();
    static inventory_changer::game_integration::Inventory gameInventory{};
    backend.run(gameInventory, std::chrono::milliseconds{ 0 });
}

void InventoryChanger::placePickEmPick(std::uint16_t group, std::uint8_t indexInGroup, int stickerID)
{
    const auto sticker = gameItemLookup.findSticker(stickerID);
    if (!sticker || !sticker->isSticker())
        return;

    const auto tournamentTeam = gameItemLookup.getStorage().getStickerKit(*sticker).tournamentTeam;
    backend.getPickEmHandler().pickSticker(backend::PickEm::PickPosition{ csgo::Tournament::PglAntwerp2022, group, indexInGroup }, tournamentTeam);
}

}
