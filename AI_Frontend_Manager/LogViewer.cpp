/**
 * @file LogViewer.cpp
 * @brief RANOnline EP7 AI系统 - 日志查看器实现文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "LogViewer.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTextStream>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QDebug>

/**
 * @brief 构造函数
 */
LogViewer::LogViewer(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_toolbarLayout(nullptr)
    , m_levelFilterCombo(nullptr)
    , m_searchEdit(nullptr)
    , m_searchButton(nullptr)
    , m_clearButton(nullptr)
    , m_exportButton(nullptr)
    , m_autoScrollCheckBox(nullptr)
    , m_logDisplay(nullptr)
    , m_statusLayout(nullptr)
    , m_statusLabel(nullptr)
    , m_countLabel(nullptr)
    , m_currentFilter(LogLevel::DEBUG)
    , m_autoScroll(true)
    , m_maxLogEntries(10000)
    , m_refreshTimer(new QTimer(this))
    , m_fileWatcher(new QFileSystemWatcher(this))
{
    initializeUI();
    setupCyberpunkStyle();
    connectSignalsAndSlots();
    
    // 启动刷新定时器
    m_refreshTimer->start(1000); // 每秒刷新一次
    
    // 添加一些初始日志
    addInfoLog("RANOnline EP7 AI系统启动", "System");
    addInfoLog("日志查看器初始化完成", "LogViewer");
    addDebugLog("调试模式已启用", "System");
}

/**
 * @brief 析构函数
 */
LogViewer::~LogViewer()
{
    // Qt自动清理
}

/**
 * @brief 初始化日志查看器UI
 */
void LogViewer::initializeUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(5, 5, 5, 5);
    m_mainLayout->setSpacing(8);
    
    // 创建各个区域
    createToolbar();
    createLogDisplay();
    createStatusBar();
    
    setLayout(m_mainLayout);
}

/**
 * @brief 创建工具栏
 */
void LogViewer::createToolbar()
{
    QGroupBox* toolbarGroup = new QGroupBox("🔧 日志控制");
    toolbarGroup->setObjectName("toolbarGroup");
    
    m_toolbarLayout = new QHBoxLayout(toolbarGroup);
    
    // 级别过滤器
    QLabel* filterLabel = new QLabel("级别:");
    filterLabel->setObjectName("toolLabel");
    
    m_levelFilterCombo = new QComboBox;
    m_levelFilterCombo->setObjectName("levelFilterCombo");
    m_levelFilterCombo->addItems({
        "全部", "调试", "信息", "警告", "错误", "关键"
    });
    m_levelFilterCombo->setCurrentIndex(0);
    
    // 搜索框
    QLabel* searchLabel = new QLabel("搜索:");
    searchLabel->setObjectName("toolLabel");
    
    m_searchEdit = new QLineEdit;
    m_searchEdit->setObjectName("searchEdit");
    m_searchEdit->setPlaceholderText("输入关键词...");
    m_searchEdit->setFixedWidth(150);
    
    m_searchButton = new QPushButton("🔍");
    m_searchButton->setObjectName("searchButton");
    m_searchButton->setFixedSize(30, 30);
    
    // 控制按钮
    m_clearButton = new QPushButton("🗑️ 清空");
    m_clearButton->setObjectName("clearButton");
    
    m_exportButton = new QPushButton("💾 导出");
    m_exportButton->setObjectName("exportButton");
    
    // 自动滚动选项
    m_autoScrollCheckBox = new QCheckBox("自动滚动");
    m_autoScrollCheckBox->setObjectName("autoScrollCheck");
    m_autoScrollCheckBox->setChecked(true);
    
    // 布局
    m_toolbarLayout->addWidget(filterLabel);
    m_toolbarLayout->addWidget(m_levelFilterCombo);
    m_toolbarLayout->addWidget(searchLabel);
    m_toolbarLayout->addWidget(m_searchEdit);
    m_toolbarLayout->addWidget(m_searchButton);
    m_toolbarLayout->addStretch();
    m_toolbarLayout->addWidget(m_autoScrollCheckBox);
    m_toolbarLayout->addWidget(m_clearButton);
    m_toolbarLayout->addWidget(m_exportButton);
    
    m_mainLayout->addWidget(toolbarGroup);
}

