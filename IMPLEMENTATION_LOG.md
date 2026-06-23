# Osiris Triggerbot & SpectatorList 实现日志

> 基准 commit: `d625727c` (2026-06-01, 两提交之前)
> 工作桌面 DLL: `C:\Users\星记\Desktop\Osiris.dll` (6月21日 GPT 版本, 226KB)
> cs2-dumper: `D:\CLionProjects\cs2-dumper` (2026-06-11, build 14165)

---

## 1. 构建系统

### ✅ 关键发现：必须用 cmake 构建

**失败：** 直接用 MSBuild `.sln` 构建的 DLL 注入后闪退
**成功：** cmake 构建的 DLL 正常工作

```powershell
cd D:\CLionProjects\Osiris\cmake-build-release-visual-studio
cmake --build . --config Release --target Osiris
```

产物路径：`D:\CLionProjects\Osiris\cmake-build-release-visual-studio\Source\Release\Osiris.dll`

CMakeLists.txt 使用 `/nodefaultlib` 和 `/ENTRY:"DllMain"`，不链接标准 CRT。

---

## 2. SpectatorList（观察者列表）

### ✅ 已修复

**根因1：`observerTargetHandle()` 逻辑错误**
- 旧方法读取旁观者的 `m_hObserverPawn` 后，又去读被观察者的 `m_pObserverServices → m_hObserverTarget`
- 本地玩家活着时 `m_pObserverServices` 为 null → 永远返回 nullopt → 列表为空
- GPT 版本用的是另一种正确实现

**修复：** 新增 `observerPawnHandle()` 直接读取 `m_hObserverPawn`(0x910)，不穿链条
```cpp
// PlayerController.h - 正确方法
[[nodiscard]] std::optional<cs2::CEntityHandle> observerPawnHandle() const noexcept {
    return entityHandleAt(playerControllerPointer, kOffsetToObserverPawnHandle);
}
```

**根因2：`getWatchedPawnHandle()` 死亡分支错误**
- 死后错误地返回 `observerPawnHandle()`（自己观战的人）而非自己的 Pawn
- 旁观者通过本地玩家的相机看，`m_hObserverPawn` 指向本地玩家 Pawn
- 修复：始终返回 `playerPawnHandle()`（本地玩家自己的 Pawn）

**根因3：`isConnected()` 过滤过严**
- 移除该检查——偏移量敏感，过滤掉有效旁观者

**安全防护：** `hide()` 加 `isValid()` 守卫，面板未创建时不操作 UI

### 已验证偏移 (cs2-dumper)

| 偏移 | 字段 | 状态 |
|------|------|------|
| 0x90C | CCSPlayerController::m_hPlayerPawn | ✅ |
| 0x910 | CCSPlayerController::m_hObserverPawn | ✅ |
| 0x914 | CCSPlayerController::m_bPawnIsAlive | ✅ |
| 0x11F8 | C_BasePlayerPawn::m_pObserverServices | ✅ |
| 0x4C | CPlayer_ObserverServices::m_hObserverTarget | ✅ |

---

## 3. Triggerbot（自动扳机）

### 当前状态：⚠️ 可能不工作

**文件：** `Source/Features/Combat/Triggerbot/Triggerbot.h`

### 3.1 目标检测（checkPlayer）
```cpp
// 当前使用 absOrigin + 64z 头部估计（不用骨骼）
const auto origin = playerPawn.absOrigin();
auto targetPos = origin.value();
targetPos.z += 64.0f;
```

### 3.2 视角瞄准（aimAt）
```cpp
// 直接写 C_CSPlayerPawn::m_angEyeAngles (0x3320) —— 瞬时，不依赖 SendInput
float* eyeAngles = reinterpret_cast<float*>(localPawn + 0x3320);
eyeAngles[0] = pitch;  // 已验证 cs2-dumper: m_angEyeAngles = 13088 (0x3320)
eyeAngles[1] = yaw;
```

**CS2 使用 Raw Input 读取鼠标硬件，`SendInput` 移动 Windows 光标对游戏视角无效！必须写内存中的视角角度。**

### 3.3 数学函数（自实现，无 CRT 依赖）
- `fastAtan2` — 多项式近似 atan2（~0.005 rad 误差）
- `fastSqrt` — Quake 快速反平方根
- `fastFabs` — 绝对值

项目用 `/nodefaultlib`，不能链接 `std::atan2`/`std::sqrt`。

### 3.4 开火（fire）
```cpp
// SendInput LEFT_DOWN + LEFT_UP 原子点击
INPUT inputs[2]{};
inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
SendInput(2, inputs, sizeof(INPUT));
```

### 3.5 防护检查
- `isGameInForeground()` — 前台窗口进程 ID = 当前进程 ID
- `isLocalPlayerAlive()` — 本地玩家存活检查
- `isHoldingFirearm()` — 排除闪光/烟雾/手雷/燃烧/诱饵/刀/C4

