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

// 主窗口類
class SimpleMainWindow : public QMainWindow
{
public:
    explicit SimpleMainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        // 設置窗口標題
        setWindowTitle("RANOnline AI Simple");
        
        // 設置窗口大小
        setMinimumSize(400, 300);
        resize(500, 400);
        
        // 創建中央組件
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        // 創建布局
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        
        // 創建標題標籤
        QLabel *titleLabel = new QLabel("RANOnline AI Simple", this);
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(16);
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        titleLabel->setAlignment(Qt::AlignCenter);
        
        // 創建版本標籤
        QLabel *versionLabel = new QLabel("v1.0.0 - Jy技術團隊", this);
        versionLabel->setAlignment(Qt::AlignCenter);
        versionLabel->setStyleSheet("color: gray; font-size: 12px;");
        
        // 添加到布局
        layout->addStretch();
        layout->addWidget(titleLabel);
        layout->addWidget(versionLabel);
        layout->addStretch();
    }
};

// 主函數 - 標準Qt6入口點
int main(int argc, char *argv[])
{
    // VM兼容性設置 - 必須在QApplication創建之前
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL, true);
    
    // 設置環境變數強制軟體渲染
    qputenv("QT_OPENGL", "software");
    qputenv("QT_QUICK_BACKEND", "software");
    
    // 創建應用程式
    QApplication app(argc, argv);
    
    // 設置應用程式資訊
    app.setApplicationName("RANOnline AI Simple");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Jy Tech Team");
    
    // 創建並顯示主窗口
    SimpleMainWindow window;
    window.show();
    
    // 進入事件循環
    return app.exec();
}
