@echo off
REM ========================================================================
REM RANOnline AI 極簡VM穩定便攜版 - MinGW構建腳本
REM 版本: 4.0.2 MinGW Build System
REM 開發團隊: Jy技術團隊 線上AI
REM 
REM 🎯 MinGW特性:
REM ✅ 使用Qt自帶MinGW編譯器
REM ✅ 完全便攜化，無MSVC依賴
REM ✅ 自動Qt依賴完整打包
REM ✅ 100%VM兼容性
REM ✅ 自動產生ZIP整合包
REM ========================================================================

setlocal enabledelayedexpansion

echo.
echo ========================================================================
echo  Jy技術團隊 線上AI - 極簡VM穩定便攜版 MinGW Build
echo  自動化構建系統 v4.0.2
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
set "BUILD_LOG=%PROJECT_DIR%build_log_mingw_%date:~0,4%%date:~5,2%%date:~8,2%_%time:~0,2%%time:~3,2%.txt"

REM 創建日誌文件
echo [%BUILD_TIME%] 開始MinGW構建 - Jy技術團隊線上AI極簡VM穩定便攜版 > "%BUILD_LOG%"
echo ========================================== >> "%BUILD_LOG%"

echo %CYAN%🔧 構建日誌: %BUILD_LOG%%RESET%
echo.

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

echo %BLUE%步驟 2/10: 自動檢測Qt6和MinGW%RESET%
echo ----------------------------------------

REM 檢測Qt6安裝路徑
set "QT6_ROOT="
set "MINGW_ROOT="

for %%d in (
    "C:\Qt\6.9.1\mingw_64"
    "C:\Qt\6.8.0\mingw_64"
    "C:\Qt\6.7.0\mingw_64"
    "C:\Qt\6.6.0\mingw_64"
    "C:\Qt\6.5.0\mingw_64"
) do (
    if exist "%%d\bin\qmake.exe" (
        set "QT6_ROOT=%%d"
        echo %GREEN%✅ 找到Qt6: %%d%RESET%
        echo [OK] Qt6路徑: %%d >> "%BUILD_LOG%"
        goto qt6_found
    )
)

:qt6_found
if "%QT6_ROOT%"=="" (
    echo %RED%❌ Qt6未找到%RESET%
    echo [ERROR] Qt6未找到 >> "%BUILD_LOG%"
    echo 請安裝Qt6並確保路徑正確
    pause
    exit /b 1
)

REM 檢測MinGW編譯器
for %%d in (
    "%QT6_ROOT%\..\..\..\Tools\mingw1310_64"
    "%QT6_ROOT%\..\..\..\Tools\mingw1120_64"
    "%QT6_ROOT%\..\..\..\Tools\mingw900_64"
    "C:\Qt\Tools\mingw1310_64"
    "C:\Qt\Tools\mingw1120_64"
) do (
    if exist "%%d\bin\g++.exe" (
        set "MINGW_ROOT=%%d"
        echo %GREEN%✅ 找到MinGW: %%d%RESET%
        echo [OK] MinGW路徑: %%d >> "%BUILD_LOG%"
        goto mingw_found
    )
)

:mingw_found
if "%MINGW_ROOT%"=="" (
    echo %RED%❌ MinGW編譯器未找到%RESET%
    echo [ERROR] MinGW編譯器未找到 >> "%BUILD_LOG%"
    echo 請安裝Qt自帶的MinGW編譯器
    pause
    exit /b 1
)

REM 設置環境路徑
set "PATH=%QT6_ROOT%\bin;%MINGW_ROOT%\bin;%PATH%"
set "Qt6_DIR=%QT6_ROOT%\lib\cmake\Qt6"

echo %CYAN%🎯 構建環境:%RESET%
echo   Qt6路徑: %QT6_ROOT%
echo   MinGW路徑: %MINGW_ROOT%
echo   Qt6_DIR: %Qt6_DIR%

echo %BLUE%步驟 3/10: 驗證構建工具%RESET%
echo ----------------------------------------

