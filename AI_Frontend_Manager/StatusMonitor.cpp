/**
 * @file StatusMonitor.cpp
 * @brief RANOnline EP7 AI系统 - 状态监控面板实现文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "StatusMonitor.h"
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtWidgets/QApplication>
#include <random>

/**
 * @brief 构造函数
 */
StatusMonitor::StatusMonitor(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_systemGroupBox(nullptr)
    , m_cpuLabel(nullptr)
    , m_memoryLabel(nullptr)
    , m_networkLabel(nullptr)
    , m_cpuBar(nullptr)
    , m_memoryBar(nullptr)
    , m_networkBar(nullptr)
    , m_cpuDisplay(nullptr)
    , m_memoryDisplay(nullptr)
    , m_aiStatsGroupBox(nullptr)
    , m_totalAiLabel(nullptr)
    , m_onlineAiLabel(nullptr)
    , m_offlineAiLabel(nullptr)
    , m_serverLoadLabel(nullptr)
    , m_totalAiDisplay(nullptr)
    , m_onlineAiDisplay(nullptr)
    , m_serverLoadBar(nullptr)
    , m_chartGroupBox(nullptr)
    , m_chartView(nullptr)
    , m_chart(nullptr)
    , m_cpuSeries(nullptr)
    , m_memorySeries(nullptr)
    , m_networkSeries(nullptr)
    , m_axisX(nullptr)
    , m_axisY(nullptr)
    , m_connectionGroupBox(nullptr)
    , m_connectionStatusLabel(nullptr)
    , m_lastUpdateLabel(nullptr)
    , m_uptimeLabel(nullptr)
    , m_updateTimer(new QTimer(this))
    , m_chartTimer(new QTimer(this))
    , m_startTime(QDateTime::currentDateTime())
    , m_dataPoints(0)
{
    initializeUI();
    setupCyberpunkStyle();
    connectSignalsAndSlots();
    
    // 启动定时器
    m_updateTimer->start(1000);  // 每秒更新状态
    m_chartTimer->start(2000);   // 每2秒更新图表
}

/**
 * @brief 析构函数
 */
StatusMonitor::~StatusMonitor()
{
    // 自动清理由Qt管理
}

/**
 * @brief 初始化监控面板UI
 */
void StatusMonitor::initializeUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(15);
    
    // 创建各个区域
    createSystemStatusSection();
    createAiStatsSection();
    createPerformanceChartSection();
    createConnectionStatusSection();
    
    setLayout(m_mainLayout);
}

/**
 * @brief 创建系统状态区域
 */
void StatusMonitor::createSystemStatusSection()
{
    m_systemGroupBox = new QGroupBox("💻 系统性能");
    m_systemGroupBox->setObjectName("systemGroupBox");
    
    QGridLayout* systemLayout = new QGridLayout(m_systemGroupBox);
    
    // CPU使用率
    m_cpuLabel = new QLabel("CPU使用率:");
    m_cpuLabel->setObjectName("statusLabel");
    m_cpuBar = new QProgressBar;
    m_cpuBar->setObjectName("cpuBar");
    m_cpuBar->setRange(0, 100);
    m_cpuDisplay = new QLCDNumber(3);
    m_cpuDisplay->setObjectName("cpuDisplay");
    m_cpuDisplay->setSegmentStyle(QLCDNumber::Flat);
    
    // 内存使用率
    m_memoryLabel = new QLabel("内存使用率:");
    m_memoryLabel->setObjectName("statusLabel");
    m_memoryBar = new QProgressBar;
    m_memoryBar->setObjectName("memoryBar");
    m_memoryBar->setRange(0, 100);
    m_memoryDisplay = new QLCDNumber(3);
    m_memoryDisplay->setObjectName("memoryDisplay");
    m_memoryDisplay->setSegmentStyle(QLCDNumber::Flat);
    
    // 网络速度
    m_networkLabel = new QLabel("网络速度:");
    m_networkLabel->setObjectName("statusLabel");
    m_networkBar = new QProgressBar;
    m_networkBar->setObjectName("networkBar");
    m_networkBar->setRange(0, 100);
    m_networkBar->setFormat("%v MB/s");
    
    // 布局设置
    systemLayout->addWidget(m_cpuLabel, 0, 0);
    systemLayout->addWidget(m_cpuBar, 0, 1);
    systemLayout->addWidget(m_cpuDisplay, 0, 2);
    
    systemLayout->addWidget(m_memoryLabel, 1, 0);
    systemLayout->addWidget(m_memoryBar, 1, 1);
    systemLayout->addWidget(m_memoryDisplay, 1, 2);
    
    systemLayout->addWidget(m_networkLabel, 2, 0);
    systemLayout->addWidget(m_networkBar, 2, 1, 1, 2);
    
    m_mainLayout->addWidget(m_systemGroupBox);
}

