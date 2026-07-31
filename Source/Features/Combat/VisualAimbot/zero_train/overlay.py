"""Overlay using subprocess + console log instead of tkinter (thread-safe)"""
from dataclasses import dataclass
from typing import List, Optional

@dataclass
class Detection:
    cls: int; cx: float; cy: float; w: float; h: float; conf: float

def init_overlay(fov_radius_px: int = 200):
    pass

def update_overlay(detections, locked: bool):
    if not detections:
        return
    best = max(detections, key=lambda d: d.conf)
    lbl = "HEAD" if best.cls == 0 else "BODY"
    print(f"\r[{'LOCK' if locked else 'IDLE'}] {lbl} {best.conf:.0%} | {len(detections)} dets  ", end="", flush=True)

def stop_overlay():
    pass
