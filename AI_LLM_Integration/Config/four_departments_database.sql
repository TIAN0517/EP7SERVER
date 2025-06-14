-- =====================================================
-- RAN Online AI 四部門資料庫建表語句
-- 生成時間: 2025-06-14
-- 說明: 嚴格限制只允許四大部門：劍術、弓術、格鬥、氣功
-- =====================================================

-- 1. AI角色狀態表
CREATE TABLE [dbo].[AiStates] (
    [AIId] NVARCHAR(32) PRIMARY KEY,
    [Name] NVARCHAR(16) NOT NULL,
    [Academy] NVARCHAR(8) NOT NULL CHECK ([Academy] IN ('聖門','懸岩','鳳凰')),
    [Department] NVARCHAR(8) NOT NULL CHECK ([Department] IN ('劍術','弓術','格鬥','氣功')),
    [Level] INT DEFAULT 1 CHECK ([Level] >= 1 AND [Level] <= 999),
    [Experience] BIGINT DEFAULT 0,
    [BehaviorType] INT DEFAULT 1,
    [PositionX] INT DEFAULT 0,
    [PositionY] INT DEFAULT 0,
    [PositionZ] INT DEFAULT 0,
    [Status] NVARCHAR(8) DEFAULT '待命' CHECK ([Status] IN ('待命','戰鬥','休息','巡邏','治療')),
    [HP] INT DEFAULT 100,
    [MaxHP] INT DEFAULT 100,
    [MP] INT DEFAULT 50,
    [MaxMP] INT DEFAULT 50,
    [AttackPower] INT DEFAULT 10,
    [DefensePower] INT DEFAULT 10,
    [Speed] INT DEFAULT 10,
    [MagicPower] INT DEFAULT 10,
    [LastUpdate] DATETIME DEFAULT GETDATE(),
    [IsActive] BIT DEFAULT 1,
    [PersonalityType] NVARCHAR(16),
    [CombatStyle] NVARCHAR(32),
    [SignatureQuote] NVARCHAR(128)
);

-- 2. AI團隊編組表
CREATE TABLE [dbo].[AiTeams] (
    [TeamId] NVARCHAR(32) PRIMARY KEY,
    [TeamName] NVARCHAR(32) NOT NULL,
    [Academy] NVARCHAR(8) NOT NULL CHECK ([Academy] IN ('聖門','懸岩','鳳凰')),
    [LeaderId] NVARCHAR(32) NOT NULL,
    [ViceLeaderId] NVARCHAR(32),
    [HealerId] NVARCHAR(32),
    [SwordMember] NVARCHAR(32), -- 劍術系成員
    [ArcherMember] NVARCHAR(32), -- 弓術系成員
    [FighterMember] NVARCHAR(32), -- 格鬥系成員
    [QigongMember] NVARCHAR(32), -- 氣功系成員
    [Formation] NVARCHAR(16) DEFAULT '標準陣型',
    [TacticsType] NVARCHAR(16) DEFAULT '平衡',
    [TeamLevel] INT DEFAULT 1,
    [TeamExperience] BIGINT DEFAULT 0,
    [CreatedAt] DATETIME DEFAULT GETDATE(),
    [IsActive] BIT DEFAULT 1,
    [LastBattleTime] DATETIME,
    [WinCount] INT DEFAULT 0,
    [LoseCount] INT DEFAULT 0,
    FOREIGN KEY ([LeaderId]) REFERENCES [AiStates]([AIId]),
    FOREIGN KEY ([ViceLeaderId]) REFERENCES [AiStates]([AIId]),
    FOREIGN KEY ([HealerId]) REFERENCES [AiStates]([AIId]),
    FOREIGN KEY ([SwordMember]) REFERENCES [AiStates]([AIId]),
    FOREIGN KEY ([ArcherMember]) REFERENCES [AiStates]([AIId]),
    FOREIGN KEY ([FighterMember]) REFERENCES [AiStates]([AIId]),
    FOREIGN KEY ([QigongMember]) REFERENCES [AiStates]([AIId])
);

-- 3. LLM決策記錄表
CREATE TABLE [dbo].[LlmDecisions] (
    [DecisionId] NVARCHAR(32) PRIMARY KEY,
    [AIId] NVARCHAR(32) NOT NULL,
    [ModelName] NVARCHAR(32) NOT NULL,
    [PromptType] NVARCHAR(16) NOT NULL,
    [Academy] NVARCHAR(8) CHECK ([Academy] IN ('聖門','懸岩','鳳凰')),
    [Department] NVARCHAR(8) CHECK ([Department] IN ('劍術','弓術','格鬥','氣功')),
    [InputPrompt] NVARCHAR(MAX) NOT NULL,
    [OutputResponse] NVARCHAR(MAX),
    [ResponseTime] INT DEFAULT 0,
    [TokensUsed] INT DEFAULT 0,
    [Success] BIT DEFAULT 0,
    [ConfidenceScore] DECIMAL(3,2) DEFAULT 0.00,
    [CreatedAt] DATETIME DEFAULT GETDATE(),
    [ProcessedAt] DATETIME,
    [ErrorMessage] NVARCHAR(512),
    FOREIGN KEY ([AIId]) REFERENCES [AiStates]([AIId])
);

