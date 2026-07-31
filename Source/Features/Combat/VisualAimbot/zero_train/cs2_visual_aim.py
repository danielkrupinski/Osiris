#!/usr/bin/env python3
"""
CS2 Visual Aimbot — 零训练方案
===============================
COCO预训练 YOLOv8n 检测人形 → HSV颜色过滤区分敌我 → 头部估计 → 瞄准控制

首次运行: pip install -r requirements.txt
自动下载 yolov8n.pt (~6MB), 无需额外配置。

用法:
    python cs2_visual_aim.py                  # 使用默认配置
    python cs2_visual_aim.py --config my.yaml # 指定配置文件
    python cs2_visual_aim.py --visualize      # 开启调试窗口

按键:
    按住 Alt    瞄准 (config.yaml 中 aim_key 可改)
    F1          切换可视化窗口
    F2          切换模型 FP16/FP32
    Esc         退出
"""

from __future__ import annotations

import argparse
import os
import sys
import time
import math
import logging
from pathlib import Path
from typing import Optional

import cv2
import numpy as np
import yaml

import torch

from ultralytics import YOLO

import overlay as overlay

# ── 日志 ──────────────────────────────────────────────────────────────────
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    datefmt="%H:%M:%S",
)
log = logging.getLogger("cs2_aim")

# ═══════════════════════════════════════════════════════════════════════════
# 配置
# ═══════════════════════════════════════════════════════════════════════════

DEFAULT_CONFIG = Path(__file__).resolve().parent / "config.yaml"


def load_config(path: Path) -> dict:
    with open(path, "r", encoding="utf-8") as f:
        return yaml.safe_load(f)


# ═══════════════════════════════════════════════════════════════════════════
# DXGI 屏幕捕获
# ═══════════════════════════════════════════════════════════════════════════

