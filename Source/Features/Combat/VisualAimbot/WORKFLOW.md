# Visual Aimbot 端到端流程文档

从「Osiris 源码」到「实时 YOLO 自瞄」的全部步骤。无需再依赖任何人指导。

## 总览

```
[阶段0] 一次性环境准备 (~30 分钟)
   ├─ 安装 Python 3.10+ / CUDA 12.x / cuDNN / TensorRT
   ├─ 创建数据采集 venv
   └─ 创建训练 venv

[阶段1] 数据采集 (3-5 小时游戏时间)
   ├─ 编译 Osiris DLL
   ├─ 注入 CS2 + 开启 BoneDumper
   ├─ 运行 auto_collect.py
   ├─ 玩 DM / demo / bot 地图
   └─ 验证标签 verify_dataset.py
        └─→ ~20-30k 样本

[阶段2] 训练 (3-6 小时)
   ├─ 划分 train/val
   ├─ 写 data.yaml
   ├─ ultralytics yolo train
   └─→ runs/detect/train/weights/best.pt

[阶段3] 导出和部署 (~1 小时)
   ├─ 导出 ONNX
   ├─ trtexec 编译 TensorRT engine
   └─→ cs2_yolo.engine

[阶段4] 实时推理
   └─ 改 zero_train/cs2_visual_aim.py 的 model 路径 → 跑起来
```

---

## 阶段 0：环境准备（一次性）

### 0.1 系统要求
- Windows 11 x64
- NVIDIA RTX 4060 (你的) 或更高
- CS2 已安装
- 30+ GB 可用磁盘（数据集 + 模型 + venv）

### 0.2 安装 CUDA + TensorRT
```powershell
# 1. 装 NVIDIA 驱动 (>= 535)
#    https://www.nvidia.com/Download/index.aspx

# 2. 装 CUDA Toolkit 12.x
#    https://developer.nvidia.com/cuda-downloads

# 3. 装 cuDNN (匹配 CUDA 12.x)
#    https://developer.nvidia.com/cudnn-downloads

# 4. 装 TensorRT (匹配 CUDA 12.x)
#    https://developer.nvidia.com/tensorrt
#    解压到 C:\Program Files\NVIDIA Corporation\TensorRT-10.x
#    把 bin/ 加到 PATH

# 验证
nvcc --version
trtexec --version
```

### 0.3 装 Python 3.10+
```powershell
# 推荐 3.11 (Ultralytics + windows-capture 都兼容)
# 从 https://www.python.org/downloads/ 装
python --version  # 应该 >= 3.10
```

### 0.4 创建两个独立 venv

数据采集和训练用不同的环境，避免依赖冲突。

```powershell
# 数据采集 venv (轻量, 只要 windows-capture + opencv)
cd D:\CLionProjects\Osiris\Source\Features\Combat\VisualAimbot\collect
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -r requirements.txt
deactivate

# 训练 venv (重, 要 ultralytics + torch + cuda)
cd D:\CLionProjects\Osiris\Source\Features\Combat\VisualAimbot\train
python -m venv .venv
.\.venv\Scripts\Activate.ps1
# torch + cuda
pip install torch torchvision --index-url https://download.pytorch.org/whl/cu121
pip install ultralytics onnx onnxruntime-gpu
deactivate
```

验证 torch 看得到 GPU：
```powershell
.\train\.venv\Scripts\Activate.ps1
python -c "import torch; print(torch.cuda.is_available(), torch.cuda.get_device_name(0))"
# 应该输出: True NVIDIA GeForce RTX 4060
```

---

## 阶段 1：数据采集

### 1.1 编译 Osiris DLL

```powershell
cd D:\CLionProjects\Osiris
cmake --build cmake-build-release-visual-studio --config Release
# 产物: cmake-build-release-visual-studio\Source\Release\Osiris.dll
```

**注意**：必须用 cmake 构建，**不能用 MSBuild .sln**（DLL 注入后会闪退）。

### 1.2 注入 DLL 到 CS2

启动 CS2 → 用注入器加载 `Osiris.dll`（注入器自选，常见的有 Xenos / Manual Map / Process Hacker）。