-- 4. 技能系統表
CREATE TABLE [dbo].[AiSkills] (
    [SkillId] NVARCHAR(32) PRIMARY KEY,
    [SkillName] NVARCHAR(32) NOT NULL,
    [Department] NVARCHAR(8) NOT NULL CHECK ([Department] IN ('劍術','弓術','格鬥','氣功')),
    [Academy] NVARCHAR(8) CHECK ([Academy] IN ('聖門','懸岩','鳳凰')),
    [SkillType] NVARCHAR(16) NOT NULL CHECK ([SkillType] IN ('攻擊','防禦','治療','輔助','控制')),
    [ManaCost] INT DEFAULT 0,
    [CooldownTime] INT DEFAULT 0,
    [Damage] INT DEFAULT 0,
    [Range] INT DEFAULT 1,
    [Description] NVARCHAR(256),
    [RequiredLevel] INT DEFAULT 1,
    [IsActive] BIT DEFAULT 1
);

-- 5. AI技能學習表
CREATE TABLE [dbo].[AiLearnedSkills] (
    [LearnId] NVARCHAR(32) PRIMARY KEY,
    [AIId] NVARCHAR(32) NOT NULL,
    [SkillId] NVARCHAR(32) NOT NULL,
    [SkillLevel] INT DEFAULT 1 CHECK ([SkillLevel] >= 1 AND [SkillLevel] <= 10),
    [Experience] INT DEFAULT 0,
    [LastUsedTime] DATETIME,
    [UsageCount] INT DEFAULT 0,
    [SuccessCount] INT DEFAULT 0,
    [LearnedAt] DATETIME DEFAULT GETDATE(),
    FOREIGN KEY ([AIId]) REFERENCES [AiStates]([AIId]),
    FOREIGN KEY ([SkillId]) REFERENCES [AiSkills]([SkillId]),
    UNIQUE([AIId], [SkillId])
);

-- 6. 戰鬥記錄表
CREATE TABLE [dbo].[BattleRecords] (
    [BattleId] NVARCHAR(32) PRIMARY KEY,
    [BattleType] NVARCHAR(16) NOT NULL CHECK ([BattleType] IN ('PVP','PVE','練習','團戰')),
    [Team1Id] NVARCHAR(32),
    [Team2Id] NVARCHAR(32),
    [WinnerTeamId] NVARCHAR(32),
    [BattleDuration] INT DEFAULT 0,
    [StartTime] DATETIME DEFAULT GETDATE(),
    [EndTime] DATETIME,
    [BattleLog] NVARCHAR(MAX),
    [MVP_AIId] NVARCHAR(32),
    [TotalDamageDealt] BIGINT DEFAULT 0,
    [TotalHealingDone] BIGINT DEFAULT 0,
    FOREIGN KEY ([Team1Id]) REFERENCES [AiTeams]([TeamId]),
    FOREIGN KEY ([Team2Id]) REFERENCES [AiTeams]([TeamId]),
    FOREIGN KEY ([WinnerTeamId]) REFERENCES [AiTeams]([TeamId]),
    FOREIGN KEY ([MVP_AIId]) REFERENCES [AiStates]([AIId])
);

-- 7. AI對話記錄表
CREATE TABLE [dbo].[AiDialogues] (
    [DialogueId] NVARCHAR(32) PRIMARY KEY,
    [AIId] NVARCHAR(32) NOT NULL,
    [TargetAIId] NVARCHAR(32),
    [DialogueType] NVARCHAR(16) CHECK ([DialogueType] IN ('嗆聲','戰術','閒聊','挑釁','鼓勵')),
    [Message] NVARCHAR(512) NOT NULL,
    [Emotion] NVARCHAR(16),
    [Context] NVARCHAR(256),
    [CreatedAt] DATETIME DEFAULT GETDATE(),
    [IsPublic] BIT DEFAULT 1,
    FOREIGN KEY ([AIId]) REFERENCES [AiStates]([AIId]),
    FOREIGN KEY ([TargetAIId]) REFERENCES [AiStates]([AIId])
);

-- =====================================================
-- 預設資料插入
-- =====================================================

-- 插入四大部門基礎技能
INSERT INTO [AiSkills] VALUES
-- 劍術系技能
('SKILL_SWORD_001', '基礎劍擊', '劍術', NULL, '攻擊', 5, 1000, 25, 1, '基本的劍術攻擊', 1, 1),
('SKILL_SWORD_002', '連環劍技', '劍術', NULL, '攻擊', 15, 3000, 60, 1, '連續三段攻擊', 10, 1),
('SKILL_SWORD_003', '破甲斬擊', '劍術', NULL, '攻擊', 25, 5000, 40, 1, '無視部分防禦力的攻擊', 20, 1),

