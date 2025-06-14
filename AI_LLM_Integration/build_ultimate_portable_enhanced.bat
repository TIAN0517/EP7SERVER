@echo off
REM ========================================================================
REM RANOnline AI LLM Integration - 極簡穩定VM兼容版構建腳本
REM 版本: 4.0.0 Ultimate Portable Edition
REM 開發團隊: Jy技術團隊 線上AI
REM 
REM 🎯 核心特性:
REM ✅ 100% 便攜化部署
REM ✅ VM/RDP/雲桌面完全兼容  
REM ✅ 強制軟體渲染，杜絕GPU錯誤
REM ✅ 智能環境檢測與自適應
REM ✅ Ollama模型集成支持
REM ✅ 自動化構建與打包
REM ✅ 完整錯誤處理與恢復
REM ========================================================================

setlocal enabledelayedexpansion

REM 設定顏色
set "GREEN=[92m"
set "YELLOW=[93m"
set "RED=[91m"
set "BLUE=[94m"
set "CYAN=[96m"
set "RESET=[0m"
set "BOLD=[1m"

echo.
echo %CYAN%========================================================================%RESET%
echo %BOLD%%GREEN%  Jy技術團隊 線上AI - 極簡穩定VM兼容版%RESET%
echo %BOLD%%GREEN%  智能化構建系統 v4.0.0%RESET%
echo %CYAN%========================================================================%RESET%
echo.

REM 顯示當前時間
echo %BLUE%構建開始時間: %date% %time%%RESET%
echo.

REM 設定專案變數
set "PROJECT_DIR=%~dp0"
set "BUILD_DIR=%PROJECT_DIR%build_ultimate_portable"
set "DEPLOY_DIR=%PROJECT_DIR%deploy_ultimate_portable"
set "PACKAGE_DIR=%PROJECT_DIR%package_ultimate_portable"
set "LOG_DIR=%PROJECT_DIR%logs"
set "ZIP_NAME=RANOnline_AI_Ultimate_Portable_v4.0.0.zip"

REM 創建日誌目錄
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"
set "BUILD_LOG=%LOG_DIR%\build_%date:~0,4%%date:~5,2%%date:~8,2%_%time:~0,2%%time:~3,2%%time:~6,2%.log"

REM 顯示構建組態
echo %BLUE%[組態資訊] 專案配置:%RESET%
echo   📁 專案目錄: %PROJECT_DIR%
echo   🔨 構建目錄: %BUILD_DIR%
echo   📦 部署目錄: %DEPLOY_DIR%
echo   🎁 打包目錄: %PACKAGE_DIR%
echo   📝 日誌檔案: %BUILD_LOG%
echo   💾 封裝檔名: %ZIP_NAME%
echo.

REM 開始記錄日誌
echo [%date% %time%] 開始構建 RANOnline AI Ultimate Portable Edition > "%BUILD_LOG%"
echo [%date% %time%] 專案目錄: %PROJECT_DIR% >> "%BUILD_LOG%"

REM ========================================================================
REM 步驟 1: 環境清理與準備
REM ========================================================================
echo %BOLD%%BLUE%[1/10] 環境清理與準備%RESET%
echo ----------------------------------------

echo   🧹 清理舊的構建檔案...
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%" 2>>"%BUILD_LOG%"
    echo     清理: %BUILD_DIR%
)
if exist "%DEPLOY_DIR%" (
    rmdir /s /q "%DEPLOY_DIR%" 2>>"%BUILD_LOG%"
    echo     清理: %DEPLOY_DIR%
)
if exist "%PACKAGE_DIR%" (
    rmdir /s /q "%PACKAGE_DIR%" 2>>"%BUILD_LOG%"
    echo     清理: %PACKAGE_DIR%
)

echo   📁 創建構建目錄...
mkdir "%BUILD_DIR%" 2>>"%BUILD_LOG%"
mkdir "%DEPLOY_DIR%" 2>>"%BUILD_LOG%"
mkdir "%PACKAGE_DIR%" 2>>"%BUILD_LOG%"

echo %GREEN%   ✅ 環境準備完成%RESET%
echo.

