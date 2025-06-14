# Jy技術團隊 線上AI - 極簡穩定VM兼容版 v4.0.0

## 🎯 項目概述

這是專為VM環境、RDP遠端桌面和雲桌面平台設計的**極簡穩定AI助手**。經過完全重構，確保在任何Windows環境下都能100%穩定運行。

## ✨ 核心特性

### 🛡️ 極致穩定性
- **100% VM兼容** - 徹底解決Qt6在虛擬機中的0xc0000005錯誤
- **強制軟體渲染** - 完全禁用硬體加速，杜絕GPU相關問題
- **零依賴安裝** - 無需預裝任何軟體，開箱即用
- **智能環境檢測** - 自動識別VM/RDP/雲桌面環境並優化

### 🤖 AI功能
- **內建智能助手** - 本地AI回應系統，無需網路連接
- **Ollama集成** - 自動檢測並支持本地Ollama模型
- **多模型支持** - 可在內建AI和Ollama模型間切換
- **對話記憶** - 維護會話歷史和上下文

### 🎨 用戶界面
- **系統原生UI** - 使用Windows原生控件，確保最佳兼容性
- **無邊框設計** - 清晰的字體顯示，優秀的視覺體驗
- **響應式布局** - 適配各種屏幕分辨率
- **狀態指示** - 實時顯示系統狀態和環境資訊

## 🚀 快速開始

### 編譯要求
- **CMake** 3.16 或更高版本
- **Qt6** 6.2.0 或更高版本
- **Visual Studio** 2017 或更高版本 (Windows)

### 一鍵構建
```batch
# 運行自動化構建腳本
build_ultimate_portable.bat
```

### 手動構建
```batch
# 1. 創建構建目錄
mkdir build_ultimate_portable
cd build_ultimate_portable

# 2. 配置CMake
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release

# 3. 編譯
cmake --build . --config Release

# 4. 部署
cmake --build . --target deploy
```

## 📁 項目結構

```
AI_LLM_Integration/
├── main_ultimate_portable.cpp          # 主程序源碼
├── CMakeLists_Ultimate_Portable.txt    # CMake配置
├── build_ultimate_portable.bat         # 自動化構建腳本
├── README_Ultimate.md                  # 本說明文檔
└── build_ultimate_portable/            # 構建輸出目錄
    └── bundle_ultimate/                 # 便攜版完整包
        ├── RANOnline_AI_Ultimate_Portable.exe
        ├── 啟動AI助手.bat
        ├── README.txt
        └── [Qt DLL files]
```

## 🔧 技術架構

### VM兼容性實現
```cpp
// 強制軟體渲染
QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL, true);
QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling, true);

// 環境變數設置
qputenv("QT_OPENGL", "software");
qputenv("QT_QUICK_BACKEND", "software");
```

### 環境檢測系統
- **VM檢測** - 檢查系統製造商、服務、進程
- **RDP檢測** - 使用Windows API檢測遠端會話
- **雲桌面檢測** - 識別AWS、Azure、GCP等環境
- **Ollama檢測** - 自動掃描可用的本地模型

### AI回應引擎
- **本地AI助手** - 內建智能回應規則
- **Ollama集成** - 支持調用本地大語言模型
- **模型切換** - 運行時動態切換AI模型
- **錯誤恢復** - 完整的異常處理機制

## 🛠️ 使用說明

### 基本操作
1. **啟動程序** - 雙擊 `啟動AI助手.bat` 或直接運行exe文件
2. **選擇模型** - 在界面上方選擇AI模型（內建或Ollama）
3. **開始對話** - 在輸入框中輸入訊息，按Enter或點擊發送
4. **查看狀態** - 底部狀態欄顯示運行資訊

### 進階功能
- **系統資訊** - 選單 → 系統 → 系統資訊
- **清空對話** - 工具 → 清空對話
- **重掃模型** - 工具 → 重新掃描Ollama模型（如有）
- **環境檢測** - 程序會自動檢測並顯示運行環境

### 指令參考
在對話中可以使用以下指令：
- `時間` - 查看當前時間
- `系統` - 查看系統資訊
- `幫助` - 顯示可用指令
- `ollama` - 檢查Ollama狀態
- `vm` - 查看VM相關資訊

## 🔍 環境兼容性

### 測試通過的環境
- ✅ **VirtualBox** - 各版本虛擬機
- ✅ **VMware** - Workstation/vSphere
- ✅ **Hyper-V** - Windows內建虛擬化
- ✅ **AWS EC2** - Windows實例
- ✅ **Azure VM** - Windows虛擬機
- ✅ **RDP** - 遠端桌面連接
- ✅ **本機環境** - 普通Windows電腦

### 支援的系統
- **Windows 10** (1903或更高版本)
- **Windows 11** (所有版本)
- **Windows Server 2019/2022**

## 🐛 故障排除

### 常見問題

#### 1. 程序無法啟動
**解決方案：**
- 確保所有DLL文件在同一目錄
- 檢查是否安裝了Visual C++ Redistributable
- 嘗試以管理員身份運行

#### 2. 在VM中顯示異常
**解決方案：**
- 程序已自動啟用安全模式
- 檢查VM的3D加速是否已禁用
- 確保VM有足夠的記憶體分配

#### 3. Ollama模型未檢測到
**解決方案：**
- 確保Ollama已正確安裝
- 檢查PATH環境變數
- 手動運行 `ollama list` 測試
- 使用工具選單重新掃描

#### 4. 字體顯示不清晰
**解決方案：**
- 程序使用系統原生字體
- 檢查Windows顯示縮放設置
- 在高DPI屏幕上可能需要調整系統DPI

### 日誌和除錯
程序運行時會在控制台輸出詳細資訊：
```batch
# 以控制台模式運行查看詳細日誌
RANOnline_AI_Ultimate_Portable.exe
```

## 📊 性能指標

### 資源使用
- **記憶體** - 約50-80MB
- **磁盤空間** - 完整包約60MB
- **CPU使用** - 低負載 (<5%)
- **網路** - 僅限Ollama調用（可選）

### 啟動時間
- **本機環境** - 約2-3秒
- **VM環境** - 約3-5秒
- **RDP環境** - 約4-6秒

## 🔄 版本歷史

### v4.0.0 (當前版本)
- ✨ 全新極簡穩定架構
- 🛡️ 完善的VM兼容性
- 🤖 Ollama模型集成
- 🎨 系統原生UI設計
- 📦 100%便攜化部署

### v3.2.0 (前版本)
- 🔧 VM環境檢測改進
- 💬 AI對話功能增強
- 🐛 穩定性修復

### v3.1.0 (VM簡化版)
- 🏗️ 移除FramelessWindow依賴
- 🖥️ 強制軟體渲染
- 🔍 VM環境自動檢測

## 👥 開發團隊

**Jy技術團隊** - 專注於跨平台AI應用開發
- 🌐 **官網:** jytech.com
- 📧 **郵箱:** jytech@example.com
- 💬 **技術支援:** 通過issue或郵件聯繫

## 📄 授權協議

本項目採用MIT授權協議，詳見LICENSE文件。

## 🙏 致謝

感謝以下開源項目的支持：
- **Qt6** - 跨平台GUI框架
- **CMake** - 構建系統
- **Ollama** - 本地AI模型支持

---

**如有任何問題或建議，歡迎通過issue或郵件聯繫我們！**
