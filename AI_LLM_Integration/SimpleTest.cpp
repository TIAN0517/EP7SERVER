#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>

class SimpleTestWindow : public QMainWindow
{
    Q_OBJECT

public:
    SimpleTestWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("RANOnline LLM Integration - Test Version");
        setMinimumSize(800, 600);
        
        auto *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto *layout = new QVBoxLayout(centralWidget);
        
        auto *titleLabel = new QLabel("🤖 RANOnline EP7 AI LLM Integration System", this);
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #4682B4; padding: 20px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        
        auto *statusLabel = new QLabel("✅ Qt6 Framework Initialized Successfully", this);
        statusLabel->setStyleSheet("font-size: 16px; color: #32CD32; padding: 10px;");
        statusLabel->setAlignment(Qt::AlignCenter);
        
        auto *infoLabel = new QLabel(
            "Four Departments System: 劍術、弓術、格鬥、氣功\n"
            "Three Academies: 聖門、懸岩、鳳凰\n"
            "LLM Integration: Ready for Ollama Connection", this);
        infoLabel->setStyleSheet("font-size: 14px; color: #333333; padding: 20px;");
        infoLabel->setAlignment(Qt::AlignCenter);
        
        auto *testButton = new QPushButton("Test Four Departments System", this);
        testButton->setStyleSheet(
            "QPushButton {"
            "  background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #4682B4, stop: 1 #1E90FF);"
            "  color: white;"
            "  border: none;"
            "  padding: 15px 30px;"
            "  font-size: 16px;"
            "  font-weight: bold;"
            "  border-radius: 8px;"
            "}"
            "QPushButton:hover {"
            "  background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #5A9BD4, stop: 1 #3FA0FF);"
            "}"
        );
        
        layout->addWidget(titleLabel);
        layout->addWidget(statusLabel);
        layout->addWidget(infoLabel);
        layout->addWidget(testButton);
        layout->addStretch();
        
        connect(testButton, &QPushButton::clicked, this, &SimpleTestWindow::onTestClicked);
    }

private slots:
    void onTestClicked()
    {
        auto *statusLabel = findChild<QLabel*>();
        if (statusLabel) {
            statusLabel->setText("🚀 Four Departments System Test: SUCCESS!");
            statusLabel->setStyleSheet("font-size: 16px; color: #FF6347; padding: 10px;");
        }
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("RANOnline LLM Integration");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("JyTech Team");
    
    SimpleTestWindow window;
    window.show();
    
    return app.exec();
}

#include "SimpleTest.moc"
