#include "../Interfaces.h"
#include "Inventory.h"
#include "../Memory.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/Panorama.h"
#include "StaticData.h"
#include "ToolUser.h"

static void initItemCustomizationNotification(const char* typeStr, const char* itemID) noexcept
{
    if (const auto idx = memory->registeredPanoramaEvents->find(memory->makePanoramaSymbol("PanoramaComponent_Inventory_ItemCustomizationNotification")); idx != -1) {
        std::string args; args += "0,'"; args += typeStr; args += "','"; args += itemID; args += '\'';
        const char* dummy;
        if (const auto event = memory->registeredPanoramaEvents->memory[idx].value.createEventFromString(nullptr, args.c_str(), &dummy))
            interfaces->panoramaUIEngine->accessUIEngine()->dispatchEvent(event);
    }
}

static void initItemCustomizationNotification(const char* typeStr, std::uint64_t itemID) noexcept
{
    initItemCustomizationNotification(typeStr, std::to_string(itemID).c_str());
}

class ToolUserImpl {
public:
    static void setDestItem(std::uint64_t itemID, ToolUser::Action action) noexcept
    {
        instance().destItemID = itemID;
        instance().action = action;
        instance().useTime = memory->globalVars->realtime + Helpers::random(0.18f, 0.48f);
    }

    static void setTool(std::uint64_t itemID) noexcept { instance().toolItemID = itemID; }
    static void setNameTag(const char* nameTag) noexcept { instance().nameTag = nameTag; }
    static void setStickerSlot(int slot) noexcept { instance().stickerSlot = slot; }
    static void setStatTrakSwapItem1(std::uint64_t itemID) noexcept { instance().statTrakSwapItem1 = itemID; }
    static void setStatTrakSwapItem2(std::uint64_t itemID) noexcept { instance().statTrakSwapItem2 = itemID; }

    static void preAddItems(CSPlayerInventory& localInventory) noexcept { instance()._preAddItems(localInventory); }
private:
    void _wearSticker(CSPlayerInventory& localInventory) const noexcept
    {
        const auto dest = Inventory::getItem(destItemID);
        if (!dest)
            return;

        if (dest->isSkin()) {
            constexpr auto wearStep = 0.12f;
            const auto newWear = (Inventory::dynamicSkinData(dest->getDynamicDataIndex()).stickers[stickerSlot].wear += wearStep);

            if (const auto shouldRemove = (newWear >= 1.0f + wearStep); shouldRemove) {
                Inventory::dynamicSkinData(dest->getDynamicDataIndex()).stickers[stickerSlot] = {};
                initItemCustomizationNotification("sticker_remove", Inventory::recreateItem(destItemID));
            } else {
                if (const auto view = memory->findOrCreateEconItemViewForItemID(destItemID)) {
                    if (const auto soc = memory->getSOCData(view)) {
                        soc->setStickerWear(stickerSlot, newWear);
                        localInventory.soUpdated(localInventory.getSOID(), (SharedObject*)soc, 4);
                    }
                }
            }
        } else if (dest->isAgent()) {
            Inventory::dynamicAgentData(dest->getDynamicDataIndex()).patches[stickerSlot] = {};
            initItemCustomizationNotification("patch_remove", Inventory::recreateItem(destItemID));
        }
    }

    void _removeNameTag() const noexcept
    {
        const auto dest = Inventory::getItem(destItemID);
        if (!dest || !dest->isSkin())
            return;

        Inventory::dynamicSkinData(dest->getDynamicDataIndex()).nameTag.clear();
        Inventory::recreateItem(destItemID);
    }

    void _activateOperationPass(InventoryItem& pass) const noexcept
    {
        const auto passWeaponID = pass.get().weaponID;
        pass.markToDelete();
        const auto coinID = passWeaponID != WeaponId::OperationHydraPass ? static_cast<WeaponId>(static_cast<int>(passWeaponID) + 1) : WeaponId::BronzeOperationHydraCoin;
        if (const auto idx = StaticData::getItemIndex(coinID, 0); idx != StaticData::InvalidItemIdx)
            Inventory::addItemNow(idx, Inventory::INVALID_DYNAMIC_DATA_IDX, true);
    }

