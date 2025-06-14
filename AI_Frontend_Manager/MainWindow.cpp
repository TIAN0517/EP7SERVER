/**
 * @file MainWindow.cpp
 * @brief RANOnline EP7 AI系统 - 主窗口实现文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "MainWindow.h"
#include "AiControlPanel.h"
#include "StatusMonitor.h"
#include "LogViewer.h"
#include "NetworkManager.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QMessageBox>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtGui/QScreen>

/**
 * @brief 构造函数 - 初始化主窗口
 */
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_titleBarLayout(nullptr)
    , m_contentLayout(nullptr)
    , m_statusBarLayout(nullptr)
    , m_logoLabel(nullptr)
    , m_titleLabel(nullptr)
    , m_minimizeButton(nullptr)
    , m_maximizeButton(nullptr)
    , m_closeButton(nullptr)
    , m_connectionStatusLabel(nullptr)
    , m_aiCountLabel(nullptr)
    , m_performanceLabel(nullptr)
    , m_cpuUsageBar(nullptr)
    , m_memoryUsageBar(nullptr)
    , m_systemTrayIcon(nullptr)
    , m_trayMenu(nullptr)
    , m_isDragging(false)
    , m_isMaximized(false)
    , m_glowAnimation(nullptr)
    , m_updateTimer(new QTimer(this))
{
    // 设置窗口属性 - 无边框、置顶、半透明
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // 设置窗口大小和位置
    resize(1400, 900);
    
    // 居中显示
    QScreen* screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // 初始化主题配色
    m_theme.backgroundColor = QColor(15, 15, 25, 230);     // 深色半透明背景
    m_theme.primaryColor = QColor(0, 150, 255);            // 蓝色主调
    m_theme.accentColor = QColor(0, 255, 150);             // 绿色强调
    m_theme.textColor = QColor(255, 255, 255);             // 白色文字
    m_theme.borderColor = QColor(0, 150, 255, 180);        // 蓝色边框
    m_theme.glowColor = QColor(0, 255, 150, 100);          // 绿色光效
    
    // 初始化UI组件
    initializeUI();
    setupCyberpunkTheme();
    connectSignalsAndSlots();
    createSystemTray();
    
    // 启动更新定时器
    m_updateTimer->start(1000); // 每秒更新一次
}

/**
 * @brief 析构函数
 */
MainWindow::~MainWindow()
{
    if (m_glowAnimation) {
        m_glowAnimation->stop();
        delete m_glowAnimation;
    }
}

/**
 * @brief 初始化UI界面
 */
void MainWindow::initializeUI()
{
    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(2);
    
    // 创建各个区域
    createTitleBar();
    createMainContent();
    createStatusBar();
    
    setLayout(m_mainLayout);
}

/**
 * @brief 创建标题栏
 */
void MainWindow::createTitleBar()
{
    // 标题栏容器
    QWidget* titleBarWidget = new QWidget;
    titleBarWidget->setFixedHeight(50);
    titleBarWidget->setObjectName("titleBar");
    
    m_titleBarLayout = new QHBoxLayout(titleBarWidget);
    m_titleBarLayout->setContentsMargins(15, 5, 5, 5);
    m_titleBarLayout->setSpacing(10);
    
    // Logo标签
    m_logoLabel = new QLabel;
    m_logoLabel->setFixedSize(32, 32);
    m_logoLabel->setScaledContents(true);
    // 设置Logo图片（需要添加资源文件）
    // m_logoLabel->setPixmap(QPixmap(":/icons/jy_logo.png"));
    m_logoLabel->setStyleSheet("QLabel { border: 2px solid #0096FF; border-radius: 16px; }");
    
    // 标题标签
    m_titleLabel = new QLabel("RANOnline EP7 AI Simulation System - Ultimate Edition");
    m_titleLabel->setObjectName("titleLabel");
    
    // 创建按钮
    m_minimizeButton = new QPushButton("—");
    m_maximizeButton = new QPushButton("□");
    m_closeButton = new QPushButton("✕");
    
    // 设置按钮大小
    QSize buttonSize(30, 30);
    m_minimizeButton->setFixedSize(buttonSize);
    m_maximizeButton->setFixedSize(buttonSize);
    m_closeButton->setFixedSize(buttonSize);
    
    // 设置按钮对象名（用于样式表）
    m_minimizeButton->setObjectName("minimizeButton");
    m_maximizeButton->setObjectName("maximizeButton");
    m_closeButton->setObjectName("closeButton");
    
    // 添加到布局
    m_titleBarLayout->addWidget(m_logoLabel);
    m_titleBarLayout->addWidget(m_titleLabel);
    m_titleBarLayout->addStretch(); // 弹性空间
    m_titleBarLayout->addWidget(m_minimizeButton);
    m_titleBarLayout->addWidget(m_maximizeButton);
    m_titleBarLayout->addWidget(m_closeButton);
    
    m_mainLayout->addWidget(titleBarWidget);
}