/**
 * @brief 创建AI统计区域
 */
void StatusMonitor::createAiStatsSection()
{
    m_aiStatsGroupBox = new QGroupBox("🤖 AI 状态统计");
    m_aiStatsGroupBox->setObjectName("aiStatsGroupBox");
    
    QGridLayout* aiLayout = new QGridLayout(m_aiStatsGroupBox);
    
    // AI总数
    m_totalAiLabel = new QLabel("AI总数:");
    m_totalAiLabel->setObjectName("statusLabel");
    m_totalAiDisplay = new QLCDNumber(4);
    m_totalAiDisplay->setObjectName("totalAiDisplay");
    m_totalAiDisplay->setSegmentStyle(QLCDNumber::Flat);
    
    // 在线AI数
    m_onlineAiLabel = new QLabel("在线AI:");
    m_onlineAiLabel->setObjectName("statusLabel");
    m_onlineAiDisplay = new QLCDNumber(4);
    m_onlineAiDisplay->setObjectName("onlineAiDisplay");
    m_onlineAiDisplay->setSegmentStyle(QLCDNumber::Flat);
    
    // 离线AI数
    m_offlineAiLabel = new QLabel("离线AI:");
    m_offlineAiLabel->setObjectName("statusLabel");
    QLCDNumber* offlineAiDisplay = new QLCDNumber(4);
    offlineAiDisplay->setObjectName("offlineAiDisplay");
    offlineAiDisplay->setSegmentStyle(QLCDNumber::Flat);
    
    // 服务器负载
    m_serverLoadLabel = new QLabel("服务器负载:");
    m_serverLoadLabel->setObjectName("statusLabel");
    m_serverLoadBar = new QProgressBar;
    m_serverLoadBar->setObjectName("serverLoadBar");
    m_serverLoadBar->setRange(0, 100);
    m_serverLoadBar->setFormat("%p%");
    
    // 布局设置
    aiLayout->addWidget(m_totalAiLabel, 0, 0);
    aiLayout->addWidget(m_totalAiDisplay, 0, 1);
    aiLayout->addWidget(m_onlineAiLabel, 0, 2);
    aiLayout->addWidget(m_onlineAiDisplay, 0, 3);
    
    aiLayout->addWidget(m_offlineAiLabel, 1, 0);
    aiLayout->addWidget(offlineAiDisplay, 1, 1);
    aiLayout->addWidget(m_serverLoadLabel, 1, 2);
    aiLayout->addWidget(m_serverLoadBar, 1, 3);
    
    m_mainLayout->addWidget(m_aiStatsGroupBox);
}

/**
 * @brief 创建性能图表区域
 */