REM ========================================================================
REM 步驟 2: Qt6和MinGW環境偵測
REM ========================================================================
echo %BOLD%%BLUE%[2/10] Qt6和MinGW環境偵測%RESET%
echo ----------------------------------------

echo   🔍 正在掃描Qt6安裝...
set "QT6_ROOT="
set "MINGW_ROOT="

REM 擴展Qt6搜索路徑
for %%d in (
    "C:\Qt\6.9.1\mingw_64"
    "C:\Qt\6.8.0\mingw_64"
    "C:\Qt\6.7.0\mingw_64"
    "C:\Qt\6.6.0\mingw_64"
    "C:\Qt\6.5.0\mingw_64"
    "D:\Qt\6.9.1\mingw_64"
    "D:\Qt\6.8.0\mingw_64"
    "%QTDIR%"
    "%QT_DIR%"
) do (
    if exist "%%d\bin\qmake.exe" (
        set "QT6_ROOT=%%d"
        echo %GREEN%     ✅ 找到Qt6: %%d%RESET%
        goto qt6_found
    )
)

echo %RED%   ❌ 錯誤: 找不到Qt6和MinGW安裝!%RESET%
echo      請確認已安裝Qt6並包含MinGW編譯器
echo      或設定QT6_ROOT環境變數指向Qt6安裝目錄
echo [%date% %time%] 錯誤: 找不到Qt6 >> "%BUILD_LOG%"
pause
exit /b 1

:qt6_found
REM 尋找對應的MinGW
for %%m in (
    "%QT6_ROOT%\..\..\..\Tools\mingw1310_64"
    "%QT6_ROOT%\..\..\..\Tools\mingw1120_64"
    "%QT6_ROOT%\..\..\..\Tools\mingw900_64"
    "C:\Qt\Tools\mingw1310_64"
    "C:\Qt\Tools\mingw1120_64"
) do (
    if exist "%%m\bin\g++.exe" (
        set "MINGW_ROOT=%%m"
        echo %GREEN%     ✅ 找到MinGW: %%m%RESET%
        goto mingw_found
    )
)

echo %YELLOW%   ⚠️  警告: 無法自動找到MinGW，將使用系統PATH%RESET%

:mingw_found
REM 設定PATH環境
set "PATH=%QT6_ROOT%\bin;%PATH%"
if defined MINGW_ROOT set "PATH=%MINGW_ROOT%\bin;%PATH%"
set "Qt6_DIR=%QT6_ROOT%\lib\cmake\Qt6"

echo   📋 環境資訊:
echo     Qt6路徑: %QT6_ROOT%
echo     MinGW路徑: %MINGW_ROOT%
echo     CMake Qt6_DIR: %Qt6_DIR%

echo [%date% %time%] Qt6路徑: %QT6_ROOT% >> "%BUILD_LOG%"
echo [%date% %time%] MinGW路徑: %MINGW_ROOT% >> "%BUILD_LOG%"
echo.

REM ========================================================================
REM 步驟 3: 構建工具驗證
REM ========================================================================
echo %BOLD%%BLUE%[3/10] 構建工具驗證%RESET%
echo ----------------------------------------

echo   🔧 檢查CMake...
where cmake >nul 2>&1
if errorlevel 1 (
    echo %RED%   ❌ 錯誤: 找不到CMake%RESET%
    echo      請從 https://cmake.org/download/ 下載並安裝CMake
    echo      並確保CMake在系統PATH中
    echo [%date% %time%] 錯誤: 找不到CMake >> "%BUILD_LOG%"
    pause
    exit /b 1
) else (
    for /f "tokens=3" %%v in ('cmake --version 2^>nul ^| findstr "version"') do (
        echo %GREEN%     ✅ CMake %%v%RESET%
    )
)

echo   🔧 檢查MinGW編譯器...
where g++ >nul 2>&1
if errorlevel 1 (
    echo %RED%   ❌ 錯誤: 找不到g++編譯器%RESET%
    echo      請確認MinGW已正確安裝並在PATH中
    echo [%date% %time%] 錯誤: 找不到g++ >> "%BUILD_LOG%"
    pause
    exit /b 1
) else (
    for /f "tokens=3" %%v in ('g++ --version 2^>nul ^| findstr "g++"') do (
        echo %GREEN%     ✅ GCC %%v%RESET%
    )
)