REM 檢查CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    echo %RED%❌ CMake 未安裝或不在PATH中%RESET%
    echo [ERROR] CMake檢查失敗 >> "%BUILD_LOG%"
    pause
    exit /b 1
) else (
    echo %GREEN%✅ CMake 已安裝%RESET%
    echo [OK] CMake已安裝 >> "%BUILD_LOG%"
)

REM 檢查g++
g++ --version >nul 2>&1
if errorlevel 1 (
    echo %RED%❌ MinGW g++ 編譯器不可用%RESET%
    echo [ERROR] g++編譯器檢查失敗 >> "%BUILD_LOG%"
    pause
    exit /b 1
) else (
    echo %GREEN%✅ MinGW g++ 編譯器已準備就緒%RESET%
    for /f "tokens=*" %%i in ('g++ --version 2^>^&1 ^| findstr "g++"') do (
        echo [OK] 編譯器: %%i >> "%BUILD_LOG%"
    )
)

REM 檢查windeployqt
windeployqt --version >nul 2>&1
if errorlevel 1 (
    echo %YELLOW%⚠️  windeployqt不可用，將手動複製依賴%RESET%
    echo [WARNING] windeployqt不可用 >> "%BUILD_LOG%"
) else (
    echo %GREEN%✅ windeployqt 已準備就緒%RESET%
    echo [OK] windeployqt可用 >> "%BUILD_LOG%"
)

echo %BLUE%步驟 4/10: 清理舊構建%RESET%
echo ----------------------------------------

if exist "build_ultimate_portable" (
    echo 清理舊構建目錄...
    rmdir /s /q "build_ultimate_portable" 2>nul
    echo %GREEN%✅ 舊構建已清理%RESET%
    echo [OK] 舊構建目錄已清理 >> "%BUILD_LOG%"
)

echo %BLUE%步驟 5/10: 創建MinGW兼容的CMake配置%RESET%
echo ----------------------------------------

REM 創建構建目錄
mkdir "build_ultimate_portable" 2>nul
cd "build_ultimate_portable"

