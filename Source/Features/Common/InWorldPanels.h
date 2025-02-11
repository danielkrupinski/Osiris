#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

#include <CS2/Panorama/CUIPanel.h>
#include <Features/Sound/Details/SoundVisualizationPanelFactory.h>
#include <Features/Sound/SoundVisualizationPanelTypes.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldPanelFactory.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanel.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCache.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelTypes.h>
#include <GameClient/Panorama/PanelHandle.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>
#include <Utils/Lvalue.h>

#include "InWorldPanelIndex.h"
#include "InWorldPanelListEntry.h"

template <typename HookContext>
class InWorldPanels {
public:
    explicit InWorldPanels(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void updateState() const noexcept
    {
        if (!containerPanelExists()) {
            hookContext.template make<PlayerInfoPanelCache>().clear();
            state().reset();
        }
    }

    [[nodiscard]] decltype(auto) getNextPlayerInfoPanel() const noexcept
    {
        if (auto&& existingPanel = getNextExistingPanel(state().playerInfoPanelListHead, perHookState().lastUsedPlayerInfoPanelIndex))
            return existingPanel.template as<PlayerInfoPanel>(hookContext.template make<PlayerInfoPanelCache>().nextEntry());
        auto&& newPanel = createPlayerInfoPanel(getOrCreateContainerPanel());
        registerNewPanel(state().playerInfoPanelListHead, perHookState().lastUsedPlayerInfoPanelIndex);
        hookContext.template make<PlayerInfoPanelCache>().allocateNewEntry();
        return newPanel.template as<PlayerInfoPanel>(hookContext.template make<PlayerInfoPanelCache>().nextEntry());
    }

    template <typename Type>
    [[nodiscard]] decltype(auto) getNextSoundVisualizationPanel() const noexcept
    {
        if (auto&& existingPanel = getNextExistingPanel(state().soundVisualizationPanelListHeads[SoundVisualizationPanelTypes::indexOf<Type>()], perHookState().lastUsedSoundVisualizationPanelIndexes[SoundVisualizationPanelTypes::indexOf<Type>()]))
            return utils::lvalue<decltype(existingPanel)>(existingPanel);
        auto&& newPanel = createSoundVisualizationPanel<Type>(getOrCreateContainerPanel());
        registerNewPanel(state().soundVisualizationPanelListHeads[SoundVisualizationPanelTypes::indexOf<Type>()], perHookState().lastUsedSoundVisualizationPanelIndexes[SoundVisualizationPanelTypes::indexOf<Type>()]);
        return utils::lvalue<decltype(newPanel)>(newPanel);
    }

    void hideUnusedPanels() const noexcept
    {
        hideUnusedPanels(state().playerInfoPanelListHead, perHookState().lastUsedPlayerInfoPanelIndex);
        for (std::size_t i = 0; i < SoundVisualizationPanelTypes::size(); ++i)
            hideUnusedPanels(state().soundVisualizationPanelListHeads[i], perHookState().lastUsedSoundVisualizationPanelIndexes[i]);
    }

private:
    void registerNewPanel(InWorldPanelIndex& panelOfTypeListHead, InWorldPanelIndex& lastUsedPanelOfTypeIndex) const noexcept
    {
        const InWorldPanelIndex newPanelIndex{static_cast<InWorldPanelIndex::ValueType>(state().panelList.getSize())};
        if (lastUsedPanelOfTypeIndex.isValid())
            state().panelList[lastUsedPanelOfTypeIndex.value] = InWorldPanelListEntry{newPanelIndex};
        else
            panelOfTypeListHead = newPanelIndex;
        lastUsedPanelOfTypeIndex = newPanelIndex;
        state().panelList.pushBack(InWorldPanelListEntry{InWorldPanelIndex{}});
    }

    void hideUnusedPanels(InWorldPanelIndex panelOfTypeListHead, InWorldPanelIndex lastUsedPanelOfTypeIndex) const noexcept
    {
        const auto firstIndex = lastUsedPanelOfTypeIndex.isValid()
            ? state().panelList[lastUsedPanelOfTypeIndex.value].indexOfNextPanelOfSameType()
            : panelOfTypeListHead;

        for (auto index = firstIndex; index.isValid(); index = state().panelList[index.value].indexOfNextPanelOfSameType()) {
            if (state().panelList[index.value].isPanelActive()) {
                state().panelList[index.value].markPanelInactive();
                deactivatePanel(getContainerPanel().children()[index.value]);
            }
        }
    }

    template <typename Type>
    [[nodiscard]] decltype(auto) createSoundVisualizationPanel(auto&& parent) const noexcept
    {
        return hookContext.template make<SoundVisualizationPanelFactory>(*static_cast<cs2::CUIPanel*>(parent))
            .createSoundVisualizationPanel(Type::soundVisualizationPanelProperties());
    }

    [[nodiscard]] decltype(auto) createPlayerInfoPanel(auto&& parent) const noexcept
    {
        auto&& factory = hookContext.template make<PlayerInfoInWorldPanelFactory>();
        auto&& containerPanel = factory.createContainerPanel(parent);
        createPanels(std::type_identity<PlayerInfoPanelTypes<HookContext>>{}, factory, containerPanel);
        return utils::lvalue<decltype(containerPanel)>(containerPanel);
    }

    template <typename... PanelTypes>
    void createPanels(std::type_identity<std::tuple<PanelTypes...>>, auto&& factory, auto&& containerPanel) const noexcept
    {
        (factory.createPanel(std::type_identity<PanelTypes>{}, containerPanel), ...);
    }

    [[nodiscard]] InWorldPanelIndex nextExistingPanelIndex(InWorldPanelIndex panelOfTypeListHead, InWorldPanelIndex lastUsedPanelIndex) const noexcept
    {
        if (lastUsedPanelIndex.isValid())
            return state().panelList[lastUsedPanelIndex.value].indexOfNextPanelOfSameType();
        return panelOfTypeListHead;
    }

    [[nodiscard]] decltype(auto) getNextExistingPanel(InWorldPanelIndex panelOfTypeListHead, InWorldPanelIndex& lastUsedPanelIndex) const noexcept
    {
        const auto nextIndex = nextExistingPanelIndex(panelOfTypeListHead, lastUsedPanelIndex);
        if (nextIndex.isValid()) {
            lastUsedPanelIndex = nextIndex;
            auto& nextPanelEntry = state().panelList[nextIndex.value];
            auto&& panel = getContainerPanel().children()[nextIndex.value];
            if (!nextPanelEntry.isPanelActive()) {
                activatePanel(panel);
                nextPanelEntry.markPanelActive();
            }
            return utils::lvalue<decltype(panel)>(panel);
        }
        return hookContext.template make<PanoramaUiPanel>(nullptr);
    }

    void activatePanel(auto&& panel) const noexcept
    {
        panel.setVisible(true);
    }

    void deactivatePanel(auto&& panel) const noexcept
    {
        panel.setVisible(false);
    }

    [[nodiscard]] auto& perHookState() const noexcept
    {
        return hookContext.inWorldPanelsPerHookState();
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.inWorldPanelsState();
    }

    [[nodiscard]] decltype(auto) getOrCreateContainerPanel() const noexcept
    {
        return containerPanelHandle().getOrInit(createContainerPanel());
    }

    [[nodiscard]] decltype(auto) getContainerPanel() const noexcept
    {
        return containerPanelHandle().get();
    }

    [[nodiscard]] decltype(auto) containerPanelHandle() const noexcept
    {
        return hookContext.template make<PanelHandle>(state().containerPanelHandle);
    }

    [[nodiscard]] bool containerPanelExists() const noexcept
    {
        return containerPanelHandle().panelExists();
    }

    [[nodiscard]] auto createContainerPanel() const noexcept
    {
        return [this] {
            auto&& panel = hookContext.panelFactory().createPanel(hookContext.hud().getHudReticle()).uiPanel();
            panel.fitParent();
            return panel;
        };
    }

private:
    HookContext& hookContext;
};
