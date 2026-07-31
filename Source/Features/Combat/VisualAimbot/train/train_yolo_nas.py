#!/usr/bin/env python3
"""
YOLO-NAS 定制训练 — CS2 玩家检测
================================
用 SuperGradients 框架训练 YOLO-NAS 模型，检测 CS2 玩家的
头部/身体/队友三个类别。

前置步骤:
  1. 采集数据: python collect_frames.py
  2. 标注数据: labelImg (class: head, body, teammate)
  3. 组织目录: data/{images,labels}/{train,val}/
  4. 训练: python train_yolo_nas.py --data ../data/ --epochs 100

用法:
    python train_yolo_nas.py --data ../data/ [--model m] [--epochs 100] [--batch 16]

训练输出:
    runs/train_cs2/{timestamp}/
      ckpt_best.pth         ← 最佳模型
      ckpt_latest.pth       ← 最新检查点
      avg_model.pth         ← 平均模型
      tensorboard/          ← 训练曲线
"""

from __future__ import annotations

import argparse
import datetime
from pathlib import Path

import torch
from super_gradients.training import Trainer
from super_gradients.training import dataloaders
from super_gradients.training import models
from super_gradients.training.losses import PPYoloELoss
from super_gradients.training.metrics import (
    DetectionMetrics,
    DetectionMetrics_050,
)
from super_gradients.training.dataloaders.dataloaders import (
    coco_detection_yolo_format_train,
    coco_detection_yolo_format_val,
)
from super_gradients.training.utils.distributed_training_utils import (
    setup_device,
)


# ── 配置 ──────────────────────────────────────────────────────────────────

CLASSES = ["head", "body", "teammate"]

MODEL_SIZES = {
    "s": "yolo_nas_s",
    "m": "yolo_nas_m",
    "l": "yolo_nas_l",
}


def get_train_params(args: argparse.Namespace) -> dict:
    """构建 SuperGradients 训练参数。"""
    return {
        # ── 基础 ──
        "max_epochs": args.epochs,
        "mixed_precision": args.fp16,
        "warmup_mode": "linear_epoch_step",
        "warmup_initial_lr": 1e-6,

        # ── 优化器 ──
        "optimizer": "AdamW",
        "initial_lr": args.lr,
        "lr_mode": "cosine",
        "cosine_final_lr_ratio": 0.01,
        "optimizer_params": {"weight_decay": 0.0001},

        # ── EMA (指数移动平均) ──
        "ema": True,
        "ema_decay": 0.9997,

        # ── 混合精度 ──
        "loss": PPYoloELoss(
            use_static_assigner=False,
            num_classes=len(CLASSES),
            reg_max=16,
        ),

        # ── 验证 ──
        "valid_metrics_list": [
            DetectionMetrics_050(
                score_thres=0.1,
                top_k_predictions=300,
                post_prediction_callback=models.get("yolo_nas_m")
                    .get_post_prediction_callback(
                        iou=0.7,
                        conf=0.5,
                    ),
                num_cls=len(CLASSES),
            ),
            DetectionMetrics(
                score_thres=0.1,
                top_k_predictions=300,
                post_prediction_callback=models.get("yolo_nas_m")
                    .get_post_prediction_callback(
                        iou=0.7,
                        conf=0.5,
                    ),
                num_cls=len(CLASSES),
            ),
        ],
        "metric_to_watch": "mAP@0.50:0.95",
        "greater_metric_to_watch_is_better": True,

        # ── 数据增强 ──
        "train_metrics_list": [],
        "phase_callbacks": [],
        "pre_prediction_callback": None,

        # ── 日志 ──
        "silent_mode": False,
        "average_best_models": True,

        # ── 检查点 ──
        "save_ckpt_epoch_list": [20, 40, 60, 80],
    }


# ── 训练入口 ──────────────────────────────────────────────────────────────

