#!/usr/bin/env python3
"""
TensorRT 引擎编译
=================
将 ONNX 模型编译为 TensorRT 引擎，获得最低推理延迟。

前置条件:
    pip install nvidia-tensorrt  (或手动安装匹配CUDA版本)
    需要匹配的 CUDA + cuDNN 版本

用法:
    python build_trt.py --onnx cs2_yolo.onnx [--fp16] [--output cs2_yolo.engine]

延迟对比 (RTX 3060, 640x640):
    PyTorch FP32: ~15ms
    ONNX Runtime:  ~8ms
    TensorRT FP32: ~4ms
    TensorRT FP16: ~1.8ms  ← 推荐
"""

from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np

# TensorRT 导入
try:
    import tensorrt as trt
except ImportError:
    print("✘ 未安装 tensorrt，请运行: pip install nvidia-tensorrt")
    print("  或从 NVIDIA 官网下载: https://developer.nvidia.com/tensorrt")
    import sys
    sys.exit(1)


TRT_LOGGER = trt.Logger(trt.Logger.WARNING)


# ── ONNX → TensorRT 编译 ──────────────────────────────────────────────────

def build_engine(
    onnx_path: Path,
    fp16: bool = True,
    workspace_gb: int = 2,
    max_batch_size: int = 1,
) -> trt.ICudaEngine:
    """从 ONNX 文件编译 TensorRT 引擎。

    Args:
        onnx_path: ONNX 文件路径
        fp16: 是否启用 FP16 (延迟降 40-50%，精度损失极小)
        workspace_gb: 构建工作空间 (GB)
        max_batch_size: 最大批次大小
    """

    builder = trt.Builder(TRT_LOGGER)
    network = builder.create_network(
        1 << int(trt.NetworkDefinitionCreationFlag.EXPLICIT_BATCH)
    )
    config = builder.create_builder_config()
    config.set_memory_pool_limit(
        trt.MemoryPoolType.WORKSPACE,
        workspace_gb * (1 << 30),
    )

    # FP16 加速
    if fp16:
        if builder.platform_has_fast_fp16:
            config.set_flag(trt.BuilderFlag.FP16)
            print("✓ FP16 已启用")
        else:
            print("⚠ GPU 不支持快速 FP16，使用 FP32")

    # 解析 ONNX
    parser = trt.OnnxParser(network, TRT_LOGGER)
    with open(onnx_path, "rb") as f:
        if not parser.parse(f.read()):
            print("✘ ONNX 解析错误:")
            for i in range(parser.num_errors):
                print(f"  {parser.get_error(i)}")
            return None

    # 优化配置
    profile = builder.create_optimization_profile()
    input_name = network.get_input(0).name
    input_shape = network.get_input(0).shape
    c, h, w = input_shape[1], input_shape[2], input_shape[3]
    profile.set_shape(
        input_name,
        (1, c, h, w),  # min
        (max_batch_size, c, h, w),  # opt
        (max_batch_size, c, h, w),  # max
    )
    config.add_optimization_profile(profile)

    # 构建
    print(f"构建 TensorRT 引擎... (输入: {c}x{h}x{w}, 批次: {max_batch_size})")
    serialized = builder.build_serialized_network(network, config)
    if serialized is None:
        print("✘ 引擎构建失败")
        return None

    runtime = trt.Runtime(TRT_LOGGER)
    engine = runtime.deserialize_cuda_engine(serialized)
    print(f"✓ 引擎构建成功")
    return engine


# ── 保存 ──────────────────────────────────────────────────────────────────

def save_engine(engine: trt.ICudaEngine, output_path: Path) -> None:
    with open(output_path, "wb") as f:
        f.write(engine.serialize())
    print(f"✓ 已保存: {output_path.resolve()}")
    print(f"  文件大小: {output_path.stat().st_size / 1024 / 1024:.1f} MB")


# ── 验证 (可选) ───────────────────────────────────────────────────────────

def verify_engine(engine: trt.ICudaEngine, onnx_path: Path) -> None:
    """用随机输入测试引擎推理。"""
    import pycuda.driver as cuda
    import pycuda.autoinit

    context = engine.create_execution_context()
    input_shape = engine.get_tensor_shape(engine.get_tensor_name(0))
    output_shape = engine.get_tensor_shape(engine.get_tensor_name(1))

    # 分配 GPU 内存
    input_size = int(np.prod(input_shape)) * np.dtype(np.float32).itemsize
    output_size = int(np.prod(output_shape)) * np.dtype(np.float32).itemsize
    d_input = cuda.mem_alloc(input_size)
    d_output = cuda.mem_alloc(output_size)

    # 随机输入
    h_input = np.random.randn(*input_shape).astype(np.float32)
    cuda.memcpy_htod(d_input, h_input)

    # 绑定
    context.set_tensor_address(engine.get_tensor_name(0), int(d_input))
    context.set_tensor_address(engine.get_tensor_name(1), int(d_output))

    # 推理
    stream = cuda.Stream()
    context.execute_async_v3(stream.handle)
    stream.synchronize()

    h_output = np.empty(output_shape, dtype=np.float32)
    cuda.memcpy_dtoh(h_output, d_output)
    print(f"✓ 推理验证通过，输出形状: {output_shape}")


# ── 入口 ──────────────────────────────────────────────────────────────────

def main() -> None:
    parser = argparse.ArgumentParser(
        description="编译 TensorRT 引擎"
    )
    parser.add_argument(
        "--onnx", type=Path, required=True,
        help="ONNX 文件路径",
    )
    parser.add_argument(
        "--output", type=Path, default=Path("cs2_yolo.engine"),
        help="输出 TensorRT 引擎文件",
    )
    parser.add_argument(
        "--fp16", action="store_true", default=True,
        help="启用 FP16 (默认开启)",
    )
    parser.add_argument(
        "--fp32", action="store_true", default=False,
        help="强制 FP32 (关闭 FP16)",
    )
    parser.add_argument(
        "--workspace", type=int, default=2,
        help="构建工作空间 GB (默认: 2)",
    )
    parser.add_argument(
        "--verify", action="store_true", default=False,
        help="构建后运行推理验证",
    )
    args = parser.parse_args()

    if not args.onnx.exists():
        print(f"✘ ONNX 文件不存在: {args.onnx}")
        return

    use_fp16 = args.fp16 and not args.fp32

    engine = build_engine(
        args.onnx,
        fp16=use_fp16,
        workspace_gb=args.workspace,
    )

    if engine is None:
        return

    save_engine(engine, args.output)

    if args.verify:
        print("运行推理验证...")
        verify_engine(engine, args.onnx)

    print()
    print("部署: 将 cs2_yolo.engine 放到 zero_train/ 目录")
    print("      修改 config.yaml: model.path → cs2_yolo.engine")


if __name__ == "__main__":
    main()