class ScreenCapture:
    """基于 dxcam 的 DXGI 屏幕捕获，比 mss/PIL 快 5-10 倍。

    自动从显示器中心计算截屏区域，支持任意分辨率。
    """

    def __init__(self, cfg: dict) -> None:
        import dxcam
        self._cam = dxcam.create(output_color="BGR")  # BGR for OpenCV

        cap = cfg["capture"]
        w, h = cap["width"], cap["height"]

        # 居中
        screen_w = self._cam.width or 2560
        screen_h = self._cam.height or 1600
        cap_x = cap.get("x_offset", (screen_w - w) // 2)
        cap_y = cap.get("y_offset", (screen_h - h) // 2)

        self.region = (cap_x, cap_y, cap_x + w, cap_y + h)
        self._max_fps = cap.get("max_fps", 240)
        self._last_grab = 0.0

        log.info("屏幕捕获: %dx%d @ (%d, %d)", w, h, cap_x, cap_y)

    def grab(self) -> Optional[np.ndarray]:
        """捕获一帧，返回值或超时时返回 None。"""
        if self._max_fps > 0:
            elapsed = time.perf_counter() - self._last_grab
            min_interval = 1.0 / self._max_fps
            if elapsed < min_interval:
                return None
        frame = self._cam.grab(region=self.region)
        if frame is not None:
            self._last_grab = time.perf_counter()
        return frame


# ═══════════════════════════════════════════════════════════════════════════
# 队伍颜色判断
# ═══════════════════════════════════════════════════════════════════════════

class TeamFilter:
    """用 HSV 颜色过滤判断检测到的目标是否为敌人。

    CS2 队伍颜色:
        T  (恐怖分子): 红/橙/黄褐
        CT (反恐精英): 蓝色系
    第三方模型 (如 COCO person) 无法区分队伍，需要此后处理。
    """

    def __init__(self, cfg: dict) -> None:
        tc = cfg["team_colors"]
        # 敌人颜色 = T + CT 都有（因为你不知道自己是哪队）
        # 用两种颜色范围，任何一种匹配即视为敌人
        self._ranges = [
            (np.array(tc["t"]["lower"]), np.array(tc["t"]["upper"])),
            (np.array(tc["ct"]["lower"]), np.array(tc["ct"]["upper"])),
        ]

    def is_enemy(self, frame_bgr: np.ndarray, bbox: tuple[int, int, int, int]) -> bool:
        """检查边界框内是否有队伍颜色。

        Args:
            frame_bgr: 完整帧 BGR
            bbox: (x1, y1, x2, y2) 边界框坐标（帧内坐标）

        Returns:
            True 如果检测到队伍颜色（敌人）
        """
        x1, y1, x2, y2 = bbox
        h, w = frame_bgr.shape[:2]

        # 边界裁剪
        x1 = max(0, min(x1, w - 1))
        y1 = max(0, min(y1, h - 1))
        x2 = max(x1 + 1, min(x2, w))
        y2 = max(y1 + 1, min(y2, h))

        roi = frame_bgr[y1:y2, x1:x2]
        if roi.size == 0:
            return False

        hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)

        for lower, upper in self._ranges:
            mask = cv2.inRange(hsv, lower, upper)
            # 体干通常是大面积实色；头部可能只有小面积
            # 用较高的阈值以减少假阳性
            color_pixels = np.count_nonzero(mask)
            roi_area = (y2 - y1) * (x2 - x1)
            ratio = color_pixels / roi_area
            # 至少 80 个彩色像素 且 占比 > 3%
            if color_pixels > 80 and ratio > 0.03:
                return True

        return False


# ═══════════════════════════════════════════════════════════════════════════
# 瞄准控制
# ═══════════════════════════════════════════════════════════════════════════

class AimController:
    """基于 pynput 的平滑鼠标移动控制。"""

    def __init__(self, cfg: dict) -> None:
        from pynput.mouse import Controller
        self._mouse = Controller()
        self._smooth = float(cfg["aim"]["smooth_factor"])
        # 上一次的 delta，用于指数平滑
        self._last_dx: float = 0.0
        self._last_dy: float = 0.0

    def move(self, dx: float, dy: float) -> None:
        """平滑移动鼠标。

        用指数平滑避免突变，使人眼和 VACnet 都无法检测到瞬时跳跃。
        """
        # 指数平滑
        sx = self._last_dx + (dx - self._last_dx) * (1.0 - self._smooth)
        sy = self._last_dy + (dy - self._last_dy) * (1.0 - self._smooth)
        self._last_dx = sx
        self._last_dy = sy

        # 舍入到整数像素
        mx, my = round(sx), round(sy)
        if mx != 0 or my != 0:
            self._mouse.move(mx, my)
            # 减去已移动的部分，避免过冲
            self._last_dx -= mx
            self._last_dy -= my


# ═══════════════════════════════════════════════════════════════════════════
# 可视化 (调试)
# ═══════════════════════════════════════════════════════════════════════════

class Visualizer:
    """OpenCV 窗口显示检测结果。"""

    def __init__(self) -> None:
        self._enabled = False
        self._window_name = "CS2 Visual Aim (F1=hide)"

    @property
    def enabled(self) -> bool:
        return self._enabled

    def toggle(self) -> None:
        self._enabled = not self._enabled
        if not self._enabled:
            cv2.destroyWindow(self._window_name)

    def show(self, frame: np.ndarray, detections: list, aim_target: Optional[tuple]) -> None:
        if not self._enabled:
            return
        vis = frame.copy()
        h, w = vis.shape[:2]
        cx_screen, cy_screen = w // 2, h // 2

        # 画 FOV 圈
        fov_r = 150  # 粗略值，可以从 cfg 传
        cv2.circle(vis, (cx_screen, cy_screen), fov_r, (0, 255, 0), 2)
        cv2.line(vis, (cx_screen - 10, cy_screen), (cx_screen + 10, cy_screen), (0, 255, 0), 1)
        cv2.line(vis, (cx_screen, cy_screen - 10), (cx_screen, cy_screen + 10), (0, 255, 0), 1)

        # 画检测框
        for det in detections:
            x1, y1, x2, y2, cls_name, conf, is_target = det
            color = (0, 0, 255) if is_target else (100, 100, 100)
            cv2.rectangle(vis, (int(x1), int(y1)), (int(x2), int(y2)), color, 2)
            label = f"{cls_name} {conf:.2f}"
            cv2.putText(vis, label, (int(x1), int(y1) - 5),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 1)

        cv2.imshow(self._window_name, vis)
        cv2.waitKey(1)


# ═══════════════════════════════════════════════════════════════════════════
# 键盘钩子
# ═══════════════════════════════════════════════════════════════════════════

class KeyListener:
    """监听全局按键，返回当前状态。

    Windows: pynput 全局钩子 + win32api GetAsyncKeyState 双保险
    """

    def __init__(self, aim_key: str, toggle_mode: bool, start_active: bool = True) -> None:
        from pynput.keyboard import Key, KeyCode, Listener as KListener

        self._aim_key = self._resolve_key(aim_key)
        self._toggle_mode = toggle_mode
        self._toggled = start_active  # start ON by default
        self._hold_active = False
        self._running = True
        self._last_toggle_time = 0.0

        self._listener = KListener(on_press=self._on_press)
        self._listener.start()
        log.info("Key listener started (toggle=%s, key=%s, active=%s)",
                 toggle_mode, aim_key, start_active)

    @staticmethod
    def _resolve_key(key_name: str):
        from pynput.keyboard import Key, KeyCode
        mapping = {
            "alt": Key.alt, "ctrl": Key.ctrl, "shift": Key.shift,
            "f6": Key.f6, "f7": Key.f7, "f8": Key.f8,
            "mouse5": KeyCode.from_vk(0x06),
            "mouse4": KeyCode.from_vk(0x05),
        }
        return mapping.get(key_name.lower(), KeyCode.from_char(key_name.lower()))

    def _on_press(self, key) -> None:
        now = time.perf_counter()
        # Debounce: 300ms between toggles
        if now - self._last_toggle_time < 0.3:
            return
        if key == self._aim_key:
            self._last_toggle_time = now
            if self._toggle_mode:
                self._toggled = not self._toggled
                log.info(">>> AIMBOT %s <<<", "ACTIVE" if self._toggled else "OFF")
            else:
                self._hold_active = True

    @property
    def aiming(self) -> bool:
        state = self._toggled if self._toggle_mode else self._hold_active
        # Windows fallback: poll F8 directly via GetAsyncKeyState
        try:
            import ctypes
            vk = 0x77  # VK_F8
            if ctypes.windll.user32.GetAsyncKeyState(vk) & 0x8000:
                now = time.perf_counter()
                if now - self._last_toggle_time > 0.5:
                    self._last_toggle_time = now
                    self._toggled = not self._toggled
                    log.info(">>> AIMBOT %s (via win32) <<<",
                             "ACTIVE" if self._toggled else "OFF")
        except Exception:
            pass
        return state

    @property
    def running(self) -> bool:
        return self._running

    def stop(self) -> None:
        self._listener.stop()


# ═══════════════════════════════════════════════════════════════════════════
# 主循环
# ═══════════════════════════════════════════════════════════════════════════

class CS2VisualAimbot:
    """CS2 视觉自瞄主类。

    将屏幕捕获、模型推理、颜色过滤、瞄准控制组合成完整管线。
    """

    def __init__(self, config_path: Path) -> None:
        self.cfg = load_config(config_path)

        # 提前从配置中提取所有需要的参数（避免在 _setup_model 中引用未定义的属性）
        self._half = self.cfg["model"].get("half", True)
        self._debug = self.cfg.get("debug_console", True)
        self._use_color_filter = self.cfg.get("color_filter", True)
        self._aim_cfg = self.cfg["aim"]
        self._fov_sq = float(self._aim_cfg["fov_radius_px"]) ** 2
        self._min_box_h = float(self._aim_cfg["min_box_height"])
        self._conf = float(self._aim_cfg["confidence"])
        self._head_ratio = float(self._aim_cfg["head_offset_ratio"])

        self._setup_model()
        self.screen = ScreenCapture(self.cfg)
        self.team_filter = TeamFilter(self.cfg)
        self.aim = AimController(self.cfg)
        self.vis = Visualizer()
        if self.cfg.get("visualize", False):
            self.vis.toggle()

        self._status_printed = False  # for one-time status

        # HUD overlay state (shared with overlay_server.py)
        try:
            from overlay_server import get_hud_state
            self._hud = get_hud_state()
            self._hud.update(fov_radius=self._aim_cfg["fov_radius_px"])
        except Exception:
            self._hud = None

        # FPS tracking
        self._frame_times: list[float] = []
        self._inference_ms: float = 0.0

        # dwViewAngles writer (like memory aimbot) — bypasses Windows mouse
        self._client_base = self._get_client_base()
        self._last_pitch: float = 0.0
        self._last_yaw: float = 0.0
        self._dwViewAngles = 0x23B9C78  # a2x/cs2-dumper build 14171

    @staticmethod
    def _get_client_base():
        try:
            import ctypes
            return ctypes.windll.kernel32.GetModuleHandleW("client.dll")
        except Exception:
            return 0

    def _read_view_angles(self):
        if not self._client_base:
            return 0.0, 0.0
        import ctypes
        a = ctypes.cast(self._client_base + self._dwViewAngles, ctypes.POINTER(ctypes.c_float))
        return a[0], a[1]  # pitch, yaw

    def _write_view_angles(self, pitch: float, yaw: float):
        if not self._client_base:
            return
        import ctypes
        a = ctypes.cast(self._client_base + self._dwViewAngles, ctypes.POINTER(ctypes.c_float))
        a[0] = pitch; a[1] = yaw; a[2] = 0.0

    def _setup_model(self) -> None:
        """加载 YOLO 模型。支持 .pt (PyTorch) 和 .engine (TensorRT)。"""
        model_path = self.cfg["model"]["path"]
        # 如果是相对路径，相对于 config.yaml 所在目录解析
        if not os.path.isabs(model_path):
            model_path = str(DEFAULT_CONFIG.parent / model_path)

        log.info("加载模型: %s", model_path)
        self.model = YOLO(model_path)
        self._is_trt = model_path.endswith(".engine")
        self._is_onnx = model_path.endswith(".onnx")

        if not self._is_trt and not self._is_onnx:
            self.model.to("cuda")
            if model_path.endswith(".pt"):
                try:
                    self.model.fuse()
                    log.info("模型层融合已完成")
                except Exception:
                    log.warning("模型 fuse 跳过（可能已融合）")

        log.info("模型就绪 | CUDA: %s | FP16: %s",
                 torch.cuda.is_available(), self._half)

    def tick(self) -> Optional[tuple[float, float]]:
        """单帧处理。

        Returns:
            (dx, dy) 需要移动的像素数，或 None 表示不瞄准。
        """
        t0 = time.perf_counter()

        # ── 1. 截屏 ──────────────────────────────────────
        frame = self.screen.grab()
        if frame is None:
            return None
        t_cap = time.perf_counter()

        # ── 2. YOLO 推理 ─────────────────────────────────
        if self._is_trt or self._is_onnx:
            results = self.model.predict(
                frame, verbose=False, conf=self._conf, device=0
            )[0]
        else:
            results = self.model(
                frame, verbose=False, half=self._half, conf=self._conf,
                imgsz=1280
            )[0]
        t_infer = time.perf_counter()

        # ── 3. 后处理 + 颜色过滤 ─────────────────────────
        h, w = frame.shape[:2]
        cx_screen, cy_screen = w / 2.0, h / 2.0
        best = None
        best_dist = self._fov_sq
        detections = []  # 用于可视化

        overlay_dets = []  # for overlay module
        if results.boxes is not None:
            boxes = results.boxes
            for i in range(len(boxes)):
                cls_id = int(boxes.cls[i])
                if cls_id not in (0, 1):  # 0=head, 1=body (custom model)
                    continue

                conf = float(boxes.conf[i])
                x1, y1, x2, y2 = boxes.xyxy[i].cpu().numpy().astype(float)

                # 太小的框忽略
                box_h = y2 - y1
                box_w = x2 - x1
                if box_h < self._min_box_h:
                    continue

                # 跳过颜色过滤（定制模型直接输出 head/body, color_filter=false）
                if self._use_color_filter:
                    is_enemy = self.team_filter.is_enemy(
                        frame, (int(x1), int(y1), int(x2), int(y2))
                    )
                    if not is_enemy:
                        continue

                # 瞄准点：head class → 框中心 (模型直接标注头部)
                #          body class → 框上沿 (头在身体框顶部附近)
                if cls_id == 0:
                    aim_x = (x1 + x2) / 2.0
                    aim_y = (y1 + y2) / 2.0
                else:
                    aim_x = (x1 + x2) / 2.0
                    aim_y = y1 + box_h * 0.12

                dx = aim_x - cx_screen
                dy = aim_y - cy_screen
                dist_sq = dx * dx + dy * dy

                # Prefer HEAD: head gets no penalty, body distance ×1.5
                score = dist_sq if cls_id == 0 else dist_sq * 1.5
                is_best = score < best_dist
                if is_best:
                    best_dist = score
                    best = (dx, dy)

                detections.append((
                    x1, y1, x2, y2, "enemy", conf, is_best
                ))
                overlay_dets.append(overlay.Detection(
                    cls=cls_id,
                    cx=(x1 + x2) / (2.0 * w),
                    cy=(y1 + y2) / (2.0 * h),
                    w=box_w / w,
                    h=box_h / h,
                    conf=conf,
                ))

        t_post = time.perf_counter()

        overlay.update_overlay(overlay_dets, best is not None)

        # ── 4. HUD 状态广播 ─────────────────────────────
        if self._hud:
            self._inference_ms = (t_infer - t_cap) * 1000
            total_ms = (t_post - t0) * 1000
            has_target = best is not None
            dist = math.sqrt(best_dist) if has_target else None

            self._hud.update(
                aimbot_active=True,
                has_target=has_target,
                detection_count=len(detections),
                target_distance=dist,
                latency_ms=total_ms,
                inference_ms=self._inference_ms,
            )

        # ── 5. 调试输出 ──────────────────────────────────
        if self._debug and detections:
            dt_cap = (t_cap - t0) * 1000
            dt_infer = (t_infer - t_cap) * 1000
            dt_post = (t_post - t_infer) * 1000
            dt_total = (t_post - t0) * 1000
            n_det = len(detections)
            best_info = ""
            if best:
                best_info = f" | 最佳: ({best[0]:+.0f}, {best[1]:+.0f})px"
            log.debug(
                "检测: %d人 | 截屏 %.1fms | 推理 %.1fms | 后处理 %.1fms | 总计 %.1fms%s",
                n_det, dt_cap, dt_infer, dt_post, dt_total, best_info,
            )

        # ── 5. 可视化 ────────────────────────────────────
        if self.vis.enabled:
            self.vis.show(frame, detections, best)

        return best

    def run(self) -> None:
        """Main loop."""
        cfg_aim = self._aim_cfg
        start_active = cfg_aim.get("start_active", True)
        keys = KeyListener(
            cfg_aim["aim_key"], cfg_aim["toggle_mode"],
            start_active=start_active
        )

        log.info("=" * 56)
        log.info("CS2 Visual Aimbot started")
        log.info("Status: %s", "ACTIVE (auto-start)" if start_active else "STANDBY")
        log.info("Toggle: %s | F1=debug | Esc=quit", cfg_aim["aim_key"])
        log.info("=" * 56)

        fov_px = cfg_aim.get("fov_radius_px", 200)
        overlay.init_overlay(fov_px)

        frame_count = 0
        aim_count = 0
        t_start = time.perf_counter()

        try:
            while keys.running:
                t_frame = time.perf_counter()

                # F1 切换可视化
                if self.vis.enabled:
                    k = cv2.waitKey(1) & 0xFF
                    if k == 27:  # Esc
                        break
                    if k == 0x70:
                        self.vis.toggle()

                if not keys.aiming:
                    if self._hud:
                        self._hud.update(aimbot_active=False, has_target=False)
                    self.aim = AimController(self.cfg)
                    time.sleep(0.005)
                    continue

                delta = self.tick()
                was_aiming = getattr(self, "_was_aiming", False)
                is_aiming = delta is not None
                if is_aiming:
                    # Write to dwViewAngles like memory aimbot — instant, no
                    # mouse conflict. Convert pixel delta to angle degrees.
                    # hfov 90°, screen 2560x1600 (16:10) → vfov ≈ 62.5°
                    import math
                    hfov = math.radians(90.0)
                    vfov = 2.0 * math.atan(math.tan(hfov/2.0) * 1600/2560)
                    sf = self._aim_cfg.get("smooth_factor", 0.7)
                    yaw_delta = delta[0] / 1280.0 * math.degrees(hfov/2.0) * sf
                    pitch_delta = -delta[1] / 800.0 * math.degrees(vfov/2.0) * sf
                    cur_p, cur_y = self._read_view_angles()
                    new_p = max(-89.0, min(89.0, cur_p + pitch_delta))
                    new_y = cur_y + yaw_delta
                    while new_y > 180.0: new_y -= 360.0
                    while new_y < -180.0: new_y += 360.0
                    self._write_view_angles(new_p, new_y)
                    aim_count += 1
                self._was_aiming = is_aiming
                # Continuous short beep every ~15 frames while locked
                self._beep_counter = getattr(self, "_beep_counter", 0) + 1
                if is_aiming and self._beep_counter >= 15:
                    self._beep_counter = 0
                    try:
                        import winsound; winsound.Beep(800, 30)
                    except Exception:
                        pass

                frame_count += 1

                if frame_count % 30 == 0 and delta is not None:
                    log.info("AIM ACTIVE | det: %d | fps: %.0f | inf: %.1fms",
                             aim_count, self._hud.fps if self._hud else 0,
                             self._inference_ms)

                # FPS tracking (rolling 1s window)
                now = time.perf_counter()
                self._frame_times.append(now)
                while self._frame_times and now - self._frame_times[0] > 1.0:
                    self._frame_times.pop(0)
                if self._hud and self._frame_times:
                    self._hud.update(fps=len(self._frame_times))

        except KeyboardInterrupt:
            pass
        finally:
            keys.stop()
            overlay.stop_overlay()
            cv2.destroyAllWindows()
            elapsed = time.perf_counter() - t_start
            fps = frame_count / elapsed if elapsed > 0 else 0
            log.info(
                "已退出 | 运行 %.0f秒 | %d帧 (%.0f fps) | %d次瞄准",
                elapsed, frame_count, fps, aim_count,
            )


# ═══════════════════════════════════════════════════════════════════════════
# 入口
# ═══════════════════════════════════════════════════════════════════════════

def main() -> None:
    parser = argparse.ArgumentParser(
        description="CS2 Visual Aimbot — 零训练方案"
    )
    parser.add_argument(
        "--config", type=Path, default=DEFAULT_CONFIG,
        help="配置文件路径 (默认: config.yaml)",
    )
    parser.add_argument(
        "--visualize", action="store_true",
        help="启动时开启调试可视化窗口",
    )
    parser.add_argument(
        "--debug", action="store_true",
        help="启用 DEBUG 日志（每帧输出）",
    )
    args = parser.parse_args()

    if args.debug:
        log.setLevel(logging.DEBUG)

    if not args.config.exists():
        log.error("配置文件不存在: %s", args.config)
        sys.exit(1)

    # 检查 CUDA
    if not torch.cuda.is_available():
        log.warning("CUDA 不可用！推理将非常慢。请安装 CUDA 版 PyTorch。")

    aimbot = CS2VisualAimbot(args.config)
    if args.visualize:
        aimbot.vis.toggle()
    aimbot.run()


if __name__ == "__main__":
    main()
