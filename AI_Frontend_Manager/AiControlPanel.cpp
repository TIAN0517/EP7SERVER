/**
 * @file AiControlPanel.cpp
 * @brief RANOnline EP7 AI系统 - AI控制面板实现文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "AiControlPanel.h"
#include <QtWidgets/QMessageBox>
#include <QtCore/QDebug>

/**
 * @brief 构造函数
 */
AiControlPanel::AiControlPanel(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_aiCountSpinBox(nullptr)
    , m_schoolComboBox(nullptr)
    , m_levelSlider(nullptr)
    , m_levelLabel(nullptr)
    , m_generateButton(nullptr)
    , m_startAllButton(nullptr)
    , m_stopAllButton(nullptr)
    , m_deleteAllButton(nullptr)
    , m_aiListWidget(nullptr)
    , m_progressBar(nullptr)
    , m_antiLagCheckBox(nullptr)
    , m_aggressionSlider(nullptr)
    , m_intelligenceSlider(nullptr)
    , m_socialSlider(nullptr)
    , m_updateTimer(new QTimer(this))
{
    initializeUI();
    setupCyberpunkStyle();
    connectSignalsAndSlots();
    
    // 启动定时器更新状态
    m_updateTimer->start(2000); // 每2秒更新一次
}

/**
 * @brief 析构函数
 */
AiControlPanel::~AiControlPanel()
{
    // 自动清理由Qt管理
}

/**
 * @brief 初始化控制面板UI
 */
void AiControlPanel::initializeUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(15);
    
    // 创建AI生成区域
    createAiGenerationSection();
    
    // 创建批量操作区域
    createBatchOperationSection();
    
    // 创建AI列表区域
    createAiListSection();
    
    // 创建AI配置区域
    createAiConfigSection();
    
    // 创建进度显示
    createProgressSection();
    
    setLayout(m_mainLayout);
}

/**
 * @brief 创建AI生成区域
 */
void AiControlPanel::createAiGenerationSection()
{
    QGroupBox* genGroupBox = new QGroupBox("🚀 AI 生成器");
    genGroupBox->setObjectName("aiGenGroupBox");
    
    QGridLayout* genLayout = new QGridLayout(genGroupBox);
    
    // AI数量设置
    QLabel* countLabel = new QLabel("AI数量:");
    countLabel->setObjectName("configLabel");
    m_aiCountSpinBox = new QSpinBox;
    m_aiCountSpinBox->setRange(1, 1000);
    m_aiCountSpinBox->setValue(10);
    m_aiCountSpinBox->setSuffix(" 个");
    m_aiCountSpinBox->setObjectName("aiCountSpinBox");
    
    // 学校选择
    QLabel* schoolLabel = new QLabel("学校类型:");
    schoolLabel->setObjectName("configLabel");
    m_schoolComboBox = new QComboBox;
    m_schoolComboBox->addItems({
        "极限学园 (Extreme School)",
        "圣飞斯学园 (Holy Spirit School)", 
        "炎魔学园 (Fire Demon School)",
        "随机分配 (Random)"
    });
    m_schoolComboBox->setObjectName("schoolComboBox");
    
    // 等级设置
    QLabel* levelLabel = new QLabel("初始等级:");
    levelLabel->setObjectName("configLabel");
    m_levelSlider = new QSlider(Qt::Horizontal);
    m_levelSlider->setRange(1, 200);
    m_levelSlider->setValue(50);
    m_levelSlider->setObjectName("levelSlider");
    
    m_levelLabel = new QLabel("50级");
    m_levelLabel->setObjectName("levelValueLabel");
    m_levelLabel->setFixedWidth(60);
    
    // 生成按钮
    m_generateButton = new QPushButton("🤖 生成 AI 玩家");
    m_generateButton->setObjectName("generateButton");
    m_generateButton->setFixedHeight(40);
    
    // 布局设置
    genLayout->addWidget(countLabel, 0, 0);
    genLayout->addWidget(m_aiCountSpinBox, 0, 1);
    genLayout->addWidget(schoolLabel, 1, 0);
    genLayout->addWidget(m_schoolComboBox, 1, 1);
    genLayout->addWidget(levelLabel, 2, 0);
    
    QHBoxLayout* levelLayout = new QHBoxLayout;
    levelLayout->addWidget(m_levelSlider);
    levelLayout->addWidget(m_levelLabel);
    genLayout->addLayout(levelLayout, 2, 1);
    
    genLayout->addWidget(m_generateButton, 3, 0, 1, 2);
    
    m_mainLayout->addWidget(genGroupBox);
}

