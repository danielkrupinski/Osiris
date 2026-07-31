#!/usr/bin/env python3
"""
CS2 训练数据采集
================
自动录制游戏截图，用于后续标注和训练。

用法:
    python collect_frames.py --output ../data/raw/ --fps 10 --duration 300

采集建议:
    - 覆盖 5 张主流地图 (dust2, mirage, inferno, nuke, ancient)
    - 各种距离 (近距离/中距离/远距离)
    - 各种姿态 (站立/蹲下/跳跃/跑动)
    - 各种光照 (室内/室外/暗处)
    - 每场录制 2-3 分钟，采样率 10fps
"""

from __future__ import annotations

import argparse
import time
from pathlib import Path

import cv2
import numpy as np
import dxcam


def main() -> None:
    parser = argparse.ArgumentParser(description="CS2 训练数据采集")
    parser.add_argument("--output", type=Path, required=True,
                        help="输出目录")
    parser.add_argument("--fps", type=int, default=10,
                        help="采样帧率 (默认 10)")
    parser.add_argument("--duration", type=int, default=300,
                        help="录制时长秒数 (默认 300)")
    parser.add_argument("--width", type=int, default=1280,
                        help="截屏宽度 (默认 1280)")
    parser.add_argument("--height", type=int, default=720,
                        help="截屏高度 (默认 720)")
    parser.add_argument("--map-name", type=str, default="unknown",
                        help="地图名 (用于文件命名)")
    args = parser.parse_args()

    # 创建输出目录
    out_dir = args.output / args.map_name
    out_dir.mkdir(parents=True, exist_ok=True)

    # 屏幕捕获
    cam = dxcam.create(output_color="BGR")
    screen_w = cam.width or 1920
    screen_h = cam.height or 1080
    x = (screen_w - args.width) // 2
    y = (screen_h - args.height) // 2
    region = (x, y, x + args.width, y + args.height)

    interval = 1.0 / args.fps
    total_frames = args.fps * args.duration
    print(f"采集开始: {args.duration}s @ {args.fps}fps → ~{total_frames}帧")
    print(f"输出目录: {out_dir.resolve()}")
    print(f"区域: {args.width}×{args.height} @ ({x},{y})")
    print("3 秒后开始...")

    time.sleep(3)

    saved = 0
    t_start = time.perf_counter()
    last_capture = 0.0

    try:
        while time.perf_counter() - t_start < args.duration:
            now = time.perf_counter()
            if now - last_capture < interval:
                time.sleep(0.001)
                continue

            frame = cam.grab(region=region)
            if frame is None:
                continue

            last_capture = now
            ts = int(now * 1000)
            fname = out_dir / f"{args.map_name}_{ts:013d}.png"
            cv2.imwrite(str(fname), frame)
            saved += 1

            elapsed = now - t_start
            remaining = args.duration - elapsed
            fps_actual = saved / elapsed if elapsed > 0 else 0
            print(f"\r已保存 {saved} ({fps_actual:.1f} fps) | "
                  f"剩余 {remaining:.0f}s   ", end="", flush=True)

    except KeyboardInterrupt:
        print("\n用户中断")

    print(f"\n完成! 共保存 {saved} 帧到 {out_dir}")


if __name__ == "__main__":
    main()
