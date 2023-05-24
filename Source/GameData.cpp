#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>

#include "fnv.h"
#include "GameData.h"
#include "Memory.h"

#include "Resources/avatar_ct.h"
#include "Resources/avatar_tt.h"

#include <stb_image.h>

#include "CSGO/Constants/ClassId.h"
#include "CSGO/ClientClass.h"
#include "CSGO/Engine.h"
#include "CSGO/Entity.h"
#include "CSGO/EntityList.h"
#include "CSGO/GlobalVars.h"
#include "CSGO/Localize.h"
#include "CSGO/LocalPlayer.h"
#include "CSGO/ModelInfo.h"
#include "CSGO/ModelRender.h"
#include "CSGO/NetworkChannel.h"
#include "CSGO/PlantedC4.h"
#include "CSGO/PlayerResource.h"
#include "CSGO/Sound.h"
#include "CSGO/Steam.h"
#include "CSGO/UtlVector.h"
#include "CSGO/WeaponId.h"
#include "CSGO/WeaponData.h"

#include <Interfaces/ClientInterfaces.h>
#include <Interfaces/OtherInterfaces.h>

auto operator<(const BaseData& a, const BaseData& b) noexcept
{
    return a.distanceToLocal > b.distanceToLocal;
}

static csgo::Matrix4x4 viewMatrix;
static LocalPlayerData localPlayerData;
static std::vector<PlayerData> playerData;
static std::vector<ObserverData> observerData;
static std::vector<WeaponData> weaponData;
static std::vector<EntityData> entityData;
static std::vector<LootCrateData> lootCrateData;
static std::forward_list<ProjectileData> projectileData;
static BombData bombData;
static std::vector<InfernoData> infernoData;
static std::atomic_int netOutgoingLatency;

static auto playerByHandleWritable(int handle) noexcept
{
    const auto it = std::ranges::find(playerData, handle, &PlayerData::handle);
    return it != playerData.end() ? &(*it) : nullptr;
}

static void updateNetLatency(const csgo::Engine& engine) noexcept
{
    if (const auto networkChannel = engine.getNetworkChannel())
        netOutgoingLatency = (std::max)(static_cast<int>(csgo::NetworkChannel::from(retSpoofGadgets->client, networkChannel).getLatency(0) * 1000.0f), 0);
    else
        netOutgoingLatency = 0;
}

constexpr auto playerVisibilityUpdateDelay = 0.1f;
static float nextPlayerVisibilityUpdateTime = 0.0f;

static bool shouldUpdatePlayerVisibility(const Memory& memory) noexcept
{
    return nextPlayerVisibilityUpdateTime <= memory.globalVars->realtime;
}

