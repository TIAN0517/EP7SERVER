
# RANOnline LLM Integration System
<div align="center">

![RANOnline Logo](Resources/icons/ran_logo.png)

**🤖 RANOnline EP7 AI Simulation System - LLM Integration Module**

*A cutting-edge Qt6 frameless tech-style GUI application for managing multiple Ollama LLM models with game integration*

[![CMake](https://img.shields.io/badge/CMake-3.20+-blue.svg)](https://cmake.org/)
[![Qt6](https://img.shields.io/badge/Qt-6.0+-green.svg)](https://www.qt.io/)
[![C++20](https://img.shields.io/badge/C++-20-red.svg)](https://en.cppreference.com/w/cpp/20)
[![License](https://img.shields.io/badge/License-Proprietary-yellow.svg)](LICENSE)

</div>

## 🎯 概述

RANOnline LLM Integration 是一個專為 RANOnline EP7 遊戲設計的高端 AI 語言模型集成系統。本系統提供了無邊框科技感界面，支援多 Ollama 模型並發處理、負載均衡、WebSocket 遊戲事件同步等企業級功能。

### ✨ 核心特性

- **🖼️ 無邊框科技感界面**: 自定義繪製的現代化 UI，支援 5 種主題和學院配色
- **🤖 多 LLM 模型支援**: 完整支援 Ollama 的 llama3、phi3、mistral、gemma、qwen2、deepseek-coder
- **⚖️ 智能負載均衡**: 輪詢、最少連接、加權分配等多種負載均衡策略
- **🌊 流式 API 支援**: 實時流式響應接收，支援大型文本生成
- **🔄 自動重試機制**: 指數退避重試，熔斷器保護，確保服務可靠性
- **🎮 WebSocket 遊戲同步**: 與遊戲伺服器實時通信，自動處理技能生成請求
- **📊 批量請求處理**: 多線程批量處理，非阻塞 UI 操作
- **🎨 動態主題系統**: 5 種科技感主題，學院專屬色彩，動畫特效
- **📝 豐富模板系統**: 預設遊戲內容生成模板（技能、NPC、任務、物品、事件）
- **📈 性能監控面板**: 實時性能指標、連接狀態、資源使用監控
- **🌍 多語言支援**: 繁體中文、簡體中文、英文、日文、韓文

## 🏗️ 系統架構

```
AI_LLM_Integration/
├── 📁 Core Components
│   ├── OllamaLLMManager     # LLM 管理核心
│   ├── LoadBalancer         # 負載均衡器
│   ├── PromptTemplateManager # 模板管理
│   └── GameEventSyncer      # 遊戲事件同步
├── 📁 GUI Framework
│   ├── FramelessWindow      # 無邊框窗口基類
│   ├── CyberProgressBar     # 科技感進度條
│   ├── LLMRequestItem       # 請求項目組件
│   ├── BatchRequestPanel    # 批量請求面板
│   └── StatsDashboard       # 統計儀表板
├── 📁 Configuration
│   ├── llm_servers.json     # 伺服器配置
│   ├── prompt_templates.json # 模板配置
│   ├── academy_themes.json  # 主題配置
│   └── default_settings.json # 預設設定
└── 📁 Resources
    ├── icons/               # 圖標資源
    ├── themes/              # 主題背景
    ├── fonts/               # 字體資源
    └── translations/        # 翻譯文件
```

## 🚀 快速開始

### 📋 系統需求

- **作業系統**: Windows 10/11, Linux (Ubuntu 20.04+), macOS 11+
- **編譯器**: GCC 10+, Clang 12+, MSVC 2019+
- **CMake**: 3.20 或更高版本
- **Qt6**: 6.2 或更高版本
- **Ollama**: 最新版本（支援 API 服務）

### 🔧 安裝依賴

#### Windows
```powershell
# 安裝 Qt6
winget install Qt.Qt

# 安裝 CMake
winget install Kitware.CMake

# 安裝 Ollama
winget install Ollama.Ollama
```

#### Linux (Ubuntu/Debian)
```bash
# 安裝開發工具
sudo apt update
sudo apt install build-essential cmake git

# 安裝 Qt6
sudo apt install qt6-base-dev qt6-websockets-dev qt6-tools-dev

# 安裝 Ollama
curl -fsSL https://ollama.ai/install.sh | sh
```

### 📦 建置和安裝

#### 自動建置（推薦）

**Windows:**
```cmd
cd AI_LLM_Integration
Scripts\build_and_install.bat Release "%USERPROFILE%\RANOnline_LLM" ON
```

**Linux/macOS:**
```bash
cd AI_LLM_Integration
chmod +x Scripts/build_and_install.sh
./Scripts/build_and_install.sh Release ~/RANOnline_LLM ON
```

#### 手動建置

```bash
# 創建建置目錄
mkdir build && cd build

# 配置 CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_INSTALL_PREFIX=~/RANOnline_LLM \
         -DBUILD_LLM_TESTS=ON

# 編譯
cmake --build . --parallel

# 運行測試
ctest --output-on-failure

# 安裝
cmake --install .
```

### 🎮 Ollama 模型準備

```bash
# 下載所需的 LLM 模型
ollama pull llama3:latest
ollama pull phi3:latest  
ollama pull mistral:latest
ollama pull gemma:latest
ollama pull qwen2:latest
ollama pull deepseek-coder:latest

# 啟動 Ollama 服務
ollama serve
```

## 📖 使用指南

### 🖥️ 界面概覽

應用程式採用無邊框設計，包含以下主要區域：

1. **🎛️ 控制面板**: 主題切換、系統狀態、連接管理
2. **📋 請求管理**: 創建、監控、取消 AI 請求
3. **📊 批量處理**: 大批量請求的並行處理
4. **🔧 伺服器管理**: Ollama 伺服器配置和監控
5. **📝 模板編輯**: 自定義提示模板編輯
6. **⚙️ 設定面板**: 應用程式參數配置

### 🎨 主題系統

系統提供 5 種科技感主題：

- **🔵 賽博藍 (Cyber Blue)**: 經典賽博朋克風格
- **🟢 霓虹綠 (Neon Green)**: 駭客帝國風格 
- **🟣 紫霧幻境 (Purple Haze)**: 神秘科幻主題
- **🔴 紅色警戒 (Red Alert)**: 高危險警戒主題
- **🟡 黃金榮耀 (Golden Glow)**: 豪華金色主題

每個主題都有對應的學院配色（戰士/法師/弓手/盜賊/牧師）。

### 🤖 LLM 請求流程

1. **選擇模型**: 從可用的 Ollama 模型中選擇
2. **選擇學院**: 設定學院類型以獲得專屬配色
3. **輸入提示**: 直接輸入或使用預設模板
4. **發送請求**: 系統自動選擇最佳伺服器
5. **監控進度**: 實時查看處理進度和流式輸出
6. **查看結果**: 完整的 AI 響應和統計資訊

### 🔄 批量處理

批量處理功能允許：

- **多提示並行**: 同時處理多個不同的提示
- **模型分配**: 自動將請求分配到不同模型
- **進度追蹤**: 每個請求獨立的進度監控
- **結果匯出**: 批量結果的 JSON/CSV 匯出

### 🎮 遊戲整合

WebSocket 遊戲事件同步功能：

- **即時通信**: 與遊戲伺服器的雙向通信
- **事件處理**: 自動處理技能生成、NPC 對話等請求
- **響應轉發**: AI 生成結果自動發送回遊戲客戶端
- **狀態同步**: 遊戲狀態和 AI 處理狀態同步

## 🔧 配置說明

### 📋 伺服器配置 (llm_servers.json)

```json
{
  "servers": [
    {
      "id": "ollama_primary",
      "name": "Primary Ollama Server",
      "host": "localhost",
      "port": 11434,
      "enabled": true,
      "max_connections": 10,
      "models": ["llama3:latest", "phi3:latest"],
      "load_balancer": {
        "strategy": "round_robin",
        "weight": 100
      }
    }
  ]
}
```

### 📝 模板配置 (prompt_templates.json)

```json
{
  "templates": {
    "skill_generation": {
      "name": "技能生成模板",
      "template": "為 {academy} 系的 {level} 級角色創建新技能...",
      "preferred_models": ["deepseek-coder", "llama3"],
      "parameters": {
        "academy": {"type": "select", "options": ["戰士", "法師"]},
        "level": {"type": "number", "min": 1, "max": 999}
      }
    }
  }
}
```

## 🧪 測試

### 🔬 單元測試

```bash
# 建置時啟用測試
cmake .. -DBUILD_LLM_TESTS=ON

# 運行所有測試
ctest --verbose

# 運行特定測試
./test_ai_llm_integration --test LLMManagerTest
```

### 🎯 測試覆蓋

- **OllamaLLMManager**: 連接管理、負載均衡、請求處理
- **FramelessWindow**: 窗口拖拽、大小調整、主題切換
- **GameEventSyncer**: WebSocket 通信、事件處理
- **PromptTemplateManager**: 模板加載、參數替換
- **LoadBalancer**: 各種負載均衡策略

## 📊 性能指標

系統提供詳細的性能監控：

- **請求統計**: 總請求數、成功率、失敗率
- **響應時間**: 平均響應時間、最大/最小響應時間
- **資源使用**: CPU、記憶體、網路使用情況
- **連接狀態**: 伺服器連接狀態、健康檢查結果
- **錯誤統計**: 錯誤類型分析、重試統計

## 🔍 故障排除

### 常見問題

**Q: Ollama 服務無法連接**
```bash
# 檢查 Ollama 服務狀態
curl http://localhost:11434/api/tags

# 重啟 Ollama 服務
ollama serve
```

**Q: Qt6 找不到**
```bash
# 設定 Qt6 環境變數
export Qt6_DIR=/path/to/qt6/lib/cmake/Qt6
```

**Q: 中文字體顯示異常**
```bash
# 安裝中文字體包
sudo apt install fonts-noto-cjk
```

## 🤝 開發團隊

**Jy技術團隊** - *RANOnline EP7 AI Simulation System*

- **架構設計**: AI 系統架構和模組化設計
- **前端開發**: Qt6 無邊框界面和科技感特效  
- **後端整合**: Ollama LLM 管理和負載均衡
- **遊戲整合**: WebSocket 通信和事件同步
- **DevOps**: 自動化建置和部署系統

## 📄 授權協議

本專案採用專有授權協議。未經許可，禁止商業使用或再分發。

---

<div align="center">

**🎮 為 RANOnline EP7 提供最先進的 AI 語言模型整合解決方案**

*讓遊戲世界更加智能和生動*

</div>
