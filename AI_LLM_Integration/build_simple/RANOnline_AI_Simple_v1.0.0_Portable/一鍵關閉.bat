@echo off
chcp 65001 >nul
title RANOnline AI Simple - 一鍵關閉

echo ========================================================================
echo RANOnline AI Simple - 一鍵關閉
echo 版本: 1.0.0 Simple Edition  
echo 開發團隊: Jy技術團隊 線上AI
echo ========================================================================
echo.

color 0C

echo [進行中] 正在關閉所有 RANOnline AI Simple 進程...

:: 強制關閉所有相關進程
taskkill /f /im "RANOnline_AI_VM_Simple.exe" >nul 2>&1

:: 等待一秒
timeout /t 1 /nobreak >nul

:: 檢查是否還有進程運行
tasklist /fi "imagename eq RANOnline_AI_VM_Simple.exe" 2>nul | find /i "RANOnline_AI_VM_Simple.exe" >nul
if %ERRORLEVEL%==0 (
    echo [!] 仍有進程在運行，正在進行深度清理...
    taskkill /f /im "RANOnline_AI_VM_Simple.exe" >nul 2>&1
    timeout /t 2 /nobreak >nul
) else (
    echo [√] 所有 RANOnline AI Simple 進程已成功關閉
)

echo.
echo [√] 關閉操作完成！
echo.
echo 按任意鍵退出...
pause >nul
