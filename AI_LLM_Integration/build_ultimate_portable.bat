@echo off
REM ========================================================================
REM RANOnline AI Ultimate Portable Edition - 自動化構建腳本
REM Jy技術團隊 線上AI - 極簡穩定VM兼容版 v4.0.0
REM ========================================================================

setlocal enabledelayedexpansion

echo.
echo ========================================================================
echo  Jy技術團隊 線上AI - 極簡穩定VM兼容版
echo  自動化構建系統 v4.0.0
echo ========================================================================
echo.

REM 設置顏色
set "GREEN=[92m"
set "YELLOW=[93m"
set "RED=[91m"
set "BLUE=[94m"
set "RESET=[0m"

REM 檢查是否存在源文件
if not exist "main_ultimate_portable.cpp" (
    echo %RED%錯誤: 找不到源文件 main_ultimate_portable.cpp%RESET%
    echo 請確保在正確的目錄中運行此腳本
    pause
    exit /b 1
)

REM 檢查是否存在CMake配置文件
if not exist "CMakeLists_Ultimate_Portable.txt" (
    echo %RED%錯誤: 找不到 CMakeLists_Ultimate_Portable.txt%RESET%
    echo 正在複製...
    copy CMakeLists.txt CMakeLists_Ultimate_Portable.txt 2>nul
)

echo %BLUE%步驟 1: 環境檢查%RESET%
echo ----------------------------------------

REM 檢查CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    echo %RED%❌ CMake 未安裝或不在PATH中%RESET%
    echo 請安裝CMake: https://cmake.org/download/
    pause
    exit /b 1
) else (
    echo %GREEN%✅ CMake 已安裝%RESET%
)

REM 檢查Qt6
set "QT6_FOUND=0"
for %%i in (qmake.exe qmake6.exe) do (
    %%i --version >nul 2>&1
    if not errorlevel 1 (
        set "QT6_FOUND=1"
        echo %GREEN%✅ Qt6 已安裝%RESET%
        goto qt6_found
    )
)

:qt6_found
if %QT6_FOUND%==0 (
    echo %RED%❌ Qt6 未安裝或不在PATH中%RESET%
    echo 請安裝Qt6並將bin目錄添加到PATH
    pause
    exit /b 1
)

REM 檢查編譯器 (Visual Studio)
where cl.exe >nul 2>&1
if errorlevel 1 (
    echo %YELLOW%⚠️  Visual Studio編譯器未在PATH中，嘗試設置環境...%RESET%
    
    REM 嘗試找到Visual Studio
    set "VSTOOLS="
    for %%v in ("2022" "2019" "2017") do (
        for %%e in ("Enterprise" "Professional" "Community" "BuildTools") do (
            if exist "C:\Program Files\Microsoft Visual Studio\%%~v\%%~e\VC\Auxiliary\Build\vcvars64.bat" (
                set "VSTOOLS=C:\Program Files\Microsoft Visual Studio\%%~v\%%~e\VC\Auxiliary\Build\vcvars64.bat"
                goto vs_found
            )
            if exist "C:\Program Files (x86)\Microsoft Visual Studio\%%~v\%%~e\VC\Auxiliary\Build\vcvars64.bat" (
                set "VSTOOLS=C:\Program Files (x86)\Microsoft Visual Studio\%%~v\%%~e\VC\Auxiliary\Build\vcvars64.bat"
                goto vs_found
            )
        )
    )
    
    :vs_found
    if defined VSTOOLS (
        echo %GREEN%✅ 找到Visual Studio，設置環境...%RESET%
        call "!VSTOOLS!"
    ) else (
        echo %RED%❌ 找不到Visual Studio編譯器%RESET%
        echo 請安裝Visual Studio 2017或更新版本
        pause
        exit /b 1
    )
) else (
    echo %GREEN%✅ Visual Studio編譯器已準備就緒%RESET%
)

echo.
echo %BLUE%步驟 2: 清理舊構建%RESET%
echo ----------------------------------------

