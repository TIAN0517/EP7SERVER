@echo off
chcp 65001 >nul
title RANOnline AI Simple - 一鍵啟動

echo ========================================================================
echo RANOnline AI Simple - 一鍵啟動
echo 版本: 1.0.0 Simple Edition
echo 開發團隊: Jy技術團隊 線上AI
echo ========================================================================
echo.

color 0A

echo [√] 正在啟動 RANOnline AI Simple...
echo.

:: 檢查可執行文件是否存在
if not exist "RANOnline_AI_VM_Simple.exe" (
    echo [X] 錯誤: 找不到 RANOnline_AI_VM_Simple.exe
    echo 請確認您在正確的部署目錄中運行此腳本
    pause
    exit /b 1
)

:: 啟動程序
echo [進行中] 啟動應用程式...
start RANOnline_AI_VM_Simple.exe

echo [√] RANOnline AI Simple 已啟動！
echo.
echo 如果程式正常顯示窗口，表示啟動成功。
echo 程式具有以下特性：
echo   • 100%% 便攜化，無需安裝
echo   • VM/RDP/雲桌面完全兼容  
echo   • 強制軟體渲染，極致穩定
echo   • 智能環境檢測
echo.
echo 按任意鍵退出啟動腳本...
pause >nul
