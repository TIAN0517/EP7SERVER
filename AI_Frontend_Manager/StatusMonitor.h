/**
 * @file StatusMonitor.h
 * @brief RANOnline EP7 AI系统 - 状态监控面板头文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLCDNumber>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCore/QTimer>
#include <QtCore/QDateTime>
#include <memory>

QT_CHARTS_USE_NAMESPACE

/**
 * @struct SystemStats
 * @brief 系统统计信息结构
 */
struct SystemStats
{
    double cpuUsage = 0.0;          // CPU使用率
    double memoryUsage = 0.0;       // 内存使用率
    double networkSpeed = 0.0;      // 网络速度
    int aiCount = 0;                // AI总数
    int onlineAiCount = 0;          // 在线AI数
    int serverLoad = 0;             // 服务器负载
    QString status = "Disconnected"; // 连接状态
};

/**
 * @class StatusMonitor
 * @brief 状态监控面板类
 * 
 * 功能包括:
 * 1. 实时系统性能监控
 * 2. AI状态统计显示
 * 3. 服务器连接状态
 * 4. 性能图表显示
 * 5. 警告和错误提示
 */
class StatusMonitor : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit StatusMonitor(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~StatusMonitor();

    /**
     * @brief 初始化监控面板UI
     */
    void initializeUI();
    
    /**
     * @brief 设置科技感样式
     */
    void setupCyberpunkStyle();
    
    /**
     * @brief 更新系统统计信息
     * @param stats 系统统计数据
     */
    void updateSystemStats(const SystemStats& stats);

signals:
    /**
     * @brief 监控数据更新信号
     * @param stats 更新的统计数据
     */
    void monitorDataUpdated(const SystemStats& stats);
    
    /**
     * @brief 警告信号
     * @param message 警告消息
     * @param level 警告级别
     */
    void warningTriggered(const QString& message, int level);

private slots:
    /**
     * @brief 定时更新监控数据
     */
    void updateMonitorData();
    
    /**
     * @brief 更新性能图表
     */
    void updatePerformanceChart();
    
    /**
     * @brief 检查系统警告
     */
    void checkSystemWarnings();

private:
    /**
     * @brief 创建系统状态区域
     */
    void createSystemStatusSection();
    
    /**
     * @brief 创建AI统计区域
     */
    void createAiStatsSection();
    
    /**
     * @brief 创建性能图表区域
     */
    void createPerformanceChartSection();
    
    /**
     * @brief 创建连接状态区域
     */
    void createConnectionStatusSection();
    
    /**
     * @brief 连接信号和槽
     */
    void connectSignalsAndSlots();
    
    /**
     * @brief 获取系统CPU使用率
     * @return CPU使用率百分比
     */
    double getCpuUsage();
    
    /**
     * @brief 获取系统内存使用率
     * @return 内存使用率百分比
     */
    double getMemoryUsage();
    
    /**
     * @brief 获取网络速度
     * @return 网络速度 (MB/s)
     */
    double getNetworkSpeed();

private:
    // UI组件
    QVBoxLayout* m_mainLayout;
    
    // 系统状态区域
    QGroupBox* m_systemGroupBox;
    QLabel* m_cpuLabel;
    QLabel* m_memoryLabel;
    QLabel* m_networkLabel;
    QProgressBar* m_cpuBar;
    QProgressBar* m_memoryBar;
    QProgressBar* m_networkBar;
    QLCDNumber* m_cpuDisplay;
    QLCDNumber* m_memoryDisplay;
    
    // AI统计区域
    QGroupBox* m_aiStatsGroupBox;
    QLabel* m_totalAiLabel;
    QLabel* m_onlineAiLabel;
    QLabel* m_offlineAiLabel;
    QLabel* m_serverLoadLabel;
    QLCDNumber* m_totalAiDisplay;
    QLCDNumber* m_onlineAiDisplay;
    QProgressBar* m_serverLoadBar;
    
    // 性能图表区域
    QGroupBox* m_chartGroupBox;
    QChartView* m_chartView;
    QChart* m_chart;
    QLineSeries* m_cpuSeries;
    QLineSeries* m_memorySeries;
    QLineSeries* m_networkSeries;
    QValueAxis* m_axisX;
    QValueAxis* m_axisY;
    
    // 连接状态区域
    QGroupBox* m_connectionGroupBox;
    QLabel* m_connectionStatusLabel;
    QLabel* m_lastUpdateLabel;
    QLabel* m_uptimeLabel;
    
    // 数据和定时器
    SystemStats m_currentStats;
    QTimer* m_updateTimer;
    QTimer* m_chartTimer;
    QDateTime m_startTime;
    int m_dataPoints;
    
    // 警告阈值
    static constexpr double CPU_WARNING_THRESHOLD = 80.0;
    static constexpr double MEMORY_WARNING_THRESHOLD = 85.0;
    static constexpr int SERVER_LOAD_WARNING_THRESHOLD = 90;
};
