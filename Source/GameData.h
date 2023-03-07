#pragma once

#include <forward_list>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "CSGO/matrix3x4.h"
#include "CSGO/Entity.h"
#include "CSGO/Vector.h"

#include "Texture.h"

#include "Memory.h"

struct LocalPlayerData;

struct PlayerData;
struct ObserverData;
struct WeaponData;
struct EntityData;
struct LootCrateData;
struct ProjectileData;
struct BombData;
struct InfernoData;

namespace csgo { struct Matrix4x4; }

class ClientInterfaces;

namespace GameData
{
    void update(const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory) noexcept;
    void clearProjectileList() noexcept;
    void clearTextures() noexcept;
    void clearUnusedAvatars() noexcept;

    class Lock {
    public:
        Lock() noexcept : lock{ mutex } {}
    private:
        std::scoped_lock<std::mutex> lock;
        static inline std::mutex mutex;
    };
    
    // Lock-free
    int getNetOutgoingLatency() noexcept;

    // You have to acquire Lock before using these getters
    const csgo::Matrix4x4& toScreenMatrix() noexcept;
    const LocalPlayerData& local() noexcept;
    const std::vector<PlayerData>& players() noexcept;
    const PlayerData* playerByHandle(int handle) noexcept;
    const std::vector<ObserverData>& observers() noexcept;
    const std::vector<WeaponData>& weapons() noexcept;
    const std::vector<EntityData>& entities() noexcept;
    const std::vector<LootCrateData>& lootCrates() noexcept;
    const std::forward_list<ProjectileData>& projectiles() noexcept;
    const BombData& plantedC4() noexcept;
    const std::vector<InfernoData>& infernos() noexcept;
}

struct LocalPlayerData {
    void update(const csgo::Engine& engine) noexcept;

    bool exists = false;
    bool alive = false;
    bool inReload = false;
    bool shooting = false;
    bool noScope = false;
    float nextWeaponAttack = 0.0f;
    int fov;
    int handle;
    float flashDuration;
    csgo::Vector aimPunch;
    csgo::Vector origin;
};

namespace csgo { struct Entity; }

struct BaseData {
    BaseData(const csgo::Entity& entity) noexcept;

    float distanceToLocal;
    csgo::Vector obbMins, obbMaxs;
    csgo::matrix3x4 coordinateFrame;
};

struct EntityData final : BaseData {
    EntityData(const csgo::Entity& entity) noexcept;
   
    const char* name;
};

struct ProjectileData : BaseData {
    ProjectileData(const ClientInterfaces& clientInterfaces, const Memory& memory, const csgo::Entity& projectile) noexcept;

    void update(const Memory& memory, const csgo::Entity& projectile) noexcept;

    constexpr auto operator==(int otherHandle) const noexcept
    {
        return handle == otherHandle;
    }

    bool exploded = false;
    bool thrownByLocalPlayer = false;
    bool thrownByEnemy = false;
    int handle;
    const char* name = nullptr;
    std::vector<std::pair<float, csgo::Vector>> trajectory;
};

enum class Team;

struct PlayerData : BaseData {
    PlayerData(const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const csgo::Entity& entity) noexcept;
    PlayerData(const PlayerData&) = delete;
    PlayerData& operator=(const PlayerData&) = delete;
    PlayerData(PlayerData&&) = default;
    PlayerData& operator=(PlayerData&&) = default;

    void update(const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const csgo::Entity& entity) noexcept;
    [[nodiscard]] ImTextureID getAvatarTexture() const noexcept;
    [[nodiscard]] float fadingAlpha(const Memory& memory) const noexcept;

    bool dormant;
    bool enemy = false;
    bool visible = false;
    bool audible;
    bool spotted;
    bool inViewFrustum;
    bool alive;
    bool immune = false;
    float flashDuration;
    float lastContactTime = 0.0f;
    int health;
    int handle;
    csgo::Team team;
    std::string name;
    csgo::Vector headMins, headMaxs;
    csgo::Vector origin;
    std::string activeWeapon;
    std::vector<std::pair<csgo::Vector, csgo::Vector>> bones;
};

struct WeaponData : BaseData {
    WeaponData(const OtherInterfaces& interfaces, const csgo::Entity& entity) noexcept;

    int clip;
    int reserveAmmo;
    const char* group = "All";
    const char* name = "All";
    std::string displayName;
};

struct LootCrateData : BaseData {
    LootCrateData(const csgo::Entity& entity) noexcept;

    const char* name = nullptr;
};

struct ObserverData {
    ObserverData(const csgo::Entity& entity, const csgo::Entity& obs, bool targetIsLocalPlayer) noexcept;

    int playerHandle;
    int targetHandle;
    bool targetIsLocalPlayer;
};

struct BombData {
    void update(const Memory& memory) noexcept;

    float blowTime;
    float timerLength;
    int defuserHandle;
    float defuseCountDown;
    float defuseLength;
    int bombsite;
};

struct InfernoData {
    InfernoData(const csgo::Entity& inferno) noexcept;

    std::vector<csgo::Vector> points;
};
