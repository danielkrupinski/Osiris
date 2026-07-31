#!/usr/bin/env python3
"""Diagnostic tool for Visual Aimbot"""
import sys, os, time

os.environ['PYTHONIOENCODING'] = 'utf-8'

def test(label, fn):
    try:
        result = fn()
        print(f"  [OK] {label}" + (f" ({result})" if result else ""))
        return True
    except Exception as e:
        print(f"  [FAIL] {label}: {e}")
        return False

def main():
    print("=" * 50)
    print("  TAC-HUD Visual Aimbot - Diagnostic")
    print("=" * 50)

    ok = True

    # 1. CUDA
    ok &= test("CUDA / PyTorch", lambda: (
        __import__('torch').cuda.is_available() and
        f"GPU: {__import__('torch').cuda.get_device_name(0)}"
    ))

    # 2. YOLO
    ok &= test("YOLOv8n model", lambda: (
        __import__('ultralytics').YOLO('yolov8n.pt').to('cuda') and
        "loaded on CUDA"
    ))

    # 3. dxcam
    import dxcam
    cam = dxcam.create(output_color="BGR")

    def grab_test():
        frame = cam.grab()
        if frame is None:
            return "WARN: grab returned None (GPU output config needed)"
        import numpy as np
        mean_val = frame.mean()
        return f"frame {frame.shape} | mean={mean_val:.1f} | {'OK' if mean_val > 10 else 'WARN: might be black'}"

    print("  dxcam init...", end=" ", flush=True)
    if cam.width:
        print(f"OK ({cam.width}x{cam.height})")
    else:
        print("OK (need manual resolution)")

    ok &= test("dxcam grab", grab_test)

    # 4. Keyboard listener
    from pynput.keyboard import Key, Listener
    print("  Keyboard test: press F6 within 3 seconds...")
    pressed = []
    def on_press(k):
        try:
            if k == Key.f6:
                pressed.append(True)
                return False
        except: pass
    listener = Listener(on_press=on_press)
    listener.start()
    listener.join(3.0)
    listener.stop()
    if pressed:
        print("  [OK] F6 captured")
    else:
        print("  [WARN] F6 not captured in 3s (may need admin or blocked by game)")

    # 5. Flask
    try:
        from overlay_server import flask_app
        with flask_app.test_client() as c:
            r = c.get('/status')
            if r.status_code == 200:
                print(f"  [OK] Flask API - status 200")
            else:
                print(f"  [FAIL] Flask API - status {r.status_code}")
    except Exception as e:
        print(f"  [FAIL] Flask: {e}")

    print("=" * 50)
    if ok:
        print("Core components OK - launch with run.bat")
    else:
        print("Issues found above - fix [FAIL] items first")
    print("=" * 50)

if __name__ == "__main__":
    main()
