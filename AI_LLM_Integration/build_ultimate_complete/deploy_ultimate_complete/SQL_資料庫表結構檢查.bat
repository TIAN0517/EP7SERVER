@echo off
chcp 65001 >nul
title 【Jy技術團隊 線上AI】SQL Server 資料庫表結構檢查與修復

echo.
echo ========================================================================
echo          【Jy技術團隊 線上AI】SQL Server 資料庫表結構檢查與修復
echo                             v4.0.0 Ultimate Edition
echo ========================================================================
echo.

set "DESKTOP=%USERPROFILE%\Desktop"
set "ERROR_LOG=%DESKTOP%\database_check_log.txt"

echo [%date% %time%] ========== 資料庫表結構檢查開始 ========== > "%ERROR_LOG%"

echo 🔍 正在檢查 RanUser 資料庫表結構...
echo.

REM 創建表結構檢查和修復腳本
(
echo -- 檢查並創建 RanUser 資料庫
echo IF NOT EXISTS ^(SELECT name FROM sys.databases WHERE name = 'RanUser'^)
echo BEGIN
echo     CREATE DATABASE RanUser;
echo     PRINT 'RanUser 資料庫已創建';
echo END
echo ELSE
echo BEGIN
echo     PRINT 'RanUser 資料庫已存在';
echo END
echo GO
echo.
echo USE RanUser;
echo GO
echo.
echo -- 檢查並創建 ChaInfo 表 ^(角色資訊^)
echo IF NOT EXISTS ^(SELECT * FROM sysobjects WHERE name='ChaInfo' AND xtype='U'^)
echo BEGIN
echo     CREATE TABLE ChaInfo ^(
echo         ChaNum INT IDENTITY^(1,1^) PRIMARY KEY,
echo         ChaName NVARCHAR^(50^) NOT NULL UNIQUE,
echo         ChaLevel INT DEFAULT 1,
echo         ChaClass INT DEFAULT 0,
echo         ChaArea INT DEFAULT 0,
echo         ChaExperience BIGINT DEFAULT 0,
echo         ChaHP INT DEFAULT 100,
echo         ChaMP INT DEFAULT 50,
echo         ChaCreateDate DATETIME DEFAULT GETDATE^(^),
echo         ChaLastLogin DATETIME DEFAULT GETDATE^(^),
echo         ChaStatus INT DEFAULT 0,
echo         ChaGold BIGINT DEFAULT 0,
echo         INDEX IX_ChaInfo_Name ^(ChaName^),
echo         INDEX IX_ChaInfo_Level ^(ChaLevel^)
echo     ^);
echo     PRINT '✅ ChaInfo 表已創建';
echo END
echo ELSE
echo BEGIN
echo     PRINT '✅ ChaInfo 表已存在';
echo END
echo GO
echo.
echo -- 檢查並創建 UserInfo 表 ^(用戶資訊^)
echo IF NOT EXISTS ^(SELECT * FROM sysobjects WHERE name='UserInfo' AND xtype='U'^)
echo BEGIN
echo     CREATE TABLE UserInfo ^(
echo         UserNum INT IDENTITY^(1,1^) PRIMARY KEY,
echo         UserID NVARCHAR^(50^) NOT NULL UNIQUE,
echo         UserPass NVARCHAR^(255^) NOT NULL,
echo         UserType INT DEFAULT 0,
echo         UserCreateDate DATETIME DEFAULT GETDATE^(^),
echo         UserLastLogin DATETIME DEFAULT GETDATE^(^),
echo         UserStatus INT DEFAULT 1,
echo         UserEmail NVARCHAR^(100^),
echo         INDEX IX_UserInfo_ID ^(UserID^)
echo     ^);
echo     PRINT '✅ UserInfo 表已創建';
echo END
echo ELSE
echo BEGIN
echo     PRINT '✅ UserInfo 表已存在';
echo END
echo GO
echo.
echo -- 檢查並創建 LogInfo 表 ^(日誌資訊^)
echo IF NOT EXISTS ^(SELECT * FROM sysobjects WHERE name='LogInfo' AND xtype='U'^)
echo BEGIN
echo     CREATE TABLE LogInfo ^(
echo         LogNum INT IDENTITY^(1,1^) PRIMARY KEY,
echo         LogType INT NOT NULL,
echo         LogDate DATETIME DEFAULT GETDATE^(^),
echo         LogMsg NVARCHAR^(500^),
echo         LogUserNum INT,
echo         LogChaNum INT,
echo         LogIP NVARCHAR^(45^),
echo         INDEX IX_LogInfo_Date ^(LogDate^),
echo         INDEX IX_LogInfo_Type ^(LogType^)
echo     ^);
echo     PRINT '✅ LogInfo 表已創建';
echo END
echo ELSE
echo BEGIN
echo     PRINT '✅ LogInfo 表已存在';
echo END
echo GO
echo.
echo -- 檢查並創建 ItemInfo 表 ^(物品資訊^)
echo IF NOT EXISTS ^(SELECT * FROM sysobjects WHERE name='ItemInfo' AND xtype='U'^)
echo BEGIN
echo     CREATE TABLE ItemInfo ^(
echo         ItemNum INT IDENTITY^(1,1^) PRIMARY KEY,
echo         ItemName NVARCHAR^(50^) NOT NULL,
echo         ItemType INT DEFAULT 0,
echo         ItemLevel INT DEFAULT 1,
echo         ItemValue INT DEFAULT 0,
echo         ItemDescription NVARCHAR^(255^),
echo         ItemCreateDate DATETIME DEFAULT GETDATE^(^),
echo         INDEX IX_ItemInfo_Name ^(ItemName^),
echo         INDEX IX_ItemInfo_Type ^(ItemType^)
echo     ^);
echo     PRINT '✅ ItemInfo 表已創建';
echo END
echo ELSE
echo BEGIN
echo     PRINT '✅ ItemInfo 表已存在';
echo END
echo GO
echo.
echo -- 檢查並創建 AI_States 表 ^(AI狀態資訊^)
echo IF NOT EXISTS ^(SELECT * FROM sysobjects WHERE name='AI_States' AND xtype='U'^)
echo BEGIN
echo     CREATE TABLE AI_States ^(
echo         AI_ID NVARCHAR^(50^) PRIMARY KEY,
echo         AI_Name NVARCHAR^(50^) NOT NULL,
echo         AI_Type INT DEFAULT 0,
echo         AI_Level INT DEFAULT 1,
echo         AI_HP INT DEFAULT 100,
echo         AI_MP INT DEFAULT 50,
echo         AI_PosX FLOAT DEFAULT 0,
echo         AI_PosY FLOAT DEFAULT 0,
echo         AI_PosZ FLOAT DEFAULT 0,
echo         AI_Status INT DEFAULT 1,
echo         AI_LastUpdate DATETIME DEFAULT GETDATE^(^),
echo         AI_CreateDate DATETIME DEFAULT GETDATE^(^),
echo         INDEX IX_AI_States_Name ^(AI_Name^),
echo         INDEX IX_AI_States_Type ^(AI_Type^),
echo         INDEX IX_AI_States_LastUpdate ^(AI_LastUpdate^)
echo     ^);
echo     PRINT '✅ AI_States 表已創建';
echo END
echo ELSE
echo BEGIN
echo     PRINT '✅ AI_States 表已存在';
echo END
echo GO
echo.
echo -- 檢查並創建 GameEvents 表 ^(遊戲事件記錄^)
echo IF NOT EXISTS ^(SELECT * FROM sysobjects WHERE name='GameEvents' AND xtype='U'^)
echo BEGIN
echo     CREATE TABLE GameEvents ^(
echo         Event_ID INT IDENTITY^(1,1^) PRIMARY KEY,
echo         Event_Type NVARCHAR^(50^) NOT NULL,
echo         Event_Data NVARCHAR^(MAX^),
echo         Event_Timestamp DATETIME DEFAULT GETDATE^(^),
echo         Event_PlayerID INT,
echo         Event_AIID NVARCHAR^(50^),
echo         INDEX IX_GameEvents_Type ^(Event_Type^),
echo         INDEX IX_GameEvents_Timestamp ^(Event_Timestamp^)
echo     ^);
echo     PRINT '✅ GameEvents 表已創建';
echo END
echo ELSE
echo BEGIN
echo     PRINT '✅ GameEvents 表已存在';
echo END
echo GO
echo.
echo -- 顯示所有表的結構資訊
echo PRINT '=====================================';
echo PRINT '資料庫表結構檢查完成';
echo PRINT '=====================================';
echo SELECT 
echo     TABLE_NAME as '表名稱',
echo     TABLE_TYPE as '表類型'
echo FROM INFORMATION_SCHEMA.TABLES 
echo WHERE TABLE_TYPE = 'BASE TABLE'
echo ORDER BY TABLE_NAME;
echo GO
echo.
echo -- 顯示表的記錄數量
echo PRINT '=====================================';
echo PRINT '表記錄數量統計';
echo PRINT '=====================================';
echo SELECT 'ChaInfo' as TableName, COUNT^(*^) as RecordCount FROM ChaInfo
echo UNION ALL
echo SELECT 'UserInfo', COUNT^(*^) FROM UserInfo  
echo UNION ALL
echo SELECT 'LogInfo', COUNT^(*^) FROM LogInfo
echo UNION ALL
echo SELECT 'ItemInfo', COUNT^(*^) FROM ItemInfo
echo UNION ALL
echo SELECT 'AI_States', COUNT^(*^) FROM AI_States
echo UNION ALL
echo SELECT 'GameEvents', COUNT^(*^) FROM GameEvents;
echo GO
) > "%TEMP%\create_tables.sql"

