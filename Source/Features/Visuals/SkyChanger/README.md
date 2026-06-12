## Sky Changer Feature

### Overview
The Sky Changer feature allows players to replace the current map's sky with skies from other Counter-Strike 2 maps.

### Supported Map Skies
- **Mirage** - Exotic desert sky
- **Inferno** - Middle Eastern urban sky
- **Nuke** - Industrial compound sky
- **Dust2** - Desert valley sky
- **Vertigo** - City rooftop sky
- **Ancient** - Ancient ruins sky
- **Anubis** - Egyptian temple sky
- **Office** - Urban office sky
- **Italy** - European countryside sky
- **Default** - Current map's default sky

### Files Structure
```
Source/Features/Visuals/SkyChanger/
├── SkyChangerState.h       - State management and SkyType enum
├── SkyTextureMapping.h     - Maps sky types to texture paths
└── SkyChanger.h            - Main implementation
```

### Usage Example
```cpp
// Enable sky changer
skyChanger.setSkyEnabled(true);

// Set specific sky
skyChanger.setSkyType(SkyType::Mirage);

// Get current sky info
auto currentSkyName = skyChanger.getCurrentSkyName();
auto currentSkyTexture = skyChanger.getCurrentSkyTexture();

// Apply the sky texture
skyChanger.applySkyTexture();
```

### Integration Points
- **VisualFeaturesStates.h** - Contains `skyChangerState` member
- **HookContext** - Must implement `skyChangerState()` method
- **Panorama UI** - GUI controls to select and apply skies
- **ViewRenderHook** - Texture application during rendering

### Technical Details
- Uses template-based design matching existing feature patterns
- No heap allocations in state management
- Constexpr texture path mapping for compile-time evaluation
- Follows Osiris1 architecture principles
