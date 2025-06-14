/**
 * @file AiControlPanel.h
 * @brief RANOnline EP7 AI系统 - AI控制面板头文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtCore/QTimer>
#include <memory>

/**
 * @class AiControlPanel
 * @brief AI控制面板类
 * 
 * 功能包括:
 * 1. 一键生成AI玩家
 * 2. 批量AI操作（启动/停止/删除）
 * 3. AI参数配置
 * 4. 学校分配管理
 * 5. AI行为模式设置
 */
class AiControlPanel : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit AiControlPanel(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~AiControlPanel();

    /**
     * @brief 初始化控制面板UI
     */
    void initializeUI();
    
    /**
     * @brief 设置科技感样式
     */
    void setupCyberpunkStyle();

signals:
    /**
     * @brief AI生成请求信号
     * @param count AI数量
     * @param school 学校类型
     * @param level 等级范围
     */
    void aiGenerationRequested(int count, const QString& school, int level);
    
    /**
     * @brief 批量操作请求信号
     * @param operation 操作类型
     * @param aiIds AI ID列表
     */
    void batchOperationRequested(const QString& operation, const QStringList& aiIds);
    
    /**
     * @brief AI配置更改信号
     * @param config 配置参数
     */
    void aiConfigChanged(const QVariantMap& config);

private slots:
    /**
     * @brief 生成AI玩家
     */
    void generateAiPlayers();
    
    /**
     * @brief 启动选中的AI
     */
    void startSelectedAi();
    
    /**
     * @brief 停止选中的AI
     */
    void stopSelectedAi();
    
    /**
     * @brief 删除选中的AI
     */
    void deleteSelectedAi();
    
    /**
     * @brief 全选AI
     */
    void selectAllAi();
    
    /**
     * @brief 反选AI
     */
    void invertSelection();
    
    /**
     * @brief 应用AI配置
     */
    void applyAiConfiguration();
    
    /**
     * @brief 更新AI列表
     */
    void updateAiList();

private:
    /**
     * @brief 创建AI生成区域
     */
    void createGenerationArea();
    
    /**
     * @brief 创建AI管理区域
     */
    void createManagementArea();
    
    /**
     * @brief 创建AI配置区域
     */
    void createConfigurationArea();
    
    /**
     * @brief 添加AI到列表
     * @param aiId AI ID
     * @param name AI名称
     * @param school 学校
     * @param level 等级
     * @param status 状态
     */
    void addAiToList(const QString& aiId, const QString& name, 
                     const QString& school, int level, const QString& status);

private:
    // 主布局
    QVBoxLayout* m_mainLayout;
    
    // AI生成区域
    QGroupBox* m_generationGroupBox;
    QSpinBox* m_aiCountSpinBox;           ///< AI数量选择
    QComboBox* m_schoolComboBox;          ///< 学校选择
    QSpinBox* m_minLevelSpinBox;          ///< 最小等级
    QSpinBox* m_maxLevelSpinBox;          ///< 最大等级
    QPushButton* m_generateButton;        ///< 生成按钮
    QProgressBar* m_generationProgress;   ///< 生成进度条
    
    // AI管理区域
    QGroupBox* m_managementGroupBox;
    QListWidget* m_aiListWidget;          ///< AI列表
    QPushButton* m_startButton;           ///< 启动按钮
    QPushButton* m_stopButton;            ///< 停止按钮
    QPushButton* m_deleteButton;          ///< 删除按钮
    QPushButton* m_selectAllButton;       ///< 全选按钮
    QPushButton* m_invertSelButton;       ///< 反选按钮
    QPushButton* m_refreshButton;         ///< 刷新按钮
    
    // AI配置区域
    QGroupBox* m_configGroupBox;
    QSlider* m_aggressionSlider;          ///< 攻击性滑块
    QSlider* m_intelligenceSlider;        ///< 智能度滑块
    QSlider* m_sociabilitySlider;         ///< 社交性滑块
    QComboBox* m_behaviorModeCombo;       ///< 行为模式
    QCheckBox* m_autoLevelUpCheckBox;     ///< 自动升级
    QCheckBox* m_autoQuestCheckBox;       ///< 自动任务
    QCheckBox* m_autoChatCheckBox;        ///< 自动聊天
    QPushButton* m_applyConfigButton;     ///< 应用配置按钮
    
    // 状态标签
    QLabel* m_totalAiLabel;               ///< 总AI数量标签
    QLabel* m_activeAiLabel;              ///< 活跃AI数量标签
    QLabel* m_serverLoadLabel;            ///< 服务器负载标签
    
    // 定时器
    QTimer* m_updateTimer;                ///< 更新定时器
    
    // 样式主题
    struct ControlTheme {
        QColor buttonColor;
        QColor buttonHoverColor;
        QColor buttonPressColor;
        QColor backgroundLight;
        QColor backgroundDark;
        QColor borderColor;
        QColor textColor;
        QColor accentColor;
    } m_theme;
};

#endif // AICONTROLPANEL_H
