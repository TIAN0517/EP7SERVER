@echo off
chcp 65001 >nul
title 關閉 Jy技術團隊 線上AI

echo 🛑 正在關閉 Jy技術團隊 線上AI...
echo.
taskkill /f /im "RANOnline_AI_Ultimate_Portable.exe" 2>nul
if %errorlevel% equ 0 (
    echo ✅ AI助手已成功關閉
) else (
    echo ℹ️ AI助手未在運行或已關閉
)

echo 🧹 清理臨時檔案...
del /q *.tmp 2>nul
del /q *.log 2>nul
echo ✅ 清理完成

timeout /t 3 /nobreak >nul
