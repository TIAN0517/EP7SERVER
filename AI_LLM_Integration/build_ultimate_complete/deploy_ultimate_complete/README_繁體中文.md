# Jy技術團隊 線上AI - Ultimate Portable Edition v4.0.0

## 🎯 極簡穩定VM兼容版特性

✅ **100% 便攜化** - 零依賴安裝，無需系統配置  
✅ **VM/RDP/雲桌面 100% 兼容** - 專為虛擬環境優化  
✅ **強制軟體渲染** - 杜絕GPU相關錯誤和崩潰  
✅ **系統原生UI** - 無自定義樣式風險，極致穩定  
✅ **智能環境檢測** - 自動適應VM/RDP/雲桌面環境  
✅ **Ollama模型集成** - 支援本地大型語言模型  
✅ **完整錯誤處理** - 傻瓜式一鍵啟動  

---

## 🚀 快速啟動

### 方法一：雙擊啟動（推薦）
```
雙擊 "一鍵啟動_Ultimate_Portable.bat"
```

### 方法二：PowerShell啟動
```powershell
# 切換到程式目錄
cd "程式所在目錄"

# 啟動程式（注意PowerShell語法）
.\RANOnline_AI_Ultimate_Portable.exe
```

### 方法三：CMD啟動
```cmd
# 切換到程式目錄
cd "程式所在目錄"

# 啟動程式
RANOnline_AI_Ultimate_Portable.exe
```

---

## 🔧 PowerShell 執行注意事項

如果在 PowerShell 中直接輸入 `RANOnline_AI_Ultimate_Portable.exe` 出現錯誤，
請使用以下正確語法：

```powershell
# 正確語法 - 加上 .\ 前綴
.\RANOnline_AI_Ultimate_Portable.exe

# 或者使用完整路徑
"C:\完整路徑\RANOnline_AI_Ultimate_Portable.exe"
```

**原因說明：**
- PowerShell 不會自動在當前目錄查找可執行文件
- 必須明確指定路徑（`.\` 表示當前目錄）
- 這是 PowerShell 的安全特性，不是程式錯誤

---

## 🖥️ 系統需求

- **作業系統：** Windows 10/11 (64位)
- **記憶體：** 最小 2GB RAM
- **硬碟空間：** 200MB 可用空間
- **網路：** 可選（Ollama模型需要）

---

## 🎮 使用指南

1. **啟動程式** - 雙擊一鍵啟動腳本或直接執行exe
2. **選擇模型** - 程式會自動檢測可用的AI模型
3. **開始對話** - 在輸入框中輸入訊息，按Enter發送
4. **查看系統資訊** - 選單 → 系統 → 系統資訊
5. **關閉程式** - 點擊關閉按鈕或雙擊一鍵關閉腳本

---

## 🔍 智能環境檢測

程式會自動檢測並適應以下環境：

- **虛擬機環境** (VirtualBox, VMware, Hyper-V等)
- **遠端桌面** (RDP連線)
- **雲桌面服務** (AWS WorkSpaces, Azure Virtual Desktop等)
- **Ollama模型** (自動掃描並集成可用模型)

---

## 🤖 Ollama 集成

如果系統已安裝 Ollama：

1. 程式會自動檢測並列出可用模型
2. 可在模型下拉選單中切換使用
3. 支援與Ollama模型進行自然語言對話

**安裝 Ollama：**
```bash
# 訪問官網下載
https://ollama.ai

# 安裝模型範例
ollama pull llama2
ollama pull codellama
```

---

## 🛠️ 故障排除

**問題：程式無法啟動**
- 確認Windows版本支援（Windows 10/11）
- 嘗試以管理員身份執行
- 檢查防毒軟體是否阻擋

**問題：PowerShell無法執行**
- 使用 `.\RANOnline_AI_Ultimate_Portable.exe` 語法
- 或改用 CMD 執行
- 或直接雙擊exe文件

**問題：AI回應異常**
- 檢查網路連線（如使用Ollama模型）
- 重新掃描Ollama模型
- 切換回內建AI模型測試

**問題：在VM中運行緩慢**
- 程式已自動啟用安全模式
- 使用強制軟體渲染確保兼容性
- 關閉VM中不必要的程序釋放資源

---

## 📦 VM部署指南

### 複製到VM的步驟：

1. **複製整個資料夾** 到VM中任意位置
2. **確保包含所有文件** - 不要遺漏任何DLL或資料夾
3. **雙擊一鍵啟動腳本** - 程式會自動適應VM環境
4. **首次啟動後** 程式會顯示環境檢測結果

### VM最佳化建議：

- 分配至少 2GB RAM 給VM
- 確保VM有穩定的網路連線（如需使用Ollama）
- 在VM中關閉不必要的視覺效果
- 使用固定大小的虛擬硬碟確保穩定性

---

## 💡 進階使用技巧

### 快速指令：
- 輸入「你好」- 查看AI問候
- 輸入「時間」- 查看當前時間
- 輸入「系統」- 查看系統資訊
- 輸入「幫助」- 查看可用指令
- 輸入「vm」- 查看VM兼容性資訊
- 輸入「ollama」- 查看Ollama狀態

### 模型切換：
- 使用模型下拉選單切換AI模型
- 內建AI助手：無需網路，即時回應
- Ollama模型：需要本地安裝，功能更強大

---

## 📞 技術支援

- **開發團隊：** Jy技術團隊
- **技術支援：** jytech@example.com  
- **版本資訊：** v4.0.0 Ultimate Portable Edition
- **建構日期：** 2025年6月14日

---

## 📁 檔案結構說明

```
RANOnline_AI_Ultimate_Portable/
├── RANOnline_AI_Ultimate_Portable.exe    # 主程式
├── 一鍵啟動_Ultimate_Portable.bat        # 啟動腳本
├── 一鍵關閉.bat                          # 關閉腳本
├── README_繁體中文.md                    # 本說明文件
├── Qt6Core.dll                          # Qt6核心庫
├── Qt6Gui.dll                           # Qt6GUI庫
├── Qt6Network.dll                       # Qt6網路庫
├── Qt6Widgets.dll                       # Qt6組件庫
├── opengl32sw.dll                       # 軟體OpenGL
├── platforms/                           # 平台插件
├── styles/                              # 樣式插件
├── imageformats/                        # 圖像格式支援
├── iconengines/                         # 圖標引擎
└── translations/                        # 多語言支援
```

**重要提醒：**
- 請保持所有檔案在同一目錄下
- 不要刪除任何 `.dll` 文件或子資料夾
- 整個資料夾可直接複製到其他電腦使用

---

## 📄 版權聲明

Copyright (C) 2025 Jy技術團隊. All rights reserved.

本軟體專為RANOnline遊戲AI系統設計，  
提供虛擬機和雲桌面環境的完美兼容性。

**特別聲明：** 此為VM兼容專版，經過特殊優化確保在各種虛擬環境中穩定運行。