if exist "build_ultimate_portable" (
    echo 清理舊構建目錄...
    rmdir /s /q "build_ultimate_portable" 2>nul
    echo %GREEN%✅ 舊構建已清理%RESET%
)

echo.
echo %BLUE%步驟 3: CMake配置%RESET%
echo ----------------------------------------

REM 創建構建目錄
mkdir "build_ultimate_portable" 2>nul
cd "build_ultimate_portable"

REM 複製CMake配置文件
copy "..\CMakeLists_Ultimate_Portable.txt" "CMakeLists.txt" >nul

echo 配置CMake項目...
cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="%Qt6_DIR%" ^
    -DQt6_DIR="%Qt6_DIR%" 2>cmake_config.log

if errorlevel 1 (
    echo %RED%❌ CMake配置失敗%RESET%
    echo 檢查日誌: build_ultimate_portable\cmake_config.log
    type cmake_config.log
    cd ..
    pause
    exit /b 1
) else (
    echo %GREEN%✅ CMake配置成功%RESET%
)

echo.
echo %BLUE%步驟 4: 編譯項目%RESET%
echo ----------------------------------------

echo 開始編譯...
cmake --build . --config Release --parallel 2>build.log

if errorlevel 1 (
    echo %RED%❌ 編譯失敗%RESET%
    echo 檢查日誌: build_ultimate_portable\build.log
    type build.log
    cd ..
    pause
    exit /b 1
) else (
    echo %GREEN%✅ 編譯成功%RESET%
)

echo.
echo %BLUE%步驟 5: 部署Qt依賴%RESET%
echo ----------------------------------------

REM 查找可執行文件
set "EXE_FILE="
for %%f in ("Release\*.exe" "Debug\*.exe" "*.exe") do (
    if exist "%%f" (
        set "EXE_FILE=%%f"
        goto exe_found
    )
)

:exe_found
if not defined EXE_FILE (
    echo %RED%❌ 找不到編譯生成的可執行文件%RESET%
    cd ..
    pause
    exit /b 1
)

echo 找到可執行文件: !EXE_FILE!

REM 創建部署目錄
mkdir "bundle_ultimate" 2>nul

REM 複製可執行文件
echo 複製可執行文件...
copy "!EXE_FILE!" "bundle_ultimate\RANOnline_AI_Ultimate_Portable.exe" >nul

REM 使用windeployqt部署
echo 部署Qt依賴庫...
windeployqt --release --no-translations --no-system-d3d-compiler --no-opengl-sw "bundle_ultimate\RANOnline_AI_Ultimate_Portable.exe" 2>deploy.log

if errorlevel 1 (
    echo %YELLOW%⚠️  windeployqt可能遇到問題，手動複製核心庫...%RESET%
    
    REM 手動複製核心Qt庫
    for %%d in ("%QT6_DIR%" "%Qt6_DIR%") do (
        if exist "%%d\bin" (
            echo 從 %%d\bin 複製Qt庫...
            copy "%%d\bin\Qt6Core.dll" "bundle_ultimate\" >nul 2>&1
            copy "%%d\bin\Qt6Gui.dll" "bundle_ultimate\" >nul 2>&1
            copy "%%d\bin\Qt6Widgets.dll" "bundle_ultimate\" >nul 2>&1
            copy "%%d\bin\Qt6Network.dll" "bundle_ultimate\" >nul 2>&1
            
            REM 複製平台插件
            mkdir "bundle_ultimate\platforms" 2>nul
            copy "%%d\plugins\platforms\qwindows.dll" "bundle_ultimate\platforms\" >nul 2>&1
            
            goto manual_copy_done
        )
    )
    :manual_copy_done
) else (
    echo %GREEN%✅ Qt依賴部署成功%RESET%
)

echo.
echo %BLUE%步驟 6: 創建便攜包%RESET%
echo ----------------------------------------

