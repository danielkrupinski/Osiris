# osiris-cs2-pattern-hotfix

## Purpose
Diagnose and fix CS2 game-update breakage in the Osiris ESP project:
1. Pattern scan failures ("Failed to find pattern …")
2. Feature regressions after CS2 updates (name display, panel rendering, etc.)

Workspace: `D:\CLionProjects\Osiris`
Build script: `D:\CLionProjects\Osiris\build-release.ps1`

---

## Quick-start

### A. Pattern errors on inject
```
Failed to find pattern <hex bytes>
```
→ Run `scripts/map-pattern-errors.ps1` with the log line, identify the pattern type,
  update the Windows/Linux pattern file, rebuild DLL.

### B. Feature not showing (no crash, no pattern error)
→ Check panel creation chain. See "Head-top name display" section below.

---

## Key code paths

```
ViewRenderHook_onRenderStart (EntryPoints.h)
  └─ RenderingHookEntityLoop::run()
       └─ PlayerInfoInWorld::drawPlayerInformation(playerPawn)
            ├─ InWorldPanels::getNextPlayerInfoPanel()   ← feet panel (health/weapon/ammo)
            │    └─ PlayerInfoPanel::drawPlayerInfo()
            │         └─ PlayerController::getName()     ← name data
            └─ InWorldPanels::getNextPlayerNamePanel()   ← head panel (name only)
                 └─ PlayerNamePanel::update()
                 └─ PlayerNamePanel::updatePosition()    ← world-to-screen at absOrigin.z+74
```

### PlayerController::getName() — multi-level fallback
```cpp
// 1. m_iszPlayerName  at offset 0x6F4  (fixed char[] — try first)
// 2. playerColor+0x18 sanitized name   (via OffsetToPlayerColor pattern)
// 3. m_sSanitizedPlayerName at 0x860   (fallback offset)
```
If all three return null/empty → panel hidden. Verify offsets against current CS2 dump.

---

## Head-top name display (implemented 2026-05-31)

### Architecture
- `PlayerNamePanel` is **NOT** in `PlayerInfoPanelTypes` (feet panel).
- It has its **own panel pool** in `InWorldPanels` (`playerNamePanelListHead`).
- World position: `absOrigin.z + 74.0f` (≈ head height above feet).
- Same scale+translate transform as `PlayerInfoPanel`.

### Panel creation pattern — CRITICAL
Use **container + label child**, NOT bare label:
```cpp
// PlayerInfoInWorldPanelFactory.h :: createPlayerNamePanel()
auto&& container = panelFactory.createPanel(parentPanel).uiPanel();
container.setWidth(kWidth);          // 256px
container.setHeight(kHeight);        // 32px
container.setPosition(kPositionX, kPositionY);           // (-128, -32)
container.setTransformOrigin(kTransformOriginX, kTransformOriginY); // (50%, 100%)

auto&& label = panelFactory.createLabelPanel(container).uiPanel();
label.setFont(kFont);
label.setAlign(kAlignment);
label.setTextShadow(kShadowParams);
label.setColor(kColor);
```

Text update:
```cpp
// PlayerNamePanel::update()
context.panel().children()[0].clientPanel().template as<PanoramaLabel>().setText(playerName);
```

> ⚠️ A bare label as direct outer-container child does NOT render text reliably.
> Always use the container+child pattern (same as PlayerActiveWeaponAmmoPanel).

### Transform params (PlayerNamePanelParams.h)
```
container: width=256px, height=32px, pos=(-128,-32), transformOrigin=(50%,100%)
label:     font=Stratum2 bold 20px, align=center/center, white text, black shadow
```

---

## Pattern fix workflow

### 1. Identify pattern → source file mapping
| Pattern bytes | File | Symbol |
|---|---|---|
| `E8????84 C0 74?41 8B ?????` | PlayerControllerPatternsWindows.h | OffsetToPlayerColor |
| `7E ? 75 28` | UiItem3dPanelPatternsWindows.h | OffsetToItem3dPanelUnknownField |
| `41 39 44 24? 44` | UiItem3dPanelPatternsWindows.h | (removed — non-critical) |

### 2. Fix options
- **Update pattern**: find new byte sequence in current CS2 binary.
- **Add offset fallback**: hardcode known-good offset as fallback when pattern fails.
- **Remove dependency**: if field is non-critical, remove the pattern entirely.

### 3. Build
```powershell
# From project root:
.\build-release.ps1
# Output: D:\CLionProjects\Osiris\cmake-build-release-visual-studio\Source\Release\Osiris.dll
# Also copied to Desktop automatically.
```

### 4. Commit & push
```powershell
cd D:\CLionProjects\Osiris
# Delete .git/index.lock if present (CLion may leave it)
# Then:
git add <changed files>
git commit -m "Fix: <description>"
git push
```

---

## Common pitfalls

| Symptom | Cause | Fix |
|---|---|---|
| Name not showing, no error | Bare label as panel (can't setText) | Use container+label child pattern |
| Name not showing, no error | getName() returns null | Check offsets 0x6F4 / 0x860 / OffsetToPlayerColor |
| Pattern error on inject | CS2 updated, bytes moved | Update pattern bytes or add offset fallback |
| `index.lock` blocks git | CLion/Git GUI holding lock | Delete `.git\index.lock`, run git from PowerShell |
| Panel visible but at wrong height | absOrigin.z offset wrong | Adjust `kPlayerNameHeightOffset` in PlayerInfoInWorld.h (currently 74.0f) |
