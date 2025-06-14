@echo off
REM ========================================================================
REM RANOnline AI 極簡VM穩定便攜版 - 增強構建腳本
REM 版本: 4.0.1 Enhanced Build System
REM 開發團隊: Jy技術團隊 線上AI
REM 
REM 🎯 增強特性:
REM ✅ 完全修復MSVC C++17編譯問題
REM ✅ 自動Qt依賴完整打包
REM ✅ 詳細錯誤日誌和除錯
REM ✅ 100%便攜化部署
REM ✅ 自動產生ZIP整合包
REM ✅ 一鍵啟動/關閉腳本
REM ========================================================================

setlocal enabledelayedexpansion

echo.
echo ========================================================================
echo  Jy技術團隊 線上AI - 極簡VM穩定便攜版 Enhanced Build
echo  自動化構建系統 v4.0.1
echo ========================================================================
echo.

REM 設置顏色和變數
set "GREEN=[92m"
set "YELLOW=[93m"
set "RED=[91m"
set "BLUE=[94m"
set "CYAN=[96m"
set "RESET=[0m"

set "BUILD_TIME=%date% %time%"
set "PROJECT_DIR=%~dp0"
set "BUILD_LOG=%PROJECT_DIR%build_log_%date:~0,4%%date:~5,2%%date:~8,2%_%time:~0,2%%time:~3,2%.txt"

REM 創建日誌文件
echo [%BUILD_TIME%] 開始構建 - Jy技術團隊線上AI極簡VM穩定便攜版 > "%BUILD_LOG%"
echo ========================================== >> "%BUILD_LOG%"

echo %CYAN%🔧 構建日誌: %BUILD_LOG%%RESET%
echo.

REM 檢查源文件
echo %BLUE%步驟 1/10: 檢查源文件%RESET%
echo ----------------------------------------
if not exist "main_ultimate_portable.cpp" (
    echo %RED%❌ 錯誤: 找不到源文件 main_ultimate_portable.cpp%RESET%
    echo [ERROR] 源文件檢查失敗 >> "%BUILD_LOG%"
    pause
    exit /b 1
) else (
    echo %GREEN%✅ 源文件檢查通過%RESET%
    echo [OK] 源文件檢查通過 >> "%BUILD_LOG%"
)

if not exist "CMakeLists_Ultimate_Portable.txt" (
    echo %RED%❌ 錯誤: 找不到 CMakeLists_Ultimate_Portable.txt%RESET%
    echo [ERROR] CMake配置文件缺失 >> "%BUILD_LOG%"
    pause
    exit /b 1
)

echo %BLUE%步驟 2/10: 環境檢查%RESET%
echo ----------------------------------------

REM 檢查CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    echo %RED%❌ CMake 未安裝或不在PATH中%RESET%
    echo [ERROR] CMake檢查失敗 >> "%BUILD_LOG%"
    echo 請安裝CMake: https://cmake.org/download/
    pause
    exit /b 1
) else (
    for /f "tokens=3" %%i in ('cmake --version 2^>nul ^| findstr /i "cmake version"') do (
        echo %GREEN%✅ CMake 已安裝 - 版本: %%i%RESET%
        echo [OK] CMake版本: %%i >> "%BUILD_LOG%"
    )
)

REM 檢查Qt6
set "QT6_FOUND=0"
set "QT6_VERSION="
for %%i in (qmake.exe qmake6.exe) do (
    %%i --version >nul 2>&1
    if not errorlevel 1 (
        set "QT6_FOUND=1"
        for /f "tokens=4" %%v in ('%%i --version 2^>nul ^| findstr /i "Qt version"') do (
            set "QT6_VERSION=%%v"
        )
        echo %GREEN%✅ Qt6 已安裝 - 版本: !QT6_VERSION!%RESET%
        echo [OK] Qt6版本: !QT6_VERSION! >> "%BUILD_LOG%"
        goto qt6_found
    )
)

:qt6_found
if %QT6_FOUND%==0 (
    echo %RED%❌ Qt6 未安裝或不在PATH中%RESET%
    echo [ERROR] Qt6檢查失敗 >> "%BUILD_LOG%"
    echo 請安裝Qt6並將bin目錄添加到PATH
    pause
    exit /b 1
)

REM 檢查並設置Visual Studio環境
echo %BLUE%步驟 3/10: 設置Visual Studio環境%RESET%
echo ----------------------------------------