### 3.6 配置
- `triggerbot_vars::Enabled` (bool, default false) — 开关
- `triggerbot_vars::VisibilityCheck` (bool, default true) — 可见性检查
- UI 位置：Combat → Triggerbot 子标签

---

## 4. UI 系统

### ✅ 已修复：CombatTab 崩溃

**根因：** ID 冲突！`createSubTab(combat, 'triggerbot')` 和 `createYesNoDropDown(..., 'triggerbot')` 用了同名 ID。
`findChildInLayoutFile("triggerbot")` 先找到 SubTab 面板（不是 DropDown），`.as<PanoramaDropDown>()` 崩溃。

**修复：** 下拉框 ID 改为 `"triggerbot_switch"`，可见性开关改为 `"triggerbot_visibility"`

**安全防护：** CombatTab::initDropDown 加了 `if (auto&& panel = ...)` 空检查

### UI 文件
- `CreateGUI.js` — Panorama 面板创建（翻译、子标签、下拉框）
- `CombatTab.h` — C++ 侧初始化/刷新下拉框
- `ConfigSchema.h` — 配置持久化注册
- `ConfigVariableTypes.h` — 配置变量类型注册

### ⚠️ CreateGUI.js 编辑注意事项
- 文件嵌入 C++ raw string literal `u8R"(...)"`
- **不能出现 `)"` 序列！** 遇到中文括号加引号时用全角括号 `（）`
- 共 5 个 raw string 段，`u8R"(` 和 `)"` 必须数量相等
- 用 Python 脚本编辑最安全，避免直接 Edit 工具

---

## 5. 骨骼读取

### ⚠️ 未完全工作

**代码：** `Source/GameClient/Entities/PlayerPawn.h::bonePosition()`

**链路：**
```
C_BaseEntity + 0x330  → CGameSceneNode*     ✅ cs2-dumper 验证: 816 (0x330)
CGameSceneNode + 0x150 → CModelState          ✅ cs2-dumper 验证: 336 (0x150)  
CModelState + 0x80    → 骨骼数组指针          ⚠️ 社区验证(3个项目), schema外, 无法cs2-dumper确认
骨骼[i*32]            → 模型空间位置 Vector   ⚠️ stride=32, position@offset 0
×矩阵变换 (0x10)      → 世界空间坐标          ✅ 列主序: m_nodeToWorld (4列×3行)
```

### ❌ 失败原因分析

1. **CModelState + 0x80 不是 cs2-dumper 可验证的字段**——是 schema 内 0x00-0x9F "暗区"的社区发现值
2. **矩阵索引格式错误**——最初用行主序 (m[row][col])，Source 2 实际是列主序 (col vectors)
   - 错误：`world.x = m[0]*x + m[1]*y + m[2]*z + m[3]`（行主序）
   - 正确：`world.x = m[0]*x + m[3]*y + m[6]*z + m[9]`（列主序）
3. **骨骼数据可能是模型空间**——需要用 m_nodeToWorld 转换，已补上但未验证成功
4. **CTransform stride 可能不是 32**——3 个不同项目确认 32 字节，position@0，但可能因版本而异

### 替代方案
- `absOrigin()` (CGameSceneNode + 0xC8 `m_vecAbsOrigin`) — 引擎维护，准确但只有脚底/身体中心
- `m_vRenderOrigin` (CGameSceneNode + 0x128) — 渲染原点，每帧更新

---

## 6. 可见性检查 / 射线追踪

### ✅ 已实现

**文件：** `Source/GameClient/Trace/EngineTrace.h`

用 `engine2.dll` 的 `CreateInterface("EngineTraceClient004")` 获取 `IEngineTrace`，调用 `TraceRay`。

```cpp
// 非虚过滤（避免 CRT _purecall/type_info 依赖）
struct SimpleTraceFilter {
    TraceFilterVTable* vtable;
    void* skipEntity;
};
// IEngineTrace vtable[4] = TraceRay
// MASK_SHOT = 0x1C1003
// trace.fraction > 0.97f → 可见/子弹可穿透
```

**CRT 依赖问题：** 最初用 `class ITraceFilter` 带 virtual 方法 → 链接错误(_purecall, type_info)。已改为手动 vtable 指针。

---

## 7. 闪光免疫

### ✅ 已实现

在 Triggerbot::execute() 每帧调用 `removeFlash()`：
```cpp
const auto flashEndTime = hookContext.patternSearchResults()
    .template get<OffsetToFlashBangEndTime>().of(localPawn).get();
if (flashEndTime && *flashEndTime > 0.0f)
    *const_cast<float*>(flashEndTime) = 0.0f;
```

特征码：`"10 87 ? ? ? ? 0F 2F ? ? 0F 86"` + 2 字节偏移

---

## 8. 已验证偏移汇总 (cs2-dumper, 2026-06-11)