    void _activateViewerPass(InventoryItem& pass) const noexcept
    {
        const auto coinID = static_cast<WeaponId>(static_cast<int>(pass.get().weaponID) + 1);
        pass.markToDelete();
        if (const auto idx = StaticData::getItemIndex(coinID, 0); idx != StaticData::InvalidItemIdx)
            initItemCustomizationNotification("ticket_activated", Inventory::addItemNow(idx, Inventory::INVALID_DYNAMIC_DATA_IDX, false));
    }

    void _unsealGraffiti(InventoryItem& sealedGraffiti) const noexcept
    {
        if (const auto idx = StaticData::getItemIndex(WeaponId::Graffiti, StaticData::paintKits()[sealedGraffiti.get().dataIndex].id); idx != StaticData::InvalidItemIdx) {
            sealedGraffiti.markToDelete();
            initItemCustomizationNotification("graffity_unseal", Inventory::addItemNow(idx, Inventory::INVALID_DYNAMIC_DATA_IDX, false));
        }
    }

    // Move this to loot generator
    static float generateWear() noexcept
    {
        float wear;
        if (const auto condition = Helpers::random(1, 10000); condition <= 1471)
            wear = Helpers::random(0.0f, 0.07f);
        else if (condition <= 3939)
            wear = Helpers::random(0.07f, 0.15f);
        else if (condition <= 8257)
            wear = Helpers::random(0.15f, 0.38f);
        else if (condition <= 9049)
            wear = Helpers::random(0.38f, 0.45f);
        else
            wear = Helpers::random(0.45f, 1.0f);
        return wear;
    }
    //