where cl.exe >nul 2>&1
if errorlevel 1 (
    echo %YELLOW%⚠️  設置Visual Studio編譯環境...%RESET%
    
    REM 尋找Visual Studio
    set "VSTOOLS="
    for %%v in ("2022" "2019" "2017") do (
        for %%e in ("Enterprise" "Professional" "Community" "BuildTools") do (
            if exist "C:\Program Files\Microsoft Visual Studio\%%~v\%%~e\VC\Auxiliary\Build\vcvars64.bat" (
                set "VSTOOLS=C:\Program Files\Microsoft Visual Studio\%%~v\%%~e\VC\Auxiliary\Build\vcvars64.bat"
                echo [INFO] 找到VS %%~v %%~e >> "%BUILD_LOG%"
                goto vs_found
            )
            if exist "C:\Program Files (x86)\Microsoft Visual Studio\%%~v\%%~e\VC\Auxiliary\Build\vcvars64.bat" (
                set "VSTOOLS=C:\Program Files (x86)\Microsoft Visual Studio\%%~v\%%~e\VC\Auxiliary\Build\vcvars64.bat"
                echo [INFO] 找到VS %%~v %%~e (x86) >> "%BUILD_LOG%"
                goto vs_found
            )
        )
    )
    
    :vs_found
    if defined VSTOOLS (
        echo %GREEN%✅ 找到Visual Studio，初始化環境...%RESET%
        call "!VSTOOLS!" >nul 2>&1
        echo [OK] Visual Studio環境已設置: !VSTOOLS! >> "%BUILD_LOG%"
    ) else (
        echo %RED%❌ 找不到Visual Studio編譯器%RESET%
        echo [ERROR] Visual Studio未找到 >> "%BUILD_LOG%"
        echo 請安裝Visual Studio 2017或更新版本
        pause
        exit /b 1
    )
) else (
    echo %GREEN%✅ Visual Studio編譯器已準備就緒%RESET%
    for /f "tokens=*" %%i in ('cl 2^>^&1 ^| findstr "Microsoft"') do (
        echo [OK] 編譯器: %%i >> "%BUILD_LOG%"
    )
)

echo %BLUE%步驟 4/10: 清理舊構建%RESET%
echo ----------------------------------------

if exist "build_ultimate_portable" (
    echo 清理舊構建目錄...
    rmdir /s /q "build_ultimate_portable" 2>nul
    echo %GREEN%✅ 舊構建已清理%RESET%
    echo [OK] 舊構建目錄已清理 >> "%BUILD_LOG%"
)

echo %BLUE%步驟 5/10: CMake配置%RESET%
echo ----------------------------------------

REM 創建構建目錄
mkdir "build_ultimate_portable" 2>nul
cd "build_ultimate_portable"

REM 複製CMake配置文件
copy "..\CMakeLists_Ultimate_Portable.txt" "CMakeLists.txt" >nul

echo 配置CMake項目...
echo [INFO] 開始CMake配置 >> "%BUILD_LOG%"

cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="%Qt6_DIR%" ^
    -DQt6_DIR="%Qt6_DIR%" ^
    -DCMAKE_CXX_STANDARD=17 ^
    -DCMAKE_CXX_STANDARD_REQUIRED=ON ^
    2>&1 | tee cmake_config.log

if errorlevel 1 (
    echo %RED%❌ CMake配置失敗%RESET%
    echo [ERROR] CMake配置失敗 >> "%BUILD_LOG%"
    echo 檢查詳細日誌: build_ultimate_portable\cmake_config.log
    type cmake_config.log >> "%BUILD_LOG%"
    cd ..
    pause
    exit /b 1
) else (
    echo %GREEN%✅ CMake配置成功%RESET%
    echo [OK] CMake配置成功 >> "%BUILD_LOG%"
    type cmake_config.log >> "%BUILD_LOG%"
)

echo %BLUE%步驟 6/10: 編譯項目%RESET%
echo ----------------------------------------

echo 開始編譯 (使用所有CPU核心: %NUMBER_OF_PROCESSORS%)...
echo [INFO] 開始編譯，使用 %NUMBER_OF_PROCESSORS% 個核心 >> "%BUILD_LOG%"

cmake --build . --config Release --parallel %NUMBER_OF_PROCESSORS% 2>&1 | tee build.log

