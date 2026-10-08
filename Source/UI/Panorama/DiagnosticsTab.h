#pragma once

#include <BuildConfig.h>
#include <Features/Diagnostics/DiagnosticsConfigVariables.h>
#include <Features/Diagnostics/DiagnosticsExporter.h>
#include <GameClient/Panorama/PanoramaDropDown.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <HookContext/HookContextMacros.h>
#include <Platform/Macros/FunctionAttributes.h>
#include <EntryPoints/GuiEntryPoints.h>
#include <Utils/StringBuilder.h>

#include "OnOffDropdownSelectionChangeHandler.h"

template <typename HookContext>
class DiagnosticsTab {
public:
    explicit DiagnosticsTab(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void init(auto&& guiPanel) const
    {
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, diagnostics_vars::SafeModeEnabled>>(guiPanel, "diagnostics_safe_mode");
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "diagnostics_safe_mode", !GET_CONFIG_VAR(diagnostics_vars::SafeModeEnabled));
        updateStatus(mainMenu);
    }

    void updateStatus(auto&& mainMenu) const noexcept
    {
        const auto& patterns = hookContext.patternSearchResults();
        setLabel(mainMenu, "diagnostics_cs2_version", "CS2: runtime module loaded");
        setVersionLabel(mainMenu);
        setLabel(mainMenu, "diagnostics_pattern_status", patterns.isValid() ? "Signatures: OK" : "Signatures: FAILED");
        setPatternLabel(mainMenu, "diagnostics_client_patterns", "Client patterns", patterns.clientPatternsValid());
        setPatternLabel(mainMenu, "diagnostics_scene_patterns", "Scene system patterns", patterns.sceneSystemPatternsValid());
        setPatternLabel(mainMenu, "diagnostics_tier0_patterns", "Tier0 patterns", patterns.tier0PatternsValid());
        setPatternLabel(mainMenu, "diagnostics_filesystem_patterns", "Filesystem patterns", patterns.fileSystemPatternsValid());
        setPatternLabel(mainMenu, "diagnostics_sound_patterns", "Sound system patterns", patterns.soundSystemPatternsValid());
        setPatternLabel(mainMenu, "diagnostics_panorama_patterns", "Panorama patterns", patterns.panoramaPatternsValid());
        setLabel(mainMenu, "diagnostics_offsets", "Offsets: radar 0x1E88 + 0x08; dynamic offsets follow signature status");
        setLabel(mainMenu, "diagnostics_safe_mode_status", GET_CONFIG_VAR(diagnostics_vars::SafeModeEnabled) ? "Safe mode: ON" : "Safe mode: OFF");
    }

    [[nodiscard]] bool exportDiagnostics() const noexcept
    {
        return diagnostics::DiagnosticsExporter{hookContext}.exportToFile();
    }

private:
    void setVersionLabel(auto&& mainMenu) const noexcept
    {
        char text[64];
        StringBuilder builder{std::span{text}};
        builder.put("Osiris DLL: v", build::kVersion);
        setLabel(mainMenu, "diagnostics_dll_version", builder.cstring());
    }

    void setPatternLabel(auto&& mainMenu, const char* panelId, const char* name, bool valid) const noexcept
    {
        char text[96];
        StringBuilder builder{std::span{text}};
        builder.put(name, ": ", valid ? "OK" : "FAILED");
        setLabel(mainMenu, panelId, builder.cstring());
    }

    void setLabel(auto&& mainMenu, const char* panelId, const char* text) const noexcept
    {
        mainMenu.findChildInLayoutFile(panelId).clientPanel().template as<PanoramaLabel>().setText(text);
    }

    void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        mainMenu.findChildInLayoutFile(dropDownId).clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    template <typename Handler>
    void initDropDown(auto&& guiPanel, const char* panelId) const
    {
        auto&& dropDown = guiPanel.findChildInLayoutFile(panelId).clientPanel().template as<PanoramaDropDown>();
        dropDown.registerSelectionChangedHandler(&GuiEntryPoints<HookContext>::template dropDownSelectionChanged<Handler>);
    }

    HookContext& hookContext;
};