/**
 * @brief 创建主内容区域
 */
void MainWindow::createMainContent()
{
    // 主内容容器
    QWidget* contentWidget = new QWidget;
    contentWidget->setObjectName("mainContent");
    
    m_contentLayout = new QHBoxLayout(contentWidget);
    m_contentLayout->setContentsMargins(10, 10, 10, 10);
    m_contentLayout->setSpacing(10);
    
    // 创建功能模块
    m_aiControlPanel = std::make_unique<AiControlPanel>();
    m_statusMonitor = std::make_unique<StatusMonitor>();
    m_logViewer = std::make_unique<LogViewer>();
    m_networkManager = std::make_unique<NetworkManager>();
    
    // 左侧：AI控制面板
    QGroupBox* controlGroupBox = new QGroupBox("🤖 AI 控制中心");
    controlGroupBox->setObjectName("controlGroupBox");
    controlGroupBox->setFixedWidth(350);
    QVBoxLayout* controlLayout = new QVBoxLayout(controlGroupBox);
    controlLayout->addWidget(m_aiControlPanel.get());
    
    // 中央：状态监控
    QGroupBox* statusGroupBox = new QGroupBox("📊 实时状态监控");
    statusGroupBox->setObjectName("statusGroupBox");
    QVBoxLayout* statusLayout = new QVBoxLayout(statusGroupBox);
    statusLayout->addWidget(m_statusMonitor.get());
    
    // 右侧：日志查看器
    QGroupBox* logGroupBox = new QGroupBox("📋 系统日志");
    logGroupBox->setObjectName("logGroupBox");
    logGroupBox->setFixedWidth(400);
    QVBoxLayout* logLayout = new QVBoxLayout(logGroupBox);
    logLayout->addWidget(m_logViewer.get());
    
    // 添加到主布局
    m_contentLayout->addWidget(controlGroupBox);
    m_contentLayout->addWidget(statusGroupBox, 1); // 可拉伸
    m_contentLayout->addWidget(logGroupBox);
    
    m_mainLayout->addWidget(contentWidget, 1); // 可拉伸
}

/**
 * @brief 创建状态栏
 */
void MainWindow::createStatusBar()
{
    // 状态栏容器
    QWidget* statusBarWidget = new QWidget;
    statusBarWidget->setFixedHeight(35);
    statusBarWidget->setObjectName("statusBar");
    
    m_statusBarLayout = new QHBoxLayout(statusBarWidget);
    m_statusBarLayout->setContentsMargins(15, 5, 15, 5);
    m_statusBarLayout->setSpacing(20);
    
    // 连接状态
    m_connectionStatusLabel = new QLabel("🔗 连接状态: 已连接");
    m_connectionStatusLabel->setObjectName("statusLabel");
    
    // AI数量
    m_aiCountLabel = new QLabel("🤖 AI数量: 0");
    m_aiCountLabel->setObjectName("statusLabel");
    
    // CPU使用率
    QLabel* cpuLabel = new QLabel("💻 CPU:");
    m_cpuUsageBar = new QProgressBar;
    m_cpuUsageBar->setFixedSize(100, 20);
    m_cpuUsageBar->setObjectName("performanceBar");
    
    // 内存使用率
    QLabel* memoryLabel = new QLabel("🧠 内存:");
    m_memoryUsageBar = new QProgressBar;
    m_memoryUsageBar->setFixedSize(100, 20);
    m_memoryUsageBar->setObjectName("performanceBar");
    
    // 性能信息
    m_performanceLabel = new QLabel("⚡ 性能: 优秀");
    m_performanceLabel->setObjectName("statusLabel");
    
    // 添加到布局
    m_statusBarLayout->addWidget(m_connectionStatusLabel);
    m_statusBarLayout->addWidget(m_aiCountLabel);
    m_statusBarLayout->addStretch();
    m_statusBarLayout->addWidget(cpuLabel);
    m_statusBarLayout->addWidget(m_cpuUsageBar);
    m_statusBarLayout->addWidget(memoryLabel);
    m_statusBarLayout->addWidget(m_memoryUsageBar);
    m_statusBarLayout->addWidget(m_performanceLabel);
    
    m_mainLayout->addWidget(statusBarWidget);
}