/**
 * @brief 创建批量操作区域
 */
void AiControlPanel::createBatchOperationSection()
{
    QGroupBox* batchGroupBox = new QGroupBox("⚡ 批量操作");
    batchGroupBox->setObjectName("batchGroupBox");
    
    QHBoxLayout* batchLayout = new QHBoxLayout(batchGroupBox);
    
    m_startAllButton = new QPushButton("▶️ 启动全部");
    m_stopAllButton = new QPushButton("⏸️ 停止全部");
    m_deleteAllButton = new QPushButton("🗑️ 删除全部");
    
    m_startAllButton->setObjectName("startAllButton");
    m_stopAllButton->setObjectName("stopAllButton");
    m_deleteAllButton->setObjectName("deleteAllButton");
    
    // 设置统一高度
    int buttonHeight = 35;
    m_startAllButton->setFixedHeight(buttonHeight);
    m_stopAllButton->setFixedHeight(buttonHeight);
    m_deleteAllButton->setFixedHeight(buttonHeight);
    
    batchLayout->addWidget(m_startAllButton);
    batchLayout->addWidget(m_stopAllButton);
    batchLayout->addWidget(m_deleteAllButton);
    
    m_mainLayout->addWidget(batchGroupBox);
}

/**
 * @brief 创建AI列表区域
 */
void AiControlPanel::createAiListSection()
{
    QGroupBox* listGroupBox = new QGroupBox("📋 AI 实例列表");
    listGroupBox->setObjectName("aiListGroupBox");
    
    QVBoxLayout* listLayout = new QVBoxLayout(listGroupBox);
    
    m_aiListWidget = new QListWidget;
    m_aiListWidget->setObjectName("aiListWidget");
    m_aiListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_aiListWidget->setFixedHeight(200);
    
    // 添加一些示例AI（实际应该从后端获取）
    for (int i = 1; i <= 5; ++i) {
        QString aiInfo = QString("AI_%1 - 极限学园 - Lv.%2 - 状态: 离线")
                        .arg(i, 3, 10, QChar('0'))
                        .arg(45 + i * 3);
        m_aiListWidget->addItem(aiInfo);
    }
    
    // 统计标签
    m_aiStatsLabel = new QLabel("总计: 5个AI | 在线: 0个 | 离线: 5个");
    m_aiStatsLabel->setObjectName("statsLabel");
    
    listLayout->addWidget(m_aiListWidget);
    listLayout->addWidget(m_aiStatsLabel);
    
    m_mainLayout->addWidget(listGroupBox);
}

/**
 * @brief 创建AI配置区域
 */