void GameData::update(const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory) noexcept
{
    static int lastFrame;
    if (lastFrame == memory.globalVars->framecount)
        return;
    lastFrame = memory.globalVars->framecount;

    updateNetLatency(engineInterfaces.getEngine());

    Lock lock;
    observerData.clear();
    weaponData.clear();
    entityData.clear();
    lootCrateData.clear();
    infernoData.clear();

    localPlayerData.update(engineInterfaces.getEngine());
    bombData.update(memory);

    if (!localPlayer) {
        playerData.clear();
        projectileData.clear();
        return;
    }

    viewMatrix = engineInterfaces.getEngine().worldToScreenMatrix();

    const auto observerTarget = csgo::Entity::from(retSpoofGadgets->client, localPlayer.get().getObserverMode() == ObsMode::InEye ? localPlayer.get().getObserverTarget() : nullptr);

    const auto highestEntityIndex = clientInterfaces.getEntityList().getHighestEntityIndex();
    for (int i = 1; i <= highestEntityIndex; ++i) {
        const auto entity = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntity(i));
        if (entity.getPOD() == nullptr)
            continue;

        if (entity.isPlayer()) {
            if (entity.getPOD() == localPlayer.get().getPOD() || entity.getPOD() == observerTarget.getPOD())
                continue;

            if (const auto player = playerByHandleWritable(entity.handle())) {
                player->update(engineInterfaces, interfaces, memory, entity);
            } else {
                playerData.emplace_back(engineInterfaces, interfaces, memory, entity);
            }

            if (!entity.getNetworkable().isDormant() && !entity.isAlive()) {
                if (const auto obs = csgo::Entity::from(retSpoofGadgets->client, entity.getObserverTarget()); obs.getPOD() != nullptr)
                    observerData.emplace_back(entity, obs, obs.getPOD() == localPlayer.get().getPOD());
            }
        } else {
            if (entity.getNetworkable().isDormant())
                continue;

            if (entity.isWeapon()) {
                if (entity.ownerEntity() == -1)
                    weaponData.emplace_back(interfaces, entity);
            } else {
                switch (entity.getNetworkable().getClientClass()->classId) {
                case ClassId::BaseCSGrenadeProjectile:
                    if (!entity.shouldDraw()) {
                        if (const auto it = std::ranges::find(projectileData, entity.handle(), &ProjectileData::handle); it != projectileData.end())
                            it->exploded = true;
                        break;
                    }
                    [[fallthrough]];
                case ClassId::BreachChargeProjectile:
                case ClassId::BumpMineProjectile:
                case ClassId::DecoyProjectile:
                case ClassId::MolotovProjectile:
                case ClassId::SensorGrenadeProjectile:
                case ClassId::SmokeGrenadeProjectile:
                case ClassId::SnowballProjectile:
                    if (const auto it = std::ranges::find(projectileData, entity.handle(), &ProjectileData::handle); it != projectileData.end())
                        it->update(memory, entity);
                    else
                        projectileData.emplace_front(clientInterfaces, memory, entity);
                    break;
                case ClassId::DynamicProp:
                    if (const auto model = entity.getRenderable().getModel(); !model || !std::strstr(model->name, "challenge_coin"))
                        break;
                    [[fallthrough]];
                case ClassId::EconEntity:
                case ClassId::Chicken:
                case ClassId::PlantedC4:
                case ClassId::Hostage:
                case ClassId::Dronegun:
                case ClassId::Cash:
                case ClassId::AmmoBox:
                case ClassId::RadarJammer:
                case ClassId::SnowballPile:
                    entityData.emplace_back(entity);
                    break;
                case ClassId::LootCrate:
                    lootCrateData.emplace_back(entity);
                    break;
                case ClassId::Inferno:
                    infernoData.emplace_back(entity);
                    break;
                default:
                    break;
                }
            }
        }
    }

    std::sort(playerData.begin(), playerData.end());
    std::sort(weaponData.begin(), weaponData.end());
    std::sort(entityData.begin(), entityData.end());
    std::sort(lootCrateData.begin(), lootCrateData.end());

    std::ranges::for_each(projectileData, [&clientInterfaces](auto& projectile) {
        if (clientInterfaces.getEntityList().getEntityFromHandle(projectile.handle) == 0)
            projectile.exploded = true;
    });

    std::erase_if(projectileData, [&memory, &clientInterfaces](const auto& projectile) { return clientInterfaces.getEntityList().getEntityFromHandle(projectile.handle) == 0
        && (projectile.trajectory.size() < 1 || projectile.trajectory[projectile.trajectory.size() - 1].first + 60.0f < memory.globalVars->realtime); });

    std::erase_if(playerData, [&clientInterfaces](const auto& player) { return clientInterfaces.getEntityList().getEntityFromHandle(player.handle) == 0; });

    if (shouldUpdatePlayerVisibility(memory))
        nextPlayerVisibilityUpdateTime = memory.globalVars->realtime + playerVisibilityUpdateDelay;
}

void GameData::clearProjectileList() noexcept
{
    Lock lock;
    projectileData.clear();
}

static void clearAvatarTextures() noexcept;