void StatusMonitor::createPerformanceChartSection()
{
    m_chartGroupBox = new QGroupBox("📈 性能趋势图");
    m_chartGroupBox->setObjectName("chartGroupBox");
    
    QVBoxLayout* chartLayout = new QVBoxLayout(m_chartGroupBox);
    
    // 创建图表
    m_chart = new QChart();
    m_chart->setTitle("系统性能实时监控");
    m_chart->setTitleBrush(QBrush(QColor(255, 255, 255)));
    m_chart->setBackgroundBrush(QBrush(QColor(15, 15, 25, 200)));
    m_chart->setPlotAreaBackgroundBrush(QBrush(QColor(0, 0, 0, 100)));
    m_chart->setPlotAreaBackgroundVisible(true);
    
    // 创建数据系列
    m_cpuSeries = new QLineSeries();
    m_cpuSeries->setName("CPU使用率");
    m_cpuSeries->setColor(QColor(0, 150, 255));
    
    m_memorySeries = new QLineSeries();
    m_memorySeries->setName("内存使用率");
    m_memorySeries->setColor(QColor(0, 255, 150));
    
    m_networkSeries = new QLineSeries();
    m_networkSeries->setName("网络速度");
    m_networkSeries->setColor(QColor(255, 150, 0));
    
    // 添加系列到图表
    m_chart->addSeries(m_cpuSeries);
    m_chart->addSeries(m_memorySeries);
    m_chart->addSeries(m_networkSeries);
    
    // 创建坐标轴
    m_axisX = new QValueAxis();
    m_axisX->setRange(0, 60); // 显示最近60秒
    m_axisX->setTitleText("时间 (秒)");
    m_axisX->setTitleBrush(QBrush(QColor(255, 255, 255)));
    m_axisX->setLabelsBrush(QBrush(QColor(200, 200, 200)));
    
    m_axisY = new QValueAxis();
    m_axisY->setRange(0, 100);
    m_axisY->setTitleText("使用率 (%)");
    m_axisY->setTitleBrush(QBrush(QColor(255, 255, 255)));
    m_axisY->setLabelsBrush(QBrush(QColor(200, 200, 200)));
    
    // 设置坐标轴
    m_chart->setAxisX(m_axisX, m_cpuSeries);
    m_chart->setAxisY(m_axisY, m_cpuSeries);
    m_chart->setAxisX(m_axisX, m_memorySeries);
    m_chart->setAxisY(m_axisY, m_memorySeries);
    m_chart->setAxisX(m_axisX, m_networkSeries);
    m_chart->setAxisY(m_axisY, m_networkSeries);
    
    // 创建图表视图
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setFixedHeight(250);
    m_chartView->setObjectName("performanceChart");
    
    chartLayout->addWidget(m_chartView);
    m_mainLayout->addWidget(m_chartGroupBox);
}

/**
 * @brief 创建连接状态区域
 */
void StatusMonitor::createConnectionStatusSection()
{
    m_connectionGroupBox = new QGroupBox("🔗 连接状态");
    m_connectionGroupBox->setObjectName("connectionGroupBox");
    
    QHBoxLayout* connectionLayout = new QHBoxLayout(m_connectionGroupBox);
    
    // 连接状态
    m_connectionStatusLabel = new QLabel("状态: 连接中...");
    m_connectionStatusLabel->setObjectName("connectionStatus");
    
    // 最后更新时间
    m_lastUpdateLabel = new QLabel("更新: --:--:--");
    m_lastUpdateLabel->setObjectName("lastUpdate");
    
    // 运行时间
    m_uptimeLabel = new QLabel("运行时间: 00:00:00");
    m_uptimeLabel->setObjectName("uptime");
    
    connectionLayout->addWidget(m_connectionStatusLabel);
    connectionLayout->addStretch();
    connectionLayout->addWidget(m_lastUpdateLabel);
    connectionLayout->addWidget(m_uptimeLabel);
    
    m_mainLayout->addWidget(m_connectionGroupBox);
}

/**
 * @brief 连接信号和槽
 */