echo 🔧 正在執行表結構檢查和修復...
echo.

REM 執行表結構檢查
sqlcmd -S localhost -U sa -P 123456 -i "%TEMP%\create_tables.sql" 2>&1 | tee "%TEMP%\db_output.txt"

if %errorlevel% == 0 (
    echo.
    echo ✅ 資料庫表結構檢查完成！
    echo [%date% %time%] 資料庫表結構檢查成功完成 >> "%ERROR_LOG%"
    
    REM 將輸出結果添加到日誌
    echo. >> "%ERROR_LOG%"
    echo ========== 資料庫輸出結果 ========== >> "%ERROR_LOG%"
    type "%TEMP%\db_output.txt" >> "%ERROR_LOG%"
    
) else (
    echo.
    echo ❌ 資料庫表結構檢查失敗！
    echo [%date% %time%] 資料庫表結構檢查失敗 >> "%ERROR_LOG%"
    echo.
    echo 建議檢查項目：
    echo • SQL Server 服務是否正在運行
    echo • sa 帳號是否已啟用且密碼正確
    echo • 防火牆是否阻擋 1433 端口
    echo.
)

REM 清理臨時檔案
if exist "%TEMP%\create_tables.sql" del /q "%TEMP%\create_tables.sql"
if exist "%TEMP%\db_output.txt" del /q "%TEMP%\db_output.txt"

echo.
echo [%date% %time%] ========== 資料庫表結構檢查結束 ========== >> "%ERROR_LOG%"
echo 📄 詳細報告已儲存至: %ERROR_LOG%
echo.
pause