-- 弓術系技能
('SKILL_ARCH_001', '精準射擊', '弓術', NULL, '攻擊', 3, 800, 20, 5, '精準的遠程攻擊', 1, 1),
('SKILL_ARCH_002', '箭雨攻擊', '弓術', NULL, '攻擊', 20, 4000, 35, 3, '範圍攻擊技能', 15, 1),
('SKILL_ARCH_003', '減速陷阱', '弓術', NULL, '控制', 10, 2000, 0, 2, '降低敵人移動速度', 8, 1),

-- 格鬥系技能
('SKILL_FIGHT_001', '基礎拳擊', '格鬥', NULL, '攻擊', 4, 900, 22, 1, '基本的拳擊攻擊', 1, 1),
('SKILL_FIGHT_002', '擒拿技', '格鬥', NULL, '控制', 18, 3500, 15, 1, '限制敵人行動', 12, 1),
('SKILL_FIGHT_003', '反擊拳', '格鬥', NULL, '攻擊', 12, 2500, 45, 1, '反擊時造成額外傷害', 18, 1),

-- 氣功系技能
('SKILL_QIGONG_001', '基礎治療', '氣功', NULL, '治療', 8, 1200, -30, 3, '回復友軍生命值', 1, 1),
('SKILL_QIGONG_002', '群體治療', '氣功', NULL, '治療', 25, 4500, -50, 5, '回復多個友軍生命值', 20, 1),
('SKILL_QIGONG_003', '強化祝福', '氣功', NULL, '輔助', 15, 3000, 0, 4, '提升友軍攻擊力', 10, 1);

-- 插入範例AI角色
INSERT INTO [AiStates] VALUES
('AI_HOLY_SWORD_001', '聖劍騎士', '聖門', '劍術', 25, 15000, 1, 100, 150, 0, '戰鬥', 450, 450, 180, 180, 65, 55, 45, 30, GETDATE(), 1, '正義', '穩健攻擊', '正義之劍，斬破黑暗！'),
('AI_MYSTIC_ARCH_001', '元素射手', '懸岩', '弓術', 23, 12500, 2, 200, 180, 0, '待命', 380, 380, 220, 220, 58, 42, 68, 75, GETDATE(), 1, '冷靜', '精準狙擊', '箭矢如星，精準無誤。'),
('AI_PHOENIX_FIGHT_001', '烈焰拳師', '鳳凰', '格鬥', 27, 18000, 1, 80, 120, 0, '戰鬥', 520, 520, 160, 160, 72, 60, 55, 28, GETDATE(), 1, '熱血', '爆發攻擊', '烈火燃燒，拳破千軍！'),
('AI_HOLY_QIGONG_001', '聖光法師', '聖門', '氣功', 24, 14000, 3, 150, 200, 0, '待命', 350, 350, 280, 280, 35, 48, 50, 85, GETDATE(), 1, '慈悲', '團隊支援', '聖光普照，眾生得救。');

-- 建立索引以提升查詢效能
CREATE INDEX IX_AiStates_Academy_Department ON [AiStates]([Academy], [Department]);
CREATE INDEX IX_AiStates_Status ON [AiStates]([Status]);
CREATE INDEX IX_LlmDecisions_AIId_CreatedAt ON [LlmDecisions]([AIId], [CreatedAt]);
CREATE INDEX IX_AiTeams_Academy ON [AiTeams]([Academy]);
CREATE INDEX IX_BattleRecords_StartTime ON [BattleRecords]([StartTime]);

-- =====================================================
-- 檢查資料完整性的預存程序
-- =====================================================
GO
CREATE PROCEDURE [dbo].[ValidateFourDepartments]
AS
BEGIN
    -- 檢查是否有非法部門
    IF EXISTS (SELECT 1 FROM [AiStates] WHERE [Department] NOT IN ('劍術','弓術','格鬥','氣功'))
    BEGIN
        RAISERROR('發現非法部門！只允許：劍術、弓術、格鬥、氣功', 16, 1)
        RETURN
    END
    
    -- 檢查是否有非法學院
    IF EXISTS (SELECT 1 FROM [AiStates] WHERE [Academy] NOT IN ('聖門','懸岩','鳳凰'))
    BEGIN
        RAISERROR('發現非法學院！只允許：聖門、懸岩、鳳凰', 16, 1)
        RETURN
    END
    
    -- 顯示統計資訊
    SELECT 
        [Academy] AS '學院',
        [Department] AS '部門',
        COUNT(*) AS 'AI數量'
    FROM [AiStates] 
    WHERE [IsActive] = 1
    GROUP BY [Academy], [Department]
    ORDER BY [Academy], [Department]
    
    PRINT '✅ 四部門資料驗證通過！'
END
GO

-- 執行驗證
EXEC [dbo].[ValidateFourDepartments]