def main() -> None:
    parser = argparse.ArgumentParser(
        description="YOLO-NAS CS2 玩家检测训练"
    )
    parser.add_argument(
        "--data", type=Path, required=True,
        help="数据集根目录 (含 images/, labels/, classes.txt)",
    )
    parser.add_argument(
        "--model", type=str, default="m", choices=["s", "m", "l"],
        help="模型大小: s(快)/m(推荐)/l(高精度) (默认: m)",
    )
    parser.add_argument(
        "--epochs", type=int, default=100,
        help="训练轮数 (默认: 100)",
    )
    parser.add_argument(
        "--batch", type=int, default=16,
        help="批次大小 (默认: 16, 6GB 显存建议 8)",
    )
    parser.add_argument(
        "--lr", type=float, default=1e-3,
        help="初始学习率 (默认: 1e-3)",
    )
    parser.add_argument(
        "--workers", type=int, default=4,
        help="数据加载线程数",
    )
    parser.add_argument(
        "--fp16", action="store_true", default=True,
        help="启用混合精度训练 (默认开启)",
    )
    parser.add_argument(
        "--resume", type=Path, default=None,
        help="从检查点恢复训练",
    )
    parser.add_argument(
        "--output", type=Path, default=Path("runs/train_cs2"),
        help="输出目录 (默认: runs/train_cs2)",
    )
    args = parser.parse_args()

    # ── 验证数据集结构 ──
    data_dir = args.data.resolve()
    required_dirs = [
        data_dir / "images" / "train",
        data_dir / "images" / "val",
        data_dir / "labels" / "train",
        data_dir / "labels" / "val",
    ]
    for d in required_dirs:
        if not d.exists():
            print(f"✘ 缺少目录: {d}")
            return

    # ── 设备 ──
    if not torch.cuda.is_available():
        print("✘ 需要 CUDA GPU")
        return
    device = setup_device("cuda")
    print(f"设备: {device}")

    # ── 数据加载器 ──
    print("加载训练数据...")
    train_loader = coco_detection_yolo_format_train(
        dataset_params={
            "data_dir": str(data_dir),
            "images_dir": "images/train",
            "labels_dir": "labels/train",
            "classes": CLASSES,
        },
        dataloader_params={
            "batch_size": args.batch,
            "num_workers": args.workers,
        },
    )

    print("加载验证数据...")
    val_loader = coco_detection_yolo_format_val(
        dataset_params={
            "data_dir": str(data_dir),
            "images_dir": "images/val",
            "labels_dir": "labels/val",
            "classes": CLASSES,
        },
        dataloader_params={
            "batch_size": args.batch,
            "num_workers": args.workers,
        },
    )

    # ── 模型 ──
    model_name = MODEL_SIZES[args.model]
    print(f"创建模型: {model_name} ({len(CLASSES)} 类)...")
    model = models.get(
        model_name,
        num_classes=len(CLASSES),
        pretrained_weights="coco",  # COCO 预训练加速收敛
    )

    # ── 训练 ──
    train_params = get_train_params(args)
    trainer = Trainer(
        experiment_name=datetime.datetime.now().strftime(
            f"cs2_yolo_nas_{args.model}_%Y%m%d_%H%M"
        ),
        ckpt_root_dir=str(args.output),
    )

    print("=" * 56)
    print(f"开始训练: {model_name}")
    print(f"  类别: {CLASSES}")
    print(f"  轮数: {args.epochs}")
    print(f"  批次: {args.batch}")
    print(f"  学习率: {args.lr}")
    print(f"  FP16: {args.fp16}")
    print(f"  数据: {data_dir}")
    print("=" * 56)

    if args.resume:
        trainer.train_from_checkpoint(
            model=model,
            training_params=train_params,
            train_loader=train_loader,
            valid_loader=val_loader,
            checkpoint_path=str(args.resume),
        )
    else:
        trainer.train(
            model=model,
            training_params=train_params,
            train_loader=train_loader,
            valid_loader=val_loader,
        )

    # ── 输出最佳模型路径 ──
    best_ckpt = args.output / trainer.experiment_name / "ckpt_best.pth"
    print(f"\n训练完成! 最佳模型: {best_ckpt.resolve()}")
    print("下一步: python export_onnx.py --weights " + str(best_ckpt))


if __name__ == "__main__":
    main()