void AiControlPanel::createAiConfigSection()
{
    QGroupBox* configGroupBox = new QGroupBox("⚙️ AI 行为配置");
    configGroupBox->setObjectName("aiConfigGroupBox");
    
    QVBoxLayout* configLayout = new QVBoxLayout(configGroupBox);
    
    // 防卡顿选项
    m_antiLagCheckBox = new QCheckBox("启用防卡顿模式");
    m_antiLagCheckBox->setObjectName("antiLagCheckBox");
    m_antiLagCheckBox->setChecked(true);
    
    // 攻击性设置
    QLabel* aggressionLabel = new QLabel("攻击性:");
    aggressionLabel->setObjectName("configLabel");
    m_aggressionSlider = new QSlider(Qt::Horizontal);
    m_aggressionSlider->setRange(1, 100);
    m_aggressionSlider->setValue(70);
    m_aggressionSlider->setObjectName("aggressionSlider");
    
    m_aggressionValueLabel = new QLabel("70%");
    m_aggressionValueLabel->setObjectName("sliderValueLabel");
    
    // 智能程度设置
    QLabel* intelligenceLabel = new QLabel("智能程度:");
    intelligenceLabel->setObjectName("configLabel");
    m_intelligenceSlider = new QSlider(Qt::Horizontal);
    m_intelligenceSlider->setRange(1, 100);
    m_intelligenceSlider->setValue(85);
    m_intelligenceSlider->setObjectName("intelligenceSlider");
    
    m_intelligenceValueLabel = new QLabel("85%");
    m_intelligenceValueLabel->setObjectName("sliderValueLabel");
    
    // 社交程度设置
    QLabel* socialLabel = new QLabel("社交程度:");
    socialLabel->setObjectName("configLabel");
    m_socialSlider = new QSlider(Qt::Horizontal);
    m_socialSlider->setRange(1, 100);
    m_socialSlider->setValue(60);
    m_socialSlider->setObjectName("socialSlider");
    
    m_socialValueLabel = new QLabel("60%");
    m_socialValueLabel->setObjectName("sliderValueLabel");
    
    // 布局配置
    configLayout->addWidget(m_antiLagCheckBox);
    
    QHBoxLayout* aggressionLayout = new QHBoxLayout;
    aggressionLayout->addWidget(aggressionLabel);
    aggressionLayout->addWidget(m_aggressionSlider);
    aggressionLayout->addWidget(m_aggressionValueLabel);
    configLayout->addLayout(aggressionLayout);
    
    QHBoxLayout* intelligenceLayout = new QHBoxLayout;
    intelligenceLayout->addWidget(intelligenceLabel);
    intelligenceLayout->addWidget(m_intelligenceSlider);
    intelligenceLayout->addWidget(m_intelligenceValueLabel);
    configLayout->addLayout(intelligenceLayout);
    
    QHBoxLayout* socialLayout = new QHBoxLayout;
    socialLayout->addWidget(socialLabel);
    socialLayout->addWidget(m_socialSlider);
    socialLayout->addWidget(m_socialValueLabel);
    configLayout->addLayout(socialLayout);
    
    m_mainLayout->addWidget(configGroupBox);
}

/**
 * @brief 创建进度显示区域
 */
void AiControlPanel::createProgressSection()
{
    m_progressBar = new QProgressBar;
    m_progressBar->setObjectName("aiProgressBar");
    m_progressBar->setVisible(false);
    m_progressBar->setFixedHeight(25);
    
    m_mainLayout->addWidget(m_progressBar);
    m_mainLayout->addStretch(); // 底部弹性空间
}

/**
 * @brief 连接信号和槽
 */
void AiControlPanel::connectSignalsAndSlots()
{
    // 按钮连接
    connect(m_generateButton, &QPushButton::clicked, this, &AiControlPanel::generateAiPlayers);
    connect(m_startAllButton, &QPushButton::clicked, this, &AiControlPanel::startSelectedAi);
    connect(m_stopAllButton, &QPushButton::clicked, this, &AiControlPanel::stopSelectedAi);
    connect(m_deleteAllButton, &QPushButton::clicked, this, &AiControlPanel::deleteSelectedAi);
    
    // 滑块值变化
    connect(m_levelSlider, &QSlider::valueChanged, this, &AiControlPanel::updateLevelDisplay);
    connect(m_aggressionSlider, &QSlider::valueChanged, this, &AiControlPanel::updateAggressionDisplay);
    connect(m_intelligenceSlider, &QSlider::valueChanged, this, &AiControlPanel::updateIntelligenceDisplay);
    connect(m_socialSlider, &QSlider::valueChanged, this, &AiControlPanel::updateSocialDisplay);
    
    // 定时器更新
    connect(m_updateTimer, &QTimer::timeout, this, &AiControlPanel::updateAiStatus);
    
    // 配置变化信号
    connect(m_antiLagCheckBox, &QCheckBox::toggled, this, &AiControlPanel::configurationChanged);
    connect(m_aggressionSlider, &QSlider::valueChanged, this, &AiControlPanel::configurationChanged);
    connect(m_intelligenceSlider, &QSlider::valueChanged, this, &AiControlPanel::configurationChanged);
    connect(m_socialSlider, &QSlider::valueChanged, this, &AiControlPanel::configurationChanged);
}

/**
 * @brief 设置科技感样式
 */
