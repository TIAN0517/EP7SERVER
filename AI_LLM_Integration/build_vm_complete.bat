@echo off
REM ========================================================================
REM RANOnline AI LLM Integration - VM相容版自動構建打包腳本
REM 版本: 3.1.0 VM專用版
REM 開發團隊: Jy技術團隊
REM 
REM 🎯 功能特性:
REM ✅ 自動檢測Qt6和MinGW環境
REM ✅ 構建通用版本和VM相容版本
REM ✅ 完整依賴分析和打包
REM ✅ 軟體渲染組件確認
REM ✅ 自動生成ZIP壓縮包
REM ✅ 依賴清單驗證
REM ========================================================================

setlocal enabledelayedexpansion

echo ========================================
echo RANOnline AI LLM Integration
echo VM相容版自動構建打包系統
echo 版本: 3.1.0 VM專用版
echo 開發團隊: Jy技術團隊
echo ========================================
echo.

REM 設置變數
set "PROJECT_DIR=%~dp0"
set "BUILD_DIR=%PROJECT_DIR%build_vm"
set "DEPLOY_DIR=%PROJECT_DIR%deploy_vm"
set "PACKAGE_DIR=%PROJECT_DIR%vm_package"
set "ZIP_NAME=RANOnline_AI_VM_Compatible_v3.1.0.zip"

REM 清理舊的構建目錄
echo [1/10] 清理舊的構建環境...
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
if exist "%DEPLOY_DIR%" rmdir /s /q "%DEPLOY_DIR%"
if exist "%PACKAGE_DIR%" rmdir /s /q "%PACKAGE_DIR%"

mkdir "%BUILD_DIR%"
mkdir "%DEPLOY_DIR%"
mkdir "%PACKAGE_DIR%"

REM 自動偵測Qt6路徑
echo [2/10] 偵測Qt6和MinGW環境...
set "QT6_ROOT="
for %%d in (
    "C:\Qt\6.9.1\mingw_64"
    "C:\Qt\6.8.0\mingw_64"
    "C:\Qt\6.7.0\mingw_64"
    "C:\Qt\6.6.0\mingw_64"
    "C:\Qt\6.5.0\mingw_64"
) do (
    if exist "%%d\bin\qmake.exe" (
        set "QT6_ROOT=%%d"
        echo Found Qt6 at: %%d
        goto qt_found
    )
)

:qt_found
if "%QT6_ROOT%"=="" (
    echo ❌ ERROR: Qt6 with MinGW not found!
    echo Please install Qt6 with MinGW or set QT6_ROOT manually.
    pause
    exit /b 1
)

REM 設置路徑
set "PATH=%QT6_ROOT%\bin;%QT6_ROOT%\..\..\..\Tools\mingw1310_64\bin;%PATH%"
set "Qt6_DIR=%QT6_ROOT%\lib\cmake\Qt6"

REM 驗證工具
echo [3/10] 驗證構建工具...
where cmake >nul 2>&1
if errorlevel 1 (
    echo ❌ ERROR: CMake not found in PATH
    pause
    exit /b 1
)

where g++ >nul 2>&1
if errorlevel 1 (
    echo ❌ ERROR: MinGW g++ not found in PATH
    pause
    exit /b 1
)

where windeployqt >nul 2>&1
if errorlevel 1 (
    echo ❌ ERROR: windeployqt not found in PATH
    pause
    exit /b 1
)

echo ✅ All build tools found

REM 複製VM專用CMakeLists.txt
echo [4/10] 準備VM專用構建配置...
copy /y "%PROJECT_DIR%CMakeLists_VM.txt" "%PROJECT_DIR%CMakeLists.txt"

REM 配置CMake
echo [5/10] 配置CMake (VM相容版)...
cd /d "%BUILD_DIR%"
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DQt6_DIR="%Qt6_DIR%" ^
    -DBUILD_VM_COMPATIBLE=ON ^
    -DBUILD_STATIC=ON ^
    -DBUILD_FULL_BUNDLE=ON ^
    -DCMAKE_CXX_FLAGS="-DQT_NO_OPENGL_ES_2 -DQT_OPENGL_SOFTWARE"

if errorlevel 1 (
    echo ❌ CMake configuration failed!
    pause
    exit /b 1
)

REM 構建專案
echo [6/10] 構建VM相容版本...
cmake --build . --config Release --parallel 4

if errorlevel 1 (
    echo ❌ Build failed!
    pause
    exit /b 1
)

echo ✅ Build completed successfully

REM 檢查生成的執行檔
echo [7/10] 驗證生成的執行檔...
if not exist "%BUILD_DIR%\ai_llm_integration.exe" (
    echo ❌ Standard version executable not found!
    pause
    exit /b 1
)

if not exist "%BUILD_DIR%\ai_llm_integration_vm.exe" (
    echo ❌ VM compatible version executable not found!
    pause
    exit /b 1
)

echo ✅ Both executables generated successfully