### 1.3 开启 BoneDumper

CS2 内按菜单键 → 进入 Osiris UI：

```
Combat 标签 → 数据采集 (子标签) → Bone Dumper = On
```

此时 DLL 开始每帧写入 `C:\temp\cs2_labels\latest.json`（30Hz 节流）。
检查文件是否在更新：
```powershell
# 在另一个 PowerShell 窗口
while ($true) {
    Get-Content C:\temp\cs2_labels\latest.json | Select-Object -First 1
    Start-Sleep -Milliseconds 500
}
# 看到时间戳每秒变化即正常
```

### 1.4 运行收集脚本

```powershell
cd D:\CLionProjects\Osiris\Source\Features\Combat\VisualAimbot\collect
.\.venv\Scripts\Activate.ps1
python auto_collect.py --output .\dataset --fps 10 --monitor 1
```

**参数说明**：
- `--output`：数据集输出目录（默认 `./dataset`）
- `--fps`：每秒抓多少帧（10 够用，越高磁盘占用越大）
- `--monitor`：主显示器是 1，副显示器是 2

**预期输出**：
```
[init] capturing monitor 1, target 10fps
[init] output: D:\...\dataset
[init] existing samples in dataset: 0
[saved] 100  (skipped no-labels: 23, stale: 0)
[saved] 200  (skipped no-labels: 41, stale: 0)
...
```

`skipped no-labels` 表示当时画面里没有可见敌人。正常。

### 1.5 游戏内行动建议（最大化数据多样性）

| 模式 | 用途 | 时间 |
|---|---|---|
| **死亡竞赛 (DM)** | 大量近-中距离对枪，多 skin/agent | 1-2h |
| **休闲模式** | 多张地图，多种战术 | 1h/地图 |
| **Workshop `aim_botz`** | 受控姿态：bot 走/蹲/跳 | 30 min |
| **GOTV demo 回放** | 职业玩家所有姿态，全免费 | 不限 |

#### Demo 回放命令（白嫖 HLTV 职业比赛）