void AiControlPanel::setupCyberpunkStyle()
{
    setStyleSheet(R"(
        /* AI控制面板整体样式 */
        AiControlPanel {
            background: transparent;
        }
        
        /* 分组框样式 */
        QGroupBox {
            font-weight: bold;
            font-size: 14px;
            color: #00FFAA;
            border: 2px solid #0096FF;
            border-radius: 8px;
            margin: 10px 0px;
            padding-top: 20px;
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(0, 150, 255, 20),
                stop:1 rgba(0, 255, 170, 10));
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 8px 0 8px;
            color: #00FFAA;
            font-weight: bold;
        }
        
        /* 标签样式 */
        QLabel#configLabel {
            color: #FFFFFF;
            font-weight: bold;
            font-size: 12px;
        }
        
        QLabel#levelValueLabel, 
        QLabel#sliderValueLabel {
            color: #00FFAA;
            font-weight: bold;
            font-size: 12px;
            min-width: 40px;
        }
        
        QLabel#statsLabel {
            color: #CCCCCC;
            font-size: 11px;
            padding: 5px;
            background: rgba(0, 0, 0, 50);
            border-radius: 4px;
        }
        
        /* 数字框样式 */
        QSpinBox {
            background: rgba(0, 0, 0, 100);
            border: 2px solid #0096FF;
            border-radius: 6px;
            color: #FFFFFF;
            font-weight: bold;
            padding: 5px;
            selection-background-color: #00FFAA;
        }
        
        QSpinBox:focus {
            border: 2px solid #00FFAA;
            box-shadow: 0 0 10px rgba(0, 255, 170, 100);
        }
        
        QSpinBox::up-button, QSpinBox::down-button {
            background: #0096FF;
            border: none;
            width: 16px;
        }
        
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background: #00FFAA;
        }
        
        /* 下拉框样式 */
        QComboBox {
            background: rgba(0, 0, 0, 100);
            border: 2px solid #0096FF;
            border-radius: 6px;
            color: #FFFFFF;
            font-weight: bold;
            padding: 5px;
            min-height: 20px;
        }
        
        QComboBox:focus {
            border: 2px solid #00FFAA;
        }
        
        QComboBox::drop-down {
            background: #0096FF;
            border: none;
            width: 20px;
        }
        
        QComboBox::down-arrow {
            image: url();
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 6px solid #FFFFFF;
        }
        
        QComboBox QAbstractItemView {
            background: rgba(0, 0, 0, 200);
            border: 2px solid #0096FF;
            selection-background-color: #00FFAA;
            color: #FFFFFF;
        }
        
        /* 滑块样式 */
        QSlider::groove:horizontal {
            background: rgba(0, 0, 0, 100);
            height: 6px;
            border-radius: 3px;
            border: 1px solid #0096FF;
        }
        
        QSlider::handle:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #00FFAA, stop:1 #0096FF);
            border: 2px solid #FFFFFF;
            width: 16px;
            height: 16px;
            border-radius: 10px;
            margin: -6px 0;
        }
        
        QSlider::handle:horizontal:hover {
            background: #00FFAA;
            box-shadow: 0 0 10px rgba(0, 255, 170, 150);
        }
        
        QSlider::sub-page:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #0096FF, stop:1 #00FFAA);
            border-radius: 3px;
        }
        
        /* 按钮样式 */
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #0096FF, stop:1 #0066CC);
            border: 2px solid #0096FF;
            border-radius: 8px;
            color: #FFFFFF;
            font-weight: bold;
            font-size: 12px;
            padding: 8px 16px;
            min-height: 25px;
        }
        
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #00FFAA, stop:1 #00CC88);
            border: 2px solid #00FFAA;
            box-shadow: 0 0 15px rgba(0, 255, 170, 100);
        }
        
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #0066CC, stop:1 #004499);
            border: 2px solid #0066CC;
        }
        
        QPushButton#generateButton {
            font-size: 14px;
            font-weight: bold;
            min-height: 35px;
        }
        
        QPushButton#deleteAllButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #FF4444, stop:1 #CC2222);
            border: 2px solid #FF4444;
        }
        
        QPushButton#deleteAllButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #FF6666, stop:1 #DD3333);
            box-shadow: 0 0 15px rgba(255, 68, 68, 100);
        }
        
        /* 列表样式 */
        QListWidget {
            background: rgba(0, 0, 0, 150);
            border: 2px solid #0096FF;
            border-radius: 6px;
            color: #FFFFFF;
            font-family: "Consolas", monospace;
            font-size: 11px;
            selection-background-color: rgba(0, 255, 170, 100);
            selection-color: #000000;
        }
        
        QListWidget::item {
            padding: 4px;
            border-bottom: 1px solid rgba(0, 150, 255, 50);
        }
        
        QListWidget::item:hover {
            background: rgba(0, 150, 255, 50);
        }
        
        QListWidget::item:selected {
            background: rgba(0, 255, 170, 150);
            color: #000000;
        }
        
        /* 复选框样式 */
        QCheckBox {
            color: #FFFFFF;
            font-weight: bold;
            spacing: 8px;
        }
        
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 2px solid #0096FF;
            border-radius: 3px;
            background: rgba(0, 0, 0, 100);
        }
        
        QCheckBox::indicator:checked {
            background: #00FFAA;
            image: url();
        }
        
        QCheckBox::indicator:checked::after {
            content: "✓";
            color: #000000;
            font-weight: bold;
        }
        
        /* 进度条样式 */
        QProgressBar {
            background: rgba(0, 0, 0, 150);
            border: 2px solid #0096FF;
            border-radius: 6px;
            text-align: center;
            color: #FFFFFF;
            font-weight: bold;
        }
        
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #0096FF, stop:1 #00FFAA);
            border-radius: 4px;
            margin: 2px;
        }
    )");
}

