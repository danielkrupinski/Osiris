#pragma once

#include <vector>

#include "../Config.h"

class Entity;
struct ModelRenderInfo;
class matrix3x4;
class Material;

class Chams {
public:
    Chams() noexcept;
    bool render(void*, void*, const ModelRenderInfo&, matrix3x4*) noexcept;
private:
    void renderPlayer(Entity* player) noexcept;
    void renderWeapons() noexcept;
    void renderHands() noexcept;
    void renderSleeves() noexcept;

    enum ChamsId {
        ALLIES = 0,
        ENEMIES,
        PLANTING,
        DEFUSING,
        LOCALPLAYER,
        WEAPONS,
        HANDS,
        BACKTRACK,
        SLEEVES
    };

    Material* normal;
    Material* flat;
    Material* animated;
    Material* platinum;
    Material* glass;
    Material* crystal;
    Material* chrome;
    Material* silver;
    Material* gold;
    Material* plastic;
    Material* glow;
    Material* pearlescent;
    Material* metallic;

    constexpr auto dispatchMaterial(int id) const noexcept
    {
        switch (id) {
        default:
        case 0: return normal;
        case 1: return flat;
        case 2: return animated;
        case 3: return platinum;
        case 4: return glass;
        case 5: return chrome;
        case 6: return crystal;
        case 7: return silver;
        case 8: return gold;
        case 9: return plastic;
        case 10: return glow;
        case 11: return pearlescent;
        case 12: return metallic;
        }
    }

    bool appliedChams;
    void* ctx;
    void* state;
    const ModelRenderInfo* info;
    matrix3x4* customBoneToWorld;

    void applyChams(const std::vector<Config::Chams::Material>& chams, int health = 0, matrix3x4* customMatrix = nullptr) noexcept;
};
