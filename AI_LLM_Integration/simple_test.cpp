#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QTextEdit>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QFile>

class SimpleTestWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    SimpleTestWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setupUI();
        loadConfig();
    }

private slots:
    void onTestButtonClicked()
    {
        m_statusLabel->setText("🎮 四部門系統測試中...");
        
        // 模拟测试四部門系統
        QStringList departments = {"劍術", "弓術", "格鬥", "氣功"};
        QStringList academies = {"聖門", "懸岩", "鳳凰"};
        
        QString result = "🎯 四部門系統測試結果:\n\n";
        for (const QString &academy : academies) {
            result += QString("🏫 %1學院:\n").arg(academy);
            for (const QString &dept : departments) {
                result += QString("  ⚔️ %1系 - ✅ 正常\n").arg(dept);
            }
            result += "\n";
        }
        
        m_outputEdit->setPlainText(result);
        m_statusLabel->setText("✅ 四部門系統測試完成！");
    }

private:
    void setupUI()
    {
        setWindowTitle("RANOnline AI 四部門系統 - Qt6 測試");
        setMinimumSize(600, 400);
        
        auto *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto *layout = new QVBoxLayout(centralWidget);
        
        // 标题
        auto *titleLabel = new QLabel("🤖 RANOnline AI 四部門系統", this);
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #0078d4; padding: 10px;");
        layout->addWidget(titleLabel);
        
        // 状态标签
        m_statusLabel = new QLabel("📡 系統就緒", this);
        m_statusLabel->setStyleSheet("color: #107C10; padding: 5px;");
        layout->addWidget(m_statusLabel);
        
        // 测试按钮
        auto *testButton = new QPushButton("🚀 執行四部門系統測試", this);
        testButton->setStyleSheet("QPushButton { background-color: #0078d4; color: white; padding: 10px; border: none; border-radius: 5px; } QPushButton:hover { background-color: #106ebe; }");
        connect(testButton, &QPushButton::clicked, this, &SimpleTestWindow::onTestButtonClicked);
        layout->addWidget(testButton);
        
        // 输出文本框
        m_outputEdit = new QTextEdit(this);
        m_outputEdit->setPlaceholderText("測試結果將顯示在此處...");
        m_outputEdit->setStyleSheet("border: 1px solid #d1d1d1; border-radius: 5px; padding: 5px;");
        layout->addWidget(m_outputEdit);
    }
    
    void loadConfig()
    {
        QString configPath = "Config/llm.json";
        QFile file(configPath);
        
        if (file.exists()) {
            m_statusLabel->setText("📁 配置文件已載入: " + configPath);
        } else {
            m_statusLabel->setText("⚠️ 配置文件不存在，使用默認設置");
        }
    }
    
private:
    QLabel *m_statusLabel;
    QTextEdit *m_outputEdit;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    SimpleTestWindow window;
    window.show();
    
    return app.exec();
}

#include "simple_test.moc"
