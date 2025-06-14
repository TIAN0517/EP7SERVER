@echo off
chcp 65001 >nul
title 構建完整版 Ultimate Portable - Jy技術團隊

echo ========================================================================
echo 構建 RANOnline AI LLM Integration - Ultimate Portable Edition
echo 版本: 4.0.0 Ultimate Portable Complete
echo 開發團隊: Jy技術團隊 線上AI
echo ========================================================================
echo.

color 0A

REM 設置顏色變數
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "MAGENTA=[95m"
set "CYAN=[96m"
set "WHITE=[97m"
set "RESET=[0m"

echo %GREEN%   🎯 極簡穩定VM兼容版特性:%RESET%
echo %GREEN%   ✅ 100%% 便攜化 - 零依賴安裝%RESET%
echo %GREEN%   ✅ VM/RDP/雲桌面 100%% 兼容%RESET%
echo %GREEN%   ✅ 強制軟體渲染 - 杜絕GPU相關錯誤%RESET%
echo %GREEN%   ✅ 系統原生UI - 無自定義樣式風險%RESET%
echo %GREEN%   ✅ 智能環境檢測和自適應%RESET%
echo %GREEN%   ✅ Ollama模型集成支持%RESET%
echo.

echo %CYAN%   📋 檢查必要文件...%RESET%
if not exist "main_ultimate_portable.cpp" (
    echo %RED%   ❌ 錯誤: 找不到 main_ultimate_portable.cpp%RESET%
    pause
    exit /b 1
)
echo %GREEN%   ✅ 找到完整版源文件: main_ultimate_portable.cpp%RESET%

REM 檢查Qt6路徑
echo %CYAN%   📋 檢測Qt6環境...%RESET%
set "QT6_FOUND=false"
set "QT6_PATHS=C:\Qt\6.9.1\mingw_64;C:\Qt\6.8.0\mingw_64;C:\Qt\6.7.0\mingw_64;C:\Qt\6.6.0\mingw_64;C:\Qt\6.5.0\mingw_64"

for %%p in (%QT6_PATHS%) do (
    if exist "%%p\bin\qmake.exe" (
        set "QT6_PATH=%%p"
        set "QT6_FOUND=true"
        echo %GREEN%   ✅ 找到Qt6安裝路徑: %%p%RESET%
        goto :qt6_found
    )
)

:qt6_found
if "%QT6_FOUND%"=="false" (
    echo %RED%   ❌ 錯誤: 未找到Qt6安裝%RESET%
    echo %YELLOW%   💡 請安裝Qt6.5或更高版本%RESET%
    pause
    exit /b 1
)

REM 檢查MinGW路徑
echo %CYAN%   📋 檢測MinGW環境...%RESET%
set "MINGW_FOUND=false"
set "MINGW_PATHS=C:\Qt\Tools\mingw1310_64;C:\Qt\Tools\mingw1120_64;C:\Qt\Tools\mingw1110_64;C:\Qt\Tools\mingw1100_64"

for %%p in (%MINGW_PATHS%) do (
    if exist "%%p\bin\gcc.exe" (
        set "MINGW_PATH=%%p"
        set "MINGW_FOUND=true"
        echo %GREEN%   ✅ 找到MinGW路徑: %%p%RESET%
        goto :mingw_found
    )
)

:mingw_found
if "%MINGW_FOUND%"=="false" (
    echo %RED%   ❌ 錯誤: 未找到MinGW安裝%RESET%
    echo %YELLOW%   💡 請安裝Qt Tools中的MinGW%RESET%
    pause
    exit /b 1
)

REM 設置環境路徑
set "PATH=%QT6_PATH%\bin;%MINGW_PATH%\bin;%PATH%"
set "CMAKE_PREFIX_PATH=%QT6_PATH%"

echo.
echo %GREEN%   ✅ 環境配置完成%RESET%
echo %WHITE%     Qt6路徑: %QT6_PATH%%RESET%
echo %WHITE%     MinGW路徑: %MINGW_PATH%%RESET%
echo.

REM 創建構建目錄
echo %CYAN%   📁 準備構建目錄...%RESET%
if exist "build_ultimate_complete" rmdir /s /q build_ultimate_complete
mkdir build_ultimate_complete
cd build_ultimate_complete

