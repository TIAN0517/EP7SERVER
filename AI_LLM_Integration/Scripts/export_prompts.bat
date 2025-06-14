@echo off
chcp 65001 >nul
title RAN Online AI 四部門提示詞導出工具

echo.
echo ============================================
echo     RAN Online AI 四部門提示詞導出工具
echo     開發團隊：Jy技術團隊
echo     版本：1.0.0
echo ============================================
echo.
echo 🎯 本工具將自動生成：
echo    • 完整四部門提示詞模板
echo    • SQL建表語句（嚴格限制四部門）
echo    • GitHub Copilot 程式碼片段
echo    • AI平台快速使用指南
echo.
echo 🏫 支援學院：聖門、懸岩、鳳凰
echo 🥋 支援部門：劍術、弓術、格鬥、氣功
echo.

:: 檢查 Python 是否安裝
python --version >nul 2>&1
if errorlevel 1 (
    echo ❌ 錯誤：未找到 Python！
    echo 請先安裝 Python 3.7 或更高版本
    echo 下載地址：https://www.python.org/downloads/
    echo.
    pause
    exit /b 1
)

echo ✅ Python 環境檢查通過
echo.

:: 執行導出腳本
echo 🔄 正在執行提示詞導出...
echo.

python "%~dp0export_prompts.py"

if errorlevel 1 (
    echo.
    echo ❌ 導出過程中發生錯誤！
    echo 請檢查以下項目：
    echo    1. Python 版本是否為 3.7+
    echo    2. Config/llm.json 文件是否存在
    echo    3. 是否有寫入權限
    echo.
) else (
    echo.
    echo 🎉 恭喜！四部門提示詞導出完成！
    echo.
    echo 📁 生成的文件：
    echo    • exports/ai_prompts_export_four_dept.md
    echo    • exports/copilot_four_dept_snippets.js
    echo.
    echo 💡 快速使用提示：
    echo    1. 打開 .md 文件查看完整提示詞
    echo    2. 複製提示詞到 ChatGPT/Copilot/Ollama
    echo    3. 替換 {參數} 為實際值
    echo    4. 立即獲得專業的四部門AI回應
    echo.
    echo 🚀 現在就開始使用吧！
)

echo.
echo 按任意鍵關閉視窗...
pause >nul