struct PlayerAvatar {
    mutable Texture texture;
    std::unique_ptr<std::uint8_t[]> rgba;
};

static std::unordered_map<int, PlayerAvatar> playerAvatars;

void GameData::clearTextures() noexcept
{
    Lock lock;

    clearAvatarTextures();
    for (const auto& [handle, avatar] : playerAvatars)
        avatar.texture.clear();
}

void GameData::clearUnusedAvatars() noexcept
{
    Lock lock;
    std::erase_if(playerAvatars, [](const auto& pair) { return std::ranges::find(std::as_const(playerData), pair.first, &PlayerData::handle) == playerData.cend(); });
}

int GameData::getNetOutgoingLatency() noexcept
{
    return netOutgoingLatency;
}

const csgo::Matrix4x4& GameData::toScreenMatrix() noexcept
{
    return viewMatrix;
}

const LocalPlayerData& GameData::local() noexcept
{
    return localPlayerData;
}

const std::vector<PlayerData>& GameData::players() noexcept
{
    return playerData;
}

const PlayerData* GameData::playerByHandle(int handle) noexcept
{
    return playerByHandleWritable(handle);
}

const std::vector<ObserverData>& GameData::observers() noexcept
{
    return observerData;
}

const std::vector<WeaponData>& GameData::weapons() noexcept
{
    return weaponData;
}

const std::vector<EntityData>& GameData::entities() noexcept
{
    return entityData;
}

const std::vector<LootCrateData>& GameData::lootCrates() noexcept
{
    return lootCrateData;
}

const std::forward_list<ProjectileData>& GameData::projectiles() noexcept
{
    return projectileData;
}

const BombData& GameData::plantedC4() noexcept
{
    return bombData;
}

const std::vector<InfernoData>& GameData::infernos() noexcept
{
    return infernoData;
}

void LocalPlayerData::update(const csgo::Engine& engine) noexcept
{
    if (!localPlayer) {
        exists = false;
        return;
    }

    exists = true;
    alive = localPlayer.get().isAlive();

    if (const auto activeWeapon = csgo::Entity::from(retSpoofGadgets->client, localPlayer.get().getActiveWeapon()); activeWeapon.getPOD() != nullptr) {
        inReload = activeWeapon.isInReload();
        shooting = localPlayer.get().shotsFired() > 1;
        noScope = activeWeapon.isSniperRifle() && !localPlayer.get().isScoped();
        nextWeaponAttack = activeWeapon.nextPrimaryAttack();
    }
    fov = localPlayer.get().fov() ? localPlayer.get().fov() : localPlayer.get().defaultFov();
    handle = localPlayer.get().handle();
    flashDuration = localPlayer.get().flashDuration();

    aimPunch = localPlayer.get().getEyePosition() + csgo::Vector::fromAngle(engine.getViewAngles() + localPlayer.get().getAimPunch()) * 1000.0f;

    const auto obsMode = localPlayer.get().getObserverMode();
    if (const auto obs = csgo::Entity::from(retSpoofGadgets->client, localPlayer.get().getObserverTarget()); obs.getPOD() != nullptr && obsMode != ObsMode::Roaming && obsMode != ObsMode::Deathcam)
        origin = obs.getAbsOrigin();
    else
        origin = localPlayer.get().getAbsOrigin();
}

BaseData::BaseData(const csgo::Entity& entity) noexcept
{
    distanceToLocal = entity.getAbsOrigin().distTo(localPlayerData.origin);
 
    if (entity.isPlayer()) {
        const auto collideable{ csgo::Collideable::from(retSpoofGadgets->client, entity.getCollideable()) };
        obbMins = collideable.obbMins();
        obbMaxs = collideable.obbMaxs();
    } else if (const auto model = entity.getRenderable().getModel()) {
        obbMins = model->mins;
        obbMaxs = model->maxs;
    }

    coordinateFrame = entity.getRenderable().toWorldTransform();
}