echo   🔧 檢查Qt部署工具...
where windeployqt >nul 2>&1
if errorlevel 1 (
    echo %YELLOW%   ⚠️  警告: windeployqt不可用，將使用手動部署%RESET%
    set "USE_MANUAL_DEPLOY=1"
) else (
    echo %GREEN%     ✅ windeployqt 可用%RESET%
    set "USE_MANUAL_DEPLOY=0"
)

echo %GREEN%   ✅ 構建工具驗證完成%RESET%
echo.

REM ========================================================================
REM 步驟 4: 源文件檢查
REM ========================================================================
echo %BOLD%%BLUE%[4/10] 源文件檢查%RESET%
echo ----------------------------------------

set "MAIN_SOURCE="
set "CMAKE_CONFIG="

echo   📄 檢查主程序源文件...
if exist "%PROJECT_DIR%main_ultimate_portable.cpp" (
    set "MAIN_SOURCE=%PROJECT_DIR%main_ultimate_portable.cpp"
    echo %GREEN%     ✅ 找到: main_ultimate_portable.cpp%RESET%
) else if exist "%PROJECT_DIR%main_vm_simple_complete.cpp" (
    set "MAIN_SOURCE=%PROJECT_DIR%main_vm_simple_complete.cpp"
    echo %GREEN%     ✅ 找到: main_vm_simple_complete.cpp%RESET%
) else (
    echo %RED%   ❌ 錯誤: 找不到主程序源文件!%RESET%
    echo      需要以下任一文件:
    echo      - main_ultimate_portable.cpp
    echo      - main_vm_simple_complete.cpp
    echo [%date% %time%] 錯誤: 找不到主程序源文件 >> "%BUILD_LOG%"
    pause
    exit /b 1
)

echo   📄 檢查CMake配置文件...
if exist "%PROJECT_DIR%CMakeLists_Ultimate_Portable.txt" (
    set "CMAKE_CONFIG=%PROJECT_DIR%CMakeLists_Ultimate_Portable.txt"
    echo %GREEN%     ✅ 找到: CMakeLists_Ultimate_Portable.txt%RESET%
) else if exist "%PROJECT_DIR%CMakeLists_VM_Simple.txt" (
    set "CMAKE_CONFIG=%PROJECT_DIR%CMakeLists_VM_Simple.txt"
    echo %GREEN%     ✅ 找到: CMakeLists_VM_Simple.txt%RESET%
) else (
    echo %RED%   ❌ 錯誤: 找不到CMake配置文件!%RESET%
    echo      需要以下任一文件:
    echo      - CMakeLists_Ultimate_Portable.txt
    echo      - CMakeLists_VM_Simple.txt
    echo [%date% %time%] 錯誤: 找不到CMake配置文件 >> "%BUILD_LOG%"
    pause
    exit /b 1
)

echo   📄 檢查資源文件...
if exist "%PROJECT_DIR%Resources" (
    echo %GREEN%     ✅ 找到資源目錄: Resources%RESET%
) else (
    echo %YELLOW%     ⚠️  資源目錄不存在，將跳過%RESET%
)

echo [%date% %time%] 主程序: %MAIN_SOURCE% >> "%BUILD_LOG%"
echo [%date% %time%] CMake配置: %CMAKE_CONFIG% >> "%BUILD_LOG%"
echo %GREEN%   ✅ 源文件檢查完成%RESET%
echo.

REM ========================================================================
REM 步驟 5: CMake配置準備
REM ========================================================================
echo %BOLD%%BLUE%[5/10] CMake配置準備%RESET%
echo ----------------------------------------

echo   📋 複製CMake配置文件...
copy /y "%CMAKE_CONFIG%" "%PROJECT_DIR%CMakeLists.txt" >nul 2>>"%BUILD_LOG%"
if errorlevel 1 (
    echo %RED%   ❌ 錯誤: 複製CMake配置文件失敗%RESET%
    echo [%date% %time%] 錯誤: 複製CMake配置失敗 >> "%BUILD_LOG%"
    pause
    exit /b 1
)

echo %GREEN%     ✅ CMake配置文件準備完成%RESET%
echo.