/**
 * @brief 生成AI玩家
 */
void AiControlPanel::generateAiPlayers()
{
    int count = m_aiCountSpinBox->value();
    QString school = m_schoolComboBox->currentText();
    int level = m_levelSlider->value();
    
    // 显示进度条
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, count);
    m_progressBar->setValue(0);
    
    // 发送生成请求信号
    emit aiGenerationRequested(count, school, level);
    
    // 模拟生成过程（实际应该与后端通信）
    qDebug() << QString("生成 %1 个AI玩家 - 学校: %2 - 等级: %3")
                .arg(count).arg(school).arg(level);
    
    // 这里应该启动实际的AI生成过程
    QTimer::singleShot(2000, this, [this]() {
        m_progressBar->setVisible(false);
        updateAiList();
        QMessageBox::information(this, "成功", "AI玩家生成完成！");
    });
}

/**
 * @brief 启动选中的AI
 */
void AiControlPanel::startSelectedAi()
{
    QStringList selectedAi;
    for (auto item : m_aiListWidget->selectedItems()) {
        selectedAi << item->text();
    }
    
    if (selectedAi.isEmpty()) {
        // 启动全部AI
        emit batchOperationRequested("start_all", QStringList());
        qDebug() << "启动全部AI";
    } else {
        emit batchOperationRequested("start", selectedAi);
        qDebug() << "启动选中的AI:" << selectedAi;
    }
}

/**
 * @brief 停止选中的AI
 */
void AiControlPanel::stopSelectedAi()
{
    QStringList selectedAi;
    for (auto item : m_aiListWidget->selectedItems()) {
        selectedAi << item->text();
    }
    
    if (selectedAi.isEmpty()) {
        // 停止全部AI
        emit batchOperationRequested("stop_all", QStringList());
        qDebug() << "停止全部AI";
    } else {
        emit batchOperationRequested("stop", selectedAi);
        qDebug() << "停止选中的AI:" << selectedAi;
    }
}

/**
 * @brief 删除选中的AI
 */
void AiControlPanel::deleteSelectedAi()
{
    QStringList selectedAi;
    for (auto item : m_aiListWidget->selectedItems()) {
        selectedAi << item->text();
    }
    
    if (selectedAi.isEmpty()) {
        // 删除全部AI
        int ret = QMessageBox::warning(this, "警告", 
            "确定要删除所有AI吗？此操作不可撤销！",
            QMessageBox::Yes | QMessageBox::No);
        
        if (ret == QMessageBox::Yes) {
            emit batchOperationRequested("delete_all", QStringList());
            qDebug() << "删除全部AI";
        }
    } else {
        int ret = QMessageBox::warning(this, "警告", 
            QString("确定要删除选中的 %1 个AI吗？此操作不可撤销！").arg(selectedAi.size()),
            QMessageBox::Yes | QMessageBox::No);
        
        if (ret == QMessageBox::Yes) {
            emit batchOperationRequested("delete", selectedAi);
            qDebug() << "删除选中的AI:" << selectedAi;
        }
    }
}