if errorlevel 1 (
    echo %RED%❌ 編譯失敗%RESET%
    echo [ERROR] 編譯失敗 >> "%BUILD_LOG%"
    echo 檢查詳細日誌: build_ultimate_portable\build.log
    type build.log >> "%BUILD_LOG%"
    cd ..
    pause
    exit /b 1
) else (
    echo %GREEN%✅ 編譯成功%RESET%
    echo [OK] 編譯成功 >> "%BUILD_LOG%"
)

echo %BLUE%步驟 7/10: 檢查編譯產物%RESET%
echo ----------------------------------------

REM 查找可執行文件
set "EXE_FILE="
for %%f in ("Release\*.exe" "Debug\*.exe" "*.exe") do (
    if exist "%%f" (
        set "EXE_FILE=%%f"
        echo 找到可執行文件: %%f
        echo [OK] 找到可執行文件: %%f >> "%BUILD_LOG%"
        goto exe_found
    )
)

:exe_found
if not defined EXE_FILE (
    echo %RED%❌ 找不到編譯生成的可執行文件%RESET%
    echo [ERROR] 可執行文件未找到 >> "%BUILD_LOG%"
    dir /s *.exe >> "%BUILD_LOG%"
    cd ..
    pause
    exit /b 1
)

echo %BLUE%步驟 8/10: 完整依賴部署%RESET%
echo ----------------------------------------

REM 創建部署目錄
mkdir "bundle_ultimate" 2>nul

REM 複製可執行文件
echo 複製可執行文件...
copy "!EXE_FILE!" "bundle_ultimate\RANOnline_AI_Ultimate_Portable.exe" >nul
echo [OK] 可執行文件已複製 >> "%BUILD_LOG%"

REM 使用windeployqt進行完整部署
echo 使用windeployqt進行完整部署...
windeployqt --debug-and-release --force --compiler-runtime ^
    --no-translations --no-system-d3d-compiler ^
    "bundle_ultimate\RANOnline_AI_Ultimate_Portable.exe" 2>&1 | tee deploy.log

if errorlevel 1 (
    echo %YELLOW%⚠️  windeployqt遇到問題，執行手動依賴複製...%RESET%
    echo [WARNING] windeployqt問題，執行手動複製 >> "%BUILD_LOG%"
    type deploy.log >> "%BUILD_LOG%"
    
    REM 手動複製關鍵Qt庫
    echo 手動複製Qt核心庫...
    for %%d in ("%QT6_DIR%" "%Qt6_DIR%") do (
        if exist "%%d\..\..\..\bin" (
            set "QT_BIN_DIR=%%d\..\..\..\bin"
            echo 從 !QT_BIN_DIR! 複製Qt庫...
            
            REM 複製核心DLL
            for %%dll in (Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6Network.dll) do (
                if exist "!QT_BIN_DIR!\%%dll" (
                    copy "!QT_BIN_DIR!\%%dll" "bundle_ultimate\" >nul
                    echo   複製: %%dll
                    echo [OK] 手動複製: %%dll >> "%BUILD_LOG%"
                )
            )
            
            REM 複製平台插件
            mkdir "bundle_ultimate\platforms" 2>nul
            if exist "!QT_BIN_DIR!\..\plugins\platforms\qwindows.dll" (
                copy "!QT_BIN_DIR!\..\plugins\platforms\qwindows.dll" "bundle_ultimate\platforms\" >nul
                echo   複製: platforms\qwindows.dll
                echo [OK] 手動複製: platforms\qwindows.dll >> "%BUILD_LOG%"
            )
            
            REM 複製樣式插件
            mkdir "bundle_ultimate\styles" 2>nul
            if exist "!QT_BIN_DIR!\..\plugins\styles" (
                copy "!QT_BIN_DIR!\..\plugins\styles\qwindowsvistastyle.dll" "bundle_ultimate\styles\" >nul 2>&1
            )
            
            goto manual_copy_done
        )
    )
    :manual_copy_done
) else (
    echo %GREEN%✅ Qt依賴部署成功%RESET%
    echo [OK] windeployqt部署成功 >> "%BUILD_LOG%"
    type deploy.log >> "%BUILD_LOG%"
)

echo %BLUE%步驟 9/10: 創建便攜包和腳本%RESET%
echo ----------------------------------------

