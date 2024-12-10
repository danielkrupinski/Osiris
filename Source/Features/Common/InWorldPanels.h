#pragma once

#include <Features/Sound/Details/SoundVisualizationPanelFactory.h>
#include <Features/Sound/SoundVisualizationPanelTypes.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldPanelFactory.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanel.h>
#include <GameClasses/PanelHandle.h>
#include "InWorldPanelIndex.h"
#include "InWorldPanelListEntry.h"

template <typename HookContext>
class InWorldPanels {
public:
    explicit InWorldPanels(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) getNextPlayerInfoPanel() const noexcept
    {
        if (auto&& existingPanel = getNextExistingPanel(state().playerInfoPanelListHead, perHookState().lastUsedPlayerInfoPanelIndex))
            return existingPanel.template as<PlayerInfoPanel>();
        auto&& newPanel = createPlayerInfoPanel(containerPanel());
        registerNewPanel(state().playerInfoPanelListHead, perHookState().lastUsedPlayerInfoPanelIndex);
        return newPanel.template as<PlayerInfoPanel>();
    }

    template <typename Type>
    [[nodiscard]] decltype(auto) getNextSoundVisualizationPanel() const noexcept
    {
        if (auto&& existingPanel = getNextExistingPanel(state().soundVisualizationPanelListHeads[SoundVisualizationPanelTypes::indexOf<Type>()], perHookState().lastUsedSoundVisualizationPanelIndexes[SoundVisualizationPanelTypes::indexOf<Type>()]))
            return utils::lvalue<decltype(existingPanel)>(existingPanel);
        auto&& newPanel = createSoundVisualizationPanel<Type>(containerPanel());
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
                deactivatePanel(containerPanel().children()[index.value]);
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
            auto&& panel = containerPanel().children()[nextIndex.value];
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

    [[nodiscard]] auto containerPanel() const noexcept
    {
        return handle().getOrInit(createContainerPanel());
    }

    [[nodiscard]] decltype(auto) handle() const noexcept
    {
        return hookContext.template make<PanelHandle>(state().containerPanelHandle);
    }

    [[nodiscard]] auto createContainerPanel() const noexcept
    {
        return [this] {
            state().onContainerPanelCreation();
            auto&& panel = hookContext.panelFactory().createPanel(hookContext.hud().getHudReticle()).uiPanel();
            panel.fitParent();
            return panel;
        };
    }

private:
    HookContext& hookContext;
};