/**
 * @brief 更新等级显示
 */
void AiControlPanel::updateLevelDisplay(int value)
{
    m_levelLabel->setText(QString("%1级").arg(value));
}

/**
 * @brief 更新攻击性显示
 */
void AiControlPanel::updateAggressionDisplay(int value)
{
    m_aggressionValueLabel->setText(QString("%1%").arg(value));
}

/**
 * @brief 更新智能程度显示
 */
void AiControlPanel::updateIntelligenceDisplay(int value)
{
    m_intelligenceValueLabel->setText(QString("%1%").arg(value));
}

/**
 * @brief 更新社交程度显示
 */
void AiControlPanel::updateSocialDisplay(int value)
{
    m_socialValueLabel->setText(QString("%1%").arg(value));
}

/**
 * @brief 配置变化处理
 */
void AiControlPanel::configurationChanged()
{
    QVariantMap config;
    config["antiLag"] = m_antiLagCheckBox->isChecked();
    config["aggression"] = m_aggressionSlider->value();
    config["intelligence"] = m_intelligenceSlider->value();
    config["social"] = m_socialSlider->value();
    
    emit aiConfigChanged(config);
    
    qDebug() << "AI配置已更新:" << config;
}

/**
 * @brief 更新AI状态
 */
void AiControlPanel::updateAiStatus()
{
    // 这里应该从后端获取AI状态并更新列表
    // 当前是模拟数据
    
    static int onlineCount = 0;
    onlineCount = (onlineCount + 1) % 6; // 模拟状态变化
    
    int totalCount = m_aiListWidget->count();
    int offlineCount = totalCount - onlineCount;
    
    m_aiStatsLabel->setText(QString("总计: %1个AI | 在线: %2个 | 离线: %3个")
                           .arg(totalCount).arg(onlineCount).arg(offlineCount));
}

/**
 * @brief 更新AI列表
 */
void AiControlPanel::updateAiList()
{
    // 这里应该从后端获取AI列表并更新
    // 当前添加一些示例数据
    
    int currentCount = m_aiListWidget->count();
    int newCount = m_aiCountSpinBox->value();
    
    for (int i = currentCount + 1; i <= currentCount + newCount; ++i) {
        QString aiInfo = QString("AI_%1 - %2 - Lv.%3 - 状态: 离线")
                        .arg(i, 3, 10, QChar('0'))
                        .arg(m_schoolComboBox->currentText().split(' ').first())
                        .arg(m_levelSlider->value() + (i % 10));
        m_aiListWidget->addItem(aiInfo);
    }
}

/**
 * @brief 获取AI配置
 */
QVariantMap AiControlPanel::getAiConfig() const
{
    QVariantMap config;
    config["count"] = m_aiCountSpinBox->value();
    config["school"] = m_schoolComboBox->currentText();
    config["level"] = m_levelSlider->value();
    config["antiLag"] = m_antiLagCheckBox->isChecked();
    config["aggression"] = m_aggressionSlider->value();
    config["intelligence"] = m_intelligenceSlider->value();
    config["social"] = m_socialSlider->value();
    
    return config;
}

/**
 * @brief 设置AI配置
 */
void AiControlPanel::setAiConfig(const QVariantMap& config)
{
    if (config.contains("count")) {
        m_aiCountSpinBox->setValue(config["count"].toInt());
    }
    
    if (config.contains("school")) {
        QString school = config["school"].toString();
        int index = m_schoolComboBox->findText(school);
        if (index >= 0) {
            m_schoolComboBox->setCurrentIndex(index);
        }
    }
    
    if (config.contains("level")) {
        m_levelSlider->setValue(config["level"].toInt());
    }
    
    if (config.contains("antiLag")) {
        m_antiLagCheckBox->setChecked(config["antiLag"].toBool());
    }
    
    if (config.contains("aggression")) {
        m_aggressionSlider->setValue(config["aggression"].toInt());
    }
    
    if (config.contains("intelligence")) {
        m_intelligenceSlider->setValue(config["intelligence"].toInt());
    }
    
    if (config.contains("social")) {
        m_socialSlider->setValue(config["social"].toInt());
    }
}