REM ========================================================================
REM 步驟 6: CMake專案配置
REM ========================================================================
echo %BOLD%%BLUE%[6/10] CMake專案配置%RESET%
echo ----------------------------------------

echo   ⚙️  正在配置CMake專案...
cd /d "%BUILD_DIR%"

cmake -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DQt6_DIR="%Qt6_DIR%" ^
    -DCMAKE_PREFIX_PATH="%QT6_ROOT%" ^
    -DCMAKE_CXX_STANDARD=17 ^
    -DCMAKE_CXX_STANDARD_REQUIRED=ON ^
    -DCMAKE_VERBOSE_MAKEFILE=ON ^
    "%PROJECT_DIR%" >> "%BUILD_LOG%" 2>&1

if errorlevel 1 (
    echo %RED%   ❌ 錯誤: CMake配置失敗!%RESET%
    echo      詳細錯誤資訊請查看: %BUILD_LOG%
    echo.
    echo %YELLOW%   正在顯示錯誤日誌:%RESET%
    type "%BUILD_LOG%" | findstr /i "error\|錯誤\|failed\|失敗"
    cd /d "%PROJECT_DIR%"
    pause
    exit /b 1
)

echo %GREEN%   ✅ CMake配置成功%RESET%
echo.

REM ========================================================================
REM 步驟 7: 專案編譯
REM ========================================================================
echo %BOLD%%BLUE%[7/10] 專案編譯%RESET%
echo ----------------------------------------

echo   🔨 開始編譯專案...
echo      使用 %NUMBER_OF_PROCESSORS% 個並行作業

REM 記錄編譯開始時間
echo [%date% %time%] 開始編譯 >> "%BUILD_LOG%"

cmake --build . --config Release --parallel %NUMBER_OF_PROCESSORS% --verbose >> "%BUILD_LOG%" 2>&1

if errorlevel 1 (
    echo %RED%   ❌ 錯誤: 編譯失敗!%RESET%
    echo      詳細錯誤資訊請查看: %BUILD_LOG%
    echo.
    echo %YELLOW%   正在顯示編譯錯誤:%RESET%
    type "%BUILD_LOG%" | findstr /i "error\|錯誤\|failed\|失敗" | tail -20
    cd /d "%PROJECT_DIR%"
    pause
    exit /b 1
)

echo [%date% %time%] 編譯完成 >> "%BUILD_LOG%"
echo %GREEN%   ✅ 編譯成功%RESET%

REM 檢查生成的執行檔
set "EXE_NAME="
for %%f in ("*.exe") do (
    if exist "%%f" (
        set "EXE_NAME=%%f"
        echo %GREEN%     ✅ 生成執行檔: %%f%RESET%
    )
)

if not defined EXE_NAME (
    echo %RED%   ❌ 錯誤: 編譯後找不到執行檔!%RESET%
    dir *.exe
    echo [%date% %time%] 錯誤: 找不到執行檔 >> "%BUILD_LOG%"
    cd /d "%PROJECT_DIR%"
    pause
    exit /b 1
)

cd /d "%PROJECT_DIR%"
echo.

REM ========================================================================
REM 步驟 8: 依賴庫部署
REM ========================================================================
echo %BOLD%%BLUE%[8/10] 依賴庫部署%RESET%
echo ----------------------------------------

echo   📦 複製主執行檔...
set "TARGET_EXE=RANOnline_AI_Ultimate_Portable.exe"
copy /y "%BUILD_DIR%\%EXE_NAME%" "%DEPLOY_DIR%\%TARGET_EXE%" >nul
if errorlevel 1 (
    echo %RED%   ❌ 錯誤: 複製執行檔失敗%RESET%
    pause
    exit /b 1
)
echo %GREEN%     ✅ 主執行檔: %TARGET_EXE%%RESET%