/**
 * @brief 设置科技感主题样式
 */
void MainWindow::setupCyberpunkTheme()
{
    QString styleSheet = R"(
        /* 主窗口样式 */
        MainWindow {
            background: rgba(15, 15, 25, 230);
            border: 2px solid rgba(0, 150, 255, 180);
            border-radius: 15px;
        }
        
        /* 标题栏样式 */
        #titleBar {
            background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                stop: 0 rgba(0, 150, 255, 100),
                stop: 1 rgba(0, 255, 150, 100));
            border-radius: 8px;
            border: 1px solid rgba(0, 150, 255, 120);
        }
        
        #titleLabel {
            color: white;
            font-family: 'Segoe UI', 'Microsoft YaHei';
            font-size: 16px;
            font-weight: bold;
            padding: 5px;
        }
        
        /* 按钮样式 */
        QPushButton {
            background: rgba(0, 150, 255, 150);
            border: 1px solid rgba(0, 150, 255, 200);
            border-radius: 15px;
            color: white;
            font-weight: bold;
            font-size: 12px;
        }
        
        QPushButton:hover {
            background: rgba(0, 255, 150, 180);
            border: 1px solid rgba(0, 255, 150, 255);
            box-shadow: 0 0 15px rgba(0, 255, 150, 100);
        }
        
        QPushButton:pressed {
            background: rgba(0, 100, 200, 200);
        }
        
        #closeButton:hover {
            background: rgba(255, 50, 50, 180);
            border: 1px solid rgba(255, 50, 50, 255);
            box-shadow: 0 0 15px rgba(255, 50, 50, 100);
        }
        
        /* 分组框样式 */
        QGroupBox {
            color: white;
            font-family: 'Segoe UI', 'Microsoft YaHei';
            font-size: 14px;
            font-weight: bold;
            border: 2px solid rgba(0, 150, 255, 150);
            border-radius: 10px;
            margin-top: 1ex;
            background: rgba(20, 20, 35, 180);
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 10px 0 10px;
        }
        
        /* 状态栏样式 */
        #statusBar {
            background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                stop: 0 rgba(0, 100, 200, 100),
                stop: 1 rgba(0, 200, 100, 100));
            border-radius: 8px;
            border: 1px solid rgba(0, 150, 255, 120);
        }
        
        #statusLabel {
            color: white;
            font-family: 'Segoe UI', 'Microsoft YaHei';
            font-size: 12px;
            padding: 2px 5px;
        }
        
        /* 进度条样式 */
        #performanceBar {
            border: 1px solid rgba(0, 150, 255, 150);
            border-radius: 10px;
            background: rgba(20, 20, 35, 150);
            text-align: center;
        }
        
        #performanceBar::chunk {
            background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                stop: 0 rgba(0, 255, 150, 200),
                stop: 1 rgba(0, 150, 255, 200));
            border-radius: 8px;
        }
    )";
    
    setStyleSheet(styleSheet);
}

/**
 * @brief 连接信号和槽
 */
void MainWindow::connectSignalsAndSlots()
{
    // 标题栏按钮连接
    connect(m_minimizeButton, &QPushButton::clicked, this, &MainWindow::minimizeWindow);
    connect(m_maximizeButton, &QPushButton::clicked, this, &MainWindow::toggleMaximizeWindow);
    connect(m_closeButton, &QPushButton::clicked, this, &MainWindow::closeWindow);
    
    // 更新定时器连接
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::updateStatusInfo);
}

/**
 * @brief 创建系统托盘
 */
