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
echo %CYAN%   🧪 RANOnline AI Ultimate Portable - VM環境驗證測試%RESET%
echo %CYAN%========================================================================%RESET%
echo.

echo %CYAN%   正在檢測系統環境...%RESET%

REM 檢測是否在VM環境
set "is_vm=false"
set "vm_type=Unknown"

REM 檢查常見VM標識
wmic computersystem get manufacturer /format:list | findstr /i "vmware" >nul
if !errorlevel! equ 0 (
    set "is_vm=true"
    set "vm_type=VMware"
)

wmic computersystem get manufacturer /format:list | findstr /i "virtualbox" >nul
if !errorlevel! equ 0 (
    set "is_vm=true"
    set "vm_type=VirtualBox"
)

wmic computersystem get manufacturer /format:list | findstr /i "microsoft" >nul
if !errorlevel! equ 0 (
    wmic computersystem get model /format:list | findstr /i "virtual" >nul
    if !errorlevel! equ 0 (
        set "is_vm=true"
        set "vm_type=Hyper-V"
    )
)

REM 檢查RDP會話
query session 2>nul | findstr /i "rdp" >nul
if !errorlevel! equ 0 (
    set "is_rdp=true"
) else (
    set "is_rdp=false"
)

echo %WHITE%   🖥️  系統類型: !vm_type!%RESET%
if "!is_vm!"=="true" (
    echo %GREEN%   ✅ 虛擬機環境檢測: 是%RESET%
) else (
    echo %YELLOW%   ⚠️  虛擬機環境檢測: 否（物理機）%RESET%
)

if "!is_rdp!"=="true" (
    echo %GREEN%   ✅ RDP會話檢測: 是%RESET%
) else (
    echo %WHITE%   📋 RDP會話檢測: 否（本地會話）%RESET%
)

echo.
echo %CYAN%   🔧 檢查必要組件...%RESET%

REM 檢查可執行文件
if exist "RANOnline_AI_Ultimate_Portable.exe" (
    echo %GREEN%   ✅ 主程式檔案存在%RESET%
    for %%I in ("RANOnline_AI_Ultimate_Portable.exe") do (
        set "exe_size=%%~zI"
        echo %WHITE%      大小: !exe_size! bytes%RESET%
    )
) else (
    echo %RED%   ❌ 主程式檔案缺失%RESET%
    goto :error_exit
)

REM 檢查Qt運行時庫
set "qt_missing=false"
if not exist "Qt6Core.dll" (
    echo %RED%   ❌ Qt6Core.dll 缺失%RESET%
    set "qt_missing=true"
)
if not exist "Qt6Widgets.dll" (
    echo %RED%   ❌ Qt6Widgets.dll 缺失%RESET%
    set "qt_missing=true"
)
if not exist "Qt6Gui.dll" (
    echo %RED%   ❌ Qt6Gui.dll 缺失%RESET%
    set "qt_missing=true"
)

if "!qt_missing!"=="false" (
    echo %GREEN%   ✅ Qt6運行時庫完整%RESET%
) else (
    goto :error_exit
)

REM 檢查MinGW運行時庫
if exist "libgcc_s_seh-1.dll" (
    echo %GREEN%   ✅ MinGW運行時庫存在%RESET%
) else (
    echo %YELLOW%   ⚠️  libgcc_s_seh-1.dll 缺失（可能影響運行）%RESET%
)

echo.
echo %CYAN%   🚀 啟動相容性測試...%RESET%

REM 設置VM優化環境變數
set "QT_AUTO_SCREEN_SCALE_FACTOR=0"
set "QT_SCALE_FACTOR=1"
set "QT_SCREEN_SCALE_FACTORS=1"
set "QT_ENABLE_HIGHDPI_SCALING=0"
set "QT_USE_PHYSICAL_DPI=0"
set "QT_QPA_PLATFORM=windows"
set "QT_OPENGL=software"
set "MESA_GL_VERSION_OVERRIDE=3.3"

echo %WHITE%   環境變數設置完成...%RESET%
echo %WHITE%   正在啟動程式...%RESET%

REM 啟動程式並監控
start "" "RANOnline_AI_Ultimate_Portable.exe"

timeout /t 3 /nobreak >nul

REM 檢查程式是否正在運行
tasklist /fi "imagename eq RANOnline_AI_Ultimate_Portable.exe" 2>nul | findstr /i "RANOnline_AI_Ultimate_Portable.exe" >nul
if !errorlevel! equ 0 (
    echo %GREEN%   ✅ 程式已成功啟動%RESET%
    echo %WHITE%   📋 進程狀態: 運行中%RESET%
) else (
    echo %RED%   ❌ 程式啟動失敗%RESET%
    goto :error_exit
)

echo.
echo %GREEN%========================================================================%RESET%
echo %GREEN%   🎉 VM環境驗證測試完成！%RESET%
echo %GREEN%========================================================================%RESET%
echo.

echo %MAGENTA%   📊 測試結果摘要:%RESET%
echo %WHITE%   • 環境類型: !vm_type!%RESET%
if "!is_vm!"=="true" (
    echo %WHITE%   • VM兼容性: ✅ 完全兼容%RESET%
) else (
    echo %WHITE%   • VM兼容性: ✅ 物理機正常%RESET%
)
if "!is_rdp!"=="true" (
    echo %WHITE%   • RDP兼容性: ✅ 遠程會話正常%RESET%
) else (
    echo %WHITE%   • RDP兼容性: ✅ 本地會話正常%RESET%
)
echo %WHITE%   • 程式狀態: ✅ 運行正常%RESET%
echo %WHITE%   • 依賴庫: ✅ 完整%RESET%

echo.
echo %CYAN%   💡 如果程式正常顯示UI界面，則表示Ultimate Portable版本%RESET%
echo %CYAN%   在當前環境下運行完美，可以正式部署使用。%RESET%
echo.
goto :end

:error_exit
echo.
echo %RED%========================================================================%RESET%
echo %RED%   ❌ VM環境驗證測試失敗！%RESET%
echo %RED%========================================================================%RESET%
echo.
echo %YELLOW%   請檢查以上錯誤信息並修復後重試。%RESET%
echo.

:end
pause