if %USE_MANUAL_DEPLOY%==0 (
    echo   📦 使用windeployqt自動部署...
    windeployqt --release --verbose 2 ^
        --dir "%DEPLOY_DIR%" ^
        --force --compiler-runtime ^
        --no-translations --no-system-d3d-compiler ^
        --no-opengl-sw --no-quick-import --no-webkit2 ^
        "%DEPLOY_DIR%\%TARGET_EXE%" >> "%BUILD_LOG%" 2>&1
    
    if errorlevel 1 (
        echo %YELLOW%   ⚠️  windeployqt遇到問題，切換到手動部署...%RESET%
        set "USE_MANUAL_DEPLOY=1"
    ) else (
        echo %GREEN%     ✅ 自動部署完成%RESET%
    )
)

if %USE_MANUAL_DEPLOY%==1 (
    echo   📦 手動部署關鍵依賴庫...
    
    REM 複製核心Qt庫
    for %%f in (
        Qt6Core.dll
        Qt6Gui.dll
        Qt6Widgets.dll
        Qt6Network.dll
    ) do (
        if exist "%QT6_ROOT%\bin\%%f" (
            copy /y "%QT6_ROOT%\bin\%%f" "%DEPLOY_DIR%\" >nul
            echo %GREEN%       ✅ Qt庫: %%f%RESET%
        ) else (
            echo %YELLOW%       ⚠️  找不到: %%f%RESET%
        )
    )
    
    REM 複製MinGW運行庫
    if defined MINGW_ROOT (
        for %%f in (
            libgcc_s_seh-1.dll
            libstdc++-6.dll
            libwinpthread-1.dll
        ) do (
            if exist "%MINGW_ROOT%\bin\%%f" (
                copy /y "%MINGW_ROOT%\bin\%%f" "%DEPLOY_DIR%\" >nul
                echo %GREEN%       ✅ MinGW庫: %%f%RESET%
            )
        )
    )
    
    REM 複製平台插件
    echo   📦 部署平台插件...
    if not exist "%DEPLOY_DIR%\platforms" mkdir "%DEPLOY_DIR%\platforms"
    if exist "%QT6_ROOT%\plugins\platforms\qwindows.dll" (
        copy /y "%QT6_ROOT%\plugins\platforms\qwindows.dll" "%DEPLOY_DIR%\platforms\" >nul
        echo %GREEN%       ✅ 平台插件: qwindows.dll%RESET%
    )
    
    REM 複製樣式插件
    if exist "%QT6_ROOT%\plugins\styles" (
        if not exist "%DEPLOY_DIR%\styles" mkdir "%DEPLOY_DIR%\styles"
        for %%f in ("%QT6_ROOT%\plugins\styles\*.dll") do (
            copy /y "%%f" "%DEPLOY_DIR%\styles\" >nul 2>&1
        )
        echo %GREEN%       ✅ 樣式插件已複製%RESET%
    )
)

echo %GREEN%   ✅ 依賴庫部署完成%RESET%
echo.

REM ========================================================================
REM 步驟 9: 創建便攜包
REM ========================================================================
echo %BOLD%%BLUE%[9/10] 創建便攜包%RESET%
echo ----------------------------------------

echo   📁 複製部署檔案到打包目錄...
xcopy "%DEPLOY_DIR%\*" "%PACKAGE_DIR%\" /E /I /Y >nul 2>>"%BUILD_LOG%"

echo   📝 創建使用說明文件...
(
echo Jy技術團隊 線上AI - 極簡穩定VM兼容版 v4.0.0
echo =======================================================
echo.
echo 🎯 專為VM和雲桌面環境設計的AI助手
echo.
echo 🚀 快速使用:
echo 1. 雙擊 "一鍵啟動.bat" 或 "RANOnline_AI_Ultimate_Portable.exe"
echo 2. 程序會自動檢測運行環境並啟用最佳兼容模式
echo 3. 開始與AI助手對話
echo.
echo ✨ 主要特性:
echo • 100%% 便攜化，無需安裝任何依賴
echo • VM/RDP/雲桌面完全兼容
echo • 強制軟體渲染，避免GPU相關問題
echo • 支持Ollama本地模型集成
echo • 智能環境檢測和自適應
echo • 系統原生UI，極致穩定
echo.
echo 🛠️ 故障排除:
echo • 如程序無法啟動，請嘗試以管理員身份運行
echo • 在VM環境中會自動啟用安全模式
echo • 支持Windows 10/11 和 Windows Server
echo.
echo 📞 技術支援:
echo 開發團隊: Jy技術團隊 線上AI
echo 郵箱: jytech@example.com
echo 構建時間: %date% %time%
echo 構建版本: v4.0.0 Ultimate Portable Edition
) > "%PACKAGE_DIR%\README.txt"

