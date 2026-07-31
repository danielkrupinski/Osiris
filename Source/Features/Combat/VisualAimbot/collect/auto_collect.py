"""
Automatic YOLO dataset collector for CS2.

Pairs every screen frame with the labels written by the Osiris BoneDumper
feature (latest.json in C:/temp/cs2_labels/). Each output sample is a
matched (image.jpg, labels.txt) pair in YOLO format.

Workflow:
    1. Inject Osiris DLL, enable BoneDumper in the in-game menu (config writes
       to BoneDumperEnabled = 1).
    2. Run this script in another terminal.
    3. Play CS2 (deathmatch, casual, demo playback). Script auto-saves every
       frame where at least one enemy was visible.
    4. Ctrl+C to stop. Resulting dataset is in --output.

Uses windows-capture (WGC) — the same Windows.Graphics.Capture API OBS uses
for non-admin display capture. Lower anti-cheat fingerprint than DXGI
Desktop Duplication.
"""
from __future__ import annotations

import argparse
import json
import os
import sys
import time
from pathlib import Path

import cv2
import numpy as np

try:
    from windows_capture import WindowsCapture, Frame, InternalCaptureControl
except ImportError:
    print("Install windows-capture first:")
    print("  pip install windows-capture")
    sys.exit(1)


LABELS_PATH = Path(r"C:/temp/cs2_labels/latest.json")
DEFAULT_OUTPUT = Path("./dataset")
TARGET_FPS = 10
MAX_LABEL_AGE_SEC = 0.20  # reject frames where labels are stale


class Collector:
    def __init__(self, output: Path, monitor_index: int, target_fps: int):
        self.output = output
        self.images_dir = output / "images"
        self.labels_dir = output / "labels"
        self.images_dir.mkdir(parents=True, exist_ok=True)
        self.labels_dir.mkdir(parents=True, exist_ok=True)

        self.target_fps = target_fps
        self.frame_interval = 1.0 / target_fps
        self.last_save_t = 0.0
        self.saved_count = self._count_existing()
        self.frames_arrived = 0
        self.skipped_no_label_file = 0
        self.skipped_empty_objects = 0
        self.skipped_too_old = 0
        self.last_label_json: dict | None = None
        self.last_label_mtime: float = 0.0
        self.last_heartbeat_t = time.time()
        self.start_t = time.time()

        self.capture = WindowsCapture(
            cursor_capture=False,
            draw_border=False,
            monitor_index=monitor_index,
        )
        # windows-capture inspects handler.__name__, so the methods MUST be
        # named exactly on_frame_arrived / on_closed. Don't rename them.
        self.capture.event(self.on_frame_arrived)
        self.capture.event(self.on_closed)
        print(f"[init] capturing monitor {monitor_index}, target {target_fps}fps")
        print(f"[init] output: {output.resolve()}")
        print(f"[init] existing samples in dataset: {self.saved_count}")

        if not LABELS_PATH.exists():
            print(f"[WARN] {LABELS_PATH} does not exist yet")
            print("[WARN] make sure CS2 is running with Osiris injected")
            print("[WARN] and Combat -> Data Collection -> Bone Dumper is ON")
        else:
            print(f"[ok] label file exists: {LABELS_PATH}")

    def _count_existing(self) -> int:
        return len(list(self.images_dir.glob("*.jpg")))

    def _read_labels(self) -> dict | None:
        try:
            mtime = LABELS_PATH.stat().st_mtime
        except FileNotFoundError:
            return None
        if mtime == self.last_label_mtime and self.last_label_json is not None:
            return self.last_label_json
        try:
            with open(LABELS_PATH, "r", encoding="utf-8") as f:
                data = json.load(f)
            self.last_label_json = data
            self.last_label_mtime = mtime
            return data
        except (json.JSONDecodeError, OSError):
            return None

    def on_frame_arrived(self, frame: Frame, capture_control: InternalCaptureControl):
        self.frames_arrived += 1
        if self.frames_arrived == 1:
            print(f"[ok] first frame arrived: {frame.width}x{frame.height}")
        self._heartbeat()

        now = time.time()
        if now - self.last_save_t < self.frame_interval:
            return
        self.last_save_t = now

        labels = self._read_labels()
        if labels is None:
            self.skipped_no_label_file += 1
            return
        objects = labels.get("objects", [])
        if not objects:
            self.skipped_empty_objects += 1
            return

        label_age = now - self.last_label_mtime
        if label_age > MAX_LABEL_AGE_SEC:
            self.skipped_too_old += 1
            return

        # Frame.frame_buffer is BGRA. Convert to BGR for JPG.
        bgra = frame.frame_buffer
        bgr = cv2.cvtColor(bgra, cv2.COLOR_BGRA2BGR)

        name = f"f{self.saved_count:07d}"
        cv2.imwrite(str(self.images_dir / f"{name}.jpg"), bgr,
                    [cv2.IMWRITE_JPEG_QUALITY, 90])

        # YOLO format: one line per object, "class cx cy w h" normalized [0,1].
        with open(self.labels_dir / f"{name}.txt", "w") as f:
            for obj in objects:
                cls = obj["c"]
                cx, cy, w, h = obj["b"]
                f.write(f"{cls} {cx:.6f} {cy:.6f} {w:.6f} {h:.6f}\n")

        self.saved_count += 1

    def _heartbeat(self):
        """Print status every 3 seconds even when nothing is being saved."""
        now = time.time()
        if now - self.last_heartbeat_t < 3.0:
            return
        self.last_heartbeat_t = now
        uptime = int(now - self.start_t)
        label_status = "MISSING" if not LABELS_PATH.exists() else "exists"
        print(
            f"[hb {uptime:>3}s] saved={self.saved_count}  frames={self.frames_arrived}  "
            f"label_file={label_status}  empty={self.skipped_empty_objects}  "
            f"no-file={self.skipped_no_label_file}  stale={self.skipped_too_old}"
        )

    def on_closed(self):
        print(f"[done] total saved: {self.saved_count}")

    def run(self):
        try:
            self.capture.start()
        except KeyboardInterrupt:
            print("\n[stop] Ctrl+C — total saved:", self.saved_count)


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--output", "-o", type=Path, default=DEFAULT_OUTPUT)
    p.add_argument("--monitor", "-m", type=int, default=1,
                   help="primary=1, secondary=2 ...")
    p.add_argument("--fps", type=int, default=TARGET_FPS)
    args = p.parse_args()
    Collector(args.output, args.monitor, args.fps).run()


if __name__ == "__main__":
    main()