EntityData::EntityData(const csgo::Entity& entity) noexcept : BaseData{ entity }
{
    name = [](const csgo::Entity& entity) {
        switch (entity.getNetworkable().getClientClass()->classId) {
        case ClassId::EconEntity: return "Defuse Kit";
        case ClassId::Chicken: return "Chicken";
        case ClassId::PlantedC4: return "Planted C4";
        case ClassId::Hostage: return "Hostage";
        case ClassId::Dronegun: return "Sentry";
        case ClassId::Cash: return "Cash";
        case ClassId::AmmoBox: return "Ammo Box";
        case ClassId::RadarJammer: return "Radar Jammer";
        case ClassId::SnowballPile: return "Snowball Pile";
        case ClassId::DynamicProp: return "Collectable Coin";
        default: assert(false); return "unknown";
        }
    }(entity);
}

ProjectileData::ProjectileData(const ClientInterfaces& clientInterfaces, const Memory& memory, const csgo::Entity& projectile) noexcept : BaseData { projectile }
{
    name = [](const csgo::Entity& projectile) {
        switch (projectile.getNetworkable().getClientClass()->classId) {
        case ClassId::BaseCSGrenadeProjectile:
            if (const auto model = projectile.getRenderable().getModel(); model && strstr(model->name, "flashbang"))
                return "Flashbang";
            else
                return "HE Grenade";
        case ClassId::BreachChargeProjectile: return "Breach Charge";
        case ClassId::BumpMineProjectile: return "Bump Mine";
        case ClassId::DecoyProjectile: return "Decoy Grenade";
        case ClassId::MolotovProjectile: return "Molotov";
        case ClassId::SensorGrenadeProjectile: return "TA Grenade";
        case ClassId::SmokeGrenadeProjectile: return "Smoke Grenade";
        case ClassId::SnowballProjectile: return "Snowball";
        default: assert(false); return "unknown";
        }
    }(projectile);

    if (const auto thrower = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntityFromHandle(projectile.thrower())); thrower.getPOD() != nullptr && localPlayer) {
        if (thrower.getPOD() == localPlayer.get().getPOD())
            thrownByLocalPlayer = true;
        else
            thrownByEnemy = localPlayer.get().isOtherEnemy(memory, thrower);
    }

    handle = projectile.handle();
}

void ProjectileData::update(const Memory& memory, const csgo::Entity& projectile) noexcept
{
    static_cast<BaseData&>(*this) = { projectile };

    if (const auto& pos = projectile.getAbsOrigin(); trajectory.size() < 1 || trajectory[trajectory.size() - 1].second != pos)
        trajectory.emplace_back(memory.globalVars->realtime, pos);
}

PlayerData::PlayerData(const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const csgo::Entity& entity) noexcept : BaseData{ entity }, handle{ entity.handle() }
{
    if (const auto steamID = entity.getSteamId(engineInterfaces.getEngine())) {
        const auto ctx = engineInterfaces.getEngine().getSteamAPIContext();
        const auto avatar = csgo::SteamFriends::from(retSpoofGadgets->client, ctx->steamFriends).getSmallFriendAvatar(steamID);
        constexpr auto rgbaDataSize = 4 * 32 * 32;

        PlayerAvatar playerAvatar;
        playerAvatar.rgba = std::make_unique<std::uint8_t[]>(rgbaDataSize);
        if (csgo::SteamUtils::from(retSpoofGadgets->client, ctx->steamUtils).getImageRGBA(avatar, playerAvatar.rgba.get(), rgbaDataSize))
            playerAvatars[handle] = std::move(playerAvatar);
    }

    update(engineInterfaces, interfaces, memory, entity);
}