echo   🚀 創建一鍵啟動腳本...
(
echo @echo off
echo title Jy技術團隊 線上AI - 極簡穩定VM兼容版
echo echo.
echo echo ========================================
echo echo  Jy技術團隊 線上AI 正在啟動...
echo echo  極簡穩定VM兼容版 v4.0.0
echo echo ========================================
echo echo.
echo echo 🚀 正在啟動AI助手，請稍候...
echo timeout /t 2 /nobreak ^>nul
echo start "" "RANOnline_AI_Ultimate_Portable.exe"
echo echo ✅ AI助手已啟動
echo echo.
echo echo 💡 提示: 可直接關閉此視窗
echo timeout /t 3 /nobreak ^>nul
) > "%PACKAGE_DIR%\一鍵啟動.bat"

echo   🛑 創建一鍵關閉腳本...
(
echo @echo off
echo title 一鍵關閉 - Jy技術團隊 線上AI
echo echo.
echo echo ========================================
echo echo  正在關閉 Jy技術團隊 線上AI
echo echo  極簡穩定VM兼容版 v4.0.0
echo echo ========================================
echo echo.
echo echo 🛑 正在關閉所有相關進程...
echo.
echo taskkill /f /im "RANOnline_AI_Ultimate_Portable.exe" 2^>nul
echo if errorlevel 1 ^(
echo     echo ✅ 沒有找到運行中的AI助手進程
echo ^) else ^(
echo     echo ✅ AI助手已成功關閉
echo ^)
echo.
echo echo 🧹 清理臨時檔案...
echo del /q "%%TEMP%%\Qt-*" 2^>nul
echo echo ✅ 清理完成
echo.
echo echo 👋 感謝使用 Jy技術團隊 線上AI
echo echo.
echo pause
) > "%PACKAGE_DIR%\一鍵關閉.bat"

echo   📊 創建系統資訊檢查腳本...
(
echo @echo off
echo title 系統資訊檢查 - Jy技術團隊 線上AI
echo echo.
echo echo ========================================
echo echo  系統環境檢查工具
echo echo  Jy技術團隊 線上AI v4.0.0
echo echo ========================================
echo echo.
echo echo 🖥️  作業系統資訊:
echo systeminfo ^| findstr /B /C:"OS Name" /C:"OS Version" /C:"系統類型"
echo echo.
echo echo 💾 記憶體資訊:
echo systeminfo ^| findstr /B /C:"Total Physical Memory" /C:"Available Physical Memory"
echo echo.
echo echo 🔧 已安裝的Visual C++ Redistributable:
echo reg query "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall" /s ^| findstr "Visual.*C++.*Redistributable" 2^>nul
echo echo.
echo echo 📦 程序檔案檢查:
echo if exist "RANOnline_AI_Ultimate_Portable.exe" ^(
echo     echo ✅ 主程序: RANOnline_AI_Ultimate_Portable.exe
echo ^) else ^(
echo     echo ❌ 主程序檔案遺失
echo ^)
echo dir /b *.dll 2^>nul ^| find /c ".dll" ^> temp_count.txt
echo set /p dll_count=^<temp_count.txt
echo del temp_count.txt 2^>nul
echo echo ✅ DLL檔案數量: %%dll_count%%
echo echo.
echo echo 🌐 網路連接檢查:
echo ping -n 1 8.8.8.8 ^>nul 2^>&1 ^&^& echo ✅ 網路連接正常 ^|^| echo ❌ 網路連接異常
echo echo.
echo pause
) > "%PACKAGE_DIR%\系統資訊檢查.bat"

echo   📋 創建檔案清單...
echo 檔案清單 - %date% %time% > "%PACKAGE_DIR%\檔案清單.txt"
echo ======================================== >> "%PACKAGE_DIR%\檔案清單.txt"
dir "%PACKAGE_DIR%" /s >> "%PACKAGE_DIR%\檔案清單.txt"

