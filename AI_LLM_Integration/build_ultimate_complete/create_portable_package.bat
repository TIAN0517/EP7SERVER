@echo off
setlocal EnableDelayedExpansion

REM 設置顏色變數
set "RED=[31m"
set "GREEN=[32m"
set "YELLOW=[33m"
set "BLUE=[34m"
set "MAGENTA=[35m"
set "CYAN=[36m"
set "WHITE=[37m"
set "RESET=[0m"

echo.
echo %CYAN%========================================================================%RESET%
echo %CYAN%   🎁 RANOnline AI Ultimate Portable Edition - 創建便攜包%RESET%
echo %CYAN%========================================================================%RESET%
echo.

REM 獲取當前日期時間
for /f "tokens=2-4 delims=/ " %%a in ('echo %date%') do (
    set "year=%%c"
    set "month=%%a"
    set "day=%%b"
)
for /f "tokens=1-3 delims=: " %%a in ('echo %time%') do (
    set "hour=%%a"
    set "minute=%%b"
    set "second=%%c"
)

set "package_name=RANOnline_AI_Ultimate_Portable_v4.0.0_%year%%month%%day%_%hour%%minute%"

echo %CYAN%   📦 創建便攜包: %package_name%.zip%RESET%
echo.

REM 檢查是否存在7-Zip
set "zip_tool="
if exist "C:\Program Files\7-Zip\7z.exe" (
    set "zip_tool=C:\Program Files\7-Zip\7z.exe"
) else if exist "C:\Program Files (x86)\7-Zip\7z.exe" (
    set "zip_tool=C:\Program Files (x86)\7-Zip\7z.exe"
) else (
    echo %YELLOW%   ⚠️ 7-Zip未找到，使用PowerShell壓縮...%RESET%
    powershell.exe -Command "Compress-Archive -Path 'deploy_ultimate_complete\*' -DestinationPath '%package_name%.zip' -Force"
    goto :package_complete
)

REM 使用7-Zip壓縮
echo %CYAN%   🔧 使用7-Zip壓縮...%RESET%
"%zip_tool%" a -tzip "%package_name%.zip" ".\deploy_ultimate_complete\*" -mx9

:package_complete
if exist "%package_name%.zip" (
    echo.
    echo %GREEN%   ✅ 便攜包創建成功！%RESET%
    echo %WHITE%   📁 位置: %CD%\%package_name%.zip%RESET%
    
    REM 顯示包大小
    for %%I in ("%package_name%.zip") do (
        set "size=%%~zI"
        if !size! gtr 1048576 (
            set /a "size_mb=!size!/1048576"
            echo %WHITE%   📊 大小: !size_mb! MB%RESET%
        ) else (
            set /a "size_kb=!size!/1024"
            echo %WHITE%   📊 大小: !size_kb! KB%RESET%
        )
    )
) else (
    echo %RED%   ❌ 便攜包創建失敗%RESET%
    pause
    exit /b 1
)

echo.
echo %CYAN%   📋 便攜包內容:%RESET%
echo %WHITE%   • RANOnline_AI_Ultimate_Portable.exe (470KB)%RESET%
echo %WHITE%   • 所有Qt6運行時庫 (約25MB)%RESET%
echo %WHITE%   • MinGW運行時庫%RESET%
echo %WHITE%   • 一鍵啟動腳本%RESET%
echo %WHITE%   • 詳細繁體中文說明文檔%RESET%
echo.

echo %MAGENTA%   🚀 部署指南:%RESET%
echo %WHITE%   1. 將壓縮包複製到目標VM/雲桌面%RESET%
echo %WHITE%   2. 解壓到任意目錄%RESET%
echo %WHITE%   3. 雙擊「一鍵啟動_Ultimate_Portable.bat」%RESET%
echo %WHITE%   4. 享受完整的AI助手功能！%RESET%
echo.

echo %GREEN%========================================================================%RESET%
echo %GREEN%   🎉 Ultimate Portable Edition 便攜包準備完成！%RESET%
echo %GREEN%========================================================================%RESET%
echo.

REM 自動測試程序
echo %CYAN%   🔧 啟動快速測試...%RESET%
cd deploy_ultimate_complete
start "" "一鍵啟動_Ultimate_Portable.bat"
cd ..

echo.
echo %YELLOW%   💡 測試完成後，便攜包即可用於生產部署。%RESET%
echo.
pause