/**
 * @brief 创建日志显示区域
 */
void LogViewer::createLogDisplay()
{
    m_logDisplay = new QTextEdit;
    m_logDisplay->setObjectName("logDisplay");
    m_logDisplay->setReadOnly(true);
    m_logDisplay->setFont(QFont("Consolas", 9));
    m_logDisplay->setLineWrapMode(QTextEdit::WidgetWidth);
    
    // 设置滚动条样式
    m_logDisplay->verticalScrollBar()->setObjectName("logScrollBar");
    
    m_mainLayout->addWidget(m_logDisplay);
}

/**
 * @brief 创建状态栏
 */
void LogViewer::createStatusBar()
{
    QWidget* statusWidget = new QWidget;
    statusWidget->setObjectName("statusWidget");
    statusWidget->setFixedHeight(25);
    
    m_statusLayout = new QHBoxLayout(statusWidget);
    m_statusLayout->setContentsMargins(10, 2, 10, 2);
    
    m_statusLabel = new QLabel("就绪");
    m_statusLabel->setObjectName("statusLabel");
    
    m_countLabel = new QLabel("日志数: 0");
    m_countLabel->setObjectName("countLabel");
    
    m_statusLayout->addWidget(m_statusLabel);
    m_statusLayout->addStretch();
    m_statusLayout->addWidget(m_countLabel);
    
    m_mainLayout->addWidget(statusWidget);
}

/**
 * @brief 连接信号和槽
 */
void LogViewer::connectSignalsAndSlots()
{
    // 工具栏信号
    connect(m_levelFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogViewer::applyFilter);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &LogViewer::searchLogs);
    connect(m_searchButton, &QPushButton::clicked, this, &LogViewer::searchLogs);
    connect(m_clearButton, &QPushButton::clicked, this, &LogViewer::clearDisplay);
    connect(m_exportButton, &QPushButton::clicked, this, &LogViewer::exportToFile);
    connect(m_autoScrollCheckBox, &QCheckBox::toggled, this, &LogViewer::toggleAutoScroll);
    
    // 定时器
    connect(m_refreshTimer, &QTimer::timeout, this, &LogViewer::refreshDisplay);
}

/**
 * @brief 设置科技感样式
 */
