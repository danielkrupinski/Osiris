#pragma once

#include <CS2/Classes/CGameSceneNode.h>
#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <MemoryPatterns/PatternTypes/GameSceneNodePatternTypes.h>

template <typename HookContext>
class BaseEntity;

template <typename HookContext>
class GameSceneNode {
public:
    GameSceneNode(HookContext& hookContext, cs2::CGameSceneNode* gameSceneNode) noexcept
        : hookContext{&hookContext}
        , gameSceneNode{gameSceneNode}
    {
    }

    explicit operator bool() const noexcept
    {
        return gameSceneNode != nullptr;
    }

    [[nodiscard]] auto absOrigin() const noexcept
    {
        return hookContext->clientPatternSearchResults().template get<OffsetToAbsOrigin>().of(gameSceneNode).toOptional();
    }

    [[nodiscard]] decltype(auto) owner() const noexcept
    {
        return hookContext->template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(hookContext->clientPatternSearchResults().template get<OffsetToGameSceneNodeOwner>().of(gameSceneNode).valueOr(nullptr)));
    }

    template <typename F>
    void forEachChild(F f) const noexcept
    {
        for (auto&& child = this->child(); child; child = child.nextSibling())
            f(child);
    }

private:
    [[nodiscard]] decltype(auto) child() const noexcept
    {
        return hookContext->template make<GameSceneNode<HookContext>>(hookContext->clientPatternSearchResults().template get<OffsetToChildGameSceneNode>().of(gameSceneNode).valueOr(nullptr));
    }

    [[nodiscard]] decltype(auto) nextSibling() const noexcept
    {
        return hookContext->template make<GameSceneNode<HookContext>>(hookContext->clientPatternSearchResults().template get<OffsetToNextSiblingGameSceneNode>().of(gameSceneNode).valueOr(nullptr));
    }

    HookContext* hookContext;
    cs2::CGameSceneNode* gameSceneNode;
};
