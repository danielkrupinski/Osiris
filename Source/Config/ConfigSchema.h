#pragma once

template <typename HookContext>
class ConfigSchema {
public:
    explicit ConfigSchema(HookContext& hookContext) noexcept
        : hookContext{hookContext}
        , featuresStates{hookContext.featuresStates()}
    {
    }

    [[nodiscard]] decltype(auto) performConversion(auto&& configConversion) noexcept
    {
        configConversion.beginRoot();
        hudObject(configConversion);
        visualsObject(configConversion);
        soundObject(configConversion);
        return configConversion.endRoot();
    }

private:
    void hudObject(auto&& configConversion) noexcept
    {
        configConversion.beginObject(u8"Hud");

        configConversion.beginObject(u8"BombTimer");
        configConversion.boolean(u8"Enabled", loadBombTimerEnabled(), saveBool(featuresStates.hudFeaturesStates.bombTimerState.enabled));
        configConversion.endObject();

        configConversion.beginObject(u8"BombDefuseAlert");
        configConversion.boolean(u8"Enabled", loadBombDefuseAlertEnabled(), saveBool(featuresStates.hudFeaturesStates.defusingAlertState.enabled));
        configConversion.endObject();

        configConversion.beginObject(u8"PreserveKillfeed");
        configConversion.boolean(u8"Enabled", loadKillfeedPreserverEnabled(), saveBool(featuresStates.hudFeaturesStates.killfeedPreserverState.enabled));
        configConversion.endObject();

        configConversion.beginObject(u8"PostRoundTimer");
        configConversion.boolean(u8"Enabled", loadPostRoundTimerEnabled(), saveBool(featuresStates.hudFeaturesStates.postRoundTimerState.enabled));
        configConversion.endObject();

        configConversion.endObject();
    }

