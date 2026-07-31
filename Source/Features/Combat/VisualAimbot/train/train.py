"""Train YOLOv11 on the CS2 BoneDumper dataset.

Usage (run from any directory, paths are absolute):
    python train.py              # start fresh training (100 epochs, batch=48)
    python train.py --resume      # resume from last checkpoint
    python train.py --epochs 50 --resume  # resume, target 50 total epochs
"""
import argparse
import multiprocessing
import os
import sys

# chdir to script's own directory so all relative paths (dataset/data.yaml,
# runs/, yolo11n.pt) resolve correctly regardless of where the command is
# invoked from.
os.chdir(os.path.dirname(os.path.abspath(__file__)))

try:
    from ultralytics import YOLO
    import torch
except ImportError:
    print("Install: pip install torch ultralytics")
    sys.exit(1)

if __name__ == "__main__":
    multiprocessing.freeze_support()

    print(f"PyTorch {torch.__version__}")
    print(f"CUDA available: {torch.cuda.is_available()}")
    if torch.cuda.is_available():
        print(f"GPU: {torch.cuda.get_device_name(0)}")
        print(f"VRAM: {torch.cuda.get_device_properties(0).total_memory / 1e9:.1f} GB")

    p = argparse.ArgumentParser()
    p.add_argument("--epochs", type=int, default=100)
    p.add_argument("--batch", type=int, default=32)
    p.add_argument("--resume", action="store_true")
    p.add_argument("--model", default="yolo11n.pt")
    p.add_argument("--data", default="dataset/data.yaml")
    args = p.parse_args()

    CHECKPOINT = "runs/detect/cs2_yolo/weights/last.pt"

    # When resuming, load the checkpoint (which carries optimizer/momentum
    # state) instead of the base weights. Without this, Ultralytics warns
    # "not a resumable training checkpoint" and starts from scratch.
    if args.resume and os.path.exists(CHECKPOINT):
        print(f"[resume] loading checkpoint: {CHECKPOINT}")
        model = YOLO(CHECKPOINT)
        model.train(data=args.data, epochs=args.epochs, imgsz=640,
                     batch=args.batch, device=0, half=True, name="cs2_yolo",
                     resume=True, exist_ok=True)
        sys.exit(0)

    # Fresh training (with OOM auto-retry)
    batch = args.batch
    for attempt in range(3):
        try:
            model = YOLO(args.model)
            model.train(data=args.data, epochs=args.epochs, imgsz=640,
                         batch=batch, device=0, half=True, name="cs2_yolo",
                         resume=False, exist_ok=True)
            break
        except RuntimeError as e:
            if "out of memory" in str(e).lower():
                torch.cuda.empty_cache()
                new_batch = max(8, batch - 8)
                print(f"[OOM] batch={batch} → {new_batch}")
                batch = new_batch
            else:
                raise
    else:
        print("ERROR: Could not find a batch size that fits in VRAM")
