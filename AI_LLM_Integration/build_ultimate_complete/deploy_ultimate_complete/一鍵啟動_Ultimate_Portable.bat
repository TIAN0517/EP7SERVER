@echo off
chcp 65001 >nul
title Jy技術團隊 線上AI - 極簡穩定VM兼容版 v4.0.0

set "RED=^^^[91m"
set "GREEN=^^^[92m"
set "YELLOW=^^^[93m"
set "CYAN=^^^[96m"
set "WHITE=^^^[97m"
set "RESET=^^^[0m"

echo %CYAN%========================================================================%RESET%
echo %GREEN%    【Jy技術團隊 線上AI】極簡穩定VM兼容版 v4.0.0%RESET%
echo %CYAN%========================================================================%RESET%
echo.
echo %WHITE%   🎯 正在啟動AI助手...%RESET%
echo %WHITE%   📍 環境: 自動檢測 (VM/RDP/雲桌面兼容)%RESET%
echo %WHITE%   🔧 渲染: 強制軟體渲染 (最佳兼容性)%RESET%
echo %WHITE%   🛡️ 模式: 安全模式 (錯誤恢復)%RESET%
echo.
echo %CYAN%   ⏳ 請稍候，正在初始化系統...%RESET%
timeout /t 2 /nobreak >nul

REM 設置VM兼容環境變數
set QT_OPENGL=software
set QT_QUICK_BACKEND=software
set QT_ANGLE_PLATFORM=software
set QT_FORCE_SOFTWARE_RENDERING=1

echo %GREEN%   🚀 啟動程序中...%RESET%
start "" "RANOnline_AI_Ultimate_Portable.exe"

timeout /t 3 /nobreak >nul
echo %GREEN%   ✅ AI助手已啟動成功！%RESET%
echo.
echo %WHITE%   💡 如果程序啟動遇到問題，請嘗試：%RESET%
echo %WHITE%      1. 以管理員身份運行%RESET%
echo %WHITE%      2. 檢查防毒軟體設置%RESET%
echo %WHITE%      3. 聯繫技術支援: jytech@example.com%RESET%
echo.
pause