void LogViewer::setupCyberpunkStyle()
{
    setStyleSheet(R"(
        /* 日志查看器整体样式 */
        LogViewer {
            background: transparent;
        }
        
        /* 工具栏分组框 */
        QGroupBox#toolbarGroup {
            font-weight: bold;
            font-size: 12px;
            color: #00FFAA;
            border: 2px solid #0096FF;
            border-radius: 6px;
            margin: 5px 0px;
            padding-top: 15px;
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(0, 150, 255, 15),
                stop:1 rgba(0, 255, 170, 8));
        }
        
        QGroupBox#toolbarGroup::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 6px 0 6px;
            color: #00FFAA;
        }
        
        /* 工具标签 */
        QLabel#toolLabel {
            color: #FFFFFF;
            font-weight: bold;
            font-size: 11px;
            margin-right: 5px;
        }
        
        /* 下拉框样式 */
        QComboBox#levelFilterCombo {
            background: rgba(0, 0, 0, 150);
            border: 2px solid #0096FF;
            border-radius: 4px;
            color: #FFFFFF;
            font-weight: bold;
            padding: 3px 8px;
            min-width: 80px;
        }
        
        QComboBox#levelFilterCombo:focus {
            border: 2px solid #00FFAA;
        }
        
        QComboBox#levelFilterCombo::drop-down {
            background: #0096FF;
            border: none;
            width: 20px;
        }
        
        QComboBox#levelFilterCombo QAbstractItemView {
            background: rgba(0, 0, 0, 200);
            border: 2px solid #0096FF;
            selection-background-color: #00FFAA;
            color: #FFFFFF;
        }
        
        /* 搜索框样式 */
        QLineEdit#searchEdit {
            background: rgba(0, 0, 0, 150);
            border: 2px solid #0096FF;
            border-radius: 4px;
            color: #FFFFFF;
            font-weight: bold;
            padding: 3px 8px;
            selection-background-color: #00FFAA;
        }
        
        QLineEdit#searchEdit:focus {
            border: 2px solid #00FFAA;
            box-shadow: 0 0 8px rgba(0, 255, 170, 80);
        }
        
        /* 按钮样式 */
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #0096FF, stop:1 #0066CC);
            border: 2px solid #0096FF;
            border-radius: 4px;
            color: #FFFFFF;
            font-weight: bold;
            font-size: 11px;
            padding: 4px 12px;
            min-height: 20px;
        }
        
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #00FFAA, stop:1 #00CC88);
            border: 2px solid #00FFAA;
            box-shadow: 0 0 10px rgba(0, 255, 170, 80);
        }
        
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #0066CC, stop:1 #004499);
        }
        
        QPushButton#searchButton {
            min-width: 25px;
            padding: 2px;
        }
        
        QPushButton#clearButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #FF6B6B, stop:1 #CC4444);
            border-color: #FF6B6B;
        }
        
        QPushButton#clearButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #FF8888, stop:1 #DD5555);
            box-shadow: 0 0 10px rgba(255, 107, 107, 80);
        }
        
        /* 复选框样式 */
        QCheckBox#autoScrollCheck {
            color: #FFFFFF;
            font-weight: bold;
            font-size: 11px;
            spacing: 6px;
        }
        
        QCheckBox#autoScrollCheck::indicator {
            width: 14px;
            height: 14px;
            border: 2px solid #0096FF;
            border-radius: 2px;
            background: rgba(0, 0, 0, 100);
        }
        
        QCheckBox#autoScrollCheck::indicator:checked {
            background: #00FFAA;
        }
        
        /* 日志显示区域 */
        QTextEdit#logDisplay {
            background: rgba(0, 0, 0, 200);
            border: 2px solid #0096FF;
            border-radius: 6px;
            color: #FFFFFF;
            font-family: "Consolas", "Monaco", monospace;
            font-size: 9px;
            selection-background-color: rgba(0, 255, 170, 100);
            selection-color: #000000;
        }
        
        /* 滚动条样式 */
        QScrollBar#logScrollBar:vertical {
            background: rgba(0, 0, 0, 100);
            width: 12px;
            border-radius: 6px;
            border: 1px solid #0096FF;
        }
        
        QScrollBar#logScrollBar::handle:vertical {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #0096FF, stop:1 #00FFAA);
            border-radius: 5px;
            min-height: 20px;
        }
        
        QScrollBar#logScrollBar::handle:vertical:hover {
            background: #00FFAA;
        }
        
        QScrollBar#logScrollBar::add-line:vertical,
        QScrollBar#logScrollBar::sub-line:vertical {
            height: 0px;
        }
        
        /* 状态栏样式 */
        QWidget#statusWidget {
            background: rgba(0, 0, 0, 100);
            border: 1px solid #0096FF;
            border-radius: 4px;
        }
        
        QLabel#statusLabel {
            color: #00FFAA;
            font-weight: bold;
            font-size: 11px;
        }
        
        QLabel#countLabel {
            color: #CCCCCC;
            font-size: 10px;
        }
    )");
}

/**
 * @brief 添加日志条目
 */
void LogViewer::addLogEntry(const LogEntry& entry)
{
    m_logEntries.append(entry);
    
    // 限制日志条目数量
    while (m_logEntries.size() > m_maxLogEntries) {
        m_logEntries.removeFirst();
    }
    
    // 更新计数
    m_countLabel->setText(QString("日志数: %1").arg(m_logEntries.size()));
    
    // 应用过滤并显示
    if (passesFilter(entry)) {
        m_filteredEntries.append(entry);
        
        // 格式化并添加到显示区域
        applyTextFormat(entry);
        
        // 自动滚动到底部
        if (m_autoScroll) {
            QTextCursor cursor = m_logDisplay->textCursor();
            cursor.movePosition(QTextCursor::End);
            m_logDisplay->setTextCursor(cursor);
        }
    }
}

