#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <string_view>
#include <type_traits>
#include <utility>

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

#include "../Helpers.h"

#include "StaticData.h"

#include "GameItems/Lookup.h"
#include "Inventory/Item.h"
#include "Inventory/Structs.h"
#include "Backend/Loadout.h"
#include "Backend/BackendSimulator.h"
#include "ItemGenerator.h"

#include "BackendResponseHandler.h"
#include "../Hooks.h"

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

static std::optional<std::list<inventory::Item>::const_iterator> getItemFromLoadout(const inventory_changer::backend::Loadout& loadout, Team team, std::uint8_t slot)
{
    switch (team) {
    case Team::None: return loadout.getItemInSlotNoTeam(slot);
    case Team::CT: return loadout.getItemInSlotCT(slot);
    case Team::TT: return loadout.getItemInSlotTT(slot);
    default: return {};
    }
}

static void applyGloves(CSPlayerInventory& localInventory, Entity* local) noexcept
{
    const auto optionalItem = getItemFromLoadout(inventory_changer::backend::BackendSimulator::instance().getLoadout(), localPlayer->getTeamNumber(), 41);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;
    const auto itemID = inventory_changer::backend::BackendSimulator::instance().getItemID(item);
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

static void applyKnife(CSPlayerInventory& localInventory, Entity* local) noexcept
{
    const auto localXuid = local->getSteamId();

    const auto optionalItem = getItemFromLoadout(inventory_changer::backend::BackendSimulator::instance().getLoadout(), local->getTeamNumber(), 0);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;
    const auto itemID = inventory_changer::backend::BackendSimulator::instance().getItemID(item);
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
        const auto optionalItem = getItemFromLoadout(inventory_changer::backend::BackendSimulator::instance().getLoadout(), localTeam, loadoutSlot);
        if (!optionalItem.has_value())
            continue;
        
        const auto& item = *optionalItem;
        if (definitionIndex != item->gameItem().getWeaponID())
            continue;

        const auto itemID = inventory_changer::backend::BackendSimulator::instance().getItemID(item);
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

    applyKnife(*localInventory, local);
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

void InventoryChanger::acknowledgeItem(std::uint64_t itemID) noexcept
{
    if (!inventory_changer::backend::BackendSimulator::instance().itemFromID(itemID).has_value())
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    if (const auto view = memory->findOrCreateEconItemViewForItemID(itemID)) {
        if (const auto soc = memory->getSOCData(view)) {
            soc->inventory = localInventory->getHighestIDs().second;
            localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)soc, 4);
        }
    }
}

static void applyMusicKit() noexcept
{
    if (!localPlayer)
        return;

    const auto pr = *memory->playerResource;
    if (pr == nullptr)
        return;

    const auto optionalItem = inventory_changer::backend::BackendSimulator::instance().getLoadout().getItemInSlotNoTeam(54);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;
    if (!item->gameItem().isMusic())
        return;

    pr->musicID()[localPlayer->index()] = StaticData::lookup().getStorage().getMusicKit(item->gameItem()).id;
}