void PlayerData::update(const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const csgo::Entity& entity) noexcept
{
    name = entity.getPlayerName(interfaces, memory);

    dormant = entity.getNetworkable().isDormant();
    if (dormant)
        return;

    team = entity.getTeamNumber();
    static_cast<BaseData&>(*this) = { entity };
    origin = entity.getAbsOrigin();
    inViewFrustum = !engineInterfaces.getEngine().cullBox(obbMins + origin, obbMaxs + origin);
    alive = entity.isAlive();
    lastContactTime = alive ? memory.globalVars->realtime : 0.0f;

    if (localPlayer) {
        enemy = localPlayer.get().isOtherEnemy(memory, entity);

        if (!inViewFrustum || !alive)
            visible = false;
        else if (shouldUpdatePlayerVisibility(memory))
            visible = entity.visibleTo(engineInterfaces, memory, localPlayer.get());
    }

    auto isEntityAudible = [&memory](int entityIndex) noexcept {
        for (int i = 0; i < memory.activeChannels->count; ++i)
            if (memory.channels[memory.activeChannels->list[i]].soundSource == entityIndex)
                return true;
        return false;
    };

    audible = isEntityAudible(entity.getNetworkable().index());
    spotted = entity.spotted();
    health = entity.health();
    immune = entity.gunGameImmunity();
    flashDuration = entity.flashDuration();

    if (const auto weapon = csgo::Entity::from(retSpoofGadgets->client, entity.getActiveWeapon()); weapon.getPOD() != nullptr) {
        audible = audible || isEntityAudible(weapon.getNetworkable().index());
        if (const auto weaponInfo = weapon.getWeaponData())
            activeWeapon = interfaces.getLocalize().findAsUTF8(weaponInfo->name);
    }

    if (!alive || !inViewFrustum)
        return;

    const auto model = entity.getRenderable().getModel();
    if (!model)
        return;

    const auto studioModel = engineInterfaces.getModelInfo().getStudioModel(model);
    if (!studioModel)
        return;

    csgo::matrix3x4 boneMatrices[MAXSTUDIOBONES];
    if (!entity.setupBones(boneMatrices, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, memory.globalVars->currenttime))
        return;

    bones.clear();
    bones.reserve(20);

    for (int i = 0; i < studioModel->numBones; ++i) {
        const auto bone = studioModel->getBone(i);

        if (!bone || bone->parent == -1 || !(bone->flags & BONE_USED_BY_HITBOX))
            continue;

        bones.emplace_back(boneMatrices[i].origin(), boneMatrices[bone->parent].origin());
    }

    const auto set = studioModel->getHitboxSet(entity.hitboxSet());
    if (!set)
        return;

    const auto headBox = set->getHitbox(0);

    headMins = transform(headBox->bbMin, boneMatrices[headBox->bone]);
    headMaxs = transform(headBox->bbMax, boneMatrices[headBox->bone]);

    if (headBox->capsuleRadius > 0.0f) {
        headMins -= headBox->capsuleRadius;
        headMaxs += headBox->capsuleRadius;
    }
}

struct PNGTexture {
    template <std::size_t N>
    PNGTexture(const std::array<char, N>& png) noexcept : pngData{ png.data() }, pngDataSize{ png.size() } {}

    ImTextureID getTexture() const noexcept
    {
        if (!texture.get()) {
            int width, height;
            stbi_set_flip_vertically_on_load_thread(false);

            if (const auto data = stbi_load_from_memory((const stbi_uc*)pngData, pngDataSize, &width, &height, nullptr, STBI_rgb_alpha)) {
                texture.init(width, height, data);
                stbi_image_free(data);
            } else {
                assert(false);
            }
        }

        return texture.get();
    }

    void clearTexture() const noexcept { texture.clear(); }

private:
    const char* pngData;
    std::size_t pngDataSize;

    mutable Texture texture;
};

static const PNGTexture avatarTT{ Resource::avatar_tt };
static const PNGTexture avatarCT{ Resource::avatar_ct };

static void clearAvatarTextures() noexcept
{
    avatarTT.clearTexture();
    avatarCT.clearTexture();
}