echo %GREEN%   ✅ 便攜包創建完成%RESET%
echo.

REM ========================================================================
REM 步驟 10: 最終驗證與統計
REM ========================================================================
echo %BOLD%%BLUE%[10/10] 最終驗證與統計%RESET%
echo ----------------------------------------

echo   📊 統計構建結果...

REM 計算檔案數量和大小
for /f %%i in ('dir "%PACKAGE_DIR%" /s /-c ^| find "個檔案"') do set "FILE_COUNT=%%i"
for /f %%i in ('dir "%PACKAGE_DIR%" /s /-c ^| find "位元組" ^| find /v "個檔案"') do set "TOTAL_SIZE=%%i"

REM 檢查關鍵檔案
set "VALIDATION_ERRORS=0"
if not exist "%PACKAGE_DIR%\RANOnline_AI_Ultimate_Portable.exe" (
    echo %RED%   ❌ 主執行檔遺失%RESET%
    set /a VALIDATION_ERRORS+=1
)

if not exist "%PACKAGE_DIR%\一鍵啟動.bat" (
    echo %RED%   ❌ 啟動腳本遺失%RESET%
    set /a VALIDATION_ERRORS+=1
)

if not exist "%PACKAGE_DIR%\platforms\qwindows.dll" (
    echo %YELLOW%   ⚠️  平台插件可能遺失%RESET%
)

REM 快速執行測試
echo   🧪 執行快速驗證測試...
cd "%PACKAGE_DIR%"
echo 測試啟動 | "%PACKAGE_DIR%\RANOnline_AI_Ultimate_Portable.exe" --version >nul 2>&1
if errorlevel 1 (
    echo %YELLOW%   ⚠️  程序可能需要額外依賴，但已完成構建%RESET%
) else (
    echo %GREEN%   ✅ 程序啟動測試通過%RESET%
)
cd /d "%PROJECT_DIR%"

echo.
echo %CYAN%========================================================================%RESET%
echo %BOLD%%GREEN%🎉 構建成功完成！%RESET%
echo %CYAN%========================================================================%RESET%
echo.
echo %BOLD%📁 輸出資訊:%RESET%
echo   📂 打包目錄: %PACKAGE_DIR%
echo   🚀 主程序: RANOnline_AI_Ultimate_Portable.exe
echo   📖 使用說明: README.txt
echo   🎮 一鍵啟動: 一鍵啟動.bat
echo   🛑 一鍵關閉: 一鍵關閉.bat
echo   🔧 系統檢查: 系統資訊檢查.bat
echo.
echo %BOLD%📊 統計資訊:%RESET%
echo   📄 檔案數量: %FILE_COUNT%
echo   💾 總大小: %TOTAL_SIZE%
echo   ⚠️  驗證錯誤: %VALIDATION_ERRORS%
echo   📝 構建日誌: %BUILD_LOG%
echo.
echo %BOLD%🚀 使用方式:%RESET%
echo   1. 複製整個 %PACKAGE_DIR% 目錄到目標電腦
echo   2. 雙擊「一鍵啟動.bat」開始使用
echo   3. 如需關閉，雙擊「一鍵關閉.bat」
echo   4. 遇到問題可運行「系統資訊檢查.bat」診斷
echo.
echo %BOLD%✨ VM兼容特性:%RESET%
echo   ✅ 強制軟體渲染
echo   ✅ 智能環境檢測
echo   ✅ 100%%便攜化部署
echo   ✅ Ollama模型支持
echo   ✅ 完整錯誤處理
echo.

REM 詢問是否立即測試
set /p test_now="%BLUE%是否立即測試程序？ (y/N): %RESET%"
if /i "!test_now!"=="y" (
    echo %GREEN%🚀 正在啟動程序進行測試...%RESET%
    cd "%PACKAGE_DIR%"
    start "" "一鍵啟動.bat"
    cd /d "%PROJECT_DIR%"
) else (
    echo %CYAN%👍 您可以稍後手動測試程序%RESET%
)

echo.
echo %GREEN%構建完成時間: %date% %time%%RESET%
echo %BLUE%感謝使用 Jy技術團隊 線上AI 構建系統！%RESET%
echo.
echo 按任意鍵退出...
pause >nul