void StatusMonitor::connectSignalsAndSlots()
{
    connect(m_updateTimer, &QTimer::timeout, this, &StatusMonitor::updateMonitorData);
    connect(m_chartTimer, &QTimer::timeout, this, &StatusMonitor::updatePerformanceChart);
}

/**
 * @brief 设置科技感样式
 */
void StatusMonitor::setupCyberpunkStyle()
{
    setStyleSheet(R"(
        /* 状态监控整体样式 */
        StatusMonitor {
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
        QLabel#statusLabel {
            color: #FFFFFF;
            font-weight: bold;
            font-size: 12px;
            min-width: 80px;
        }
        
        QLabel#connectionStatus {
            color: #00FFAA;
            font-weight: bold;
            font-size: 13px;
        }
        
        QLabel#lastUpdate, QLabel#uptime {
            color: #CCCCCC;
            font-size: 11px;
            margin: 0px 10px;
        }
        
        /* 进度条样式 */
        QProgressBar {
            background: rgba(0, 0, 0, 150);
            border: 2px solid #0096FF;
            border-radius: 6px;
            text-align: center;
            color: #FFFFFF;
            font-weight: bold;
            font-size: 11px;
            min-height: 20px;
        }
        
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #0096FF, stop:1 #00FFAA);
            border-radius: 4px;
            margin: 2px;
        }
        
        QProgressBar#cpuBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #FF6B6B, stop:1 #FFE66D);
        }
        
        QProgressBar#memoryBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #4ECDC4, stop:1 #44A08D);
        }
        
        QProgressBar#networkBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #667eea, stop:1 #764ba2);
        }
        
        QProgressBar#serverLoadBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #f093fb, stop:1 #f5576c);
        }
        
        /* LCD数字显示样式 */
        QLCDNumber {
            background: rgba(0, 0, 0, 200);
            border: 2px solid #0096FF;
            border-radius: 6px;
            color: #00FFAA;
        }
        
        QLCDNumber#cpuDisplay {
            color: #FF6B6B;
        }
        
        QLCDNumber#memoryDisplay {
            color: #4ECDC4;
        }
        
        QLCDNumber#totalAiDisplay {
            color: #00FFAA;
            border-color: #00FFAA;
        }
        
        QLCDNumber#onlineAiDisplay {
            color: #67B26F;
            border-color: #67B26F;
        }
        
        QLCDNumber#offlineAiDisplay {
            color: #FF6B6B;
            border-color: #FF6B6B;
        }
        
        /* 图表视图样式 */
        QChartView {
            background: rgba(0, 0, 0, 100);
            border: 2px solid #0096FF;
            border-radius: 8px;
        }
    )");
}

/**
 * @brief 定时更新监控数据
 */
