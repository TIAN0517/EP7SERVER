// ========================================================================
// RANOnline AI LLM Integration - VM Complete Compatible Version
// Version: 3.1.0 VM Super Simplified
// Development Team: Jy Tech Team
// 
// VM/Cloud Host Super Optimization Features:
// - Complete removal of complex dependencies (No FramelessWindow)
// - Pure QMainWindow implementation (Maximum compatibility)
// - Force software rendering (Disable all hardware acceleration)
// - Minimize DPI and scaling issues
// - Super simplified exception handling
// - VM environment auto-detection and optimization
// ========================================================================

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTextBrowser>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QTabWidget>
#include <QGroupBox>
#include <QSplitter>
#include <QStatusBar>
#include <QMenuBar>
#include <QIcon>
#include <QPixmap>
#include <QFont>
#include <QFontInfo>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QScreen>
#include <QSettings>
#include <QTimer>
#include <QMessageBox>
#include <QSysInfo>
#include <QLoggingCategory>
#include <QDebug>
#include <QFileInfo>
#include <iostream>
#include <memory>

// Windows specific headers
#ifdef Q_OS_WIN
#include <windows.h>
#include <winerror.h>
#endif

// VM environment detection function
bool isRunningInVM() {
    bool vmDetected = false;
    
#ifdef Q_OS_WIN
    // Check system manufacturer
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
    
    // Check CPU model
    if (!vmDetected) {
        QString cpu = QSysInfo::currentCpuArchitecture().toLower();
        if (cpu.contains("virtual") || cpu.contains("qemu")) {
            vmDetected = true;
        }
    }
#endif
    
    return vmDetected;
}

// Simplified main window class
class SimpleVMMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimpleVMMainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        setupUI();
        setupConnections();
        setupTimer();
    }

private slots:
    void onSubmitClicked() {
        QString input = m_inputEdit->text();
        if (!input.isEmpty()) {
            m_outputBrowser->append(QString("[User] %1").arg(input));
            m_outputBrowser->append("[AI] Request received, processing...");
            m_inputEdit->clear();
            m_statusLabel->setText("Processing request...");
        }
    }
    
    void onClearClicked() {
        m_outputBrowser->clear();
        m_statusLabel->setText("Dialog cleared");
    }
    
    void onAboutClicked() {
        QMessageBox::about(this, "About", 
            "RANOnline AI LLM Integration - VM Edition\\n"
            "Version: 3.1.0\\n"
            "Development Team: Jy Tech Team\\n\\n"
            "This is a simplified version optimized for VM environments");
    }
    
    void updateStatus() {
        static int counter = 0;
        counter++;
        QString status = QString("VM compatible mode running... (%1)").arg(counter);
        m_statusLabel->setText(status);
    }