REM 創建MinGW優化的CMakeLists.txt
echo 創建MinGW專用CMake配置...
(
echo # ========================================================================
echo # CMakeLists.txt for RANOnline AI Ultimate Portable Edition - MinGW Build
echo # Jy技術團隊 線上AI - 極簡穩定VM兼容版
echo # Version: 4.0.2 MinGW Build
echo # ========================================================================
echo.
echo cmake_minimum_required^(VERSION 3.16^)
echo.
echo # 項目設置
echo project^(RANOnline_AI_Ultimate_Portable 
echo     VERSION 4.0.2
echo     DESCRIPTION "Jy Tech Team AI - Ultimate Portable VM Compatible Edition - MinGW Build"
echo     LANGUAGES CXX
echo ^)
echo.
echo # C++標準
echo set^(CMAKE_CXX_STANDARD 17^)
echo set^(CMAKE_CXX_STANDARD_REQUIRED ON^)
echo.
echo # MinGW特定設置
echo if^(MINGW^)
echo     # 設置編譯器標誌
echo     set^(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall"^)
echo     set^(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG"^)
echo     
echo     # 靜態鏈接部分運行時庫以減少依賴
echo     set^(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc"^)
echo     
echo     # Windows應用程式設置
echo     set^(CMAKE_WIN32_EXECUTABLE TRUE^)
echo endif^(^)
echo.
echo # Qt設置
echo set^(CMAKE_AUTOMOC ON^)
echo set^(CMAKE_AUTORCC ON^)
echo set^(CMAKE_AUTOUIC ON^)
echo.
echo # 尋找Qt6
echo find_package^(Qt6 REQUIRED COMPONENTS 
echo     Core 
echo     Widgets 
echo     Gui
echo     Network
echo ^)
echo.
echo # 源文件
echo set^(SOURCES
echo     ../main_ultimate_portable.cpp
echo ^)
echo.
echo # 創建可執行文件
echo add_executable^(${PROJECT_NAME} WIN32 ${SOURCES}^)
echo.
echo # 鏈接Qt庫
echo target_link_libraries^(${PROJECT_NAME}
echo     Qt6::Core
echo     Qt6::Widgets
echo     Qt6::Gui
echo     Qt6::Network
echo ^)
echo.
echo # Windows特定鏈接庫
echo if^(WIN32^)
echo     target_link_libraries^(${PROJECT_NAME}
echo         psapi      # 用於進程和系統資訊
echo         user32     # Windows API
echo         gdi32      # 圖形設備接口
echo         shell32    # Shell API
echo         advapi32   # 高級API
echo         ole32      # OLE API
echo         oleaut32   # OLE自動化
echo         uuid       # UUID庫
echo         winmm      # Windows多媒體
echo     ^)
echo endif^(^)
echo.
echo # 編譯器特定設置
echo target_compile_definitions^(${PROJECT_NAME} PRIVATE
echo     QT_DISABLE_DEPRECATED_BEFORE=0x060000
echo     UNICODE
echo     _UNICODE
echo     VM_COMPATIBLE_BUILD=1
echo     FORCE_SOFTWARE_RENDERING=1
echo     DISABLE_HARDWARE_ACCELERATION=1
echo ^)
echo.
echo # 設置可執行文件名稱
echo set_target_properties^(${PROJECT_NAME} PROPERTIES
echo     OUTPUT_NAME "RANOnline_AI_Ultimate_Portable"
echo ^)
) > CMakeLists.txt

echo [OK] MinGW專用CMake配置已創建 >> "%BUILD_LOG%"

echo %BLUE%步驟 6/10: CMake配置%RESET%
echo ----------------------------------------

echo 配置CMake項目 (MinGW Makefiles)...
echo [INFO] 開始CMake配置 (MinGW) >> "%BUILD_LOG%"

cmake -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="%QT6_ROOT%" ^
    -DQt6_DIR="%Qt6_DIR%" ^
    -DCMAKE_CXX_STANDARD=17 ^
    .. 2>&1 | tee cmake_config.log

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
)

echo %BLUE%步驟 7/10: 編譯項目%RESET%
echo ----------------------------------------

echo 開始編譯 (使用所有CPU核心: %NUMBER_OF_PROCESSORS%)...
echo [INFO] 開始編譯，使用 %NUMBER_OF_PROCESSORS% 個核心 >> "%BUILD_LOG%"

mingw32-make -j%NUMBER_OF_PROCESSORS% 2>&1 | tee build.log

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

echo %BLUE%步驟 8/10: 檢查編譯產物%RESET%
echo ----------------------------------------

if exist "RANOnline_AI_Ultimate_Portable.exe" (
    echo %GREEN%✅ 找到可執行文件: RANOnline_AI_Ultimate_Portable.exe%RESET%
    echo [OK] 找到可執行文件 >> "%BUILD_LOG%"
    
    REM 顯示文件大小
    for %%f in (RANOnline_AI_Ultimate_Portable.exe) do (
        set "size=%%~zf"
        set /a "sizeMB=!size!/1024/1024"
        echo 可執行文件大小: !sizeMB! MB
        echo [INFO] 可執行文件大小: !sizeMB! MB >> "%BUILD_LOG%"
    )
) else (
    echo %RED%❌ 找不到編譯生成的可執行文件%RESET%
    echo [ERROR] 可執行文件未找到 >> "%BUILD_LOG%"
    dir *.exe >> "%BUILD_LOG%"
    cd ..
    pause
    exit /b 1
)

echo %BLUE%步驟 9/10: 完整依賴部署%RESET%
echo ----------------------------------------

REM 創建部署目錄
mkdir "bundle_ultimate" 2>nul

REM 複製可執行文件
echo 複製可執行文件...
copy "RANOnline_AI_Ultimate_Portable.exe" "bundle_ultimate\" >nul
echo [OK] 可執行文件已複製 >> "%BUILD_LOG%"

REM 嘗試使用windeployqt
windeployqt --version >nul 2>&1
if not errorlevel 1 (
    echo 使用windeployqt進行部署...
    windeployqt --release --force --compiler-runtime ^
        --no-translations --no-system-d3d-compiler ^
        "bundle_ultimate\RANOnline_AI_Ultimate_Portable.exe" 2>&1 | tee deploy.log
        
    if not errorlevel 1 (
        echo %GREEN%✅ windeployqt部署成功%RESET%
        echo [OK] windeployqt部署成功 >> "%BUILD_LOG%"
        goto deploy_success
    )
)

REM 手動複製依賴
echo %YELLOW%執行手動依賴複製...%RESET%
echo [INFO] 執行手動依賴複製 >> "%BUILD_LOG%"

echo 複製Qt核心庫...
for %%dll in (Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6Network.dll) do (
    if exist "%QT6_ROOT%\bin\%%dll" (
        copy "%QT6_ROOT%\bin\%%dll" "bundle_ultimate\" >nul
        echo   複製: %%dll
        echo [OK] 複製: %%dll >> "%BUILD_LOG%"
    )
)

echo 複製MinGW運行時庫...
for %%dll in (libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll) do (
    if exist "%MINGW_ROOT%\bin\%%dll" (
        copy "%MINGW_ROOT%\bin\%%dll" "bundle_ultimate\" >nul
        echo   複製: %%dll
        echo [OK] 複製: %%dll >> "%BUILD_LOG%"
    ) else if exist "%QT6_ROOT%\bin\%%dll" (
        copy "%QT6_ROOT%\bin\%%dll" "bundle_ultimate\" >nul
        echo   複製: %%dll (from Qt)
        echo [OK] 複製: %%dll (from Qt) >> "%BUILD_LOG%"
    )
)

echo 複製平台插件...
mkdir "bundle_ultimate\platforms" 2>nul
if exist "%QT6_ROOT%\plugins\platforms\qwindows.dll" (
    copy "%QT6_ROOT%\plugins\platforms\qwindows.dll" "bundle_ultimate\platforms\" >nul
    echo   複製: platforms\qwindows.dll
    echo [OK] 複製: platforms\qwindows.dll >> "%BUILD_LOG%"
)

echo 複製樣式插件...
mkdir "bundle_ultimate\styles" 2>nul
if exist "%QT6_ROOT%\plugins\styles" (
    for %%f in ("%QT6_ROOT%\plugins\styles\*.dll") do (
        copy "%%f" "bundle_ultimate\styles\" >nul 2>&1
        echo [OK] 複製樣式插件: %%~nxf >> "%BUILD_LOG%"
    )
)

:deploy_success

echo %BLUE%步驟 10/10: 創建便攜包和腳本%RESET%
echo ----------------------------------------

REM 創建一鍵啟動腳本
echo 創建一鍵啟動腳本...
(
echo @echo off
echo title Jy技術團隊 線上AI - 極簡VM穩定版 v4.0.2
echo echo.
echo echo ========================================
echo echo  Jy技術團隊 線上AI - 極簡VM穩定版
echo echo  正在啟動AI助手...
echo echo ========================================
echo echo.
echo if not exist "RANOnline_AI_Ultimate_Portable.exe" ^(
echo     echo ❌ 錯誤: 找不到主程序文件！
echo     echo 請確保在正確的目錄中運行此腳本
echo     pause
echo     exit /b 1
echo ^)
echo echo 檢查依賴文件...
echo for %%%%f in ^(Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll^) do ^(
echo     if not exist "%%%%f" ^(
echo         echo ❌ 警告: 缺少 %%%%f
echo     ^) else ^(
echo         echo ✅ 找到 %%%%f
echo     ^)
echo ^)
echo echo.
echo echo 啟動程序...
echo start "" "RANOnline_AI_Ultimate_Portable.exe"
echo if errorlevel 1 ^(
echo     echo ❌ 啟動失敗！請檢查錯誤信息...
echo     pause
echo ^) else ^(
echo     echo ✅ 程序已啟動
echo     timeout /t 2 /nobreak ^>nul
echo ^)
) > "bundle_ultimate\一鍵啟動.bat"

REM 創建一鍵關閉腳本
echo 創建一鍵關閉腳本...
(
echo @echo off
echo title 關閉 Jy技術團隊 線上AI
echo echo 正在關閉Jy技術團隊線上AI...
echo taskkill /f /im "RANOnline_AI_Ultimate_Portable.exe" 2^>nul
echo if errorlevel 1 ^(
echo     echo 程序未運行或已關閉
echo ^) else ^(
echo     echo 程序已成功關閉
echo ^)
echo timeout /t 2 /nobreak ^>nul
) > "bundle_ultimate\一鍵關閉.bat"

REM 創建依賴檢查腳本
echo 創建依賴檢查腳本...
(
echo @echo off
echo title 依賴檢查 - Jy技術團隊 線上AI
echo echo 正在檢查程序依賴...
echo echo.
echo echo 主程序:
echo if exist "RANOnline_AI_Ultimate_Portable.exe" ^(
echo     echo ✅ RANOnline_AI_Ultimate_Portable.exe - 找到
echo ^) else ^(
echo     echo ❌ RANOnline_AI_Ultimate_Portable.exe - 缺失
echo ^)
echo echo.
echo echo Qt庫文件:
echo for %%%%f in ^(Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6Network.dll^) do ^(
echo     if exist "%%%%f" ^(
echo         echo ✅ %%%%f - 找到
echo     ^) else ^(
echo         echo ❌ %%%%f - 缺失
echo     ^)
echo ^)
echo echo.
echo echo MinGW運行時:
echo for %%%%f in ^(libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll^) do ^(
echo     if exist "%%%%f" ^(
echo         echo ✅ %%%%f - 找到
echo     ^) else ^(
echo         echo ❌ %%%%f - 缺失
echo     ^)
echo ^)
echo echo.
echo echo 平台插件:
echo if exist "platforms\qwindows.dll" ^(
echo     echo ✅ platforms\qwindows.dll - 找到
echo ^) else ^(
echo     echo ❌ platforms\qwindows.dll - 缺失
echo ^)
echo echo.
echo pause
) > "bundle_ultimate\依賴檢查.bat"

REM 創建詳細README文件
echo 創建README說明文件...
(
echo Jy技術團隊 線上AI - 極簡VM穩定便攜版 v4.0.2
echo ================================================================
echo.
echo 🎯 這是專為VM和雲桌面環境設計的100%%便攜化AI助手
echo 🔧 使用MinGW編譯，完全無MSVC依賴
echo.
echo 🚀 快速使用指南:
echo ================================
echo 1. 雙擊「一鍵啟動.bat」啟動AI助手
echo 2. 或直接運行「RANOnline_AI_Ultimate_Portable.exe」
echo 3. 使用完畢後雙擊「一鍵關閉.bat」關閉程序
echo 4. 如有問題，運行「依賴檢查.bat」檢查文件完整性
echo.
echo ✨ 核心特性:
echo ================================
echo • 100%%便攜化 - 無需安裝任何依賴
echo • MinGW編譯 - 完全無MSVC運行時依賴
echo • VM/RDP/雲桌面完全兼容
echo • 強制軟體渲染 - 杜絕GPU相關錯誤
echo • 支持Ollama本地AI模型集成
echo • 智能環境檢測和自適應優化
echo • 系統原生UI設計，極致穩定
echo.
echo 🔧 故障排除:
echo ================================
echo • 如程序無法啟動，請運行「依賴檢查.bat」
echo • 確保是Windows 10/11系統
echo • 如顯示異常，程序會自動啟用VM安全模式
echo • 如需Ollama支持，請先安裝: https://ollama.ai
echo.
echo 📋 系統需求:
echo ================================
echo • Windows 10 ^(1903+^) 或 Windows 11
echo • 最少 4GB 記憶體
echo • 約 100MB 磁碟空間
echo • 無需額外安裝任何軟體或運行時
echo.
echo 🎮 使用說明:
echo ================================
echo 1. 在對話框中輸入訊息與AI交流
echo 2. 可以問時間、系統資訊、或進行一般對話
echo 3. 支持指令: 時間、系統、幫助、ollama、vm
echo 4. 程序會自動檢測環境並啟用最佳相容模式
echo.
echo 構建時間: %BUILD_TIME%
echo 編譯器: MinGW-w64 ^(Qt自帶^)
echo 開發團隊: Jy技術團隊
echo 技術支援: jytech@example.com
) > "bundle_ultimate\README.txt"

REM 顯示部署文件清單
echo %CYAN%📁 部署文件清單:%RESET%
echo ----------------------------------------
dir "bundle_ultimate" /B
echo.
dir "bundle_ultimate\platforms" 2>nul | findstr ".dll" >nul && echo 平台插件: 已部署
dir "bundle_ultimate\styles" 2>nul | findstr ".dll" >nul && echo 樣式插件: 已部署

REM 計算總大小
for /f %%i in ('dir "bundle_ultimate" /s /-c ^| find "個檔案"') do (
    echo 檔案數量: %%i
    echo [INFO] 檔案數量: %%i >> "%BUILD_LOG%"
)

REM 創建ZIP整合包
echo.
echo %CYAN%📦 創建ZIP整合包...%RESET%
cd "bundle_ultimate"

powershell -Command "Get-Command Compress-Archive" >nul 2>&1
if not errorlevel 1 (
    echo 使用PowerShell創建ZIP包...
    powershell -Command "Compress-Archive -Path '.\*' -DestinationPath '..\RANOnline_AI_Ultimate_Portable_MinGW_v4.0.2.zip' -Force"
    if not errorlevel 1 (
        echo %GREEN%✅ ZIP整合包已創建: RANOnline_AI_Ultimate_Portable_MinGW_v4.0.2.zip%RESET%
        echo [OK] ZIP包已創建 >> "%BUILD_LOG%"
    ) else (
        echo %YELLOW%⚠️  ZIP創建可能有問題%RESET%
        echo [WARNING] ZIP創建問題 >> "%BUILD_LOG%"
    )
) else (
    echo %YELLOW%⚠️  PowerShell壓縮不可用，請手動打包bundle_ultimate資料夾%RESET%
    echo [WARNING] PowerShell壓縮不可用 >> "%BUILD_LOG%"
)

cd ..
cd ..

REM 複製構建日誌
copy "%BUILD_LOG%" "build_ultimate_portable\bundle_ultimate\構建日誌.txt" >nul

echo.
echo ========================================================================
echo %GREEN%🎉 MinGW構建完成！%RESET%
echo ========================================================================
echo.
echo %CYAN%📁 輸出位置:%RESET% build_ultimate_portable\bundle_ultimate\
echo %CYAN%📱 主程序:%RESET% RANOnline_AI_Ultimate_Portable.exe
echo %CYAN%🚀 啟動器:%RESET% 一鍵啟動.bat
echo %CYAN%🛑 關閉器:%RESET% 一鍵關閉.bat
echo %CYAN%🔍 依賴檢查:%RESET% 依賴檢查.bat
echo %CYAN%📖 說明文檔:%RESET% README.txt
echo %CYAN%📝 構建日誌:%RESET% 構建日誌.txt
echo %CYAN%📦 ZIP整合包:%RESET% RANOnline_AI_Ultimate_Portable_MinGW_v4.0.2.zip
echo.
echo %BLUE%🎯 使用說明:%RESET%
echo 1. 將 bundle_ultimate 整個資料夾複製到目標機器
echo 2. 雙擊「一鍵啟動.bat」開始使用
echo 3. 使用完畢後雙擊「一鍵關閉.bat」關閉程序
echo 4. 完全便攜化，無需安裝任何額外軟體
echo 5. 如有問題，運行「依賴檢查.bat」診斷
echo.
echo %GREEN%✅ MinGW特性已啟用:%RESET%
echo • 無MSVC運行時依賴
echo • 強制軟體渲染
echo • 禁用硬體加速
echo • 智能環境檢測
echo • Ollama模型支持
echo • 100%%便攜化部署
echo.

echo [FINAL] MinGW構建完成於 %date% %time% >> "%BUILD_LOG%"
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