REM 創建簡化的CMakeLists.txt用於Ultimate Portable版本
echo %CYAN%   📝 創建Ultimate Portable CMake配置...%RESET%
(
echo cmake_minimum_required^(VERSION 3.22^)
echo.
echo project^(ai_llm_integration_ultimate_portable 
echo     VERSION 4.0.0 
echo     DESCRIPTION "RANOnline AI LLM Integration - Ultimate Portable Edition"
echo     LANGUAGES CXX^)
echo.
echo set^(CMAKE_CXX_STANDARD 17^)
echo set^(CMAKE_CXX_STANDARD_REQUIRED ON^)
echo set^(CMAKE_CXX_EXTENSIONS OFF^)
echo.
echo if^(NOT CMAKE_BUILD_TYPE^)
echo     set^(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE^)
echo endif^(^)
echo.
echo if^(WIN32^)
echo     set^(CMAKE_WIN32_EXECUTABLE TRUE^)
echo     set^(CMAKE_SYSTEM_VERSION 10.0^)
echo endif^(^)
echo.
echo # Qt6組件
echo find_package^(Qt6 REQUIRED COMPONENTS 
echo     Core 
echo     Widgets 
echo     Gui
echo     Network
echo ^)
echo.
echo set^(CMAKE_AUTOMOC ON^)
echo set^(CMAKE_AUTORCC ON^)
echo set^(CMAKE_AUTOUIC ON^)
echo.
echo # 編譯器設定
echo if^(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "GNU"^)
echo     set^(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3"^)
echo     set^(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG"^)
echo     set^(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_OPENGL_ES_2"^)
echo     set^(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_OPENGL_SOFTWARE"^)
echo     set^(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVM_ULTIMATE_PORTABLE"^)
echo     set^(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++"^)
echo     set^(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++"^)
echo endif^(^)
echo.
echo # 資源文件
echo set^(RESOURCES ""^)
echo if^(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../Resources/icons.qrc"^)
echo     set^(RESOURCES ../Resources/icons.qrc^)
echo endif^(^)
echo.
echo # Windows資源文件
echo if^(WIN32^)
echo     set^(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/ultimate_portable_resources.rc"^)
echo     set^(ICO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../jy1.ico"^)
echo     
echo     if^(NOT EXISTS "${ICO_FILE}"^)
echo         set^(ICO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../Resources/jy1.ico"^)
echo     endif^(^)
echo     
echo     if^(NOT EXISTS "${ICO_FILE}"^)
echo         set^(ICO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../icogui/jy1.ico"^)
echo     endif^(^)
echo     
echo     if^(EXISTS "${ICO_FILE}"^)
echo         file^(WRITE "${RC_FILE}" 
echo "IDI_ICON1 ICON \\\"${ICO_FILE}\\\"\\n"
echo "VS_VERSION_INFO VERSIONINFO\\n"
echo "FILEVERSION 4,0,0,0\\n"
echo "PRODUCTVERSION 4,0,0,0\\n"
echo "FILEFLAGSMASK 0x17L\\n"
echo "FILEFLAGS 0x0L\\n"
echo "FILEOS 0x4L\\n"
echo "FILETYPE 0x1L\\n"
echo "FILESUBTYPE 0x0L\\n"
echo "BEGIN\\n"
echo "    BLOCK \\\"StringFileInfo\\\"\\n"
echo "    BEGIN\\n"
echo "        BLOCK \\\"040904b0\\\"\\n"
echo "        BEGIN\\n"
echo "            VALUE \\\"CompanyName\\\", \\\"Jy技術團隊\\\"\\n"
echo "            VALUE \\\"FileDescription\\\", \\\"RANOnline AI LLM Integration - Ultimate Portable Edition\\\"\\n"
echo "            VALUE \\\"FileVersion\\\", \\\"4.0.0.0\\\"\\n"
echo "            VALUE \\\"InternalName\\\", \\\"ai_llm_integration_ultimate_portable\\\"\\n"
echo "            VALUE \\\"LegalCopyright\\\", \\\"Copyright ^(C^) 2025 Jy技術團隊\\\"\\n"
echo "            VALUE \\\"OriginalFilename\\\", \\\"RANOnline_AI_Ultimate_Portable.exe\\\"\\n"
echo "            VALUE \\\"ProductName\\\", \\\"RANOnline AI LLM Integration Ultimate Portable\\\"\\n"
echo "            VALUE \\\"ProductVersion\\\", \\\"4.0.0.0\\\"\\n"
echo "        END\\n"
echo "    END\\n"
echo "    BLOCK \\\"VarFileInfo\\\"\\n"
echo "    BEGIN\\n"
echo "        VALUE \\\"Translation\\\", 0x409, 1200\\n"
echo "    END\\n"
echo "END\\n"
echo ^)
echo         set^(RESOURCES ${RESOURCES} ${RC_FILE}^)
echo     endif^(^)
echo endif^(^)
echo.
echo # 建立可執行檔案
echo add_executable^(ai_llm_integration_ultimate_portable
echo     ../main_ultimate_portable.cpp
echo     ${RESOURCES}
echo ^)
echo.
echo target_link_libraries^(ai_llm_integration_ultimate_portable
echo     Qt6::Core
echo     Qt6::Widgets
echo     Qt6::Gui
echo     Qt6::Network
echo ^)
echo.
echo target_include_directories^(ai_llm_integration_ultimate_portable PRIVATE
echo     ${CMAKE_CURRENT_SOURCE_DIR}/..
echo     ${CMAKE_CURRENT_BINARY_DIR}
echo ^)
echo.
echo # VM版本特定編譯定義
echo target_compile_definitions^(ai_llm_integration_ultimate_portable PRIVATE
echo     QT_NO_OPENGL_ES_2
echo     QT_OPENGL_SOFTWARE
echo     VM_ULTIMATE_PORTABLE
echo     VM_COMPATIBLE_BUILD
echo     ULTIMATE_PORTABLE_EDITION
echo ^)
echo.
echo # 設置輸出名稱
echo set_target_properties^(ai_llm_integration_ultimate_portable PROPERTIES
echo     OUTPUT_NAME "RANOnline_AI_Ultimate_Portable"
echo     RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
echo ^)
) > CMakeLists.txt

echo %GREEN%   ✅ CMake配置文件已創建%RESET%

echo %CYAN%   🔧 正在配置CMake（Ultimate Portable版本）...%RESET%
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="%QT6_PATH%" ^
    -DQt6_DIR="%QT6_PATH%\lib\cmake\Qt6"

if %ERRORLEVEL% neq 0 (
    echo %RED%   ❌ CMake配置失敗%RESET%
    pause
    exit /b 1
)

echo %CYAN%   🔨 正在編譯Ultimate Portable版本...%RESET%
cmake --build . --config Release

if %ERRORLEVEL% neq 0 (
    echo %RED%   ❌ 編譯失敗%RESET%
    pause
    exit /b 1
)

echo.
echo %GREEN%   ✅ 編譯成功！%RESET%
echo %CYAN%   📦 準備Ultimate Portable部署...%RESET%

REM 創建部署目錄
if exist "deploy_ultimate_complete" rmdir /s /q deploy_ultimate_complete
mkdir deploy_ultimate_complete

REM 複製可執行文件
copy "RANOnline_AI_Ultimate_Portable.exe" "deploy_ultimate_complete\" >nul
cd deploy_ultimate_complete

echo %CYAN%   📦 部署Qt依賴...%RESET%
"%QT6_PATH%\bin\windeployqt.exe" --qmldir "%QT6_PATH%\qml" RANOnline_AI_Ultimate_Portable.exe

REM 複製額外的DLL
echo %CYAN%   📦 複製MinGW運行時庫...%RESET%
copy "%MINGW_PATH%\bin\libgcc_s_seh-1.dll" . >nul 2>&1
copy "%MINGW_PATH%\bin\libstdc++-6.dll" . >nul 2>&1
copy "%MINGW_PATH%\bin\libwinpthread-1.dll" . >nul 2>&1

REM 創建一鍵啟動腳本
echo %CYAN%   📝 創建一鍵啟動腳本...%RESET%
(
echo @echo off
echo chcp 65001 ^>nul
echo title Jy技術團隊 線上AI - Ultimate Portable 啟動器
echo.
echo echo ========================================================================
echo echo Jy技術團隊 線上AI - Ultimate Portable Edition v4.0.0
echo echo ========================================================================
echo echo.
echo echo 🎯 極簡穩定VM兼容版特性:
echo echo ✅ 100%% 便攜化 - 零依賴安裝
echo echo ✅ VM/RDP/雲桌面 100%% 兼容
echo echo ✅ 強制軟體渲染 - 杜絕GPU相關錯誤
echo echo ✅ 智能環境檢測和自適應
echo echo ✅ Ollama模型集成支持
echo echo.
echo echo 正在啟動AI助手...
echo echo.
echo.
echo REM 設置VM兼容環境變數
echo set QT_OPENGL=software
echo set QT_QUICK_BACKEND=software
echo set QT_ANGLE_PLATFORM=software
echo.
echo REM 啟動程序
echo start "" "RANOnline_AI_Ultimate_Portable.exe"
echo.
echo echo AI助手已啟動！
echo echo 如果程序無法啟動，請檢查系統兼容性。
echo pause
) > "一鍵啟動_Ultimate_Portable.bat"

REM 創建一鍵關閉腳本
(
echo @echo off
echo chcp 65001 ^>nul
echo title 關閉 Jy技術團隊 線上AI
echo.
echo echo 正在關閉 RANOnline AI Ultimate Portable...
echo taskkill /f /im "RANOnline_AI_Ultimate_Portable.exe" 2^>nul
echo echo.
echo echo AI助手已關閉。
echo timeout /t 2 /nobreak ^>nul
) > "一鍵關閉.bat"

REM 創建README
echo %CYAN%   📝 創建README文檔...%RESET%
(
echo # Jy技術團隊 線上AI - Ultimate Portable Edition v4.0.0
echo.
echo ## 🎯 極簡穩定VM兼容版特性
echo.
echo ✅ **100%% 便攜化** - 零依賴安裝，無需系統配置
echo ✅ **VM/RDP/雲桌面 100%% 兼容** - 專為虛擬環境優化
echo ✅ **強制軟體渲染** - 杜絕GPU相關錯誤和崩潰
echo ✅ **系統原生UI** - 無自定義樣式風險，極致穩定
echo ✅ **智能環境檢測** - 自動適應VM/RDP/雲桌面環境
echo ✅ **Ollama模型集成** - 支援本地大型語言模型
echo ✅ **完整錯誤處理** - 傻瓜式一鍵啟動
echo.
echo ## 🚀 快速啟動
echo.
echo **方法一：雙擊啟動**
echo ```
echo 雙擊 "一鍵啟動_Ultimate_Portable.bat"
echo ```
echo.
echo **方法二：PowerShell啟動**
echo ```powershell
echo # 切換到程式目錄
echo cd "程式所在目錄"
echo.
echo # 啟動程式（注意PowerShell語法）
echo .\RANOnline_AI_Ultimate_Portable.exe
echo ```
echo.
echo **方法三：CMD啟動**
echo ```cmd
echo # 切換到程式目錄
echo cd "程式所在目錄"
echo.
echo # 啟動程式
echo RANOnline_AI_Ultimate_Portable.exe
echo ```
echo.
echo ## 🔧 PowerShell 執行注意事項
echo.
echo 如果在 PowerShell 中直接輸入 `RANOnline_AI_Ultimate_Portable.exe` 出現錯誤，
echo 請使用以下正確語法：
echo.
echo ```powershell
echo # 正確語法 - 加上 .\ 前綴
echo .\RANOnline_AI_Ultimate_Portable.exe
echo.
echo # 或者使用完整路徑
echo "C:\完整路徑\RANOnline_AI_Ultimate_Portable.exe"
echo ```
echo.
echo **原因說明：**
echo - PowerShell 不會自動在當前目錄查找可執行文件
echo - 必須明確指定路徑（`.\` 表示當前目錄）
echo - 這是 PowerShell 的安全特性，不是程式錯誤
echo.
echo ## 🖥️ 系統需求
echo.
echo - **作業系統：** Windows 10/11 ^(64位^)
echo - **記憶體：** 最小 2GB RAM
echo - **硬碟空間：** 100MB 可用空間
echo - **網路：** 可選（Ollama模型需要）
echo.
echo ## 🎮 使用指南
echo.
echo 1. **啟動程式** - 雙擊一鍵啟動腳本或直接執行exe
echo 2. **選擇模型** - 程式會自動檢測可用的AI模型
echo 3. **開始對話** - 在輸入框中輸入訊息，按Enter發送
echo 4. **查看系統資訊** - 選單 → 系統 → 系統資訊
echo 5. **關閉程式** - 點擊關閉按鈕或雙擊一鍵關閉腳本
echo.
echo ## 🔍 智能環境檢測
echo.
echo 程式會自動檢測並適應以下環境：
echo.
echo - **虛擬機環境** ^(VirtualBox, VMware, Hyper-V等^)
echo - **遠端桌面** ^(RDP連線^)
echo - **雲桌面服務** ^(AWS WorkSpaces, Azure Virtual Desktop等^)
echo - **Ollama模型** ^(自動掃描並集成可用模型^)
echo.
echo ## 🤖 Ollama 集成
echo.
echo 如果系統已安裝 Ollama：
echo.
echo 1. 程式會自動檢測並列出可用模型
echo 2. 可在模型下拉選單中切換使用
echo 3. 支援與Ollama模型進行自然語言對話
echo.
echo **安裝 Ollama：**
echo ```bash
echo # 訪問官網下載
echo https://ollama.ai
echo.
echo # 安裝模型範例
echo ollama pull llama2
echo ollama pull codellama
echo ```
echo.
echo ## 🛠️ 故障排除
echo.
echo **問題：程式無法啟動**
echo - 確認Windows版本支援（Windows 10/11）
echo - 嘗試以管理員身份執行
echo - 檢查防毒軟體是否阻擋
echo.
echo **問題：PowerShell無法執行**
echo - 使用 `.\RANOnline_AI_Ultimate_Portable.exe` 語法
echo - 或改用 CMD 執行
echo - 或直接雙擊exe文件
echo.
echo **問題：AI回應異常**
echo - 檢查網路連線（如使用Ollama模型）
echo - 重新掃描Ollama模型
echo - 切換回內建AI模型測試
echo.
echo ## 📞 技術支援
echo.
echo - **開發團隊：** Jy技術團隊
echo - **技術支援：** jytech@example.com
echo - **版本資訊：** v4.0.0 Ultimate Portable Edition
echo - **建構日期：** %date% %time%
echo.
echo ## 📄 版權聲明
echo.
echo Copyright ^(C^) 2025 Jy技術團隊. All rights reserved.
echo.
echo 本軟體專為RANOnline遊戲AI系統設計，
echo 提供虛擬機和雲桌面環境的完美兼容性。
) > "README_繁體中文.md"

cd ..

echo.
echo %GREEN%========================================================================%RESET%
echo %GREEN%   🎉 Ultimate Portable Edition 構建成功完成！%RESET%
echo %GREEN%========================================================================%RESET%
echo.
echo %WHITE%   📁 部署位置: %CD%\deploy_ultimate_complete\%RESET%
echo %WHITE%   🚀 可執行文件: RANOnline_AI_Ultimate_Portable.exe%RESET%
echo %WHITE%   📝 啟動腳本: 一鍵啟動_Ultimate_Portable.bat%RESET%
echo %WHITE%   📖 說明文檔: README_繁體中文.md%RESET%
echo.
echo %CYAN%   💡 PowerShell執行提示:%RESET%
echo %YELLOW%      在PowerShell中請使用: .\RANOnline_AI_Ultimate_Portable.exe%RESET%
echo %YELLOW%      （注意要加 .\ 前綴）%RESET%
echo.
echo %CYAN%   🔧 測試運行程序...%RESET%
cd deploy_ultimate_complete
echo %WHITE%   正在啟動Ultimate Portable Edition...%RESET%
start "" "RANOnline_AI_Ultimate_Portable.exe"

echo.
echo %GREEN%   ✅ Ultimate Portable Edition程序已啟動！%RESET%
echo.
echo %CYAN%   如果程序正常顯示完整的AI界面（包含智能環境檢測、AI對話等功能），%RESET%
echo %CYAN%   則Ultimate Portable版本構建成功。%RESET%
echo.
echo %MAGENTA%   🎯 主要特性驗證清單:%RESET%
echo %WHITE%   □ 程序正常啟動（無錯誤訊息）%RESET%
echo %WHITE%   □ 環境檢測顯示正確（VM/RDP/本機）%RESET%
echo %WHITE%   □ AI對話功能正常%RESET%
echo %WHITE%   □ 模型選擇器工作正常%RESET%
echo %WHITE%   □ Ollama集成檢測（如有安裝）%RESET%
echo %WHITE%   □ 系統資訊顯示完整%RESET%
echo.
pause