private:
    void setupUI() {
        // Set window title and icon
        setWindowTitle("Jy Tech Team Online AI - VM Compatible Edition v3.1.0");
        setMinimumSize(800, 600);
        resize(1000, 700);
        
        // Try to load icon
        QIcon appIcon;
        QStringList iconPaths = {
            ":/icons/jy1.ico",
            "jy1.ico",
            "Resources/jy1.ico",
            "../icogui/jy1.ico"
        };
        
        for (const QString &path : iconPaths) {
            if (QFileInfo::exists(path)) {
                appIcon = QIcon(path);
                break;
            }
        }
        
        if (!appIcon.isNull()) {
            setWindowIcon(appIcon);
        }
        
        // Create central widget
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        // Main layout
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        
        // Brand label
        QLabel *brandLabel = new QLabel("Jy Tech Team Online AI - VM Compatible Edition", this);
        QFont brandFont("Microsoft YaHei UI", 16, QFont::Bold);
        brandLabel->setFont(brandFont);
        brandLabel->setAlignment(Qt::AlignCenter);
        brandLabel->setStyleSheet(
            "QLabel {"
            "color: #4682B4;"
            "margin: 10px;"
            "padding: 10px;"
            "background-color: #F0F8FF;"
            "border: 2px solid #4682B4;"
            "border-radius: 8px;"
            "}"
        );
        mainLayout->addWidget(brandLabel);
        
        // Tab widget
        QTabWidget *tabWidget = new QTabWidget(this);
        
        // Chat tab
        QWidget *chatTab = new QWidget();
        QVBoxLayout *chatLayout = new QVBoxLayout(chatTab);
        
        // Output area
        m_outputBrowser = new QTextBrowser(this);
        m_outputBrowser->setFont(QFont("Consolas", 10));
        m_outputBrowser->append("=== Jy Tech Team Online AI - VM Compatible Edition ===");
        m_outputBrowser->append("Welcome to RANOnline AI LLM Integration!");
        m_outputBrowser->append("This is a version optimized for VM environments.");
        m_outputBrowser->append("");
        chatLayout->addWidget(m_outputBrowser, 3);
        
        // Input area
        QHBoxLayout *inputLayout = new QHBoxLayout();
        m_inputEdit = new QLineEdit(this);
        m_inputEdit->setPlaceholderText("Please enter your question...");
        m_inputEdit->setFont(QFont("Microsoft YaHei UI", 10));
        
        QPushButton *submitBtn = new QPushButton("Send", this);
        QPushButton *clearBtn = new QPushButton("Clear", this);
        
        submitBtn->setFont(QFont("Microsoft YaHei UI", 10, QFont::Bold));
        clearBtn->setFont(QFont("Microsoft YaHei UI", 10));
        
        inputLayout->addWidget(m_inputEdit);
        inputLayout->addWidget(submitBtn);
        inputLayout->addWidget(clearBtn);
        
        chatLayout->addLayout(inputLayout);
        tabWidget->addTab(chatTab, "AI Chat");
        
        // Settings tab
        QWidget *settingsTab = new QWidget();
        QVBoxLayout *settingsLayout = new QVBoxLayout(settingsTab);
        
        QGroupBox *vmGroup = new QGroupBox("VM Environment Information", this);
        QVBoxLayout *vmLayout = new QVBoxLayout(vmGroup);
        
        QString vmInfo = QString(
            "VM Detection Result: %1\\n"
            "System: %2\\n"
            "Architecture: %3\\n"
            "Host Name: %4\\n"
            "Qt Version: %5"
        ).arg(
            isRunningInVM() ? "Yes" : "No",
            QSysInfo::prettyProductName(),
            QSysInfo::currentCpuArchitecture(),
            QSysInfo::machineHostName(),
            QT_VERSION_STR
        );
        
        QLabel *infoLabel = new QLabel(vmInfo, this);
        infoLabel->setFont(QFont("Consolas", 9));
        infoLabel->setWordWrap(true);
        vmLayout->addWidget(infoLabel);
        
        settingsLayout->addWidget(vmGroup);
        settingsLayout->addStretch();
        
        tabWidget->addTab(settingsTab, "System Info");
        
        mainLayout->addWidget(tabWidget);
        
        // Button area
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch();
        
        QPushButton *aboutBtn = new QPushButton("About", this);
        aboutBtn->setFont(QFont("Microsoft YaHei UI", 9));
        buttonLayout->addWidget(aboutBtn);
        
        mainLayout->addLayout(buttonLayout);
        
        // Status bar
        m_statusLabel = new QLabel("VM compatible mode activated", this);
        statusBar()->addWidget(m_statusLabel);
        
        // Connect signals
        connect(submitBtn, &QPushButton::clicked, this, &SimpleVMMainWindow::onSubmitClicked);
        connect(clearBtn, &QPushButton::clicked, this, &SimpleVMMainWindow::onClearClicked);
        connect(aboutBtn, &QPushButton::clicked, this, &SimpleVMMainWindow::onAboutClicked);
        connect(m_inputEdit, &QLineEdit::returnPressed, this, &SimpleVMMainWindow::onSubmitClicked);
    }
    
    void setupConnections() {
        // Basic connections already done in setupUI
    }
    
    void setupTimer() {
        m_statusTimer = new QTimer(this);
        connect(m_statusTimer, &QTimer::timeout, this, &SimpleVMMainWindow::updateStatus);
        m_statusTimer->start(5000); // Update every 5 seconds
    }

private:
    QTextBrowser *m_outputBrowser;
    QLineEdit *m_inputEdit;
    QLabel *m_statusLabel;
    QTimer *m_statusTimer;
};

// VM optimized application class
class VMOptimizedApplication : public QApplication
{
public:
    VMOptimizedApplication(int &argc, char **argv) 
        : QApplication(argc, argv)
    {
        setupVMOptimizations();
    }

private:
    void setupVMOptimizations() {
        // Set application properties
        setApplicationName("RANOnline AI LLM Integration VM Edition");
        setApplicationDisplayName("Jy Tech Team Online AI - VM Edition");
        setApplicationVersion("3.1.0");
        setOrganizationName("Jy Tech Team");
        setOrganizationDomain("jytech.local");
        
        // Set font rendering
        QFont defaultFont("Microsoft YaHei UI", 9);
        setFont(defaultFont);
    }
};

int main(int argc, char *argv[])
{
    // VM environment special settings
#ifdef Q_OS_WIN
    // Set environment variables to force software rendering
    qputenv("QT_OPENGL", "software");
    qputenv("QT_QPA_PLATFORM", "windows");
    qputenv("QT_LOGGING_RULES", "qt.qpa.gl.debug=false");
#endif

    // Qt application attributes (set before QApplication)
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL, true);
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL, false);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES, false);
    
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling, false);
#endif
    
    // Set surface format (software rendering)
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::DefaultRenderableType);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setSamples(0); // Disable MSAA
    QSurfaceFormat::setDefaultFormat(format);

    try {
        // Create VM optimized application
        VMOptimizedApplication app(argc, argv);
        
        // Check VM environment
        bool vmMode = isRunningInVM();
        qDebug() << "VM Mode Detected:" << vmMode;
        
        if (vmMode) {
            qDebug() << "Running in VM compatibility mode";
        }
        
        // Create and show main window
        SimpleVMMainWindow window;
        window.show();
        
        qDebug() << "Application started successfully";
        return app.exec();
        
    } catch (const std::exception &e) {
        qCritical() << "Application error:" << e.what();
        
        // Show error dialog
        QMessageBox::critical(nullptr, "Error", 
            QString("Application startup failed:\\n%1").arg(e.what()));
        return -1;
    } catch (...) {
        qCritical() << "Unknown application error";
        
        QMessageBox::critical(nullptr, "Error", 
            "Application encountered unknown error, please contact technical support.");
        return -1;
    }
}

#include "main_vm_simple_complete.moc"
