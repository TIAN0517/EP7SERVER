#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QIcon>

class SimpleTestWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    SimpleTestWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {        setWindowTitle("Jy技術團隊 線上AI - LLM 整合系統 - 測試版");
        setWindowIcon(QIcon(":/icons/jy1.ico"));
        setMinimumSize(800, 600);
        
        // 創建中央Widget
        auto *central = new QWidget(this);
        setCentralWidget(central);
        
        // 創建佈局
        auto *layout = new QVBoxLayout(central);
        
        // 添加歡迎標籤
        auto *welcomeLabel = new QLabel("歡迎使用Jy技術團隊 線上AI - LLM 整合系統", this);
        welcomeLabel->setAlignment(Qt::AlignCenter);
        welcomeLabel->setStyleSheet(
            "QLabel {"
            "    font-size: 24px;"
            "    font-weight: bold;"
            "    color: #4682ff;"
            "    padding: 20px;"
            "    background: linear-gradient(45deg, #f0f8ff, #e6f3ff);"
            "    border: 2px solid #4682ff;"
            "    border-radius: 10px;"
            "}"
        );
        layout->addWidget(welcomeLabel);
        
        // 添加版本信息
        auto *versionLabel = new QLabel("版本: 3.0.0 - Jy技術團隊", this);
        versionLabel->setAlignment(Qt::AlignCenter);
        versionLabel->setStyleSheet("font-size: 14px; color: #666; margin: 10px;");
        layout->addWidget(versionLabel);
        
        // 添加狀態信息
        auto *statusLabel = new QLabel("系統初始化完成，準備就緒", this);
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setStyleSheet("font-size: 16px; color: #00964b; margin: 10px;");
        layout->addWidget(statusLabel);
        
        // 設置樣式表
        setStyleSheet(
            "QMainWindow {"
            "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
            "                stop:0 #f0f8ff, stop:1 #ddeeff);"
            "}"
        );
          // 顯示歡迎訊息
        QMessageBox::information(this, "系統啟動", 
                               "🎉 Jy技術團隊 線上AI - LLM 整合系統已成功啟動！\n\n"
                               "✅ 繁體中文介面\n"
                               "✅ Jy技術團隊品牌\n" 
                               "✅ Qt6 GUI框架\n"
                               "✅ MinGW編譯器");
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
      // 設置應用程式資訊
    app.setApplicationName("Jy技術團隊 線上AI - LLM 整合系統");
    app.setApplicationDisplayName("RANOnline AI LLM Integration");
    app.setApplicationVersion("3.0.0");
    app.setOrganizationName("Jy技術團隊");
    
    // 設置應用程式圖標
    app.setWindowIcon(QIcon(":/icons/jy1.ico"));
    
    try {
        // 創建並顯示主窗口
        SimpleTestWindow window;
        window.show();
        
        return app.exec();    }
    catch (const std::exception &e) {
        QMessageBox::critical(nullptr, "錯誤", 
                            QString("應用程式啟動失敗：%1").arg(e.what()));
        return -1;
    }
}

#include "main_simple.moc"