/**
 * @brief 应用过滤器
 */
void LogViewer::applyFilter()
{
    m_currentFilter = static_cast<LogLevel>(m_levelFilterCombo->currentIndex() - 1);
    if (m_levelFilterCombo->currentIndex() == 0) {
        m_currentFilter = LogLevel::DEBUG; // 全部显示
    }
    
    refreshDisplay();
    emit filterChanged(m_currentFilter, m_currentKeyword);
}

/**
 * @brief 搜索日志
 */
void LogViewer::searchLogs()
{
    m_currentKeyword = m_searchEdit->text();
    refreshDisplay();
    
    int matchCount = 0;
    for (const auto& entry : m_filteredEntries) {
        if (entry.message.contains(m_currentKeyword, Qt::CaseInsensitive)) {
            matchCount++;
        }
    }
    
    m_statusLabel->setText(QString("搜索到 %1 条匹配记录").arg(matchCount));
}

/**
 * @brief 切换自动滚动
 */
void LogViewer::toggleAutoScroll(bool enabled)
{
    m_autoScroll = enabled;
    
    if (enabled) {
        QTextCursor cursor = m_logDisplay->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_logDisplay->setTextCursor(cursor);
    }
}

/**
 * @brief 清空日志显示
 */
void LogViewer::clearDisplay()
{
    m_logDisplay->clear();
    m_logEntries.clear();
    m_filteredEntries.clear();
    
    m_countLabel->setText("日志数: 0");
    m_statusLabel->setText("日志已清空");
    
    // 添加清空记录
    addInfoLog("日志已清空", "LogViewer");
}

/**
 * @brief 导出日志文件
 */
void LogViewer::exportToFile()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "导出日志文件",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + 
        "/ranonline_logs_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".txt",
        "文本文件 (*.txt);;所有文件 (*.*)"
    );
    
    if (filename.isEmpty()) {
        return;
    }
    
    if (exportLogs(filename)) {
        m_statusLabel->setText(QString("日志已导出到: %1").arg(QFileInfo(filename).fileName()));
        addInfoLog(QString("日志导出成功: %1").arg(filename), "LogViewer");
    } else {
        m_statusLabel->setText("日志导出失败");
        addErrorLog(QString("日志导出失败: %1").arg(filename), "LogViewer");
    }
}

/**
 * @brief 定时刷新显示
 */
void LogViewer::refreshDisplay()
{
    // 重新应用过滤器
    m_filteredEntries.clear();
    m_logDisplay->clear();
    
    for (const auto& entry : m_logEntries) {
        if (passesFilter(entry)) {
            m_filteredEntries.append(entry);
            applyTextFormat(entry);
        }
    }
    
    // 滚动到底部
    if (m_autoScroll) {
        QTextCursor cursor = m_logDisplay->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_logDisplay->setTextCursor(cursor);
    }
}

/**
 * @brief 格式化日志条目
 */
QString LogViewer::formatLogEntry(const LogEntry& entry)
{
    QString timestamp = entry.timestamp.toString("hh:mm:ss.zzz");
    QString levelName = getLogLevelName(entry.level);
    QString source = entry.source.isEmpty() ? "System" : entry.source;
    
    return QString("[%1] [%2] [%3] %4")
           .arg(timestamp)
           .arg(levelName)
           .arg(source)
           .arg(entry.message);
}

/**
 * @brief 获取日志级别颜色
 */
QColor LogViewer::getLogLevelColor(LogLevel level)
{
    switch (level) {
        case LogLevel::DEBUG:
            return QColor(150, 150, 150);  // 灰色
        case LogLevel::INFO:
            return QColor(100, 200, 255);  // 浅蓝色
        case LogLevel::WARNING:
            return QColor(255, 200, 100);  // 橙色
        case LogLevel::ERROR:
            return QColor(255, 100, 100);  // 红色
        case LogLevel::CRITICAL:
            return QColor(255, 50, 150);   // 洋红色
        default:
            return QColor(255, 255, 255);  // 白色
    }
}

