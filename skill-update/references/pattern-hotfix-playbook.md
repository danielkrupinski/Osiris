# Pattern Hotfix Playbook

## Session history

### 2026-05-18 — Initial name display feature
- Added `PlayerNamePanel` to `PlayerInfoPanelTypes` (feet panel, flow layout)
- Added `PlayerController::getName()` reading `m_iszPlayerName` at 0x6F4
- Pattern errors: `OffsetToPlayerColor` scan failing → added offset fallback 0x848

### 2026-05-18 — Pattern scan fixes
- `E8????84 C0 74?41 8B ?????` (OffsetToPlayerColor): updated Windows pattern
- `7E ? 75 28` (UiItem3dPanel): updated Windows pattern  
- `41 39 44 24? 44` (UiItem3dPanel unknown field): removed dependency entirely

### 2026-05-18 — getName() multi-level fallback
- Order: `m_iszPlayerName(0x6F4)` → `playerColor+0x18` → `0x860 fallback`
- Reason: single pattern dependency too fragile; name must show even if pattern drifts

### 2026-05-31 — Head-top name display
- Moved `PlayerNamePanel` OUT of `PlayerInfoPanelTypes`
- Added separate panel pool (`playerNamePanelListHead` in `InWorldPanelsState`)
- World-space projection at `absOrigin.z + 74.0f`
- **Fix**: changed bare-label creation to container+label child
  (bare label's `clientPanel().as<PanoramaLabel>()` unreliable for standalone panels)
- Adjusted height offset: `74.0f` units above feet ≈ CS2 head height

## Offset reference (CS2, as of 2026-05-31)
| Field | Class | Offset |
|---|---|---|
| m_iszPlayerName | CBasePlayerController | 0x6F4 |
| m_sSanitizedPlayerName | CCSPlayerController | 0x860 |
| m_iPlayerColor | CCSPlayerController | 0x848 (fallback) |
