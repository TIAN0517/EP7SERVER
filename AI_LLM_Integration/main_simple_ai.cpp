// ========================================================================
// RANOnline AI Simple - 最簡Qt6 GUI專案
// 版本: 1.0.0 Simple Edition
// 開發團隊: Jy技術團隊 線上AI
// 
// 🎯 最簡化特性:
// ✅ 只顯示一個標題窗口
// ✅ 100% Qt6兼容
// ✅ 強制軟體渲染
// ✅ 完全無其他依賴
// ========================================================================

#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QFont>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

// 環境檢測
bool isRunningInVM() {
    bool vmDetected = false;
    
#ifdef Q_OS_WIN
    QString manufacturer = QSysInfo::machineHostName().toLower();
    QStringList vmKeywords = {
        "vbox", "virtualbox", "vmware", "qemu", "kvm", 
        "hyper-v", "hyperv", "xen", "parallels", "virtual",
        "vm-", "-vm", "test", "sandbox"
    };
    
    for (const QString &keyword : vmKeywords) {
        if (manufacturer.contains(keyword)) {
            vmDetected = true;
            break;
        }
    }
#endif
    
    return vmDetected;
}

// 主窗口類
class SimpleAIMainWindow : public QMainWindow
{
public:
    explicit SimpleAIMainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
        , m_messageCount(0)
    {
        setWindowTitle("Jy技術團隊 線上AI - 極簡穩定VM兼容版 v4.0.0");
        setMinimumSize(700, 500);
        resize(900, 700);
        
        setupUI();
        setupConnections();
        
        // 檢測VM環境
        bool isVM = isRunningInVM();
        QString envText = isVM ? "VM環境 (安全模式)" : "標準環境";
        statusBar()->showMessage(QString("就緒 | 環境: %1").arg(envText));
        
        showWelcomeMessage();
    }

private:
    void setupUI() {
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(10);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        
        // 標題
        QLabel *titleLabel = new QLabel("Jy技術團隊 線上AI - 極簡穩定VM兼容版", this);
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(14);
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);
        
        QLabel *versionLabel = new QLabel("v4.0.0 Ultimate Portable Edition", this);
        versionLabel->setAlignment(Qt::AlignCenter);
        versionLabel->setStyleSheet("color: gray; font-size: 10px;");
        mainLayout->addWidget(versionLabel);
        
        // 聊天區域
        QGroupBox *chatGroup = new QGroupBox("AI對話", this);
        QVBoxLayout *chatLayout = new QVBoxLayout(chatGroup);
        
        m_chatDisplay = new QTextEdit(this);
        m_chatDisplay->setReadOnly(true);
        m_chatDisplay->setMinimumHeight(300);
        chatLayout->addWidget(m_chatDisplay);
        mainLayout->addWidget(chatGroup);
        
        // 輸入區域
        QGroupBox *inputGroup = new QGroupBox("訊息輸入", this);
        QVBoxLayout *inputLayout = new QVBoxLayout(inputGroup);
        
        QHBoxLayout *inputRowLayout = new QHBoxLayout();
        
        m_inputEdit = new QLineEdit(this);
        m_inputEdit->setPlaceholderText("輸入您的訊息...");
        
        m_sendButton = new QPushButton("發送", this);
        m_sendButton->setFixedWidth(80);
        
        m_clearButton = new QPushButton("清空", this);
        m_clearButton->setFixedWidth(80);
        
        inputRowLayout->addWidget(m_inputEdit);
        inputRowLayout->addWidget(m_sendButton);
        inputRowLayout->addWidget(m_clearButton);
        
        inputLayout->addLayout(inputRowLayout);
        mainLayout->addWidget(inputGroup);
        