void MainWindow::createSystemTray()
{
    // 创建托盘图标
    m_systemTrayIcon = new QSystemTrayIcon(this);
    // m_systemTrayIcon->setIcon(QIcon(":/icons/tray_icon.png"));
    m_systemTrayIcon->setToolTip("RANOnline AI System");
    
    // 创建托盘菜单
    m_trayMenu = new QMenu(this);
    m_trayMenu->addAction("显示主窗口", this, &QWidget::show);
    m_trayMenu->addAction("隐藏主窗口", this, &QWidget::hide);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction("关于", this, &MainWindow::showAboutDialog);
    m_trayMenu->addAction("退出", this, &MainWindow::closeWindow);
    
    m_systemTrayIcon->setContextMenu(m_trayMenu);
    
    // 连接托盘点击事件
    connect(m_systemTrayIcon, &QSystemTrayIcon::activated, 
            this, &MainWindow::handleTrayIconClick);
    
    m_systemTrayIcon->show();
}

/**
 * @brief 绘制事件 - 自定义窗口外观
 */
void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制背景
    QRect backgroundRect = rect().adjusted(4, 4, -4, -4);
    painter.fillRect(backgroundRect, m_theme.backgroundColor);
    
    // 绘制边框光效
    QPen glowPen(m_theme.borderColor, 3);
    painter.setPen(glowPen);
    painter.drawRoundedRect(backgroundRect, 15, 15);
    
    // 绘制内部光效（可选）
    QPen innerGlowPen(m_theme.glowColor, 1);
    painter.setPen(innerGlowPen);
    painter.drawRoundedRect(backgroundRect.adjusted(2, 2, -2, -2), 13, 13);
}

/**
 * @brief 鼠标按下事件 - 开始拖拽
 */
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

/**
 * @brief 鼠标移动事件 - 执行拖拽
 */
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_isDragging) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

/**
 * @brief 鼠标释放事件 - 结束拖拽
 */
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }
}

/**
 * @brief 双击事件 - 最大化/还原窗口
 */
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        toggleMaximizeWindow();
        event->accept();
    }
}

/**
 * @brief 最小化窗口
 */
void MainWindow::minimizeWindow()
{
    showMinimized();
}

/**
 * @brief 最大化/还原窗口
 */
void MainWindow::toggleMaximizeWindow()
{
    if (m_isMaximized) {
        // 还原窗口
        setGeometry(m_normalGeometry);
        m_isMaximized = false;
        m_maximizeButton->setText("□");
    } else {
        // 最大化窗口
        m_normalGeometry = geometry();
        QScreen* screen = QApplication::primaryScreen();
        setGeometry(screen->availableGeometry());
        m_isMaximized = true;
        m_maximizeButton->setText("❐");
    }
}

/**
 * @brief 关闭窗口
 */
void MainWindow::closeWindow()
{
    QApplication::quit();
}

/**
 * @brief 显示关于对话框
 */
void MainWindow::showAboutDialog()
{
    QMessageBox::about(this, "关于",
        "RANOnline EP7 AI Simulation System\n"
        "Ultimate C++ Edition\n\n"
        "版本: 2.0.0\n"
        "开发团队: Jy技术团队\n"
        "发布日期: 2025年6月14日\n\n"
        "功能特色:\n"
        "• 无边框科技感界面\n"
        "• 高性能C++后端引擎\n"
        "• SQL Server 2022集成\n"
        "• 实时AI状态监控\n"
        "• 企业级架构设计");
}

/**
 * @brief 更新状态信息
 */
void MainWindow::updateStatusInfo()
{
    // 更新CPU和内存使用率（示例值）
    static int cpuUsage = 25;
    static int memoryUsage = 45;
    
    cpuUsage = (cpuUsage + (rand() % 10 - 5)) % 100;
    if (cpuUsage < 0) cpuUsage = 0;
    
    memoryUsage = (memoryUsage + (rand() % 6 - 3)) % 100;
    if (memoryUsage < 0) memoryUsage = 0;
    
    m_cpuUsageBar->setValue(cpuUsage);
    m_memoryUsageBar->setValue(memoryUsage);
    
    // 更新AI数量（从网络管理器获取）
    if (m_networkManager) {
        // int aiCount = m_networkManager->getActiveAiCount();
        // m_aiCountLabel->setText(QString("🤖 AI数量: %1").arg(aiCount));
    }
}

/**
 * @brief 处理系统托盘点击
 */
void MainWindow::handleTrayIconClick()
{
    if (isVisible()) {
        hide();
    } else {
        show();
        raise();
        activateWindow();
    }
}

#include "MainWindow.moc"
