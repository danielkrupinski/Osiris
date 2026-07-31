#!/usr/bin/env python3
"""
导出 YOLO-NAS → ONNX
====================
将 SuperGradients 训练好的 YOLO-NAS 模型导出为 ONNX 格式，
作为 TensorRT 编译的中间步骤。

用法:
    python export_onnx.py --weights runs/train_cs2/.../ckpt_best.pth [--output cs2_yolo.onnx]
"""

from __future__ import annotations

import argparse
from pathlib import Path

import torch
from super_gradients.training import models

CLASSES = ["head", "body", "teammate"]


def main() -> None:
    parser = argparse.ArgumentParser(
        description="导出 YOLO-NAS → ONNX"
    )
    parser.add_argument(
        "--weights", type=Path, required=True,
        help="训练好的 .pth 权重文件",
    )
    parser.add_argument(
        "--output", type=Path, default=Path("cs2_yolo.onnx"),
        help="输出 ONNX 文件名",
    )
    parser.add_argument(
        "--input-size", type=int, nargs=2, default=[640, 640],
        help="模型输入尺寸 (默认: 640 640)",
    )
    parser.add_argument(
        "--batch-size", type=int, default=1,
        help="批次大小 (默认: 1)",
    )
    parser.add_argument(
        "--opset", type=int, default=16,
        help="ONNX opset 版本 (默认: 16)",
    )
    args = parser.parse_args()

    if not args.weights.exists():
        print(f"✘ 权重文件不存在: {args.weights}")
        return

    print(f"加载权重: {args.weights}")
    model = models.get(
        "yolo_nas_m",  # 自动检测实际架构
        num_classes=len(CLASSES),
        checkpoint_path=str(args.weights),
    )
    model.eval()

    if torch.cuda.is_available():
        model = model.cuda()

    # ONNX 需要固定输入形状
    dummy_input = torch.randn(
        args.batch_size, 3, *args.input_size,
        device=next(model.parameters()).device,
    )

    output_path = args.output
    print(f"导出 ONNX: {output_path} (opset={args.opset})")

    torch.onnx.export(
        model,
        dummy_input,
        str(output_path),
        opset_version=args.opset,
        input_names=["images"],
        output_names=["output"],
        dynamic_axes={
            "images": {0: "batch"},
            "output": {0: "batch"},
        } if args.batch_size == 1 else None,
        do_constant_folding=True,
    )

    # 验证
    import onnx
    onnx_model = onnx.load(str(output_path))
    onnx.checker.check_model(onnx_model)
    print(f"✓ ONNX 导出成功: {output_path.resolve()}")
    print(f"  文件大小: {output_path.stat().st_size / 1024 / 1024:.1f} MB")
    print("下一步: python build_trt.py --onnx " + str(output_path))


if __name__ == "__main__":
    main()
