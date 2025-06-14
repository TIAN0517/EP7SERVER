# Jy技術團隊 線上AI - 完整自動化系統檢查與修復方案

## 📋 功能概述

這是一套完整的自動化系統檢查和修復解決方案，專為 Jy技術團隊的 RANOnline AI 助手應用程式設計，支援 SQL Server 整合、多模型切換和智能資源管理。

## 🎯 核心功能

### 1. 系統完整性檢查
- ✅ SQL Server 1433 端口狀態檢查
- ✅ ODBC 連線與 sa 帳號驗證 (localhost:1433, 帳號:sa, 密碼:123456)
- ✅ RanUser 資料庫和表結構完整性驗證
- ✅ AI_CONFIG.INI 配置檔案格式檢查 (8個必要區塊)
- ✅ WebSocket 服務配置驗證
- ✅ 系統依賴項目檢查 (Qt6 DLLs)

### 2. 自動修復功能
- 🔧 自動重啟 SQL Server 服務
- 🔧 重新生成完整的 AI_CONFIG.INI 配置檔案
- 🔧 自動創建缺失的資料庫表結構
- 🔧 修復權限和連線問題
- 🔧 WebSocket 端口衝突自動解決
- 🔧 VM 環境變數自動配置

### 3. 智能功能
- 🤖 多模型檢測與自動切換 (Ollama vs 內建AI)
- 👥 系統資源分析與玩家數量自動調整
- 📊 即時系統狀態監控
- 📝 彩色進度條和狀態顯示
- 📄 詳細錯誤報告自動生成到桌面

### 4. 一鍵操作
- 🚀 一鍵重啟 AI 助手和資料庫服務
- 🔍 一鍵完整系統檢查
- 📋 一鍵資料庫表結構驗證
- 🧹 一鍵系統清理

## 📁 檔案結構

```
deploy_ultimate_complete/
├── 一鍵系統檢查與修復.bat          # 主要的完整檢查工具
├── 智能啟動_增強版_SQL支援.bat      # 增強版智能啟動系統
├── SQL_資料庫表結構檢查.bat        # 專用資料庫表結構檢查工具
├── 系統完整性檢查.bat             # 原有的系統檢查工具 (已增強)
├── AI_CONFIG.INI                 # 主配置檔案 (支援SQL Server)
├── RANOnline_AI_Ultimate_Portable.exe  # 主程式
├── WebSocketServer.exe           # WebSocket 服務
├── Qt6Core.dll                   # Qt 依賴庫
├── Qt6Gui.dll
├── Qt6Widgets.dll
├── Qt6Network.dll
└── logs/                         # 日誌目錄
    └── ai_system.log
```

## 🚀 使用方法

### 1. 完整系統檢查與修復
```bash
# 執行完整的 8 步驟檢查
一鍵系統檢查與修復.bat
```

**檢查項目：**
1. SQL Server 1433 端口狀態
2. ODBC 連線與 sa 帳號驗證
3. RanUser 資料庫及表結構驗證
4. AI_CONFIG.INI 配置檔案格式
5. WebSocket 服務配置
6. 系統依賴項目檢查
7. AI 模型切換功能測試
8. 玩家數量自動調整驗證

### 2. 智能啟動系統
```bash
# 執行增強版智能啟動
智能啟動_增強版_SQL支援.bat
```

**啟動流程：**
1. SQL Server 完整性檢查
2. 配置檔案智能檢查與生成
3. AI 模型檢測與多模型切換
4. 系統資源分析與玩家數量調整
5. WebSocket 端口檢查與衝突解決
6. VM 環境配置與 AI 助手啟動

### 3. 資料庫表結構檢查
```bash
# 專用資料庫檢查工具
SQL_資料庫表結構檢查.bat
```

## 📊 配置說明

### AI_CONFIG.INI 結構

```ini
[AI]                    # AI 核心設置
[Database]              # SQL Server 連線設置
[Server]                # WebSocket 服務設置
[Log]                   # 日誌系統設置
[PlayerAI]              # AI 玩家系統設置
[Security]              # 安全性設置
[VM]                    # 虛擬機優化設置
[Performance]           # 效能優化設置
```

### SQL Server 連線配置

```ini
[Database]
Type=SQLServer
Server=localhost
Port=1433
Database=RanUser
Username=sa
Password=123456
TrustServerCertificate=true
Encrypt=false
```

## 🗄️ 資料庫表結構

系統會自動檢查並創建以下表結構：

1. **ChaInfo** - 角色資訊表
2. **UserInfo** - 用戶資訊表
3. **LogInfo** - 日誌資訊表
4. **ItemInfo** - 物品資訊表
5. **AI_States** - AI 狀態表
6. **GameEvents** - 遊戲事件記錄表

## 🎮 智能玩家數量調整

系統會根據硬體資源自動調整最大AI玩家數量：

| 記憶體大小 | 玩家數量 | 效能模式 |
|------------|----------|----------|
| ≥16GB      | 100      | HIGH     |
| ≥8GB       | 50       | STANDARD |
| ≥4GB       | 30       | BALANCED |
| <4GB       | 20       | LOW      |

## 🔧 常見問題解決

### SQL Server 連線問題
1. 確認 SQL Server 服務已啟動
2. 檢查防火牆是否阻擋 1433 端口
3. 確認 sa 帳號已啟用且密碼正確
4. 啟用 SQL Server 混合驗證模式

### AI 助手啟動失敗
1. 檢查是否缺少 Qt 依賴庫
2. 以管理員身份執行
3. 檢查防毒軟體設置
4. 確認 VM 環境變數配置

### WebSocket 端口衝突
1. 系統會自動檢測端口 8080 佔用情況
2. 自動結束佔用進程或使用替代端口 8081
3. 更新配置檔案中的端口設置

## 📝 錯誤日誌

系統會自動生成詳細的錯誤報告：

- **啟動日誌**: `%DESKTOP%\startup_log.txt`
- **檢查日誌**: `%DESKTOP%\error_log.txt`
- **資料庫日誌**: `%DESKTOP%\database_check_log.txt`
- **系統日誌**: `logs\ai_system.log`

## 📞 技術支援

- **版本**: v4.0.0 Ultimate Edition
- **支援郵件**: jytech@example.com
- **更新日期**: 2025年6月14日

## 🔄 更新記錄

### v4.0.0 (2025/06/14)
- ✨ 新增完整的 SQL Server 1433 端口檢查
- ✨ 新增 ODBC 連線與 sa 帳號驗證
- ✨ 新增 RanUser 資料庫完整性檢查
- ✨ 新增自動資料庫表結構創建
- ✨ 新增多模型檢測與自動切換
- ✨ 新增智能玩家數量調整
- ✨ 新增 WebSocket 端口衝突自動解決
- ✨ 新增彩色進度條和狀態顯示
- ✨ 新增實時管理介面
- ✨ 新增一鍵系統修復功能
- 🔧 改進 VM 環境兼容性
- 🔧 增強錯誤處理和日誌記錄

## 🎯 系統要求

- **作業系統**: Windows 10/11 (x64)
- **資料庫**: SQL Server (本地或遠程)
- **記憶體**: 最少 4GB RAM (建議 8GB+)
- **網路端口**: 1433 (SQL Server), 8080/8081 (WebSocket)
- **權限**: 建議以管理員身份執行