static void applyPlayerAgent() noexcept
{
    if (!localPlayer)
        return;

    const auto optionalItem = getItemFromLoadout(inventory_changer::backend::BackendSimulator::instance().getLoadout(), localPlayer->getTeamNumber(), 38);
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

    if (const auto agent = item->get<inventory::Agent>()) {
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

static void applyMedal() noexcept
{
    if (!localPlayer)
        return;

    const auto pr = *memory->playerResource;
    if (!pr)
        return;

    const auto optionalItem = inventory_changer::backend::BackendSimulator::instance().getLoadout().getItemInSlotNoTeam(55);
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

void InventoryChanger::run(FrameStage stage) noexcept
{
    static int localPlayerHandle = -1;

    if (localPlayer)
        localPlayerHandle = localPlayer->handle();

    if (stage == FrameStage::NET_UPDATE_POSTDATAUPDATE_START) {
        onPostDataUpdateStart(localPlayerHandle);
        if (hudUpdateRequired && localPlayer && !localPlayer->isDormant())
            updateHud();
    }

    if (stage != FrameStage::RENDER_START)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    if (localPlayer)
        applyGloves(*localInventory, localPlayer.get());

    applyMusicKit();
    applyPlayerAgent();
    applyMedal();

    using namespace inventory_changer::backend;

    processEquipRequests();
    BackendSimulator::instance().run(inventory_changer::BackendResponseHandler{ BackendSimulator::instance() }, std::chrono::milliseconds{ 300 });
}

void InventoryChanger::scheduleHudUpdate() noexcept
{
    interfaces->cvar->findVar("cl_fullupdate")->changeCallback();
    hudUpdateRequired = true;
}

void InventoryChanger::overrideHudIcon(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (event.getInt("attacker") != localPlayer->getUserId())
        return;

    if (const auto weapon = std::string_view{ event.getString("weapon") }; weapon != "knife" && weapon != "knife_t")
        return;

    const auto optionalItem = getItemFromLoadout(inventory_changer::backend::BackendSimulator::instance().getLoadout(), localPlayer->getTeamNumber(), 0);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;

    if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item->gameItem().getWeaponID())) {
        if (const auto defName = def->getDefinitionName(); defName && std::string_view{ defName }.starts_with("weapon_"))
            event.setString("weapon", defName + 7);
    }
}

void InventoryChanger::updateStatTrak(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    const auto weapon = localPlayer->getActiveWeapon();
    if (!weapon)
        return;

    const auto itemID = weapon->itemID();

    const auto optionalItem = inventory_changer::backend::BackendSimulator::instance().itemFromID(itemID);
    if (!optionalItem.has_value())
        return;

    const auto item = *optionalItem;
    const auto skin = item->get<inventory::Skin>();
    if (!skin)
        return;

    if (skin->statTrak > -1)
        inventory_changer::backend::BackendSimulator::instance().handleRequest<inventory_changer::backend::request::UpdateStatTrak>(item, skin->statTrak + 1);
}

[[nodiscard]] static bool isLocalPlayerMVP(GameEvent& event)
{
    return localPlayer && localPlayer->getUserId() == event.getInt("userid");
}

void InventoryChanger::onRoundMVP(GameEvent& event) noexcept
{
    if (!isLocalPlayerMVP(event))
        return;

    const auto optionalItem = inventory_changer::backend::BackendSimulator::instance().getLoadout().getItemInSlotNoTeam(54);
    if (!optionalItem.has_value())
        return;

    const auto& item = *optionalItem;
    const auto music = item->get<inventory::Music>();
    if (!music)
        return;

    if (music->statTrak > -1) {
        event.setInt("musickitmvps", music->statTrak + 1);
        inventory_changer::backend::BackendSimulator::instance().handleRequest<inventory_changer::backend::request::UpdateStatTrak>(item, music->statTrak + 1);
    }
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
        drawGUI(true);
        ImGui::EndTabItem();
    }
}

static ImTextureID getItemIconTexture(std::string_view iconpath) noexcept;

[[nodiscard]] const game_items::ItemName& getItemName(const game_items::Item& item)
{
    const auto& storage = StaticData::lookup().getStorage();

    if (item.isSkin() || item.isGloves())
        return storage.getPaintKit(item).name;
    if (item.isMusic())
        return storage.getMusicKit(item).name;
    if (item.isSticker())
        return storage.getStickerKit(item).name;
    if (item.isGraffiti())
        return storage.getGraffitiKit(item).name;
    if (item.isPatch())
        return storage.getPatch(item).name;

    static constexpr game_items::ItemName fallback{ "", L"" };
    return fallback;
}

namespace ImGui
{
    static bool SkinSelectable(const game_items::Item& item, const ImVec2& iconSizeSmall, const ImVec2& iconSizeLarge, ImU32 rarityColor, int* toAddCount = nullptr) noexcept
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const auto itemName = StaticData::getWeaponName(item.getWeaponID()).data();
        const auto itemNameSize = CalcTextSize(itemName, nullptr);

