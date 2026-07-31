@echo off
set PATH=C:\Users\星记\Downloads\TensorRT\TensorRT-11.1.0.106\bin;%PATH%
cd /d D:\CLionProjects\Osiris\Source\Features\Combat\VisualAimbot\zero_train
call ..\train\.venv\Scripts\activate.bat
python cs2_visual_aim.py
pause