REM 創建一鍵啟動腳本
echo 創建一鍵啟動腳本...
echo @echo off > "bundle_ultimate\一鍵啟動.bat"
echo title Jy技術團隊 線上AI - 極簡VM穩定版 >> "bundle_ultimate\一鍵啟動.bat"
echo echo. >> "bundle_ultimate\一鍵啟動.bat"
echo echo ======================================== >> "bundle_ultimate\一鍵啟動.bat"
echo echo  Jy技術團隊 線上AI - 極簡VM穩定版 >> "bundle_ultimate\一鍵啟動.bat"
echo echo  正在啟動AI助手... >> "bundle_ultimate\一鍵啟動.bat"
echo echo ======================================== >> "bundle_ultimate\一鍵啟動.bat"
echo echo. >> "bundle_ultimate\一鍵啟動.bat"
echo start "" "RANOnline_AI_Ultimate_Portable.exe" >> "bundle_ultimate\一鍵啟動.bat"
echo if errorlevel 1 ( >> "bundle_ultimate\一鍵啟動.bat"
echo     echo 啟動失敗！請檢查是否有缺少的依賴... >> "bundle_ultimate\一鍵啟動.bat"
echo     pause >> "bundle_ultimate\一鍵啟動.bat"
echo ^) >> "bundle_ultimate\一鍵啟動.bat"

REM 創建一鍵關閉腳本
echo 創建一鍵關閉腳本...
echo @echo off > "bundle_ultimate\一鍵關閉.bat"
echo title 關閉 Jy技術團隊 線上AI >> "bundle_ultimate\一鍵關閉.bat"
echo echo 正在關閉Jy技術團隊線上AI... >> "bundle_ultimate\一鍵關閉.bat"
echo taskkill /f /im "RANOnline_AI_Ultimate_Portable.exe" 2^>nul >> "bundle_ultimate\一鍵關閉.bat"
echo if errorlevel 1 ( >> "bundle_ultimate\一鍵關閉.bat"
echo     echo 程序未運行或已關閉 >> "bundle_ultimate\一鍵關閉.bat"
echo ^) else ( >> "bundle_ultimate\一鍵關閉.bat"
echo     echo 程序已成功關閉 >> "bundle_ultimate\一鍵關閉.bat"
echo ^) >> "bundle_ultimate\一鍵關閉.bat"
echo timeout /t 2 /nobreak ^>nul >> "bundle_ultimate\一鍵關閉.bat"

REM 創建詳細README文件
echo 創建README說明文件...
echo Jy技術團隊 線上AI - 極簡VM穩定便攜版 v4.0.1 > "bundle_ultimate\README.txt"
echo ================================================================ >> "bundle_ultimate\README.txt"
echo. >> "bundle_ultimate\README.txt"
echo 🎯 這是專為VM和雲桌面環境設計的100%%便攜化AI助手 >> "bundle_ultimate\README.txt"
echo. >> "bundle_ultimate\README.txt"
echo 🚀 快速使用指南: >> "bundle_ultimate\README.txt"
echo ================================ >> "bundle_ultimate\README.txt"
echo 1. 雙擊「一鍵啟動.bat」啟動AI助手 >> "bundle_ultimate\README.txt"
echo 2. 或直接運行「RANOnline_AI_Ultimate_Portable.exe」 >> "bundle_ultimate\README.txt"
echo 3. 使用完畢後雙擊「一鍵關閉.bat」關閉程序 >> "bundle_ultimate\README.txt"
echo. >> "bundle_ultimate\README.txt"
echo ✨ 核心特性: >> "bundle_ultimate\README.txt"
echo ================================ >> "bundle_ultimate\README.txt"
echo • 100%%便攜化 - 無需安裝任何依賴 >> "bundle_ultimate\README.txt"
echo • VM/RDP/雲桌面完全兼容 >> "bundle_ultimate\README.txt"
echo • 強制軟體渲染 - 杜絕GPU相關錯誤 >> "bundle_ultimate\README.txt"
echo • 支持Ollama本地AI模型 >> "bundle_ultimate\README.txt"
echo • 智能環境檢測和自適應 >> "bundle_ultimate\README.txt"
echo • 系統原生UI設計 >> "bundle_ultimate\README.txt"
echo. >> "bundle_ultimate\README.txt"
echo 🔧 故障排除: >> "bundle_ultimate\README.txt"
echo ================================ >> "bundle_ultimate\README.txt"
echo • 如程序無法啟動，請確保是Windows 10/11系統 >> "bundle_ultimate\README.txt"
echo • 如顯示異常，程序會自動啟用VM安全模式 >> "bundle_ultimate\README.txt"
echo • 如需Ollama支持，請先安裝Ollama: https://ollama.ai >> "bundle_ultimate\README.txt"
echo • 技術支援: jytech@example.com >> "bundle_ultimate\README.txt"
echo. >> "bundle_ultimate\README.txt"
echo 📋 系統需求: >> "bundle_ultimate\README.txt"
echo ================================ >> "bundle_ultimate\README.txt"
echo • Windows 10 (1903+) 或 Windows 11 >> "bundle_ultimate\README.txt"
echo • 最少 4GB 記憶體 >> "bundle_ultimate\README.txt"
echo • 約 100MB 磁碟空間 >> "bundle_ultimate\README.txt"
echo. >> "bundle_ultimate\README.txt"
echo 構建時間: %BUILD_TIME% >> "bundle_ultimate\README.txt"
echo 開發團隊: Jy技術團隊 >> "bundle_ultimate\README.txt"

