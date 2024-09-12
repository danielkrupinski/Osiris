#pragma once

#include <CS2/Classes/CGameSceneNode.h>

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
        return deps().offsetToAbsOrigin.of(gameSceneNode).toOptional();
    }

    [[nodiscard]] decltype(auto) owner() const noexcept
    {
        return hookContext->template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(deps().offsetToOwner.of(gameSceneNode).valueOr(nullptr)));
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
        return hookContext->template make<GameSceneNode<HookContext>>(deps().offsetToChild.of(gameSceneNode).valueOr(nullptr));
    }

    [[nodiscard]] decltype(auto) nextSibling() const noexcept
    {
        return hookContext->template make<GameSceneNode<HookContext>>(deps().offsetToNextSibling.of(gameSceneNode).valueOr(nullptr));
    }

    [[nodiscard]] const auto& deps() const noexcept
    {
        return hookContext->gameDependencies().gameSceneNodeDeps;
    }

    HookContext* hookContext;
    cs2::CGameSceneNode* gameSceneNode;
};
