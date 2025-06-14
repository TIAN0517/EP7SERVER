/**
 * @file MainWindow.h
 * @brief RANOnline EP7 AI系统 - 主窗口头文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 * 
 * 功能特色:
 * - 无边框科技感界面设计
 * - 可自由拖拽移动
 * - 霓虹光效和赛博朋克风格
 * - Qt6现代化UI架构
 */

#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QMenu>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtCore/QTimer>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QEasingCurve>
#include <memory>

// 前置声明
class AiControlPanel;
class StatusMonitor;
class LogViewer;
class NetworkManager;

/**
 * @class MainWindow
 * @brief 主窗口类 - 无边框科技感界面
 * 
 * 核心功能:
 * 1. 无边框窗口设计，支持自定义拖拽
 * 2. 科技感UI布局，霓虹光效
 * 3. AI控制面板集成
 * 4. 实时状态监控显示
 * 5. 彩色日志查看器
 * 6. 系统托盘支持
 */
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~MainWindow();

    /**
     * @brief 初始化UI界面
     */
    void initializeUI();
    
    /**
     * @brief 设置科技感主题样式
     */
    void setupCyberpunkTheme();
    
    /**
     * @brief 连接信号和槽
     */
    void connectSignalsAndSlots();

protected:
    /**
     * @brief 绘制事件 - 自定义窗口外观
     * @param event 绘制事件
     */
    void paintEvent(QPaintEvent *event) override;
    
    /**
     * @brief 鼠标按下事件 - 实现拖拽功能
     * @param event 鼠标事件
     */
    void mousePressEvent(QMouseEvent *event) override;
    
    /**
     * @brief 鼠标移动事件 - 实现拖拽功能
     * @param event 鼠标事件
     */
    void mouseMoveEvent(QMouseEvent *event) override;
    
    /**
     * @brief 鼠标释放事件 - 结束拖拽
     * @param event 鼠标事件
     */
    void mouseReleaseEvent(QMouseEvent *event) override;
    
    /**
     * @brief 双击事件 - 最大化/还原窗口
     * @param event 鼠标事件
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    /**
     * @brief 最小化窗口
     */
    void minimizeWindow();
    
    /**
     * @brief 最大化/还原窗口
     */
    void toggleMaximizeWindow();
    
    /**
     * @brief 关闭窗口
     */
    void closeWindow();
    
    /**
     * @brief 显示关于对话框
     */
    void showAboutDialog();
    
    /**
     * @brief 更新状态信息
     */
    void updateStatusInfo();
    
    /**
     * @brief 处理系统托盘点击
     */
    void handleTrayIconClick();

private:
    /**
     * @brief 创建标题栏
     */
    void createTitleBar();
    
    /**
     * @brief 创建主内容区域
     */
    void createMainContent();
    
    /**
     * @brief 创建状态栏
     */
    void createStatusBar();
    
    /**
     * @brief 创建系统托盘
     */
    void createSystemTray();
    
    /**
     * @brief 应用霓虹光效
     * @param widget 目标控件
     * @param color 光效颜色
     */
    void applyNeonGlow(QWidget* widget, const QColor& color);

private:
    // UI组件
    QVBoxLayout* m_mainLayout;          ///< 主布局
    QHBoxLayout* m_titleBarLayout;      ///< 标题栏布局
    QHBoxLayout* m_contentLayout;       ///< 内容区布局
    QHBoxLayout* m_statusBarLayout;     ///< 状态栏布局
    
    // 标题栏组件
    QLabel* m_logoLabel;                ///< Logo标签
    QLabel* m_titleLabel;               ///< 标题标签
    QPushButton* m_minimizeButton;      ///< 最小化按钮
    QPushButton* m_maximizeButton;      ///< 最大化按钮
    QPushButton* m_closeButton;         ///< 关闭按钮
    
    // 主要功能模块
    std::unique_ptr<AiControlPanel> m_aiControlPanel;    ///< AI控制面板
    std::unique_ptr<StatusMonitor> m_statusMonitor;      ///< 状态监控器
    std::unique_ptr<LogViewer> m_logViewer;              ///< 日志查看器
    std::unique_ptr<NetworkManager> m_networkManager;    ///< 网络管理器
    
    // 状态栏组件
    QLabel* m_connectionStatusLabel;    ///< 连接状态标签
    QLabel* m_aiCountLabel;            ///< AI数量标签
    QLabel* m_performanceLabel;        ///< 性能标签
    QProgressBar* m_cpuUsageBar;       ///< CPU使用率进度条
    QProgressBar* m_memoryUsageBar;    ///< 内存使用率进度条
    
    // 系统托盘
    QSystemTrayIcon* m_systemTrayIcon;  ///< 系统托盘图标
    QMenu* m_trayMenu;                  ///< 托盘菜单
    
    // 窗口状态
    bool m_isDragging;                  ///< 是否正在拖拽
    QPoint m_dragPosition;              ///< 拖拽位置
    bool m_isMaximized;                 ///< 是否最大化
    QRect m_normalGeometry;             ///< 正常窗口几何信息
    
    // 动画效果
    QPropertyAnimation* m_glowAnimation; ///< 光效动画
    QTimer* m_updateTimer;              ///< 更新定时器
    
    // 主题配色
    struct Theme {
        QColor backgroundColor;         ///< 背景色
        QColor primaryColor;           ///< 主色调
        QColor accentColor;            ///< 强调色
        QColor textColor;              ///< 文字色
        QColor borderColor;            ///< 边框色
        QColor glowColor;              ///< 光效色
    } m_theme;
};

/**
 * @class CustomTitleBar
 * @brief 自定义标题栏类
 */
class CustomTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit CustomTitleBar(QWidget* parent = nullptr);
    
    void setTitle(const QString& title);
    void setIcon(const QPixmap& icon);

signals:
    void minimizeRequested();
    void maximizeRequested();
    void closeRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    QLabel* m_iconLabel;
    QLabel* m_titleLabel;
    QPushButton* m_minimizeButton;
    QPushButton* m_maximizeButton;
    QPushButton* m_closeButton;
    
    bool m_isDragging;
    QPoint m_dragPosition;
};

#endif // MAINWINDOW_H
