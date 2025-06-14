/**
 * @file LogViewer.h
 * @brief RANOnline EP7 AI系统 - 日志查看器头文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QScrollBar>
#include <QtCore/QTimer>
#include <QtCore/QDateTime>
#include <QtCore/QFileSystemWatcher>
#include <QtGui/QTextCursor>
#include <QtGui/QTextCharFormat>
#include <memory>

/**
 * @enum LogLevel
 * @brief 日志级别枚举
 */
enum class LogLevel
{
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

/**
 * @struct LogEntry
 * @brief 日志条目结构
 */
struct LogEntry
{
    QDateTime timestamp;
    LogLevel level;
    QString category;
    QString message;
    QString source;
};

/**
 * @class LogViewer
 * @brief 日志查看器类
 * 
 * 功能包括:
 * 1. 实时日志显示
 * 2. 日志级别过滤
 * 3. 关键词搜索
 * 4. 日志导出
 * 5. 自动滚动和暂停
 * 6. 颜色编码显示
 */
class LogViewer : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit LogViewer(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~LogViewer();

    /**
     * @brief 初始化日志查看器UI
     */
    void initializeUI();
    
    /**
     * @brief 设置科技感样式
     */
    void setupCyberpunkStyle();
    
    /**
     * @brief 添加日志条目
     * @param entry 日志条目
     */
    void addLogEntry(const LogEntry& entry);
    
    /**
     * @brief 清空日志
     */
    void clearLogs();
    
    /**
     * @brief 导出日志
     * @param filename 文件名
     * @return 是否成功
     */
    bool exportLogs(const QString& filename);

signals:
    /**
     * @brief 日志过滤更改信号
     * @param level 过滤级别
     * @param keyword 关键词
     */
    void filterChanged(LogLevel level, const QString& keyword);
    
    /**
     * @brief 日志导出请求信号
     * @param filename 文件名
     */
    void exportRequested(const QString& filename);

public slots:
    /**
     * @brief 添加调试日志
     * @param message 消息内容
     * @param source 来源
     */
    void addDebugLog(const QString& message, const QString& source = "System");
    
    /**
     * @brief 添加信息日志
     * @param message 消息内容
     * @param source 来源
     */
    void addInfoLog(const QString& message, const QString& source = "System");
    
    /**
     * @brief 添加警告日志
     * @param message 消息内容
     * @param source 来源
     */
    void addWarningLog(const QString& message, const QString& source = "System");
    
    /**
     * @brief 添加错误日志
     * @param message 消息内容
     * @param source 来源
     */
    void addErrorLog(const QString& message, const QString& source = "System");
    
    /**
     * @brief 添加关键日志
     * @param message 消息内容
     * @param source 来源
     */
    void addCriticalLog(const QString& message, const QString& source = "System");

private slots:
    /**
     * @brief 应用过滤器
     */
    void applyFilter();
    
    /**
     * @brief 切换自动滚动
     * @param enabled 是否启用
     */
    void toggleAutoScroll(bool enabled);
    
    /**
     * @brief 搜索日志
     */
    void searchLogs();
    
    /**
     * @brief 清空日志显示
     */
    void clearDisplay();
    
    /**
     * @brief 导出日志文件
     */
    void exportToFile();
    
    /**
     * @brief 定时刷新显示
     */
    void refreshDisplay();

private:
    /**
     * @brief 创建工具栏
     */
    void createToolbar();
    
    /**
     * @brief 创建日志显示区域
     */
    void createLogDisplay();
    
    /**
     * @brief 创建状态栏
     */
    void createStatusBar();
    
    /**
     * @brief 连接信号和槽
     */
    void connectSignalsAndSlots();
    
    /**
     * @brief 格式化日志条目
     * @param entry 日志条目
     * @return 格式化后的字符串
     */
    QString formatLogEntry(const LogEntry& entry);
    
    /**
     * @brief 获取日志级别颜色
     * @param level 日志级别
     * @return 颜色
     */
    QColor getLogLevelColor(LogLevel level);
    
    /**
     * @brief 获取日志级别名称
     * @param level 日志级别
     * @return 级别名称
     */
    QString getLogLevelName(LogLevel level);
    
    /**
     * @brief 应用文本格式
     * @param entry 日志条目
     */
    void applyTextFormat(const LogEntry& entry);
    
    /**
     * @brief 过滤日志条目
     * @param entry 日志条目
     * @return 是否通过过滤
     */
    bool passesFilter(const LogEntry& entry);

private:
    // UI组件
    QVBoxLayout* m_mainLayout;
    
    // 工具栏
    QHBoxLayout* m_toolbarLayout;
    QComboBox* m_levelFilterCombo;
    QLineEdit* m_searchEdit;
    QPushButton* m_searchButton;
    QPushButton* m_clearButton;
    QPushButton* m_exportButton;
    QCheckBox* m_autoScrollCheckBox;
    
    // 日志显示区域
    QTextEdit* m_logDisplay;
    
    // 状态栏
    QHBoxLayout* m_statusLayout;
    QLabel* m_statusLabel;
    QLabel* m_countLabel;
    
    // 数据存储
    QVector<LogEntry> m_logEntries;
    QVector<LogEntry> m_filteredEntries;
    
    // 设置
    LogLevel m_currentFilter;
    QString m_currentKeyword;
    bool m_autoScroll;
    int m_maxLogEntries;
    
    // 定时器
    QTimer* m_refreshTimer;
    
    // 文件监视器
    QFileSystemWatcher* m_fileWatcher;
};
