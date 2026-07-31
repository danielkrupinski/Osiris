#!/usr/bin/env python3
"""
TAC-HUD Overlay Server
======================
提供 /status API + 覆盖层 HTML。
Aimbot 主进程写共享状态，HTML 每秒轮询 10 次。

覆盖层窗口使用系统浏览器 app 模式（无需 pywebview）。
"""

from __future__ import annotations

import json
import os
import subprocess
import sys
import threading
import time
from pathlib import Path
from dataclasses import dataclass, field
from typing import Optional

from flask import Flask, jsonify

# ── 共享状态 ──────────────────────────────────────────────────────────────

@dataclass
class HUDState:
    aimbot_active: bool = False
    has_target: bool = False
    detection_count: int = 0
    target_distance: Optional[float] = None
    fps: float = 0.0
    latency_ms: Optional[float] = None
    inference_ms: Optional[float] = None
    fov_radius: int = 150

    _lock: threading.Lock = field(default_factory=threading.Lock, repr=False)

    def update(self, **kwargs) -> None:
        with self._lock:
            for k, v in kwargs.items():
                if hasattr(self, k):
                    setattr(self, k, v)

    def snapshot(self) -> dict:
        with self._lock:
            return {
                "aimbot_active": self.aimbot_active,
                "has_target": self.has_target,
                "detection_count": self.detection_count,
                "target_distance": self.target_distance,
                "fps": self.fps,
                "latency_ms": self.latency_ms,
                "inference_ms": self.inference_ms,
                "fov_radius": self.fov_radius,
            }


_hud_state = HUDState()


def get_hud_state() -> HUDState:
    return _hud_state


# ── Flask ─────────────────────────────────────────────────────────────────

_OVERLAY_DIR = Path(__file__).resolve().parent

flask_app = Flask(
    __name__,
    static_folder=str(_OVERLAY_DIR),
    static_url_path="",
)


@flask_app.route("/")
def index():
    return flask_app.send_static_file("overlay.html")


@flask_app.route("/status")
def status():
    return jsonify(_hud_state.snapshot())


def run_flask():
    flask_app.run(
        host="127.0.0.1",
        port=18921,
        debug=False,
        use_reloader=False,
    )


# ── 覆盖层窗口启动 ───────────────────────────────────────────────────────

def launch_overlay_window():
    """用系统浏览器 app 模式打开覆盖层。

    Edge (Chromium): --app 模式 = 无边框、无地址栏、无菜单
    Chrome:          --app 模式同样可用
    """
    url = "http://127.0.0.1:18921/"

    import tkinter as tk
    root = tk.Tk()
    root.withdraw()
    w, h = root.winfo_screenwidth(), root.winfo_screenheight()
    root.destroy()

    # 尝试 Edge (Windows 默认)
    edge_paths = [
        r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe",
        r"C:\Program Files\Microsoft\Edge\Application\msedge.exe",
    ]
    chrome_paths = [
        r"C:\Program Files\Google\Chrome\Application\chrome.exe",
        r"C:\Program Files (x86)\Google\Chrome\Application\chrome.exe",
    ]

    for browser in edge_paths + chrome_paths:
        if os.path.exists(browser):
            cmd = [
                browser,
                f"--app={url}",
                f"--window-size={w},{h}",
                "--window-position=0,0",
            ]
            print(f"启动覆盖层: {browser}")
            subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            return

    # 回退: 普通浏览器窗口
    print("未找到 Edge/Chrome，用默认浏览器打开")
    import webbrowser
    webbrowser.open(url)


# ── 入口 ──────────────────────────────────────────────────────────────────

def main():
    import tkinter as tk
    root = tk.Tk()
    root.withdraw()
    w, h = root.winfo_screenwidth(), root.winfo_screenheight()
    root.destroy()

    print("═" * 48)
    print("  TAC-HUD Overlay Server")
    print(f"  API:    http://127.0.0.1:18921/status")
    print(f"  HUD:    http://127.0.0.1:18921/")
    print(f"  分辨率: {w}x{h}")
    print("═" * 48)

    # 启动 Flask
    flask_thread = threading.Thread(target=run_flask, daemon=True)
    flask_thread.start()
    time.sleep(1.0)

    # 启动覆盖层窗口
    launch_overlay_window()

    print("覆盖层已启动，按 Ctrl+C 退出")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\n已退出")


if __name__ == "__main__":
    main()