1. 从 [HLTV](https://www.hltv.org/results) 下载 demo (`.dem` 文件)
2. 放到 `<Steam>\steamapps\common\Counter-Strike Global Offensive\game\csgo\demos\`
3. CS2 控制台：
```
playdemo demos\比赛名.dem
```
4. 用 `demo_resume` / `demo_togglepause` 控制播放，BoneDumper 自动 dump 标签

#### Bot 地图脚本化

```
// 控制台命令
bot_kick                    // 踢光现有 bot
bot_add_t                   // 加 T 阵营 bot
bot_difficulty 3            // 高难度（动作多样）
mp_warmuptime 9999          // 不结束热身
bot_stop 1                  // 让 bot 站住别动 (静态训练数据)
bot_stop 0                  // 恢复 bot 移动
```

### 1.6 验证标签对齐

收集几百张后，停掉脚本（Ctrl+C），运行：

```powershell
python verify_dataset.py --dataset .\dataset --count 20
# 产物: dataset\preview\*.jpg
```

打开 `dataset\preview\` 下任意 .jpg：
- **红框** = 头部 (class 0)
- **绿框** = 身体 (class 1)

**正确**：框对准敌人头部和躯干。
**不对**：框偏移、框在敌人外、根本看不见敌人。如果不对：
- 检查 `C:\temp\cs2_labels\latest.json` 里的 bbox 坐标是不是 [0,1] 范围
- 可能 BoneDumper 的骨骼偏移在新版 CS2 上失效，回退方案见后面"故障排查"

### 1.7 目标产出

游戏 3-5 小时应得到：
- 20,000-40,000 张图（每张 ~50-100KB JPG，总计 1-4GB）
- 等数量 .txt 标签

```
dataset/
├── images/
│   ├── f0000000.jpg
│   ├── f0000001.jpg
│   └── ... (~30k)
└── labels/
    ├── f0000000.txt
    ├── f0000001.txt
    └── ... (~30k)
```

---

## 阶段 2：训练 YOLOv11

### 2.1 划分 train / val（90/10）

```powershell
cd D:\CLionProjects\Osiris\Source\Features\Combat\VisualAimbot\train
.\.venv\Scripts\Activate.ps1
```

用下面这个 Python 脚本（保存为 `split.py`）：

```python
import random, shutil
from pathlib import Path

SRC = Path("../collect/dataset")
DST = Path("./dataset")
VAL_RATIO = 0.10

random.seed(42)
all_imgs = sorted((SRC / "images").glob("*.jpg"))
random.shuffle(all_imgs)
n_val = int(len(all_imgs) * VAL_RATIO)
val_set = set(all_imgs[:n_val])

for split, items in [("val", all_imgs[:n_val]), ("train", all_imgs[n_val:])]:
    img_dir = DST / "images" / split
    lbl_dir = DST / "labels" / split
    img_dir.mkdir(parents=True, exist_ok=True)
    lbl_dir.mkdir(parents=True, exist_ok=True)
    for img in items:
        shutil.copy(img, img_dir / img.name)
        lbl = SRC / "labels" / (img.stem + ".txt")
        if lbl.exists():
            shutil.copy(lbl, lbl_dir / lbl.name)
    print(f"{split}: {len(items)} samples")
```

```powershell
python split.py
```

### 2.2 写 data.yaml

```powershell
@'
path: D:/CLionProjects/Osiris/Source/Features/Combat/VisualAimbot/train/dataset
train: images/train
val: images/val
names:
  0: head
  1: body
'@ | Out-File -FilePath data.yaml -Encoding utf8
```

### 2.3 训练

```powershell
yolo train model=yolo11n.pt data=data.yaml epochs=100 imgsz=640 batch=16 device=0 half=true name=cs2_yolo
```

**参数**：
- `model=yolo11n.pt`：nano 模型（最快，~6MB）。可换 `yolo11s.pt` / `yolo11m.pt` 精度更高但慢
- `epochs=100`：训练轮数。30k 数据集 100 epochs 在 RTX 4060 约 4-6 小时
- `imgsz=640`：输入分辨率
- `batch=16`：RTX 4060 8GB 显存够用。OOM 就降到 8
- `half=true`：FP16 混合精度加速
- `name=cs2_yolo`：输出目录名

**首次运行**会自动下载 `yolo11n.pt` (~6MB)。

**监控训练**：另开终端
```powershell
tensorboard --logdir runs/detect/cs2_yolo
# 浏览器打开 http://localhost:6006
```

关注：
- **box_loss** 应该持续下降
- **mAP50-95** 应该上升到 > 0.6
- **mAP50** (IoU=0.5) 应该 > 0.85

### 2.4 评估

```powershell
yolo val model=runs/detect/cs2_yolo/weights/best.pt data=data.yaml
```

期望输出：
```
Class      Images  Instances  Box(P     R     mAP50  mAP50-95)
all        ~3000   ~25000    0.92  0.88  0.91   0.65
head       ~3000   ~12000    0.90  0.85  0.88   0.55
body       ~3000   ~13000    0.94  0.91  0.93   0.75
```

如果 mAP50 < 0.7：
- 数据量不够（收集更多）
- 训练 epochs 不够（再训 50 epochs：`yolo train ... resume=true`）
- 模型太小（换 `yolo11s.pt`）

### 2.5 快速可视化预测

```powershell
yolo predict model=runs/detect/cs2_yolo/weights/best.pt source=dataset/images/val save=true
# 产物: runs/detect/predict/ 下带框的 jpg
```

肉眼看几张预测图，确认头/身框对得准。

---

## 阶段 3：导出和 TensorRT 部署

### 3.1 导出 ONNX

```powershell
yolo export model=runs/detect/cs2_yolo/weights/best.pt format=onnx imgsz=640 half=true
# 产物: runs/detect/cs2_yolo/weights/best.onnx
```

### 3.2 编译 TensorRT engine

```powershell
trtexec --onnx=runs/detect/cs2_yolo/weights/best.onnx `
        --saveEngine=cs2_yolo.engine `
        --fp16 `
        --workspace=4096
# 5-15 分钟编译完成
# 产物: cs2_yolo.engine (~10-25MB, 显卡型号绑定)
```

**注意**：engine 是显卡型号绑定的。RTX 4060 编译的 engine 在 RTX 4090 上跑可能要重新编译。

### 3.3 测试推理延迟

```powershell
trtexec --loadEngine=cs2_yolo.engine --shapes=images:1x3x640x640
# 输出: Latency: ... mean = X.X ms
# RTX 4060 期望 < 3ms
```

---

## 阶段 4：实时推理

### 4.1 修改 cs2_visual_aim.py

```powershell
cd ..\zero_train
.\.venv\Scripts\Activate.ps1  # 复用 collect venv 或新建
pip install onnxruntime-gpu  # 或装 tensorrt python bindings
```

打开 `zero_train/config.yaml`：

```yaml
model: D:/CLionProjects/Osiris/Source/Features/Combat/VisualAimbot/train/cs2_yolo.engine
capture:
  width: 640
  height: 640
  x_offset: 640
  y_offset: 200
fov:
  radius_px: 150
confidence: 0.5
smooth:
  factor: 0.4
visualize: false
# 新增: 用 head class (0) 而不是 person
target_class: 0     # 0=head, 1=body
```

### 4.2 跑推理

```powershell
python cs2_visual_aim.py
```

进游戏，检测到敌人头部应该自动瞄准。

### 4.3 性能监控

```powershell
# GPU 占用
nvidia-smi -l 1

# 期望: GPU util 2-5%, mem ~200MB
# 若 > 30%, 可能 PyTorch 没用 TensorRT engine
```

---

## 故障排查

### 1. BoneDumper 不写文件

**症状**：`C:\temp\cs2_labels\latest.json` 不存在或不更新。

**排查**：
1. 确认 UI 里 Bone Dumper 是 On
2. 当前必须有敌人可见（脚本会跳过空帧，但文件依然每帧覆写）
3. 检查 DLL 是否成功注入：CS2 应该看到 Osiris 菜单
4. 手动创建目录：`mkdir C:\temp\cs2_labels`
5. 看 Windows 事件日志有没有 Osiris 的崩溃

### 2. 标签框对不上画面

**症状**：`verify_dataset.py` 输出的预览图里框偏移。

**可能原因**：
- CS2 更新后骨骼索引变化（`Source/CS2/Constants/BoneIndex.h` 里的数字）
- 用 cs2-dumper 重新 dump：
```powershell
cd D:\CLionProjects\cs2-dumper
cargo run --release
# 输出在 output/client_dll.json
```
- 检查 `m_pGameSceneNode (0x330)`、`m_modelState (0x150)` 是否变化

### 3. windows-capture 装不上

```powershell
pip install --upgrade pip
pip install windows-capture --no-cache-dir
# 如果还失败, 安装 Visual C++ Build Tools
```

### 4. 训练 OOM

```powershell
# 降低 batch size
yolo train model=yolo11n.pt data=data.yaml epochs=100 batch=4 device=0
# 或者用 CPU (慢 50 倍)
yolo train model=yolo11n.pt data=data.yaml epochs=100 device=cpu
```

### 5. TensorRT 装不上

替代方案：用 ONNX Runtime GPU（慢一点但简单）
```powershell
pip install onnxruntime-gpu
# 推理脚本里加载 .onnx 而非 .engine
```

### 6. 实时推理掉游戏帧

**原因**：GPU 同时跑游戏和推理。

**解决**：
- 降游戏画质
- 推理改用 `device=cpu`（慢，但不抢 GPU）
- 终极方案：双 PC（一台游戏，一台推理，HDMI 采集卡 + KMBox 输入回路）

### 7. 推理结果"漂移"或抖动

**原因**：YOLO 检测有帧间抖动，加上鼠标平滑因子设置不当。

**解决**：
- `config.yaml` 里 `smooth.factor` 调到 0.2-0.3（更平滑）
- 检测到的框做 EMA 平滑（多帧加权平均）
- 推理 + Aimbot Humanization 一起开

---

## 资源占用估算

| 阶段 | 时间 | 磁盘 | GPU 占用 | 内存 |
|---|---|---|---|---|
| 数据采集 | 3-5h 游戏 | 1-4GB | < 5% | 200MB |
| 训练 | 3-6h | +500MB | 100% | 2GB |
| 导出 | 10-30 min | +30MB | < 5% | 500MB |
| TRT 编译 | 5-15 min | +25MB | < 10% | 1GB |
| 实时推理 | 持续 | 0 | 2-5% | 200MB |

**总磁盘需求**：约 5-10GB。

---

## 一键脚本（可选）

把常用步骤封装。保存为 `run_all.ps1`：

```powershell
# 1. 编译 DLL
cd D:\CLionProjects\Osiris
cmake --build cmake-build-release-visual-studio --config Release

# 2. 启动收集脚本（需手动启动 CS2 + 注入 DLL + 开 BoneDumper）
Start-Process powershell -ArgumentList @(
    "-NoExit",
    "-Command",
    "cd D:\CLionProjects\Osiris\Source\Features\Combat\VisualAimbot\collect; .\.venv\Scripts\Activate.ps1; python auto_collect.py --fps 10"
)

# 3. 实时观察统计
while ($true) {
    $count = (Get-ChildItem 'D:\CLionProjects\Osiris\Source\Features\Combat\VisualAimbot\collect\dataset\images' -ErrorAction SilentlyContinue).Count
    Write-Host "Collected: $count samples"
    Start-Sleep -Seconds 10
}
```

---

## 关键文件路径速查

| 用途 | 路径 |
|---|---|
| Osiris 源码 | `D:\CLionProjects\Osiris\Source\` |
| 编译产物 | `D:\CLionProjects\Osiris\cmake-build-release-visual-studio\Source\Release\Osiris.dll` |
| BoneDumper 配置 | UI: Combat → 数据采集 |
| 标签输出 | `C:\temp\cs2_labels\latest.json` |
| 采集脚本 | `Source\Features\Combat\VisualAimbot\collect\auto_collect.py` |
| 校验脚本 | `Source\Features\Combat\VisualAimbot\collect\verify_dataset.py` |
| 数据集 | `Source\Features\Combat\VisualAimbot\collect\dataset\` |
| 训练目录 | `Source\Features\Combat\VisualAimbot\train\` |
| TRT engine | `Source\Features\Combat\VisualAimbot\train\cs2_yolo.engine` |
| 推理脚本 | `Source\Features\Combat\VisualAimbot\zero_train\cs2_visual_aim.py` |
| cs2-dumper | `D:\CLionProjects\cs2-dumper\output\client_dll.json` |

---

## 下一步可选优化

如果整套跑通了，按效果迭代方向：

1. **数据量翻倍**：5h → 10h 游戏，模型精度通常提升 5-10%
2. **更大模型**：`yolo11n` → `yolo11s` → `yolo11m`，精度上升延迟也上升
3. **类别扩展**：加上 `weapon` / `grenade` / `teammate` 类别，区分不同射击优先级
4. **多分辨率训练**：`imgsz=[640, 832, 1024]`，对小目标更准
5. **TTA (Test Time Augmentation)**：`yolo val ... augment=true`，精度小幅提升
6. **量化**：FP16 → INT8，推理再快 2 倍（需校准数据集）
7. **双 PC + KMBox**：完全规避客户端反作弊
8. **拟人化轨迹**：复用 Aimbot 的 Woodworth 模型 + Tremor，避免 VACnet 服务端检测

---

## 参考

- [Ultralytics YOLOv11 文档](https://docs.ultralytics.com/models/yolo11/)
- [windows-capture (WGC Python)](https://github.com/NiiightmareXD/windows-capture)
- [TensorRT 部署](https://developer.nvidia.com/tensorrt)
- [cs2-dumper (CS2 偏移量)](https://github.com/a2x/cs2-dumper)
- [HLTV demo 下载](https://www.hltv.org/results)
- [XGuardian 反作弊论文](https://ar5iv.labs.arxiv.org/html/2601.18068) (VACnet ML 原理)
