# CS2 Visual Aimbot — 视觉识别自瞄

基于深度学习目标检测 (YOLO) 的 CS2 视觉自瞄方案。读屏 + 模拟输入，
不依赖游戏内存读写。本目录涵盖三个独立环节：

```
[1] 数据采集 (collect/)      ← Osiris BoneDumper + WGC 截屏自动配对
[2] 模型训练 (train/)         ← Ultralytics YOLOv11 / SuperGradients NAS
[3] 推理部署 (zero_train/)    ← 实时推理 + 鼠标控制
```

## 整体架构

```
游戏运行
   │
   ├─ Osiris DLL (注入)
   │     └─ BoneDumper → C:/temp/cs2_labels/latest.json (每帧)
   │
   └─ 屏幕画面
         │
         ▼
  [WGC 截屏]  ←  Python 收集脚本
         │       └─ 读 latest.json + 截屏配对 → dataset/{images,labels}/
         ▼
  YOLOv11 训练  →  cs2_yolo.onnx  →  TensorRT  →  cs2_yolo.engine
                                                       │
                                                       ▼
                                             [实时推理 + 鼠标控制]
```

---

## 屏幕截屏 API 选择

| API | 延迟 | 反作弊指纹 | 推荐场景 |
|---|---|---|---|
| **Windows Graphics Capture (WGC)** | < 2ms | ✅ 低 (微软标准 API, OBS 同款) | **首选** |
| DXGI Desktop Duplication | < 2ms | ⚠️ 中 (FaceIt 等会标记) | 兼容性场景 |
| OBS Virtual Camera | 30-50ms | ✅ 低 | 不适合实时瞄准 |
| OBS NDI 输出 | 16-50ms | ✅ 低 | 不适合实时瞄准 |
| OBS 插件读 backbuffer | < 5ms | ✅ 低 | 配置复杂, 需要 OBS 常驻 |

WGC 就是 OBS 的 "Display Capture (Win 10 1903+)" 用的那个 API。Win10 1903+ 系统自带，
Python 用 `windows-capture` 包封装。**绕开 DXGI 直接用 WGC，截屏路径合法、延迟低、
反作弊指纹弱**。不需要 OBS 进程在跑。

QQ 音乐桌面歌词只是个 Win32 透明置顶窗口，不截屏。无参考价值。

---

## 真实端到端延迟

```
DXGI/WGC 截屏 ~1-2ms
    + YOLO 推理 (RTX 4060 FP16) ~2.5ms
    + HSV/后处理 ~0.3ms
    + SendInput ~0.1ms
    + CS2 RawInput tick 8-15ms   ← 这是限速器
    + 服务器 tick (64-tick = 15.6ms)
─────────────────────────────────
约 25-35ms 端到端
```

仍远低于人类反应 (~200ms)，可用。"4ms 总延迟"的宣传是 GPU 视角，**游戏体验视角是 25-35ms**。
240Hz 截屏 + 推理可以做到，但鼠标输入到游戏命中检测的真实速率受 input/server tick 限制。

---

## 反作弊评估

| 路径 | VAC 客户端 | VACnet (服务端 ML) | FaceIt/ESEA |
|---|---|---|---|
| 注入 DLL 内存读写 | ❌ 高风险 | ❌ 行为分析 | ❌ 内核扫描 |
| **本目录方案: 外部推理 + SendInput** | ✅ 没注入物 | ⚠️ 仍可分析瞄准轨迹 | ⚠️ 进程扫描可见 |
| **双 PC + KMBox** | ✅ | ✅ 配合人形轨迹拟合 | ✅ 完全隔离 |

- **VAC 客户端**: 看签名/注入物，外部纯读屏方案隐身
- **VACnet (服务端)**: 看时间序列瞄准轨迹，需要拟人化 (参考 Aimbot 模块的 humanization)
- **FaceIt/ESEA 内核反作弊**: 扫所有进程内存/句柄/通信。**真正隐身只能靠双 PC + KMBox**

---

## 训练数据 — Osiris 自动标注流程 (核心)

**手动 LabelImg 是死路** (2000 张 = 30-50 小时)。**Osiris DLL 本身就是完美的 ground truth oracle**:

```cpp
// PlayerPawn 已经实现的接口:
playerPawn.bonePosition(BoneIndex::Head)       // 头部骨骼世界坐标
playerPawn.viewOffset()                         // 眼睛偏移
playerPawn.isDormant()                          // 是否被服务器停止同步
WorldToClipSpaceConverter::toClipSpace(world)   // 投到屏幕 NDC
```

BoneDumper feature 利用这些已有数据，**每帧自动写出 YOLO 格式标签**:

| 维度 | 人工 LabelImg | Osiris BoneDumper |
|---|---|---|
| 每张耗时 | 5 分钟 | **0 秒** (DLL 自动) |
| 标签精度 | 受标注疲劳影响 | **像素级精确** (引擎坐标投影) |
| 类别数 | 通常 head/body | head/body (可扩展到 head/neck/chest/pelvis/feet) |
| 蹲下/跳跃 | 标注员现场判断 | **自动跟随骨骼动画** |
| 烟雾/闪光 | 模糊到看不见就标不了 | 引擎坐标永远对，**模型反而学到"看不清也能猜"** |
| 单小时产出 | ~12 张 | **30,000+ 帧** |

