@echo off
cd /d D:\CLionProjects\Osiris\Source\Features\Combat\VisualAimbot\train
call .venv\Scripts\activate.bat
python train.py >> training.log 2>&1
echo Training finished. Check training.log
pause