| 偏移 | 类 | 字段 |
|------|-----|------|
| 0x330 | C_BaseEntity | m_pGameSceneNode |
| 0x150 | CSkeletonInstance | m_modelState |
| 0xC8 | CGameSceneNode | m_vecAbsOrigin |
| 0x10 | CGameSceneNode | m_nodeToWorld |
| 0x103 | CGameSceneNode | m_bDormant |
| 0x3320 | C_CSPlayerPawn | m_angEyeAngles |
| 0x90C | CCSPlayerController | m_hPlayerPawn |
| 0x910 | CCSPlayerController | m_hObserverPawn |
| 0x914 | CCSPlayerController | m_bPawnIsAlive |
| 0x11F8 | C_BasePlayerPawn | m_pObserverServices |
| 0x4C | CPlayer_ObserverServices | m_hObserverTarget |

---

## 8.5 视角写入（View Angle）尝试记录

### 尝试1: SendInput MOUSE_MOVE — ❌ 失败
CS2 使用 Raw Input 直接从鼠标硬件读取，完全忽略 Windows 光标位置。
`SendInput` + `MOUSEEVENTF_ABSOLUTE` 对游戏视角无效。

### 尝试2: 写入 m_angEyeAngles (0x3320) — ❌ 不工作
仅写入 C_CSPlayerPawn::m_angEyeAngles。视角不变化。

### 尝试3: 写入 v_angle (0x1298) — ❌ 不工作
仅写入 C_BasePlayerPawn::v_angle。视角不变化。

### 尝试4: 同时写入两个字段 — ❌ 不工作（当前代码）
写入 v_angle(0x1298) + m_angEyeAngles(0x3320)。视角仍不变化。

### 待验证方案
- 写入后需要调用 `SetViewAngles` 或 `UpdateClientSideAnimation` 让游戏接受新角度
- 可能需要写入 `CPlayer_MovementServices` 中的角度字段
- 可能需要通过 engine client 的 `SetViewAngles` 虚函数
- 可能需要关闭视角平滑/插值 (`m_bOverrideViewAngles` 或类似标志)
- 参考其他 CS2 内部外挂的角度写入方式

---

## 9. 待办 / 已知问题

| 问题 | 状态 | 备注 |
|------|------|------|
| Triggerbot 基础触发 | ⚠️ 不稳定 | absOrigin+300px半径，会触发但视角不跟随 |
| 视角瞬移 | ❌ 不工作 | 同时写 v_angle(0x1298) + m_angEyeAngles(0x3320) 仍未生效 |
| 骨骼读取 | ❌ 未验证 | 矩阵变换已修复（列主序），但骨骼数组 0x80 无法 cs2-dumper 验证 |
| 实时跟踪 | ❌ | 每帧只检查第一个目标(触发即停) |
| 装备误丢 | ⚠️ | isHoldingFirearm() 已移除(0x990偏移未验证)，偶发 |
| 可见性检查 | ✅ | TraceRay 已实现，UI 开关正常 |
| 闪光免疫 | ✅ | 每帧清除，UI 开关默认开启 |
| UI 开关 | ✅ | Combat → Triggerbot 全部正常 |

---

## 10. 修改文件清单

```
Source/CS2/Constants/EntityClasses.h          (+CCSPlayerController注册)
Source/CS2/Constants/BoneIndex.h              (骨骼索引枚举, Head=6)
Source/Config/ConfigSchema.h                  (+SpectatorList, +Triggerbot, +VisibilityCheck)
Source/Config/ConfigVariableTypes.h           (+SpectatorListEnabled, +triggerbot变量)
Source/Config/ConfigVariableChangeHandler.h   (+triggerbot/SpectatorList handlers)
Source/Features/FeaturesStates.h              (+TriggerbotState)
Source/Features/Hud/HudFeaturesStates.h       (+SpectatorListState)
Source/Features/Common/RenderingHookEntityLoop.h (+SpectatorList, +Triggerbot)
Source/Features/Hud/SpectatorList/*           (5个新文件)
Source/Features/Combat/Triggerbot/*           (4个新文件)
Source/GameClient/Entities/PlayerController.h (+observerPawnHandle, +observerTargetHandle, +playerPawnHandle, +isPawnAlive, +rawPointer)
Source/GameClient/Entities/PlayerPawn.h       (+bonePosition, +headPosition, +isDormant, +rawPointer)
Source/GameClient/Hud/Hud.h                   (+rootPanel)
Source/GameClient/Trace/EngineTrace.h         (新文件, 射线追踪)
Source/UI/Panorama/CombatTab.h                (+triggerbot_switch, +triggerbot_visibility)
Source/UI/Panorama/CreateGUI.js               (+SpectatorList, +Triggerbot UI区)
Source/UI/Panorama/HudTab.h                   (+spectator_list)
Source/UI/Panorama/SetCommandHandler.h        (预留, 未使用)
Source/Osiris.vcxproj / .vcxproj.filters      (+新文件注册)
```
