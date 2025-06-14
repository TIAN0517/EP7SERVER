# RAN Online AI 四部門提示詞包

<div align="center">

![RAN Online Four Departments](../Resources/icons/ran_logo.png)

**🎮 RANOnline EP7 AI 四部門專用提示詞系統**

*劍術 • 弓術 • 格鬥 • 氣功*

[![Python](https://img.shields.io/badge/Python-3.7+-blue.svg)](https://python.org/)
[![AI Ready](https://img.shields.io/badge/AI-Ready-green.svg)](README.md)
[![Traditional Chinese](https://img.shields.io/badge/Language-繁體中文-red.svg)](README.md)

</div>

## 🚀 快速開始

### 📋 一鍵導出
```bash
# Windows 用戶
cd Scripts
export_prompts.bat

# Linux/macOS 用戶  
cd Scripts
python export_prompts.py
```

### 📁 生成文件
執行後將在 `exports/` 目錄生成：
- `ai_prompts_export_four_dept.md` - 完整提示詞模板
- `copilot_four_dept_snippets.js` - GitHub Copilot 程式碼片段

## 🎯 如何使用

### 1️⃣ 複製提示詞到 AI 平台
✅ **GitHub Copilot**：直接貼上，立即可用  
✅ **ChatGPT**：複製提示詞，替換參數變數  
✅ **Ollama**：適用於本地部署的LLM  
✅ **Claude**：支援長文本對話  
✅ **Gemini**：Google的AI助手  

### 2️⃣ 參數替換
將模板中的參數替換為實際值：
- `{count}` → 具體數字（如：5）
- `{academy}` → 學院名稱（聖門/懸岩/鳳凰）
- `{department}` → 部門名稱（劍術/弓術/格鬥/氣功）
- `{combat_situation}` → 具體戰況描述

### 3️⃣ 獲得專業回應
所有AI回應都將：
- 🇹🇼 使用繁體中文
- ✅ 限定四大部門
- 🎮 符合RAN Online遊戲風格
- 📊 提供結構化輸出

## 📝 快速範例

### 🤖 批量生成AI角色
```
你是一個《RAN Online》自動AI生成器。請根據三大學院（聖門、懸岩、鳳凰）與四大部門（劍術、弓術、格鬥、氣功）隨機產生5名AI角色，每個角色須有：學院、部門、姓名（6字以內）、性格特徵、戰鬥風格、專屬台詞。輸出格式：JSON陣列。
```

### ⚔️ PK互動嗆聲  
```
你是鳳凰/劍術AI，在PK中遇到敵方挑釁：「你們鳳凰學院都是莽夫！」，請用充滿個性的繁體中文嗆聲回應。
```

### 🛡️ 戰術指揮
```
你是RAN Online裡的AI隊長，帶領聖門分隊進行PVP。當前局勢：敵人數量5、地形橋樑狹道、剩餘資源MP充足。請下達最優行動指令給劍術、弓術、格鬥、氣功隊員。
```

### ⚡ 技能自動決策
```
你是AI氣功師，當前狀況：劍術系HP剩30%，格鬥系被石化，弓術系MP不足。請選擇最適合的技能並說明原因。
```

## 🗄️ SQL建表（四部門限定）

```sql
CREATE TABLE [dbo].[AiStates] (
    [AIId] NVARCHAR(32) PRIMARY KEY,
    [Name] NVARCHAR(16) NOT NULL,
    [Academy] NVARCHAR(8) NOT NULL CHECK ([Academy] IN ('聖門','懸岩','鳳凰')),
    [Department] NVARCHAR(8) NOT NULL CHECK ([Department] IN ('劍術','弓術','格鬥','氣功')),
    [Level] INT DEFAULT 1,
    [Status] NVARCHAR(8),
    [LastUpdate] DATETIME DEFAULT GETDATE(),
    [IsActive] BIT DEFAULT 1
);
```

## 🎨 部門特色

### 🗡️ 劍術系
- **風格**：近戰突擊專家
- **特長**：高爆發單體攻擊、快速接近、連擊技能
- **代表技能**：衝鋒斬擊、連環劍技、破甲攻擊

### 🏹 弓術系  
- **風格**：遠程狙擊專家
- **特長**：精準射擊、風箏戰術、控制節奏
- **代表技能**：精準射擊、範圍箭雨、減速陷阱

### 👊 格鬥系
- **風格**：格鬥搏擊專家  
- **特長**：近身纏鬥、摔投技、反擊
- **代表技能**：擒拿技、反擊拳、破招式

### ⚡ 氣功系
- **風格**：治療支援專家
- **特長**：團隊治療、BUFF管理、狀態淨化  
- **代表技能**：群體治療、狀態增強、驅散詛咒

## 🏫 三大學院

### 🔆 聖門學院
- **理念**：神聖系，注重防守和治療
- **特色**：穩健防守、團隊治療、神聖攻擊
- **說話風格**：莊重威嚴，正式但親和

### 🌙 懸岩學院  
- **理念**：元素系，精通法術和控制
- **特色**：遠程輸出、元素控制、戰場操控
- **說話風格**：深沉睿智，優雅富哲理

### 🔥 鳳凰學院
- **理念**：戰鬥系，專精攻擊和爆發
- **特色**：快速突擊、高爆發、近戰優勢
- **說話風格**：豪邁直爽，簡潔有力

## 🔧 AI模型推薦

| 用途 | 推薦模型 | 說明 |
|------|----------|------|
| 角色生成 | `llama3:latest`, `deepseek-coder:latest` | 創造性角色設計 |
| 戰術規劃 | `phi3:latest`, `gemma:latest` | 邏輯分析和策略 |
| 角色扮演 | `mistral:latest`, `qwen2:latest` | 自然對話互動 |
| 技能自動化 | `deepseek-coder:latest` | 程式邏輯決策 |

## 📋 品質保證

### ✅ 嚴格驗證
- **部門限制**：僅允許劍術、弓術、格鬥、氣功
- **學院限制**：僅允許聖門、懸岩、鳳凰
- **語言一致**：全部使用繁體中文
- **風格統一**：符合RAN Online遊戲風格

### ✅ 內容過濾
- 避免現實世界政治話題
- 確保遊戲內容適當性
- 維持角色設定一致性
- 保證技能邏輯合理性

## 📞 技術支援

### 🐛 常見問題
**Q: Python 執行錯誤？**
A: 確保安裝 Python 3.7+ 並檢查文件路徑

**Q: 生成的角色不符合四部門？**  
A: 檢查 `Config/llm.json` 配置是否正確

**Q: AI 回應不是繁體中文？**
A: 在提示詞末尾加上「請用繁體中文回應」

### 🔗 相關連結
- [GitHub Copilot 使用指南](https://github.com/features/copilot)
- [Ollama 本地部署](https://ollama.ai/)
- [OpenAI ChatGPT](https://chatgpt.com/)

---

<div align="center">

**🎮 專為 RANOnline EP7 設計的 AI 四部門提示詞系統**

*讓每個 AI 角色都有學院特色和部門專精*

**開發團隊**: Jy技術團隊 | **版本**: 1.0.0 | **更新**: 2025-06-14

</div>
