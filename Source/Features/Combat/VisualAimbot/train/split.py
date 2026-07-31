"""Split collect dataset into train/val (90/10)."""
import random
import shutil
from pathlib import Path

SRC = Path(r"D:\CLionProjects\Osiris\Source\Features\Combat\VisualAimbot\collect\dataset")
DST = Path(r"D:\CLionProjects\Osiris\Source\Features\Combat\VisualAimbot\train\dataset")
VAL_RATIO = 0.10

random.seed(42)
all_imgs = sorted((SRC / "images").glob("*.jpg"))
print(f"total images found: {len(all_imgs)}")

random.shuffle(all_imgs)
n_val = int(len(all_imgs) * VAL_RATIO)

for split, items in [("val", all_imgs[:n_val]), ("train", all_imgs[n_val:])]:
    img_dir = DST / "images" / split
    lbl_dir = DST / "labels" / split
    img_dir.mkdir(parents=True, exist_ok=True)
    lbl_dir.mkdir(parents=True, exist_ok=True)
    for img in items:
        shutil.move(str(img), str(img_dir / img.name))
        lbl = SRC / "labels" / (img.stem + ".txt")
        if lbl.exists():
            shutil.move(str(lbl), str(lbl_dir / lbl.name))
    print(f"{split}: {len(items)} samples")

# Write data.yaml (Ultralytics format)
yaml_path = DST / "data.yaml"
yaml_path.write_text(f"""\
path: {DST.resolve().as_posix()}
train: images/train
val: images/val
names:
  0: head
  1: body
""")
print(f"wrote {yaml_path}")