ImTextureID PlayerData::getAvatarTexture() const noexcept
{
    const auto it = std::as_const(playerAvatars).find(handle);
    if (it == playerAvatars.cend())
        return team == csgo::Team::TT ? avatarTT.getTexture() : avatarCT.getTexture();

    const auto& avatar = it->second;
    if (!avatar.texture.get())
        avatar.texture.init(32, 32, avatar.rgba.get());
    return avatar.texture.get();
}

float PlayerData::fadingAlpha(const Memory& memory) const noexcept
{
    constexpr float fadeTime = 1.50f;
    return std::clamp(1.0f - (memory.globalVars->realtime - lastContactTime - 0.25f) / fadeTime, 0.0f, 1.0f);
}

WeaponData::WeaponData(const OtherInterfaces& interfaces, const csgo::Entity& entity) noexcept : BaseData{ entity }
{
    clip = entity.clip();
    reserveAmmo = entity.reserveAmmoCount();

    if (const auto weaponInfo = entity.getWeaponData()) {
        group = [](WeaponType type, WeaponId weaponId) {
            switch (type) {
            case WeaponType::Pistol: return "Pistols";
            case WeaponType::SubMachinegun: return "SMGs";
            case WeaponType::Rifle: return "Rifles";
            case WeaponType::SniperRifle: return "Sniper Rifles";
            case WeaponType::Shotgun: return "Shotguns";
            case WeaponType::Machinegun: return "Machineguns";
            case WeaponType::Grenade: return "Grenades";
            case WeaponType::Melee: return "Melee";
            default:
                switch (weaponId) {
                case WeaponId::C4:
                case WeaponId::Healthshot:
                case WeaponId::BumpMine:
                case WeaponId::ZoneRepulsor:
                case WeaponId::Shield:
                    return "Other";
                default: return "All";
                }
            }
        }(weaponInfo->type, entity.itemDefinitionIndex());
        name = [](WeaponId weaponId) {
            switch (weaponId) {
            default: return "All";

            case WeaponId::Glock: return "Glock-18";
            case WeaponId::Hkp2000: return "P2000";
            case WeaponId::Usp_s: return "USP-S";
            case WeaponId::Elite: return "Dual Berettas";
            case WeaponId::P250: return "P250";
            case WeaponId::Tec9: return "Tec-9";
            case WeaponId::Fiveseven: return "Five-SeveN";
            case WeaponId::Cz75a: return "CZ75-Auto";
            case WeaponId::Deagle: return "Desert Eagle";
            case WeaponId::Revolver: return "R8 Revolver";

            case WeaponId::Mac10: return "MAC-10";
            case WeaponId::Mp9: return "MP9";
            case WeaponId::Mp7: return "MP7";
            case WeaponId::Mp5sd: return "MP5-SD";
            case WeaponId::Ump45: return "UMP-45";
            case WeaponId::P90: return "P90";
            case WeaponId::Bizon: return "PP-Bizon";

            case WeaponId::GalilAr: return "Galil AR";
            case WeaponId::Famas: return "FAMAS";
            case WeaponId::Ak47: return "AK-47";
            case WeaponId::M4A1: return "M4A4";
            case WeaponId::M4a1_s: return "M4A1-S";
            case WeaponId::Sg553: return "SG 553";
            case WeaponId::Aug: return "AUG";

            case WeaponId::Ssg08: return "SSG 08";
            case WeaponId::Awp: return "AWP";
            case WeaponId::G3SG1: return "G3SG1";
            case WeaponId::Scar20: return "SCAR-20";

            case WeaponId::Nova: return "Nova";
            case WeaponId::Xm1014: return "XM1014";
            case WeaponId::Sawedoff: return "Sawed-Off";
            case WeaponId::Mag7: return "MAG-7";

            case WeaponId::M249: return "M249";
            case WeaponId::Negev: return "Negev";

            case WeaponId::Flashbang: return "Flashbang";
            case WeaponId::HeGrenade: return "HE Grenade";
            case WeaponId::SmokeGrenade: return "Smoke Grenade";
            case WeaponId::Molotov: return "Molotov";
            case WeaponId::Decoy: return "Decoy Grenade";
            case WeaponId::IncGrenade: return "Incendiary";
            case WeaponId::TaGrenade: return "TA Grenade";
            case WeaponId::Firebomb: return "Fire Bomb";
            case WeaponId::Diversion: return "Diversion";
            case WeaponId::FragGrenade: return "Frag Grenade";
            case WeaponId::Snowball: return "Snowball";

            case WeaponId::Axe: return "Axe";
            case WeaponId::Hammer: return "Hammer";
            case WeaponId::Spanner: return "Wrench";

            case WeaponId::C4: return "C4";
            case WeaponId::Healthshot: return "Healthshot";
            case WeaponId::BumpMine: return "Bump Mine";
            case WeaponId::ZoneRepulsor: return "Zone Repulsor";
            case WeaponId::Shield: return "Shield";
            }
        }(entity.itemDefinitionIndex());

        displayName = interfaces.getLocalize().findAsUTF8(weaponInfo->name);
    }
}