    void visualsObject(auto&& configConversion) noexcept
    {
        configConversion.beginObject(u8"Visuals");

        configConversion.beginObject(u8"ModelGlow");
        configConversion.boolean(u8"Enabled", loadModelGlowEnabled(), saveBool(featuresStates.visualFeaturesStates.modelGlowState.masterSwitch == ModelGlowState::State::Enabled));
        configConversion.beginObject(u8"Players");
        configConversion.boolean(u8"Enabled", loadPlayerModelGlowEnabled(), saveBool(featuresStates.visualFeaturesStates.modelGlowState.playerModelGlow == ModelGlowState::State::Enabled));
        configConversion.boolean(u8"OnlyEnemies", loadBool(featuresStates.visualFeaturesStates.modelGlowState.showOnlyEnemies), saveBool(featuresStates.visualFeaturesStates.modelGlowState.showOnlyEnemies));
        configConversion.uint(u8"ColorMode", loadPlayerModelGlowColorMode(), savePlayerModelGlowColorMode());
        configConversion.endObject();
        configConversion.boolean(u8"Weapons", loadWeaponModelGlowEnabled(), saveBool(featuresStates.visualFeaturesStates.modelGlowState.weaponModelGlow == ModelGlowState::State::Enabled));
        configConversion.boolean(u8"DroppedBomb", loadDroppedBombModelGlowEnabled(), saveBool(featuresStates.visualFeaturesStates.modelGlowState.droppedBombModelGlow == ModelGlowState::State::Enabled));
        configConversion.boolean(u8"TickingBomb", loadTickingBombModelGlowEnabled(), saveBool(featuresStates.visualFeaturesStates.modelGlowState.tickingBombModelGlow == ModelGlowState::State::Enabled));
        configConversion.boolean(u8"DefuseKits", loadDefuseKitModelGlowEnabled(), saveBool(featuresStates.visualFeaturesStates.modelGlowState.defuseKitModelGlow == ModelGlowState::State::Enabled));
        configConversion.boolean(u8"GrenadeProjectiles", loadGrenadeProjectileModelGlowEnabled(), saveBool(featuresStates.visualFeaturesStates.modelGlowState.grenadeProjectileModelGlow == ModelGlowState::State::Enabled));
        configConversion.endObject();

        configConversion.beginObject(u8"OutlineGlow");
        configConversion.boolean(u8"Enabled", loadOutlineGlowEnabled(), saveBool(featuresStates.visualFeaturesStates.outlineGlowState.masterSwitch));
        configConversion.beginObject(u8"Players");
        configConversion.boolean(u8"Enabled", loadBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForPlayers), saveBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForPlayers));
        configConversion.boolean(u8"OnlyEnemies", loadBool(featuresStates.visualFeaturesStates.outlineGlowState.showOnlyEnemies), saveBool(featuresStates.visualFeaturesStates.outlineGlowState.showOnlyEnemies));
        configConversion.uint(u8"ColorMode", loadPlayerOutlineGlowColorMode(), savePlayerOutlineGlowColorMode());
        configConversion.endObject();
        configConversion.boolean(u8"Weapons", loadBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForWeapons), saveBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForWeapons));
        configConversion.boolean(u8"DroppedBomb", loadBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForDroppedBomb), saveBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForDroppedBomb));
        configConversion.boolean(u8"TickingBomb", loadBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForTickingBomb), saveBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForTickingBomb));
        configConversion.boolean(u8"DefuseKits", loadBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForDefuseKits), saveBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForDefuseKits));
        configConversion.boolean(u8"Hostages", loadBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForHostages), saveBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForHostages));
        configConversion.boolean(u8"GrenadeProjectiles", loadBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForGrenadeProjectiles), saveBool(featuresStates.visualFeaturesStates.outlineGlowState.enabledForGrenadeProjectiles));
        configConversion.endObject();

        configConversion.beginObject(u8"PlayerInfoInWorld");
        configConversion.boolean(u8"Enabled", loadBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.enabled), saveBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.enabled));
        configConversion.boolean(u8"OnlyEnemies", loadBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showOnlyEnemies), saveBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showOnlyEnemies));

        configConversion.beginObject(u8"PlayerPositionArrow");
        configConversion.boolean(u8"Enabled", loadBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showPlayerPosition), saveBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showPlayerPosition));
        configConversion.uint(u8"ColorMode", loadPlayerPositionArrowColorMode(), savePlayerPositionArrowColorMode());
        configConversion.endObject();

        configConversion.beginObject(u8"Health");
        configConversion.boolean(u8"Enabled", loadBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showPlayerHealth), saveBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showPlayerHealth));
        configConversion.uint(u8"ColorMode", loadPlayerHealthColorMode(), savePlayerHealthColorMode());
        configConversion.endObject();

        configConversion.boolean(u8"ActiveWeaponIcon", loadBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showPlayerActiveWeapon), saveBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showPlayerActiveWeapon));
        configConversion.boolean(u8"BombCarrierIcon", loadBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showBombCarrierIcon), saveBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showBombCarrierIcon));
        configConversion.boolean(u8"BombPlantIcon", loadBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showBombPlantingIcon), saveBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showBombPlantingIcon));
        configConversion.boolean(u8"ActiveWeaponAmmo", loadBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showPlayerActiveWeaponAmmo), saveBool(featuresStates.visualFeaturesStates.playerInfoInWorldState.showPlayerActiveWeaponAmmo));
        configConversion.boolean(u8"BombDefuseIcon", loadDefuseIconEnabled(), saveDefuseIconEnabled());
        configConversion.boolean(u8"HostagePickupIcon", loadHostagePickupIconEnabled(), saveHostagePickupIconEnabled());
        configConversion.boolean(u8"HostageRescueIcon", loadHostageRescueIconEnabled(), saveHostageRescueIconEnabled());
        configConversion.boolean(u8"BlindedIcon", loadBlindedIconEnabled(), saveBlindedIconEnabled());
        configConversion.endObject();

        configConversion.endObject();
    }

    void soundObject(auto&& configConversion) noexcept
    {
        configConversion.beginObject(u8"Sound");
        configConversion.beginObject(u8"Visualizations");

        configConversion.beginObject(u8"Footsteps");
        configConversion.boolean(u8"Enabled", loadFootstepSoundVisualizationEnabled(), saveBool(featuresStates.soundFeaturesStates.footstepVisualizerState.enabled));
        configConversion.endObject();

        configConversion.beginObject(u8"BombPlant");
        configConversion.boolean(u8"Enabled", loadBombPlantSoundVisualizationEnabled(), saveBool(featuresStates.soundFeaturesStates.bombPlantVisualizerState.enabled));
        configConversion.endObject();

        configConversion.beginObject(u8"BombBeep");
        configConversion.boolean(u8"Enabled", loadBombBeepSoundVisualizationEnabled(), saveBool(featuresStates.soundFeaturesStates.bombBeepVisualizerState.enabled));
        configConversion.endObject();

        configConversion.beginObject(u8"BombDefuse");
        configConversion.boolean(u8"Enabled", loadBombDefuseSoundVisualizationEnabled(), saveBool(featuresStates.soundFeaturesStates.bombDefuseVisualizerState.enabled));
        configConversion.endObject();

        configConversion.beginObject(u8"WeaponScope");
        configConversion.boolean(u8"Enabled", loadWeaponScopeSoundVisualizationEnabled(), saveBool(featuresStates.soundFeaturesStates.weaponScopeVisualizerState.enabled));
        configConversion.endObject();

        configConversion.beginObject(u8"WeaponReload");
        configConversion.boolean(u8"Enabled", loadWeaponReloadSoundVisualizationEnabled(), saveBool(featuresStates.soundFeaturesStates.weaponReloadVisualizerState.enabled));
        configConversion.endObject();

        configConversion.endObject();
        configConversion.endObject();
    }

    [[nodiscard]] auto loadBool(bool& dest) const noexcept
    {
        return [&dest](bool value) {
            dest = value;
        };
    }

    [[nodiscard]] auto saveBool(bool value) const noexcept
    {
        return [value] {
            return value;
        };
    }

    [[nodiscard]] auto saveBombTimerEnabled() const noexcept
    {
        return [this] {
            return featuresStates.hudFeaturesStates.bombTimerState.enabled;
        };
    }

    [[nodiscard]] auto loadBombTimerEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& bombTimerToggle = BombTimerToggle{BombTimerContext{hookContext}};
            if (value)
                bombTimerToggle.enable();
            else
                bombTimerToggle.disable();
        };
    }

    [[nodiscard]] auto loadBombDefuseAlertEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& bombDefuseAlertToggle = DefusingAlertToggle{DefusingAlertContext{hookContext}};
            if (value)
                bombDefuseAlertToggle.enable();
            else
                bombDefuseAlertToggle.disable();
        };
    }

    [[nodiscard]] auto loadKillfeedPreserverEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& killfeedPreserverToggle = KillfeedPreserveToggle{KillfeedPreserverContext{hookContext}};
            if (value)
                killfeedPreserverToggle.enable();
            else
                killfeedPreserverToggle.disable();
        };
    }

    [[nodiscard]] auto loadPostRoundTimerEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& postRoundTimerToggle = hookContext.template make<PostRoundTimerToggle>();
            if (value)
                postRoundTimerToggle.enable();
            else
                postRoundTimerToggle.disable();
        };
    }

    [[nodiscard]] auto loadModelGlowEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& modelGlow = hookContext.template make<ModelGlow>();
            if (value)
                modelGlow.enable();
            else
                modelGlow.disable();
        };
    }

    [[nodiscard]] auto loadPlayerModelGlowEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& playerModelGlow = hookContext.template make<PlayerModelGlow>();
            if (value)
                playerModelGlow.enable();
            else
                playerModelGlow.disable();
        };
    }

    [[nodiscard]] auto loadWeaponModelGlowEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& weaponModelGlow = hookContext.template make<WeaponModelGlow>();
            if (value)
                weaponModelGlow.enable();
            else
                weaponModelGlow.disable();
        };
    }

    [[nodiscard]] auto loadDroppedBombModelGlowEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& droppedBombModelGlow = hookContext.template make<DroppedBombModelGlow>();
            if (value)
                droppedBombModelGlow.enable();
            else
                droppedBombModelGlow.disable();
        };
    }

    [[nodiscard]] auto loadTickingBombModelGlowEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& tickingBombModelGlow = hookContext.template make<TickingBombModelGlow>();
            if (value)
                tickingBombModelGlow.enable();
            else
                tickingBombModelGlow.disable();
        };
    }

    [[nodiscard]] auto loadDefuseKitModelGlowEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& defuseKitModelGlow = hookContext.template make<DefuseKitModelGlow>();
            if (value)
                defuseKitModelGlow.enable();
            else
                defuseKitModelGlow.disable();
        };
    }

    [[nodiscard]] auto loadGrenadeProjectileModelGlowEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& grenadeProjectileModelGlow = hookContext.template make<GrenadeProjectileModelGlow>();
            if (value)
                grenadeProjectileModelGlow.enable();
            else
                grenadeProjectileModelGlow.disable();
        };
    }

    [[nodiscard]] auto loadPlayerModelGlowColorMode() const noexcept
    {
        return [this](std::uint64_t value) {
            featuresStates.visualFeaturesStates.modelGlowState.playerModelGlowColorType = static_cast<PlayerModelGlowColorType>(value);
        };
    }

    [[nodiscard]] auto savePlayerModelGlowColorMode() const noexcept
    {
        return [this] {
            return static_cast<std::underlying_type_t<PlayerModelGlowColorType>>(featuresStates.visualFeaturesStates.modelGlowState.playerModelGlowColorType);
        };
    }

    [[nodiscard]] auto loadOutlineGlowEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& outlineGlow = hookContext.template make<OutlineGlow>();
            if (value)
                outlineGlow.enable();
            else
                outlineGlow.disable();
        };
    }

    [[nodiscard]] auto loadPlayerOutlineGlowColorMode() const noexcept
    {
        return [this](std::uint64_t value) {
            featuresStates.visualFeaturesStates.outlineGlowState.playerGlowColorType = static_cast<PlayerOutlineGlowColorType>(value);
        };
    }

    [[nodiscard]] auto savePlayerOutlineGlowColorMode() const noexcept
    {
        return [this] {
            return static_cast<std::underlying_type_t<PlayerOutlineGlowColorType>>(featuresStates.visualFeaturesStates.outlineGlowState.playerGlowColorType);
        };
    }

    [[nodiscard]] auto loadPlayerPositionArrowColorMode() const noexcept
    {
        return [this](std::uint64_t value) {
            featuresStates.visualFeaturesStates.playerInfoInWorldState.playerPositionArrowColor = static_cast<PlayerPositionArrowColorType>(value);
        };
    }

    [[nodiscard]] auto savePlayerPositionArrowColorMode() const noexcept
    {
        return [this] {
            return static_cast<std::underlying_type_t<PlayerPositionArrowColorType>>(featuresStates.visualFeaturesStates.playerInfoInWorldState.playerPositionArrowColor);
        };
    }
    
    [[nodiscard]] auto loadPlayerHealthColorMode() const noexcept
    {
        return [this](std::uint64_t value) {
            featuresStates.visualFeaturesStates.playerInfoInWorldState.playerHealthTextColor = static_cast<PlayerHealthTextColor>(value);
        };
    }

    [[nodiscard]] auto savePlayerHealthColorMode() const noexcept
    {
        return [this] {
            return static_cast<std::underlying_type_t<PlayerHealthTextColor>>(featuresStates.visualFeaturesStates.playerInfoInWorldState.playerHealthTextColor);
        };
    }

    [[nodiscard]] auto loadDefuseIconEnabled() const noexcept
    {
        return [this](bool value) {
            if (value)
                featuresStates.visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.set<DefuseIconPanel>();
            else
                featuresStates.visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.unset<DefuseIconPanel>();
        };
    }

    [[nodiscard]] auto saveDefuseIconEnabled() const noexcept
    {
        return [this] {
            return featuresStates.visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.has<DefuseIconPanel>();
        };
    }

    [[nodiscard]] auto loadHostagePickupIconEnabled() const noexcept
    {
        return [this](bool value) {
            if (value)
                featuresStates.visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.set<HostagePickupPanel>();
            else
                featuresStates.visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.unset<HostagePickupPanel>();
        };
    }

    [[nodiscard]] auto saveHostagePickupIconEnabled() const noexcept
    {
        return [this] {
            return featuresStates.visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.has<HostagePickupPanel>();
        };
    }

    [[nodiscard]] auto loadHostageRescueIconEnabled() const noexcept
    {
        return [this](bool value) {
            if (value)
                featuresStates.visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.set<HostageRescuePanel>();
            else
                featuresStates.visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.unset<HostageRescuePanel>();
        };
    }

    [[nodiscard]] auto saveHostageRescueIconEnabled() const noexcept
    {
        return [this] {
            return featuresStates.visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.has<HostageRescuePanel>();
        };
    }

    [[nodiscard]] auto loadBlindedIconEnabled() const noexcept
    {
        return [this](bool value) {
            if (value)
                featuresStates.visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.set<BlindedIconPanel>();
            else
                featuresStates.visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.unset<BlindedIconPanel>();
        };
    }

    [[nodiscard]] auto saveBlindedIconEnabled() const noexcept
    {
        return [this] {
            return featuresStates.visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.has<BlindedIconPanel>();
        };
    }

    [[nodiscard]] auto loadFootstepSoundVisualizationEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& toggle = hookContext.features().soundFeatures().footstepVisualizerToggle();
            if (value)
                toggle.enable();
            else
                toggle.disable();
        };
    }

    [[nodiscard]] auto loadBombPlantSoundVisualizationEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& toggle = hookContext.features().soundFeatures().bombPlantVisualizerToggle();
            if (value)
                toggle.enable();
            else
                toggle.disable();
        };
    }

    [[nodiscard]] auto loadBombBeepSoundVisualizationEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& toggle = hookContext.features().soundFeatures().bombBeepVisualizerToggle();
            if (value)
                toggle.enable();
            else
                toggle.disable();
        };
    }

    [[nodiscard]] auto loadBombDefuseSoundVisualizationEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& toggle = hookContext.features().soundFeatures().bombDefuseVisualizerToggle();
            if (value)
                toggle.enable();
            else
                toggle.disable();
        };
    }

    [[nodiscard]] auto loadWeaponScopeSoundVisualizationEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& toggle = hookContext.features().soundFeatures().weaponScopeVisualizerToggle();
            if (value)
                toggle.enable();
            else
                toggle.disable();
        };
    }

    [[nodiscard]] auto loadWeaponReloadSoundVisualizationEnabled() const noexcept
    {
        return [this](bool value) {
            auto&& toggle = hookContext.features().soundFeatures().weaponReloadVisualizerToggle();
            if (value)
                toggle.enable();
            else
                toggle.disable();
        };
    }

    HookContext& hookContext;
    FeaturesStates& featuresStates;
};