REM 創建啟動腳本
echo @echo off > "bundle_ultimate\啟動AI助手.bat"
echo echo 正在啟動Jy技術團隊線上AI... >> "bundle_ultimate\啟動AI助手.bat"
echo RANOnline_AI_Ultimate_Portable.exe >> "bundle_ultimate\啟動AI助手.bat"

REM 創建README文件
echo Jy技術團隊 線上AI - 極簡穩定VM兼容版 v4.0.0 > "bundle_ultimate\README.txt"
echo ======================================================= >> "bundle_ultimate\README.txt"
echo. >> "bundle_ultimate\README.txt"
echo 🎯 這是專為VM和雲桌面環境設計的AI助手 >> "bundle_ultimate\README.txt"
echo. >> "bundle_ultimate\README.txt"
echo 🚀 使用方法: >> "bundle_ultimate\README.txt"
echo 1. 解壓到任意目錄 >> "bundle_ultimate\README.txt"
echo 2. 雙擊"啟動AI助手.bat"或直接運行"RANOnline_AI_Ultimate_Portable.exe" >> "bundle_ultimate\README.txt"
echo 3. 享受AI對話服務 >> "bundle_ultimate\README.txt"
echo. >> "bundle_ultimate\README.txt"
echo ✨ 特性: >> "bundle_ultimate\README.txt"
echo • 100%%便攜化，無需安裝 >> "bundle_ultimate\README.txt"
echo • VM/RDP/雲桌面完全兼容 >> "bundle_ultimate\README.txt"
echo • 支持Ollama模型集成 >> "bundle_ultimate\README.txt"
echo • 智能環境檢測 >> "bundle_ultimate\README.txt"
echo • 強制軟體渲染，極致穩定 >> "bundle_ultimate\README.txt"
echo. >> "bundle_ultimate\README.txt"
echo 開發團隊: Jy技術團隊 >> "bundle_ultimate\README.txt"
echo 技術支援: jytech@example.com >> "bundle_ultimate\README.txt"

REM 檢查文件大小
cd "bundle_ultimate"
for %%f in (RANOnline_AI_Ultimate_Portable.exe) do (
    set "size=%%~zf"
    set /a "sizeMB=!size!/1024/1024"
    echo 可執行文件大小: !sizeMB! MB
)

cd ..

echo %GREEN%✅ 便攜包創建完成%RESET%

echo.
echo %BLUE%步驟 7: 測試運行%RESET%
echo ----------------------------------------

echo 執行快速測試...
cd "bundle_ultimate"
echo 測試程序啟動... | "RANOnline_AI_Ultimate_Portable.exe" --test >nul 2>&1
if errorlevel 1 (
    echo %YELLOW%⚠️  程序可能需要額外的依賴，但構建已完成%RESET%
) else (
    echo %GREEN%✅ 程序可以正常啟動%RESET%
)

cd ..
cd ..

echo.
echo ========================================================================
echo %GREEN%🎉 構建完成！%RESET%
echo ========================================================================
echo.
echo 📁 輸出位置: build_ultimate_portable\bundle_ultimate\
echo 📱 主程序: RANOnline_AI_Ultimate_Portable.exe
echo 🚀 啟動器: 啟動AI助手.bat
echo 📖 說明文檔: README.txt
echo.
echo %BLUE%使用說明:%RESET%
echo 1. 將bundle_ultimate整個資料夾複製到目標機器
echo 2. 雙擊「啟動AI助手.bat」開始使用
echo 3. 無需安裝任何額外軟體
echo.
echo %GREEN%VM兼容性特性已啟用:%RESET%
echo ✅ 強制軟體渲染
echo ✅ 禁用硬體加速
echo ✅ 智能環境檢測
echo ✅ Ollama模型支持
echo.

REM 詢問是否立即測試
set /p test_now="是否立即測試程序? (y/n): "
if /i "!test_now!"=="y" (
    echo 啟動程序進行測試...
    cd "build_ultimate_portable\bundle_ultimate"
    start "" "RANOnline_AI_Ultimate_Portable.exe"
    cd ..\..
)

echo.
echo 按任意鍵退出...
pause >nul