        // 創建選單
        setupMenuBar();
    }
    
    void setupMenuBar() {
        QMenuBar *menuBar = this->menuBar();
        
        QMenu *systemMenu = menuBar->addMenu("系統");
        
        QAction *aboutAction = systemMenu->addAction("關於");
        connect(aboutAction, &QAction::triggered, this, &SimpleAIMainWindow::showAbout);
        
        QAction *systemInfoAction = systemMenu->addAction("系統資訊");
        connect(systemInfoAction, &QAction::triggered, this, &SimpleAIMainWindow::showSystemInfo);
        
        systemMenu->addSeparator();
        
        QAction *exitAction = systemMenu->addAction("退出");
        connect(exitAction, &QAction::triggered, this, &QWidget::close);
        
        QMenu *toolsMenu = menuBar->addMenu("工具");
        
        QAction *clearAction = toolsMenu->addAction("清空對話");
        connect(clearAction, &QAction::triggered, this, &SimpleAIMainWindow::clearChat);
    }
    
    void setupConnections() {
        connect(m_sendButton, &QPushButton::clicked, this, &SimpleAIMainWindow::sendMessage);
        connect(m_clearButton, &QPushButton::clicked, this, &SimpleAIMainWindow::clearChat);
        connect(m_inputEdit, &QLineEdit::returnPressed, this, &SimpleAIMainWindow::sendMessage);
    }
    
    void sendMessage() {
        QString userInput = m_inputEdit->text().trimmed();
        if (userInput.isEmpty()) {
            return;
        }
        
        QString timestamp = QTime::currentTime().toString("hh:mm:ss");
        m_chatDisplay->append(QString("[用戶 %1] %2").arg(timestamp).arg(userInput));
        
        QString aiResponse = generateAIResponse(userInput);
        m_chatDisplay->append(QString("[AI %1] %2").arg(timestamp).arg(aiResponse));
        m_chatDisplay->append("");
        
        QScrollBar *scrollBar = m_chatDisplay->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
        
        m_inputEdit->clear();
        m_messageCount++;
        
        statusBar()->showMessage(QString("運行中 | 訊息數: %1 | 環境: %2")
            .arg(m_messageCount)
            .arg(isRunningInVM() ? "VM環境" : "標準環境"));
    }
    
    void clearChat() {
        m_chatDisplay->clear();
        m_messageCount = 0;
        showWelcomeMessage();
        statusBar()->showMessage("對話已清空");
    }
    
    void showAbout() {
        QString aboutText = QString(
            "Jy技術團隊 線上AI\n"
            "極簡穩定VM兼容版 v4.0.0\n\n"
            "專為VM和雲桌面環境設計的AI助手\n\n"
            "特點：\n"
            "• 100%便攜化，無需安裝\n"
            "• VM/RDP/雲桌面完全兼容\n"
            "• 強制軟體渲染，極致穩定\n"
            "• 智能環境檢測\n\n"
            "開發團隊: Jy技術團隊\n"
            "構建日期: %1"
        ).arg(__DATE__);
        
        QMessageBox::about(this, "關於", aboutText);
    }
    
    void showSystemInfo() {
        QString info = QString(
            "系統環境資訊\n"
            "==================\n"
            "作業系統: %1\n"
            "Qt版本: %2\n"
            "CPU架構: %3\n"
            "虛擬機環境: %4\n"
            "安全模式: %5"
        ).arg(QSysInfo::prettyProductName())
         .arg(QLibraryInfo::version().toString())
         .arg(QSysInfo::currentCpuArchitecture())
         .arg(isRunningInVM() ? "是" : "否")
         .arg(isRunningInVM() ? "已啟用" : "未啟用");
        
        QMessageBox::information(this, "系統資訊", info);
    }
    
    void showWelcomeMessage() {
        QString welcome = QString(
            "🎉 歡迎使用 Jy技術團隊 線上AI！\n"
            "極簡穩定VM兼容版 v4.0.0\n\n"
            "當前環境: %1\n"
            "安全模式: %2\n\n"
            "快速開始：\n"
            "• 直接輸入訊息與AI對話\n"
            "• 輸入「時間」查看當前時間\n"
            "• 輸入「系統」查看系統資訊\n"
            "• 輸入「幫助」查看可用指令\n\n"
            "提示：按Enter鍵或點擊發送按鈕來發送訊息"
        ).arg(isRunningInVM() ? "虛擬機環境" : "本機環境")
         .arg(isRunningInVM() ? "已啟用" : "未啟用");
        
        m_chatDisplay->setPlainText(welcome);
    }
    
    QString generateAIResponse(const QString &input) {
        QString lowerInput = input.toLower();
        
        if (lowerInput.contains("你好") || lowerInput.contains("hello") || lowerInput.contains("hi")) {
            return "您好！我是Jy技術團隊開發的AI助手。很高興為您服務！";
        }
        else if (lowerInput.contains("時間") || lowerInput.contains("time")) {
            return QString("現在時間是：%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        }
        else if (lowerInput.contains("系統") || lowerInput.contains("system")) {
            return QString("系統資訊：%1 | Qt版本：%2")
                .arg(QSysInfo::prettyProductName())
                .arg(QLibraryInfo::version().toString());
        }
        else if (lowerInput.contains("vm") || lowerInput.contains("虛擬機")) {
            return "這個程序專為虛擬機環境優化，使用強制軟體渲染確保最佳兼容性。";
        }
        else if (lowerInput.contains("幫助") || lowerInput.contains("help")) {
            return "可用指令：\n• 詢問時間\n• 查看系統資訊\n• VM相關資訊\n或者直接與我對話！";
        }
        else {
            QStringList responses = {
                "我理解您的問題。作為AI助手，我會盡力協助您。",
                "這是一個很好的問題！讓我為您分析一下。",
                "感謝您的提問。基於我的理解，我建議...",
                "我正在處理您的請求，請稍候...",
                "這個問題很有趣！根據我的分析...",
            };
            int index = QTime::currentTime().msec() % responses.size();
            return responses[index];
        }
    }

private:
    QTextEdit *m_chatDisplay;
    QLineEdit *m_inputEdit;
    QPushButton *m_sendButton;
    QPushButton *m_clearButton;
    int m_messageCount;
};

// 主函數
int main(int argc, char *argv[])
{
    try {
        // VM兼容性設置
        QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL, true);
        QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling, true);
        QCoreApplication::setAttribute(Qt::AA_Use96Dpi, true);
        
        // 設置環境變數強制軟體渲染
        qputenv("QT_OPENGL", "software");
        qputenv("QT_QUICK_BACKEND", "software");
        qputenv("QT_ANGLE_PLATFORM", "software");
        
        QApplication app(argc, argv);
        
        app.setApplicationName("Jy Tech Team AI");
        app.setApplicationVersion("4.0.0");
        app.setOrganizationName("Jy Tech Team");
        
        // 使用系統原生樣式
        app.setStyle(QStyleFactory::create("Fusion"));
        
        // 設置字體
        QFont defaultFont("Microsoft YaHei", 9);
        app.setFont(defaultFont);
        
        // 表面格式設置（VM兼容性）
        QSurfaceFormat format;
        format.setRenderableType(QSurfaceFormat::DefaultRenderableType);
        format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        format.setRedBufferSize(8);
        format.setGreenBufferSize(8);
        format.setBlueBufferSize(8);
        format.setAlphaBufferSize(8);
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
        format.setSamples(0);
        format.setVersion(2, 1);
        QSurfaceFormat::setDefaultFormat(format);
        
        SimpleAIMainWindow window;
        window.show();
        
        // 顯示啟動成功訊息
        QTimer::singleShot(500, [&]() {
            bool isVM = isRunningInVM();
            QString startupMessage = QString(
                "Jy技術團隊 線上AI 已成功啟動！\n\n"
                "環境: %1\n"
                "模式: %2\n"
                "渲染: 軟體渲染"
            ).arg(isVM ? "虛擬機環境" : "本機環境")
             .arg(isVM ? "安全模式" : "標準模式");
            
            QMessageBox::information(&window, "啟動成功", startupMessage);
        });
        
        return app.exec();
        
    } catch (const std::exception &e) {
        QApplication app(argc, argv);
        QString errorMsg = QString("程式啟動時發生錯誤：\n%1\n\n請嘗試重新啟動程式。").arg(e.what());
        QMessageBox::critical(nullptr, "啟動錯誤", errorMsg);
        return -1;
    } catch (...) {
        QApplication app(argc, argv);
        QMessageBox::critical(nullptr, "未知錯誤", "程式啟動時發生未知錯誤，請聯繫技術支援。");
        return -2;
    }
}

// 為Qt6兼容性定義qMain - 這是必需的
int qMain(int argc, char *argv[])
{
    return main(argc, argv);
}