REM 顯示文件清單和大小
echo %CYAN%📁 部署文件清單:%RESET%
echo ----------------------------------------
dir "bundle_ultimate" /B
echo.

REM 計算總大小
for /f %%i in ('dir "bundle_ultimate" /s /-c ^| find "個檔案"') do (
    echo 檔案數量: %%i
    echo [INFO] 檔案數量: %%i >> "%BUILD_LOG%"
)

echo %BLUE%步驟 10/10: 創建ZIP整合包%RESET%
echo ----------------------------------------

cd "bundle_ultimate"

REM 檢查PowerShell壓縮功能
powershell -Command "Get-Command Compress-Archive" >nul 2>&1
if not errorlevel 1 (
    echo 使用PowerShell創建ZIP包...
    powershell -Command "Compress-Archive -Path '.\*' -DestinationPath '..\RANOnline_AI_Ultimate_Portable_v4.0.1.zip' -Force"
    echo %GREEN%✅ ZIP整合包已創建%RESET%
    echo [OK] ZIP包已創建 >> "%BUILD_LOG%"
) else (
    echo %YELLOW%⚠️  PowerShell壓縮不可用，請手動打包bundle_ultimate資料夾%RESET%
    echo [WARNING] PowerShell壓縮不可用 >> "%BUILD_LOG%"
)

cd ..
cd ..

REM 複製日誌到bundle目錄
copy "%BUILD_LOG%" "build_ultimate_portable\bundle_ultimate\構建日誌.txt" >nul

echo.
echo ========================================================================
echo %GREEN%🎉 構建完成！%RESET%
echo ========================================================================
echo.
echo %CYAN%📁 輸出位置:%RESET% build_ultimate_portable\bundle_ultimate\
echo %CYAN%📱 主程序:%RESET% RANOnline_AI_Ultimate_Portable.exe
echo %CYAN%🚀 啟動器:%RESET% 一鍵啟動.bat
echo %CYAN%🛑 關閉器:%RESET% 一鍵關閉.bat
echo %CYAN%📖 說明文檔:%RESET% README.txt
echo %CYAN%📝 構建日誌:%RESET% 構建日誌.txt
echo %CYAN%📦 ZIP整合包:%RESET% RANOnline_AI_Ultimate_Portable_v4.0.1.zip
echo.
echo %BLUE%🎯 使用說明:%RESET%
echo 1. 將 bundle_ultimate 整個資料夾複製到目標機器
echo 2. 雙擊「一鍵啟動.bat」開始使用
echo 3. 使用完畢後雙擊「一鍵關閉.bat」關閉程序
echo 4. 完全便攜化，無需安裝任何額外軟體
echo.
echo %GREEN%✅ VM兼容性特性已啟用:%RESET%
echo • 強制軟體渲染
echo • 禁用硬體加速  
echo • 智能環境檢測
echo • Ollama模型支持
echo • MSVC C++17標準修復
echo.

REM 最終構建報告
echo [FINAL] 構建完成於 %date% %time% >> "%BUILD_LOG%"
echo [FINAL] 所有步驟成功完成 >> "%BUILD_LOG%"

REM 詢問是否立即測試
set /p test_now="是否立即測試程序? (y/n): "
if /i "!test_now!"=="y" (
    echo 啟動程序進行測試...
    cd "build_ultimate_portable\bundle_ultimate"
    start "" "一鍵啟動.bat"
    cd ..\..
)

echo.
echo %CYAN%按任意鍵退出...%RESET%
pause >nul