REM 創建部署目錄結構
echo [8/10] 創建部署包結構...
mkdir "%PACKAGE_DIR%\Standard_Version"
mkdir "%PACKAGE_DIR%\VM_Compatible_Version"

REM 部署通用版本
echo [8a/10] 部署通用版本 (完整依賴)...
copy "%BUILD_DIR%\ai_llm_integration.exe" "%PACKAGE_DIR%\Standard_Version\"
cd /d "%PACKAGE_DIR%\Standard_Version"
windeployqt.exe --verbose 2 --force --compiler-runtime --translations --opengl-sw ai_llm_integration.exe

REM 部署VM相容版本 (最小依賴)
echo [8b/10] 部署VM相容版本 (最小依賴+軟體渲染)...
copy "%BUILD_DIR%\ai_llm_integration_vm.exe" "%PACKAGE_DIR%\VM_Compatible_Version\"
cd /d "%PACKAGE_DIR%\VM_Compatible_Version"
windeployqt.exe --verbose 2 --force --compiler-runtime --no-translations --no-system-d3d-compiler --no-virtualkeyboard --no-webkit2 --no-angle --no-opengl-sw ai_llm_integration_vm.exe

REM 手動複製VM關鍵依賴
echo [8c/10] 手動複製VM關鍵依賴...

REM 確保軟體渲染DLL存在
if exist "%QT6_ROOT%\bin\opengl32sw.dll" (
    copy "%QT6_ROOT%\bin\opengl32sw.dll" "%PACKAGE_DIR%\VM_Compatible_Version\"
    echo ✅ opengl32sw.dll copied (software rendering)
) else (
    echo ⚠️ Warning: opengl32sw.dll not found
)

REM 複製基本平台插件
if not exist "%PACKAGE_DIR%\VM_Compatible_Version\platforms" mkdir "%PACKAGE_DIR%\VM_Compatible_Version\platforms"
copy "%QT6_ROOT%\plugins\platforms\qwindows.dll" "%PACKAGE_DIR%\VM_Compatible_Version\platforms\" 2>nul

REM 複製基本樣式插件
if not exist "%PACKAGE_DIR%\VM_Compatible_Version\styles" mkdir "%PACKAGE_DIR%\VM_Compatible_Version\styles"
copy "%QT6_ROOT%\plugins\styles\qwindowsvistastyle.dll" "%PACKAGE_DIR%\VM_Compatible_Version\styles\" 2>nul

REM 複製圖像格式插件
if not exist "%PACKAGE_DIR%\VM_Compatible_Version\imageformats" mkdir "%PACKAGE_DIR%\VM_Compatible_Version\imageformats"
copy "%QT6_ROOT%\plugins\imageformats\qico.dll" "%PACKAGE_DIR%\VM_Compatible_Version\imageformats\" 2>nul
copy "%QT6_ROOT%\plugins\imageformats\qjpeg.dll" "%PACKAGE_DIR%\VM_Compatible_Version\imageformats\" 2>nul
copy "%QT6_ROOT%\plugins\imageformats\qpng.dll" "%PACKAGE_DIR%\VM_Compatible_Version\imageformats\" 2>nul

REM 創建VM啟動腳本
echo [9/10] 創建VM專用啟動腳本...

REM VM相容版啟動腳本
(
echo @echo off
echo REM VM相容模式啟動腳本
echo set QT_OPENGL=software
echo set QT_ANGLE_PLATFORM=d3d11
echo set QT_AUTO_SCREEN_SCALE_FACTOR=0
echo set QT_SCALE_FACTOR=1
echo set QT_QPA_PLATFORM=windows
echo echo 正在啟動VM相容版本...
echo echo 使用軟體渲染模式，適用於虛擬機環境
echo start "RANOnline AI VM版" ai_llm_integration_vm.exe
) > "%PACKAGE_DIR%\VM_Compatible_Version\啟動VM相容版.bat"

REM 通用版啟動腳本
(
echo @echo off
echo echo 正在啟動通用版本...
echo echo 使用完整硬體加速，適用於物理機環境
echo start "RANOnline AI 通用版" ai_llm_integration.exe
) > "%PACKAGE_DIR%\Standard_Version\啟動通用版.bat"

