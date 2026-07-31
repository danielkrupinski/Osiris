"""
Visual sanity-check: overlays the YOLO labels on a few random sample images
to confirm the BoneDumper bbox math agrees with the rendered screen.

Run this after collecting a few hundred samples — if the boxes don't line
up with enemy heads/bodies, the bone offsets or the WorldToClipSpace
projection are wrong (most likely a game-update issue).
"""
import argparse
import random
import sys
from pathlib import Path

import cv2


def overlay(image_path: Path, label_path: Path, out_path: Path):
    img = cv2.imread(str(image_path))
    if img is None:
        return
    h, w = img.shape[:2]
    colors = {0: (0, 0, 255), 1: (0, 255, 0)}  # head=red, body=green
    names = {0: "head", 1: "body"}
    with open(label_path) as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) != 5:
                continue
            c = int(parts[0])
            cx, cy, ww, hh = map(float, parts[1:])
            x1 = int((cx - ww / 2) * w)
            y1 = int((cy - hh / 2) * h)
            x2 = int((cx + ww / 2) * w)
            y2 = int((cy + hh / 2) * h)
            color = colors.get(c, (255, 255, 255))
            cv2.rectangle(img, (x1, y1), (x2, y2), color, 2)
            cv2.putText(img, names.get(c, str(c)), (x1, max(y1 - 5, 12)),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 1)
    cv2.imwrite(str(out_path), img)


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--dataset", "-d", type=Path, default=Path("./dataset"))
    p.add_argument("--count", "-n", type=int, default=10)
    p.add_argument("--out", type=Path, default=Path("./dataset/preview"))
    args = p.parse_args()

    images_dir = args.dataset / "images"
    labels_dir = args.dataset / "labels"
    if not images_dir.exists():
        print(f"images dir not found: {images_dir}")
        sys.exit(1)
    args.out.mkdir(parents=True, exist_ok=True)

    all_images = list(images_dir.glob("*.jpg"))
    if not all_images:
        print("no images found")
        sys.exit(1)
    sample = random.sample(all_images, min(args.count, len(all_images)))
    for img_path in sample:
        lbl_path = labels_dir / (img_path.stem + ".txt")
        if not lbl_path.exists():
            continue
        out_path = args.out / img_path.name
        overlay(img_path, lbl_path, out_path)
        print(f"wrote {out_path}")


if __name__ == "__main__":
    main()