void StatusMonitor::updateMonitorData()
{
    // 获取系统性能数据
    SystemStats stats;
    stats.cpuUsage = getCpuUsage();
    stats.memoryUsage = getMemoryUsage();
    stats.networkSpeed = getNetworkSpeed();
    stats.aiCount = 150; // 模拟数据
    stats.onlineAiCount = 120; // 模拟数据
    stats.serverLoad = static_cast<int>(stats.cpuUsage + stats.memoryUsage) / 2;
    stats.status = "Connected";
    
    updateSystemStats(stats);
    
    // 检查警告
    checkSystemWarnings();
    
    // 更新运行时间
    qint64 secondsElapsed = m_startTime.secsTo(QDateTime::currentDateTime());
    int hours = secondsElapsed / 3600;
    int minutes = (secondsElapsed % 3600) / 60;
    int seconds = secondsElapsed % 60;
    
    m_uptimeLabel->setText(QString("运行时间: %1:%2:%3")
                          .arg(hours, 2, 10, QChar('0'))
                          .arg(minutes, 2, 10, QChar('0'))
                          .arg(seconds, 2, 10, QChar('0')));
    
    // 更新最后更新时间
    m_lastUpdateLabel->setText(QString("更新: %1")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    
    emit monitorDataUpdated(stats);
}

/**
 * @brief 更新性能图表
 */
void StatusMonitor::updatePerformanceChart()
{
    // 添加新的数据点
    double currentTime = m_dataPoints * 2; // 每2秒一个数据点
    
    m_cpuSeries->append(currentTime, m_currentStats.cpuUsage);
    m_memorySeries->append(currentTime, m_currentStats.memoryUsage);
    m_networkSeries->append(currentTime, m_currentStats.networkSpeed);
    
    // 限制显示的数据点数量（最近30个点，即60秒）
    if (m_cpuSeries->count() > 30) {
        m_cpuSeries->remove(0);
        m_memorySeries->remove(0);
        m_networkSeries->remove(0);
        
        // 调整X轴范围
        double minX = m_cpuSeries->at(0).x();
        double maxX = minX + 60;
        m_axisX->setRange(minX, maxX);
    }
    
    m_dataPoints++;
}

/**
 * @brief 检查系统警告
 */
void StatusMonitor::checkSystemWarnings()
{
    if (m_currentStats.cpuUsage > CPU_WARNING_THRESHOLD) {
        emit warningTriggered(QString("CPU使用率过高: %1%").arg(m_currentStats.cpuUsage), 2);
    }
    
    if (m_currentStats.memoryUsage > MEMORY_WARNING_THRESHOLD) {
        emit warningTriggered(QString("内存使用率过高: %1%").arg(m_currentStats.memoryUsage), 2);
    }
    
    if (m_currentStats.serverLoad > SERVER_LOAD_WARNING_THRESHOLD) {
        emit warningTriggered(QString("服务器负载过高: %1%").arg(m_currentStats.serverLoad), 3);
    }
}

/**
 * @brief 更新系统统计信息
 */
void StatusMonitor::updateSystemStats(const SystemStats& stats)
{
    m_currentStats = stats;
    
    // 更新CPU显示
    m_cpuBar->setValue(static_cast<int>(stats.cpuUsage));
    m_cpuDisplay->display(stats.cpuUsage);
    
    // 更新内存显示
    m_memoryBar->setValue(static_cast<int>(stats.memoryUsage));
    m_memoryDisplay->display(stats.memoryUsage);
    
    // 更新网络速度显示
    m_networkBar->setValue(static_cast<int>(stats.networkSpeed));
    
    // 更新AI统计显示
    m_totalAiDisplay->display(stats.aiCount);
    m_onlineAiDisplay->display(stats.onlineAiCount);
    
    // 更新服务器负载
    m_serverLoadBar->setValue(stats.serverLoad);
    
    // 更新连接状态
    QString statusText = QString("状态: %1").arg(stats.status);
    if (stats.status == "Connected") {
        statusText = "🟢 " + statusText;
        m_connectionStatusLabel->setStyleSheet("color: #00FFAA;");
    } else {
        statusText = "🔴 " + statusText;
        m_connectionStatusLabel->setStyleSheet("color: #FF6B6B;");
    }
    m_connectionStatusLabel->setText(statusText);
}

/**
 * @brief 获取系统CPU使用率
 */
double StatusMonitor::getCpuUsage()
{
    // 在实际实现中，这里应该调用系统API获取真实的CPU使用率
    // 目前使用模拟数据
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(10.0, 85.0);
    
    return dis(gen);
}

/**
 * @brief 获取系统内存使用率
 */
double StatusMonitor::getMemoryUsage()
{
    // 在实际实现中，这里应该调用系统API获取真实的内存使用率
    // 目前使用模拟数据
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(40.0, 80.0);
    
    return dis(gen);
}

/**
 * @brief 获取网络速度
 */
double StatusMonitor::getNetworkSpeed()
{
    // 在实际实现中，这里应该监控网络接口获取真实的网络速度
    // 目前使用模拟数据
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(1.0, 25.0);
    
    return dis(gen);
}