LootCrateData::LootCrateData(const csgo::Entity& entity) noexcept : BaseData{ entity }
{
    const auto model = entity.getRenderable().getModel();
    if (!model)
        return;

    name = [](const char* modelName) -> const char* {
        switch (fnv::hashRuntime(modelName)) {
        case fnv::hash("models/props_survival/cases/case_pistol.mdl"): return "Pistol Case";
        case fnv::hash("models/props_survival/cases/case_light_weapon.mdl"): return "Light Case";
        case fnv::hash("models/props_survival/cases/case_heavy_weapon.mdl"): return "Heavy Case";
        case fnv::hash("models/props_survival/cases/case_explosive.mdl"): return "Explosive Case";
        case fnv::hash("models/props_survival/cases/case_tools.mdl"): return "Tools Case";
        case fnv::hash("models/props_survival/cash/dufflebag.mdl"): return "Cash Dufflebag";
        default: return nullptr;
        }
    }(model->name);
}

ObserverData::ObserverData(const csgo::Entity& entity, const csgo::Entity& obs, bool targetIsLocalPlayer) noexcept : playerHandle{ entity.handle() }, targetHandle{ obs.handle() }, targetIsLocalPlayer{ targetIsLocalPlayer } {}

void BombData::update(const Memory& memory) noexcept
{
    if (memory.plantedC4s->size > 0 && (!*memory.gameRules || csgo::Entity::from(retSpoofGadgets->client, *memory.gameRules).mapHasBombTarget())) {
        if (const auto bomb = csgo::PlantedC4::from(retSpoofGadgets->client, (*memory.plantedC4s)[0]); bomb.getPOD() != nullptr && bomb.c4Ticking()) {
            blowTime = bomb.c4BlowTime();
            timerLength = bomb.c4TimerLength();
            defuserHandle = bomb.c4Defuser();
            if (defuserHandle != -1) {
                defuseCountDown = bomb.c4DefuseCountDown();
                defuseLength = bomb.c4DefuseLength();
            }

            if (*memory.playerResource) {
                const auto& bombOrigin = bomb.getEntity().origin();
                bombsite = bombOrigin.distTo((*memory.playerResource)->bombsiteCenterA()) > bombOrigin.distTo((*memory.playerResource)->bombsiteCenterB());
            }
            return;
        }
    }
    blowTime = 0.0f;
}

InfernoData::InfernoData(const csgo::Entity& inferno) noexcept
{
    const auto& origin = inferno.getAbsOrigin();

    points.reserve(inferno.fireCount());
    for (int i = 0; i < inferno.fireCount(); ++i) {
        if (inferno.fireIsBurning()[i])
            points.emplace_back(inferno.fireXDelta()[i] + origin.x, inferno.fireYDelta()[i] + origin.y, inferno.fireZDelta()[i] + origin.z);
    }
}
