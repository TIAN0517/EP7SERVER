#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QStandardPaths>
#include <QDir>
#include "LLMMainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 設置應用程序信息
    app.setApplicationName("RANOnline LLM Integration");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Jy Tech Team");
    app.setOrganizationDomain("ranonline.com");
    
    // 載入翻譯
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ai_llm_integration_" + QLocale(locale).name();
        if (translator.load(":/translations/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }
    
    // 創建主窗口
    LLMMainWindow window;
    window.show();
    
    return app.exec();
}
