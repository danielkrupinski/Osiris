#pragma once

#include <Features/Combat/AimBot/AimBotConfigVariables.h>
#include <Features/Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVisConfigVariables.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorld.h>
#include <GameClient/Panorama/Slider.h>
#include <GameClient/Panorama/TextEntry.h>
#include <Platform/Macros/FunctionAttributes.h>
#include <Utils/StringParser.h>
#include "Tabs/VisualsTab/HueSlider.h"
#include "Tabs/VisualsTab/IntSlider.h"

template <typename HookContext>
struct SetCommandHandler {
    SetCommandHandler(StringParser& parser, HookContext& hookContext) noexcept
        : parser{parser}
        , hookContext{hookContext}
    {
    }

    void operator()() noexcept
    {
        if (const auto section = parser.getLine('/'); section == "combat") {
            handleCombatSection();
        } else if (section == "hud") {
            handleHudSection();
        } else if (section == "visuals") {
            handleVisualsSection();
        } else if (section == "sound") {
            handleSoundSection();
        }
    }

private:
    void handleCombatSection() const noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "aim_bot_fov") {
            handleIntSlider<aim_bot_vars::FovPixels>("aim_bot_fov");
        } else if (feature == "aim_bot_fov_text") {
            handleIntSliderTextEntry<aim_bot_vars::FovPixels>("aim_bot_fov");
        } else if (feature == "aim_bot_max_distance") {
            handleIntSlider<aim_bot_vars::MaxDistance>("aim_bot_max_distance");
        } else if (feature == "aim_bot_max_distance_text") {
            handleIntSliderTextEntry<aim_bot_vars::MaxDistance>("aim_bot_max_distance");
        } else if (feature == "aim_bot_smooth") {
            handleIntSlider<aim_bot_vars::Smooth>("aim_bot_smooth");
        } else if (feature == "aim_bot_smooth_text") {
            handleIntSliderTextEntry<aim_bot_vars::Smooth>("aim_bot_smooth");
        } else if (feature == "aim_bot_vertical_adj") {
            handleIntSlider<aim_bot_vars::VerticalAdjustment>("aim_bot_vertical_adj");
        } else if (feature == "aim_bot_vertical_adj_text") {
            handleIntSliderTextEntry<aim_bot_vars::VerticalAdjustment>("aim_bot_vertical_adj");
        } else if (feature == "aim_bot_deadzone") {
            handleIntSlider<aim_bot_vars::DeadzonePixels>("aim_bot_deadzone");
        } else if (feature == "aim_bot_deadzone_text") {
            handleIntSliderTextEntry<aim_bot_vars::DeadzonePixels>("aim_bot_deadzone");
        }
    }

    void handleHudSection() const noexcept
    {
    }

    void handleSoundSection() const noexcept
    {
    }

    void handleVisualsSection() const noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "viewmodel_fov") {
            handleIntSlider<viewmodel_mod_vars::Fov>("viewmodel_fov");
        } else if (feature == "viewmodel_fov_text") {
            handleIntSliderTextEntry<viewmodel_mod_vars::Fov>("viewmodel_fov");
        }
    }

    template <typename ConfigVariable>
    void handleIntSlider(const char* sliderId) const noexcept
    {
        using ValueType = typename ConfigVariable::ValueType::ValueType;
        const auto newVariableValue = handleIntSlider(sliderId, ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax, static_cast<ValueType>(GET_CONFIG_VAR(ConfigVariable)));
        hookContext.config().template setVariable<ConfigVariable>(typename ConfigVariable::ValueType{newVariableValue});
    }

    template <std::unsigned_integral ValueType>
    [[nodiscard]] ValueType handleIntSlider(const char* sliderId, ValueType min, ValueType max, ValueType current) const noexcept
    {
        ValueType value{};
        if (!parser.parseInt(value) || value == current || value < min || value > max)
            return current;

        auto&& hueSlider = getIntSlider(sliderId);
        hueSlider.updateTextEntry(value);
        return value;
    }

    template <typename ConfigVariable>
    void handleIntSliderTextEntry(const char* sliderId) const noexcept
    {
        using ValueType = typename ConfigVariable::ValueType::ValueType;
        const auto newVariableValue = handleIntSliderTextEntry(sliderId, ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax, static_cast<ValueType>(GET_CONFIG_VAR(ConfigVariable)));
        hookContext.config().template setVariable<ConfigVariable>(typename ConfigVariable::ValueType{newVariableValue});
    }

    template <std::unsigned_integral ValueType>
    [[nodiscard]] ValueType handleIntSliderTextEntry(const char* sliderId, ValueType min, ValueType max, ValueType current) const noexcept
    {
        auto&& slider = getIntSlider(sliderId);
        ValueType value{};
        if (!parser.parseInt(value) || value < min || value > max) {
            slider.updateTextEntry(current);
            return current;
        }

        if (value == current)
            return current;

        slider.updateSlider(value);
        return value;
    }

    [[nodiscard]] decltype(auto) getIntSlider(const char* sliderId) const noexcept
    {
        const auto mainMenuPointer = hookContext.patternSearchResults().template get<MainMenuPanelPointer>();
        auto&& mainMenu = hookContext.template make<ClientPanel>(mainMenuPointer ? *mainMenuPointer : nullptr).uiPanel();
        return hookContext.template make<IntSlider>(mainMenu.findChildInLayoutFile(sliderId));
    }

    StringParser& parser;
    HookContext& hookContext;
};