    void _openContainer(InventoryItem& container) const noexcept
    {
        assert(container.isCase());
        const auto& caseData = StaticData::cases()[container.get().dataIndex];
        assert(caseData.hasLoot());
        if (caseData.hasLoot()) {

            // Move this to loot generator
            const auto unlockedItemIdx = StaticData::caseLoot()[Helpers::random(static_cast<int>(caseData.lootBeginIdx), static_cast<int>(caseData.lootEndIdx - 1))];
            std::size_t dynamicDataIdx = Inventory::INVALID_DYNAMIC_DATA_IDX;

            if (const auto& item = StaticData::gameItems()[unlockedItemIdx]; caseData.willProduceStatTrak && item.isMusic()) {
                DynamicMusicData dynamicData;
                dynamicData.statTrak = 0;
                dynamicDataIdx = Inventory::emplaceDynamicData(std::move(dynamicData));
            } else if (item.isSkin()) {
                DynamicSkinData dynamicData;
                const auto& staticData = StaticData::paintKits()[item.dataIndex];
                dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, generateWear());
                dynamicData.seed = Helpers::random(1, 1000);

                if (caseData.isSouvenirPackage)
                    dynamicData.isSouvenir = true;
                else if (Helpers::random(0, 9) == 0)
                    dynamicData.statTrak = 0;

                dynamicDataIdx = Inventory::emplaceDynamicData(std::move(dynamicData));
            }
            //

            container.markToDelete();
            if (const auto tool = Inventory::getItem(toolItemID); tool && tool->isCaseKey())
                tool->markToDelete();
            initItemCustomizationNotification("crate_unlock", Inventory::addItemNow(unlockedItemIdx, dynamicDataIdx, false));
        }
    }

    void _applySticker(InventoryItem& sticker) const noexcept
    {
        assert(sticker.isSticker());
        const auto dest = Inventory::getItem(destItemID);
        if (!dest || !dest->isSkin())
            return;

        Inventory::dynamicSkinData(dest->getDynamicDataIndex()).stickers[stickerSlot].stickerID = StaticData::paintKits()[sticker.get().dataIndex].id;
        Inventory::dynamicSkinData(dest->getDynamicDataIndex()).stickers[stickerSlot].wear = 0.0f;
        sticker.markToDelete();
        initItemCustomizationNotification("sticker_apply", Inventory::recreateItem(destItemID));
    }

    void _addNameTag(InventoryItem& nameTagItem) const noexcept
    {
        assert(nameTagItem.isNameTag());
        const auto dest = Inventory::getItem(destItemID);
        if (!dest || !dest->isSkin())
            return;

        Inventory::dynamicSkinData(dest->getDynamicDataIndex()).nameTag = nameTag;
        nameTagItem.markToDelete();
        initItemCustomizationNotification("nametag_add", Inventory::recreateItem(destItemID));
    }

    void _applyPatch(InventoryItem& patch) const noexcept
    {
        assert(patch.isPatch());
        const auto dest = Inventory::getItem(destItemID);
        if (!dest || !dest->isAgent())
            return;

        Inventory::dynamicAgentData(dest->getDynamicDataIndex()).patches[stickerSlot].patchID = StaticData::paintKits()[patch.get().dataIndex].id;
        patch.markToDelete();
        initItemCustomizationNotification("patch_apply", Inventory::recreateItem(destItemID));
    }

    void _swapStatTrak(InventoryItem& statTrakSwapTool) const noexcept
    {
        const auto item1 = Inventory::getItem(statTrakSwapItem1);
        if (!item1 || !item1->isSkin())
            return;

        const auto item2 = Inventory::getItem(statTrakSwapItem2);
        if (!item2 || !item2->isSkin())
            return;

        std::swap(Inventory::dynamicSkinData(item1->getDynamicDataIndex()).statTrak, Inventory::dynamicSkinData(item2->getDynamicDataIndex()).statTrak);
        statTrakSwapTool.markToDelete();

        const auto recreatedItemID1 = Inventory::recreateItem(statTrakSwapItem1);
        const auto recreatedItemID2 = Inventory::recreateItem(statTrakSwapItem2);
        if (const auto inventoryComponent = *memory->uiComponentInventory) {
            memory->setItemSessionPropertyValue(inventoryComponent, recreatedItemID1, "updated", "1");
            memory->setItemSessionPropertyValue(inventoryComponent, recreatedItemID2, "updated", "1");
        }
        initItemCustomizationNotification("stattrack_swap", recreatedItemID2);
    }

    void _useTool() const noexcept
    {
        if (const auto destItem = Inventory::getItem(destItemID); destItem && destItem->isCase()) {
            _openContainer(*destItem);
            return;
        }

        const auto tool = Inventory::getItem(toolItemID);
        if (!tool)
            return;

        if (tool->isSealedGraffiti()) {
            _unsealGraffiti(*tool);
        } else if (tool->isOperationPass()) {
            _activateOperationPass(*tool);
        } else if (tool->isSticker()) {
            _applySticker(*tool);
        } else if (tool->isNameTag()) {
            _addNameTag(*tool);
        } else if (tool->isPatch()) {
            _applyPatch(*tool);
        } else if (tool->isStatTrakSwapTool()) {
            _swapStatTrak(*tool);
        } else if (tool->isViewerPass()) {
            _activateViewerPass(*tool);
        }
    }

    void _preAddItems(CSPlayerInventory& localInventory) noexcept
    {
        if (useTime > memory->globalVars->realtime)
            return;

        if (action == ToolUser::Action::WearSticker) {
            _wearSticker(localInventory);
        } else if (action == ToolUser::Action::RemoveNameTag) {
            _removeNameTag();
        } else if (action == ToolUser::Action::Use) {
            _useTool();
        }

        toolItemID = destItemID = statTrakSwapItem1 = statTrakSwapItem2 = 0;
    }

    static ToolUserImpl& instance() noexcept
    {
        static ToolUserImpl toolUser;
        return toolUser;
    }

    std::uint64_t toolItemID = 0;
    std::uint64_t destItemID = 0;
    std::uint64_t statTrakSwapItem1 = 0;
    std::uint64_t statTrakSwapItem2 = 0;
    ToolUser::Action action;
    float useTime = 0.0f;
    int stickerSlot = 0;
    std::string nameTag;
};

void ToolUser::setTool(std::uint64_t itemID) noexcept
{
    ToolUserImpl::setTool(itemID);
}

void ToolUser::setDestItem(std::uint64_t itemID, Action action) noexcept
{
    ToolUserImpl::setDestItem(itemID, action);
}

void ToolUser::setNameTag(const char* nameTag) noexcept
{
    ToolUserImpl::setNameTag(nameTag);
}

void ToolUser::setStickerSlot(int slot) noexcept
{
    ToolUserImpl::setStickerSlot(slot);
}

void ToolUser::setStatTrakSwapItem1(std::uint64_t itemID) noexcept
{
    ToolUserImpl::setStatTrakSwapItem1(itemID);
}

void ToolUser::setStatTrakSwapItem2(std::uint64_t itemID) noexcept
{
    ToolUserImpl::setStatTrakSwapItem2(itemID);
}

void ToolUser::preAddItems(CSPlayerInventory& localInventory) noexcept
{
    ToolUserImpl::preAddItems(localInventory);
}