REM 創建使用說明
echo [9a/10] 創建使用說明文件...
(
echo # RANOnline AI LLM Integration - VM相容版 v3.1.0
echo.
echo ## 版本說明
echo.
echo ### 通用版本 ^(Standard_Version^)
echo - 適用於物理機環境
echo - 完整硬體加速支援
echo - 最佳性能表現
echo - 檔案: ai_llm_integration.exe
echo.
echo ### VM相容版本 ^(VM_Compatible_Version^)
echo - 專為虛擬機/雲端主機優化
echo - 強制軟體渲染，避免硬體加速問題
echo - 最小化依賴，最高穩定性
echo - 檔案: ai_llm_integration_vm.exe
echo.
echo ## 使用方法
echo.
echo ### 在虛擬機中使用
echo 1. 解壓縮到任意目錄
echo 2. 進入 VM_Compatible_Version 資料夾
echo 3. 雙擊 "啟動VM相容版.bat"
echo.
echo ### 在物理機中使用
echo 1. 解壓縮到任意目錄
echo 2. 進入 Standard_Version 資料夾
echo 3. 雙擊 "啟動通用版.bat"
echo.
echo ## 技術規格
echo.
echo - Qt版本: 6.x
echo - 目標平台: Windows 10/11 x64
echo - VM軟體渲染: OpenGL Software ^(opengl32sw.dll^)
echo - 依賴: 已完整打包，無需額外安裝
echo.
echo ## 故障排除
echo.
echo 如果VM版本仍然無法啟動，請嘗試：
echo 1. 確保VM分配足夠的記憶體 ^(建議4GB+^)
echo 2. 啟用VM的3D加速 ^(如果可用^)
echo 3. 更新VM工具/增強功能
echo 4. 檢查Windows事件檢視器中的錯誤日誌
echo.
echo ## 開發團隊
echo Jy技術團隊 - 2025
) > "%PACKAGE_DIR%\README.md"

REM 生成依賴清單
echo [9b/10] 生成依賴清單...
(
echo ========================================
echo RANOnline AI VM相容版 - 依賴清單
echo 生成時間: %date% %time%
echo ========================================
echo.
echo 【通用版本依賴】
dir "%PACKAGE_DIR%\Standard_Version" /s /b
echo.
echo ========================================
echo.
echo 【VM相容版本依賴】
dir "%PACKAGE_DIR%\VM_Compatible_Version" /s /b
echo.
echo ========================================
echo 【關鍵DLL檢查】
if exist "%PACKAGE_DIR%\VM_Compatible_Version\Qt6Core.dll" echo ✅ Qt6Core.dll
if exist "%PACKAGE_DIR%\VM_Compatible_Version\Qt6Gui.dll" echo ✅ Qt6Gui.dll
if exist "%PACKAGE_DIR%\VM_Compatible_Version\Qt6Widgets.dll" echo ✅ Qt6Widgets.dll
if exist "%PACKAGE_DIR%\VM_Compatible_Version\opengl32sw.dll" echo ✅ opengl32sw.dll ^(軟體渲染^)
if exist "%PACKAGE_DIR%\VM_Compatible_Version\platforms\qwindows.dll" echo ✅ platforms/qwindows.dll
if exist "%PACKAGE_DIR%\VM_Compatible_Version\libgcc_s_seh-1.dll" echo ✅ libgcc_s_seh-1.dll
if exist "%PACKAGE_DIR%\VM_Compatible_Version\libstdc++-6.dll" echo ✅ libstdc++-6.dll
if exist "%PACKAGE_DIR%\VM_Compatible_Version\libwinpthread-1.dll" echo ✅ libwinpthread-1.dll
echo ========================================
) > "%PACKAGE_DIR%\dependency_list.txt"

REM 創建ZIP壓縮包
echo [10/10] 創建ZIP壓縮包...
cd /d "%PROJECT_DIR%"

REM 使用PowerShell創建ZIP
powershell -Command "Compress-Archive -Path '%PACKAGE_DIR%\*' -DestinationPath '%PROJECT_DIR%\%ZIP_NAME%' -Force"

if exist "%PROJECT_DIR%\%ZIP_NAME%" (
    echo ✅ ZIP package created successfully: %ZIP_NAME%
    for %%I in ("%PROJECT_DIR%\%ZIP_NAME%") do echo 檔案大小: %%~zI bytes
) else (
    echo ❌ Failed to create ZIP package
    pause
    exit /b 1
)

REM 最終驗證
echo.
echo ========================================
echo 構建完成! 最終驗證結果:
echo ========================================
echo.
echo 📁 部署目錄: %PACKAGE_DIR%
echo 📦 ZIP壓縮包: %ZIP_NAME%
echo.
echo 🖥️ 通用版本:
echo    - 執行檔: %PACKAGE_DIR%\Standard_Version\ai_llm_integration.exe
echo    - 啟動腳本: %PACKAGE_DIR%\Standard_Version\啟動通用版.bat
echo.
echo 💻 VM相容版本:
echo    - 執行檔: %PACKAGE_DIR%\VM_Compatible_Version\ai_llm_integration_vm.exe
echo    - 啟動腳本: %PACKAGE_DIR%\VM_Compatible_Version\啟動VM相容版.bat
echo.
echo 📋 依賴清單: %PACKAGE_DIR%\dependency_list.txt
echo 📖 使用說明: %PACKAGE_DIR%\README.md
echo.
echo ========================================
echo 🎉 VM相容版本構建打包完成!
echo 版本: 3.1.0 VM專用版
echo 開發團隊: Jy技術團隊
echo ========================================
echo.
echo 按任意鍵開啟部署目錄...
pause >nul
explorer "%PACKAGE_DIR%"

endlocal