### 用法 (今天就能跑)

**步骤 1**: Osiris 注入到 CS2 后, 在菜单里打开 BoneDumper:
```
Combat → BoneDumper → Enabled = On
```
DLL 开始往 `C:/temp/cs2_labels/latest.json` 写每帧标签 (30Hz)。

**步骤 2**: 另开终端跑 Python 收集脚本:
```bash
cd Source/Features/Combat/VisualAimbot/collect
pip install -r requirements.txt
python auto_collect.py --output ./dataset --fps 10
```

**步骤 3**: 进游戏。死亡竞赛 / 休闲 / Bot 地图 / GOTV demo 回放都可以。
脚本每秒抓 10 帧, 跳过没敌人可见的帧, 自动配对保存到:
```
dataset/
  images/f0000000.jpg
  labels/f0000000.txt   (YOLO format: "class cx cy w h")
```

**步骤 4 (可选)**: 验证标签是否对齐:
```bash
python verify_dataset.py --dataset ./dataset --count 20
# 输出: dataset/preview/*.jpg (带框可视化), 检查头/身框有没有对准
```

### 数据多样化技巧

1. **GOTV demo 回放白嫖职业比赛**: 从 HLTV 下 100 场 demo, `cs2 -insecure +playdemo X.dem`,
   Osiris 同时 dump 标签。**纯职业玩家真实战场, 所有地图, 所有姿态**。
2. **Workshop bot 地图** (`aim_botz` 等): 受控 bot 站位多样, 可脚本切换 skin/weapon/姿态。
3. **多 skin/agent** (CS2 几十套): 在 DM 服务器多打几场, 自然覆盖。
4. **多地图**: dust2/mirage/inferno/nuke/ancient/anubis 各跑 30 分钟。

**实测预期**: 一小时 DM ≈ 8000-15000 张样本 (按 10fps 采集, 排除空帧)。
**3 小时游戏可达 30k+ 样本, 直接进训练。**

---

## 训练 — Ultralytics YOLOv11

> README 之前推荐的 YOLO-NAS 在 2025-2026 时间点已被 YOLOv11 超越,
> 且 Ultralytics 生态/文档/导出工具远更成熟。

```bash
cd Source/Features/Combat/VisualAimbot/train
pip install ultralytics
```

数据集结构 (Ultralytics 标准):
```
dataset/
  images/train/, images/val/
  labels/train/, labels/val/
  data.yaml
```

`data.yaml`:
```yaml
path: /absolute/path/to/dataset
train: images/train
val: images/val
names:
  0: head
  1: body
```

训练:
```python
from ultralytics import YOLO
model = YOLO("yolo11n.pt")  # nano = 最快, 也可选 s/m/l
model.train(
    data="data.yaml",
    epochs=100,
    imgsz=640,
    batch=16,
    device=0,
    half=True,  # FP16 加速
)
```

RTX 4060 上, 30k 张 100 epochs 约 **3-5 小时**。

导出 ONNX:
```python
model.export(format="onnx", imgsz=640, half=True)
```

编译 TensorRT (FP16):
```bash
trtexec --onnx=yolo11n.onnx --saveEngine=cs2_yolo.engine --fp16
```

---

## 推理部署 (zero_train/)

老版本零训练方案保留在 `zero_train/` 作为对比/降级方案。
真正用就用上面流程训练的定制模型, 精度远高于 COCO YOLOv8n。

---

## 硬件建议

| 配置 | 适用 |
|---|---|
| 单 PC, RTX 4060+ | 开发/调试, GPU 与 CS2 共享 (CS2 帧率会掉 ~10-15%) |
| 单 PC, 副 GPU 专推理 | 减少干扰, 推理走副卡 |
| **双 PC + KMBox** | 生产部署, 完全隔离, 反作弊不可见 |

---

## 当前已实现

| 组件 | 状态 |
|---|---|
| Osiris BoneDumper feature | ✅ 已实现 |
| Python 自动收集脚本 (`collect/auto_collect.py`) | ✅ 已实现 |
| 数据集校验工具 (`collect/verify_dataset.py`) | ✅ 已实现 |
| YOLOv11 训练 pipeline | ⏳ data.yaml + 训练脚本待写 |
| TensorRT 部署 | ⏳ 待训练完成后导出 |
| 实时推理 + 鼠标控制 | ⏳ 替换 `zero_train/cs2_visual_aim.py` 里的模型路径即可 |

---

## 参考资料

- [Ultralytics YOLOv11](https://docs.ultralytics.com/models/yolo11/)
- [windows-capture (WGC Python 封装)](https://github.com/NiiightmareXD/windows-capture)
- [Windows.Graphics.Capture API](https://learn.microsoft.com/en-us/uwp/api/windows.graphics.capture)
- [TensorRT 部署](https://developer.nvidia.com/tensorrt)