        const auto paintKitName = getItemName(item).forDisplay.data();
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

    static void SkinItem(const game_items::Item& item, const ImVec2& iconSizeSmall, const ImVec2& iconSizeLarge, ImU32 rarityColor, bool& shouldDelete) noexcept
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return;

        const ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const auto itemName = StaticData::getWeaponName(item.getWeaponID()).data();
        const auto itemNameSize = CalcTextSize(itemName, nullptr);

        const auto paintKitName = getItemName(item).forDisplay.data();
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

void InventoryChanger::drawGUI(bool contentOnly) noexcept
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
            InventoryChanger::scheduleHudUpdate();
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
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint("##search", "Search weapon skins, stickers, knives, gloves, music kits..", &filter);

        constexpr auto passesFilter = [](const std::wstring& str, std::wstring filter) {
            constexpr auto delimiter = L" ";
            wchar_t* _;
            wchar_t* token = std::wcstok(filter.data(), delimiter, &_);
            while (token) {
                if (!std::wcsstr(str.c_str(), token))
                    return false;
                token = std::wcstok(nullptr, delimiter, &_);
            }
            return true;
        };

        if (ImGui::BeginChild("##scrollarea", ImVec2{ 0.0f, contentOnly ? 400.0f : 0.0f })) {
            static std::vector<std::reference_wrapper<const game_items::Item>> itemIndices{ StaticData::lookup().getStorage().getItems().begin(), StaticData::lookup().getStorage().getItems().end() };
            static std::vector<int> toAddCount(itemIndices.size(), 1);

            if (static bool sorted = false; !sorted) {
                std::ranges::sort(itemIndices, [](const game_items::Item& a, const game_items::Item& b) {
                    if (a.getWeaponID() == b.getWeaponID())
                        return getItemName(a).forSearch < getItemName(b).forSearch;
                    const auto comp = StaticData::getWeaponNameUpper(a.getWeaponID()).compare(StaticData::getWeaponNameUpper(b.getWeaponID()));
                    if (comp == 0)
                        return a.getWeaponID() < b.getWeaponID();
                    return comp < 0;
                });
                sorted = true;
            }

            const std::wstring filterWide{ Helpers::ToUpperConverter{}.toUpper(Helpers::toWideString(filter)) };
            for (std::size_t i = 0; i < itemIndices.size(); ++i) {
                const auto& gameItem = itemIndices[i].get();
                if (!filter.empty() && !passesFilter(std::wstring(StaticData::getWeaponNameUpper(gameItem.getWeaponID())), filterWide) && (!passesFilter(std::wstring(getItemName(gameItem).forSearch), filterWide)))
                    continue;
                ImGui::PushID(i);

                if (ImGui::SkinSelectable(gameItem, { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(gameItem.getRarity()), &toAddCount[i])) {
                    for (int j = 0; j < toAddCount[i]; ++j)
                        inventory_changer::backend::BackendSimulator::instance().addItemUnacknowledged(inventory::Item{ gameItem, ItemGenerator::createDefaultDynamicData(gameItem) });
                    toAddCount[i] = 1;
                }
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    } else {
        if (ImGui::BeginChild("##scrollarea2", ImVec2{ 0.0f, contentOnly ? 400.0f : 0.0f })) {
            const auto& inventory = inventory_changer::backend::BackendSimulator::instance().getInventory();
            std::size_t i = 0;
            for (auto it = inventory.rbegin(); it != inventory.rend();) {
                ImGui::PushID(i);
                bool shouldDelete = false;
                ImGui::SkinItem(it->gameItem(), { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(it->gameItem().getRarity()), shouldDelete);
                if (shouldDelete) {
                    it = std::make_reverse_iterator(inventory_changer::backend::BackendSimulator::instance().removeItem(std::next(it).base()));
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

void InventoryChanger::clearInventory() noexcept
{
    resetConfig();
}

void InventoryChanger::onItemEquip(Team team, int slot, std::uint64_t& itemID) noexcept
{
    if (const auto itemOptional = inventory_changer::backend::BackendSimulator::instance().itemFromID(itemID); itemOptional.has_value()) {
        const auto& itemIterator = *itemOptional;

        if (slot != 0xFFFF) {
            using inventory_changer::backend::Loadout;
            if (auto& backendSimulator = inventory_changer::backend::BackendSimulator::instance(); team == Team::CT) {
                backendSimulator.markItemEquippedCT(itemIterator, static_cast<Loadout::Slot>(slot));
            } else if (team == Team::TT) {
                backendSimulator.markItemEquippedTT(itemIterator, static_cast<Loadout::Slot>(slot));
            } else if (team == Team::None) {
                backendSimulator.markItemEquippedNoTeam(itemIterator, static_cast<Loadout::Slot>(slot));
            }

            equipRequests.push_back({ std::chrono::steady_clock::now(), itemID, itemIterator->gameItem().getWeaponID() });
        } else {
           // unequip
        }

        itemID = (std::uint64_t(0xF) << 60) | static_cast<short>(itemIterator->gameItem().getWeaponID());
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

void InventoryChanger::onUserTextMsg(const void*& data, int& size) noexcept
{
    if (!localPlayer)
        return;

    const auto optionalItem = getItemFromLoadout(inventory_changer::backend::BackendSimulator::instance().getLoadout(), localPlayer->getTeamNumber(), 0);
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

        if (strings[0] != "#SFUI_Notice_CannotDropWeapon" &&
            strings[0] != "#SFUI_Notice_YouDroppedWeapon")
            return;

        if (!isDefaultKnifeNameLocalizationString(strings[1]))
            return;

        const auto itemSchema = memory->itemSystem()->getItemSchema();
        if (!itemSchema)
            return;

        const auto def = itemSchema->getItemDefinitionInterface(item->gameItem().getWeaponID());
        if (!def)
            return;

        static std::vector<char> buffer;
        buffer = buildTextUserMessage(HUD_PRINTCENTER, strings[0], def->getItemBaseName());
        data = buffer.data();
        size = static_cast<int>(buffer.size());
    } else if (reader.readInt32(1) == HUD_PRINTTALK) {
        const auto strings = reader.readRepeatedString(3);
        if (strings.size() < 3)
            return;

        if (strings[0] != "#Player_Cash_Award_Killed_Enemy" &&
            strings[0] != "#Player_Point_Award_Killed_Enemy" &&
            strings[0] != "#Player_Point_Award_Killed_Enemy_Plural")
            return;

        if (!isDefaultKnifeNameLocalizationString(strings[2]))
            return;

        const auto itemSchema = memory->itemSystem()->getItemSchema();
        if (!itemSchema)
            return;

        const auto def = itemSchema->getItemDefinitionInterface(item->gameItem().getWeaponID());
        if (!def)
            return;

        static std::vector<char> buffer;
        buffer = buildTextUserMessage(HUD_PRINTTALK, strings[0], strings[1], def->getItemBaseName());
        data = buffer.data();
        size = static_cast<int>(buffer.size());
    }
}

static std::uint64_t stringToUint64(const char* str) noexcept
{
    std::uint64_t result = 0;
    std::from_chars(str, str + std::strlen(str), result);
    return result;
}

namespace inventory_changer
{
    class BackendRequestBuilder {
    public:
        explicit BackendRequestBuilder(backend::BackendSimulator& backend) : backend{ backend } {}

        void setToolItemID(std::uint64_t itemID) noexcept
        {
            toolItemID = itemID;
        }

        void setStickerSlot(std::uint8_t slot) noexcept
        {
            stickerSlot = slot;
        }

        void setStatTrakSwapItem1(std::uint64_t itemID) noexcept
        {
            statTrakSwapItemID1 = itemID;
        }

        void setStatTrakSwapItem2(std::uint64_t itemID) noexcept
        {
            statTrakSwapItemID2 = itemID;
        }

        void setNameTag(std::string_view newName)
        {
            nameTag = newName;
        }

        void useToolOn(std::uint64_t destItemID)
        {
            const auto toolItem = backend.itemFromID(toolItemID);
            const auto destItem = backend.itemFromID(destItemID);

            if (toolItem.has_value() && destItem.has_value()) {
                useToolOnItem(*toolItem, *destItem);
            } else if (toolItem.has_value()) {
                useTool(*toolItem);
            } else if (destItem.has_value()) {
                useItem(*destItem);
            }
        }

        void wearStickerOf(std::uint64_t itemID)
        {
            const auto item = backend.itemFromID(itemID);
            if (!item.has_value())
                return;

            if (const auto gameItem = (*item)->gameItem(); gameItem.isSkin())
                backend.handleRequest<backend::request::WearSticker>(*item, stickerSlot);
            else if (gameItem.isAgent())
                backend.handleRequest<backend::request::RemovePatch>(*item, stickerSlot);
        }

        void removeNameTagFrom(std::uint64_t itemID)
        {
            const auto item = backend.itemFromID(itemID);
            if (!item.has_value())
                return;

            backend.handleRequest<backend::request::RemoveNameTag>(*item);
        }

        [[nodiscard]] static BackendRequestBuilder& instance()
        {
            static BackendRequestBuilder builder{ backend::BackendSimulator::instance() };
            return builder;
        }

    private:
        void useToolOnItem(backend::ItemConstIterator tool, backend::ItemConstIterator destItem)
        {
            if (tool->gameItem().isSticker() && destItem->gameItem().isSkin()) {
                backend.handleRequest<backend::request::ApplySticker>(destItem, tool, stickerSlot);
            } else if (tool->gameItem().isCaseKey() && destItem->gameItem().isCase()) {
                backend.handleRequest<backend::request::OpenContainer>(destItem, tool);
            } else if (tool->gameItem().isPatch() && destItem->gameItem().isAgent()) {
                backend.handleRequest<backend::request::ApplyPatch>(destItem, tool, stickerSlot);
            } else if (tool->gameItem().isNameTag() && destItem->gameItem().isSkin()) {
                backend.handleRequest<backend::request::AddNameTag>(destItem, tool, nameTag);
            }
        }

        void useTool(backend::ItemConstIterator tool)
        {
            if (tool->gameItem().isStatTrakSwapTool()) {
                const auto statTrakSwapItem1 = backend.itemFromID(statTrakSwapItemID1);
                const auto statTrakSwapItem2 = backend.itemFromID(statTrakSwapItemID2);

                if (statTrakSwapItem1.has_value() && statTrakSwapItem2.has_value())
                    backend.handleRequest<backend::request::SwapStatTrak>(*statTrakSwapItem1, *statTrakSwapItem2, tool);
            } else if (tool->gameItem().isOperationPass()) {
                backend.handleRequest<backend::request::ActivateOperationPass>(tool);
            } else if (tool->gameItem().isViewerPass()) {
                backend.handleRequest<backend::request::ActivateViewerPass>(tool);
            } else if (tool->gameItem().isSouvenirToken()) {
                backend.handleRequest<backend::request::ActivateSouvenirToken>(tool);
            } else if (tool->gameItem().isGraffiti()) {
                backend.handleRequest<backend::request::UnsealGraffiti>(tool);
            }
        }

        void useItem(backend::ItemConstIterator item)
        {
            if (item->gameItem().isCase())
                backend.handleRequest<backend::request::OpenContainer>(item);
        }

        backend::BackendSimulator& backend;
        std::uint64_t toolItemID = 0;
        std::uint8_t stickerSlot = 0;
        std::uint64_t statTrakSwapItemID1 = 0;
        std::uint64_t statTrakSwapItemID2 = 0;
        std::string nameTag;
    };
}

void InventoryChanger::getArgAsStringHook(const char* string, std::uintptr_t returnAddress, void* params) noexcept
{
    if (returnAddress == memory->useToolGetArgAsStringReturnAddress) {
        inventory_changer::BackendRequestBuilder::instance().setToolItemID(stringToUint64(string));
    } else if (returnAddress == memory->useToolGetArg2AsStringReturnAddress) {
        inventory_changer::BackendRequestBuilder::instance().useToolOn(stringToUint64(string));
    } else if (returnAddress == memory->wearItemStickerGetArgAsStringReturnAddress) {
        inventory_changer::BackendRequestBuilder::instance().wearStickerOf(stringToUint64(string));
    } else if (returnAddress == memory->setNameToolStringGetArgAsStringReturnAddress) {
        inventory_changer::BackendRequestBuilder::instance().setNameTag(string);
    } else if (returnAddress == memory->clearCustomNameGetArgAsStringReturnAddress) {
        inventory_changer::BackendRequestBuilder::instance().removeNameTagFrom(stringToUint64(string));
    } else if (returnAddress == memory->deleteItemGetArgAsStringReturnAddress) {
        auto& backend = inventory_changer::backend::BackendSimulator::instance();
        if (const auto itOptional = backend.itemFromID(stringToUint64(string)); itOptional.has_value())
            backend.removeItem(*itOptional);
    } else if (returnAddress == memory->acknowledgeNewItemByItemIDGetArgAsStringReturnAddress) {
        InventoryChanger::acknowledgeItem(stringToUint64(string));
    } else if (returnAddress == memory->setStatTrakSwapToolItemsGetArgAsStringReturnAddress1) {
        inventory_changer::BackendRequestBuilder::instance().setStatTrakSwapItem1(stringToUint64(string));
    } else if (returnAddress == memory->setStatTrakSwapToolItemsGetArgAsStringReturnAddress2) {
        inventory_changer::BackendRequestBuilder::instance().setStatTrakSwapItem2(stringToUint64(string));
    } else if (returnAddress == memory->setItemAttributeValueAsyncGetArgAsStringReturnAddress) {
        auto& backend = inventory_changer::backend::BackendSimulator::instance();
        if (const auto itOptional = backend.itemFromID(stringToUint64(string)); itOptional.has_value() && (*itOptional)->gameItem().isTournamentCoin()) {
            const auto attribute = hooks->panoramaMarshallHelper.callOriginal<const char*, 7>(params, 1);
            if (attribute && std::strcmp(attribute, "sticker slot 0 id") == 0) {
                const auto graffitiID = (int)hooks->panoramaMarshallHelper.callOriginal<double, 5>(params, 2);
                backend.handleRequest<inventory_changer::backend::request::SelectTeamGraffiti>(*itOptional, static_cast<std::uint16_t>(graffitiID));
            }
        }
    }
}

void InventoryChanger::getArgAsNumberHook(int number, std::uintptr_t returnAddress) noexcept
{
    if (returnAddress == memory->setStickerToolSlotGetArgAsNumberReturnAddress || returnAddress == memory->wearItemStickerGetArgAsNumberReturnAddress)
        inventory_changer::BackendRequestBuilder::instance().setStickerSlot(static_cast<std::uint8_t>(number));
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

        const auto start = std::chrono::high_resolution_clock::now();

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

        const auto end = std::chrono::high_resolution_clock::now();
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
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_FALCHION_IDLE1;
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return Helpers::random(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return Helpers::random(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
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

void InventoryChanger::fixKnifeAnimation(Entity* viewModelWeapon, long& sequence) noexcept
{
    if (!localPlayer)
        return;

    if (!Helpers::isKnife(viewModelWeapon->itemDefinitionIndex()))
        return;

    if (const auto optionalItem = getItemFromLoadout(inventory_changer::backend::BackendSimulator::instance().getLoadout(), localPlayer->getTeamNumber(), 0); !optionalItem.has_value())
        return;

    sequence = remapKnifeAnim(viewModelWeapon->itemDefinitionIndex(), sequence);
}