/**
 * @brief 获取日志级别名称
 */
QString LogViewer::getLogLevelName(LogLevel level)
{
    switch (level) {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO ";
        case LogLevel::WARNING:
            return "WARN ";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::CRITICAL:
            return "CRIT ";
        default:
            return "UNKN ";
    }
}

/**
 * @brief 应用文本格式
 */
void LogViewer::applyTextFormat(const LogEntry& entry)
{
    QTextCursor cursor = m_logDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    
    QTextCharFormat format;
    format.setForeground(getLogLevelColor(entry.level));
    
    // 关键日志加粗
    if (entry.level >= LogLevel::ERROR) {
        format.setFontWeight(QFont::Bold);
    }
    
    // 搜索高亮
    QString text = formatLogEntry(entry);
    if (!m_currentKeyword.isEmpty() && 
        text.contains(m_currentKeyword, Qt::CaseInsensitive)) {
        QTextCharFormat highlightFormat = format;
        highlightFormat.setBackground(QColor(255, 255, 0, 100));
        
        // 应用高亮（简化实现）
        format.setBackground(QColor(255, 255, 0, 50));
    }
    
    cursor.setCharFormat(format);
    cursor.insertText(text + "\n");
    
    m_logDisplay->setTextCursor(cursor);
}

/**
 * @brief 过滤日志条目
 */
bool LogViewer::passesFilter(const LogEntry& entry)
{
    // 级别过滤
    if (m_levelFilterCombo->currentIndex() > 0) {
        if (entry.level < m_currentFilter) {
            return false;
        }
    }
    
    // 关键词过滤
    if (!m_currentKeyword.isEmpty()) {
        if (!entry.message.contains(m_currentKeyword, Qt::CaseInsensitive) &&
            !entry.source.contains(m_currentKeyword, Qt::CaseInsensitive)) {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief 导出日志
 */
bool LogViewer::exportLogs(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out.setCodec("UTF-8");
    
    // 写入头部信息
    out << "RANOnline EP7 AI System - Log Export\n";
    out << "Export Time: " << QDateTime::currentDateTime().toString() << "\n";
    out << "Total Entries: " << m_logEntries.size() << "\n";
    out << "=".repeated(80) << "\n\n";
    
    // 写入日志条目
    for (const auto& entry : m_logEntries) {
        out << formatLogEntry(entry) << "\n";
    }
    
    file.close();
    emit exportRequested(filename);
    
    return true;
}

/**
 * @brief 清空日志
 */
void LogViewer::clearLogs()
{
    clearDisplay();
}

// 公共槽函数实现
void LogViewer::addDebugLog(const QString& message, const QString& source)
{
    LogEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.level = LogLevel::DEBUG;
    entry.category = "DEBUG";
    entry.message = message;
    entry.source = source;
    
    addLogEntry(entry);
}

void LogViewer::addInfoLog(const QString& message, const QString& source)
{
    LogEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.level = LogLevel::INFO;
    entry.category = "INFO";
    entry.message = message;
    entry.source = source;
    
    addLogEntry(entry);
}

void LogViewer::addWarningLog(const QString& message, const QString& source)
{
    LogEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.level = LogLevel::WARNING;
    entry.category = "WARNING";
    entry.message = message;
    entry.source = source;
    
    addLogEntry(entry);
}

void LogViewer::addErrorLog(const QString& message, const QString& source)
{
    LogEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.level = LogLevel::ERROR;
    entry.category = "ERROR";
    entry.message = message;
    entry.source = source;
    
    addLogEntry(entry);
}

void LogViewer::addCriticalLog(const QString& message, const QString& source)
{
    LogEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.level = LogLevel::CRITICAL;
    entry.category = "CRITICAL";
    entry.message = message;
    entry.source = source;
    
    addLogEntry(entry);
}
