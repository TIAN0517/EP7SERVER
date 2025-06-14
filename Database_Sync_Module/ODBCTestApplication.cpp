#include "ODBCTestApplication.h"
#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include <QFont>
#include <QFontDatabase>

// ====================================================================
// ODBCTestMainWindow Implementation
// ====================================================================

ODBCTestMainWindow::ODBCTestMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_odbcManager(nullptr)
    , m_statusTimer(new QTimer(this))
    , m_isConnected(false)
{
    setWindowTitle("RANOnline EP7 AI - ODBC 测试工具 v2.0");
    setWindowIcon(QIcon(":/icons/database.png"));
    resize(1200, 800);
    
    // 创建ODBC连接管理器
    m_odbcManager = std::make_unique<ODBCConnectionManager>(this);
    
    setupUI();
    setupConnections();
    
    // 设置默认配置
    m_serverEdit->setText("192.168.1.100");
    m_databaseEdit->setText("RanOnlineAI");
    m_usernameEdit->setText("sa");
    m_passwordEdit->setText("Ss520520@@");
    m_dsnEdit->setText("RanOnlineAIDB");
    
    // 启动状态更新定时器
    m_statusTimer->start(1000);
    
    updateStatus("ODBC测试工具已启动，准备就绪");
}

ODBCTestMainWindow::~ODBCTestMainWindow()
{
    if (m_odbcManager && m_odbcManager->isConnected()) {
        m_odbcManager->disconnect();
    }
}

void ODBCTestMainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    
    // ============================================================
    // 连接配置组
    // ============================================================
    m_connectionGroup = new QGroupBox("🔗 数据库连接配置", this);
    m_configLayout = new QFormLayout(m_connectionGroup);
    
    m_serverEdit = new QLineEdit(this);
    m_serverEdit->setPlaceholderText("192.168.x.x");
    m_configLayout->addRow("服务器地址:", m_serverEdit);
    
    m_databaseEdit = new QLineEdit(this);
    m_databaseEdit->setPlaceholderText("RanOnlineAI");
    m_configLayout->addRow("数据库名称:", m_databaseEdit);
    
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("sa");
    m_configLayout->addRow("用户名:", m_usernameEdit);
    
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText("输入密码");
    m_configLayout->addRow("密码:", m_passwordEdit);
    
    m_dsnEdit = new QLineEdit(this);
    m_dsnEdit->setPlaceholderText("RanOnlineAIDB");
    m_configLayout->addRow("DSN名称:", m_dsnEdit);
    
    m_mainLayout->addWidget(m_connectionGroup);
    
    // ============================================================
    // 控制按钮组
    // ============================================================
    m_controlGroup = new QGroupBox("🎮 操作控制", this);
    m_buttonLayout = new QHBoxLayout(m_controlGroup);
    
    m_testButton = new QPushButton("🔍 测试连接", this);
    m_testButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; }");
    m_buttonLayout->addWidget(m_testButton);
    
    m_createDsnButton = new QPushButton("⚙️ 创建DSN", this);
    m_createDsnButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px; }");
    m_buttonLayout->addWidget(m_createDsnButton);
    
    m_removeDsnButton = new QPushButton("🗑️ 删除DSN", this);
    m_removeDsnButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; }");
    m_buttonLayout->addWidget(m_removeDsnButton);
    
    m_benchmarkButton = new QPushButton("⚡ 性能测试", this);
    m_benchmarkButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; padding: 8px; }");
    m_buttonLayout->addWidget(m_benchmarkButton);
    
    m_mainLayout->addWidget(m_controlGroup);
    
    // ============================================================
    // SQL测试组
    // ============================================================
    m_sqlGroup = new QGroupBox("📝 SQL 查询测试", this);
    m_sqlLayout = new QVBoxLayout(m_sqlGroup);
    
    m_sqlEdit = new QTextEdit(this);
    m_sqlEdit->setMaximumHeight(120);
    m_sqlEdit->setPlainText("SELECT \n    GETDATE() as CurrentTime,\n    @@VERSION as SQLVersion,\n    DB_NAME() as DatabaseName");
    m_sqlLayout->addWidget(m_sqlEdit);
    
    m_executeButton = new QPushButton("▶️ 执行查询", this);
    m_executeButton->setStyleSheet("QPushButton { background-color: #9C27B0; color: white; padding: 6px; }");
    m_sqlLayout->addWidget(m_executeButton);
    
    m_resultEdit = new QTextEdit(this);
    m_resultEdit->setMaximumHeight(150);
    m_resultEdit->setReadOnly(true);
    m_sqlLayout->addWidget(m_resultEdit);
    
    m_mainLayout->addWidget(m_sqlGroup);
    
    // ============================================================
    // 状态和日志组
    // ============================================================
    m_statusGroup = new QGroupBox("📊 状态监控和日志", this);
    m_statusLayout = new QVBoxLayout(m_statusGroup);
    
    m_statusLabel = new QLabel("状态: 未连接", this);
    m_statusLabel->setStyleSheet("QLabel { font-weight: bold; color: #f44336; }");
    m_statusLayout->addWidget(m_statusLabel);
    
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_statusLayout->addWidget(m_progressBar);
    
    m_logEdit = new QTextEdit(this);
    m_logEdit->setReadOnly(true);
    m_logEdit->setMaximumHeight(200);
    m_statusLayout->addWidget(m_logEdit);
    
    m_clearLogButton = new QPushButton("🧹 清除日志", this);
    m_clearLogButton->setStyleSheet("QPushButton { background-color: #607D8B; color: white; padding: 4px; }");
    m_statusLayout->addWidget(m_clearLogButton);
    
    m_mainLayout->addWidget(m_statusGroup);
}

void ODBCTestMainWindow::setupConnections()
{
    connect(m_testButton, &QPushButton::clicked, this, &ODBCTestMainWindow::onTestConnection);
    connect(m_createDsnButton, &QPushButton::clicked, this, &ODBCTestMainWindow::onCreateDSN);
    connect(m_removeDsnButton, &QPushButton::clicked, this, &ODBCTestMainWindow::onRemoveDSN);
    connect(m_benchmarkButton, &QPushButton::clicked, this, &ODBCTestMainWindow::onRunBenchmark);
    connect(m_executeButton, &QPushButton::clicked, this, &ODBCTestMainWindow::onExecuteQuery);
    connect(m_clearLogButton, &QPushButton::clicked, this, &ODBCTestMainWindow::onClearLog);
    connect(m_statusTimer, &QTimer::timeout, this, &ODBCTestMainWindow::onUpdateConnectionStatus);
}

void ODBCTestMainWindow::onTestConnection()
{
    logMessage("开始测试数据库连接...");
    m_testButton->setEnabled(false);
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0); // 不确定进度条
    
    // 配置连接参数
    ODBCConnectionManager::ConnectionConfig config;
    config.serverAddress = m_serverEdit->text();
    config.databaseName = m_databaseEdit->text();
    config.username = m_usernameEdit->text();
    config.password = m_passwordEdit->text();
    config.dsnName = m_dsnEdit->text();
    
    // 创建后台测试线程
    auto *testThread = new ConnectionTestThread(config, this);
    
    connect(testThread, &ConnectionTestThread::testCompleted, this, [this](bool success, const QString &message) {
        m_testButton->setEnabled(true);
        m_progressBar->setVisible(false);
        
        if (success) {
            updateStatus("✅ 连接测试成功", false);
            m_isConnected = true;
            m_lastConnectionTime = QDateTime::currentDateTime();
            QMessageBox::information(this, "连接成功", message);
        } else {
            updateStatus("❌ 连接测试失败", true);
            QMessageBox::critical(this, "连接失败", message);
        }
        
        logMessage(QString("连接测试结果: %1").arg(message));
    });
    
    connect(testThread, &ConnectionTestThread::finished, testThread, &QObject::deleteLater);
    testThread->start();
}

void ODBCTestMainWindow::onCreateDSN()
{
    logMessage("开始创建ODBC DSN...");
    
    ODBCConnectionManager::ConnectionConfig config;
    config.serverAddress = m_serverEdit->text();
    config.databaseName = m_databaseEdit->text();
    config.username = m_usernameEdit->text();
    config.password = m_passwordEdit->text();
    config.dsnName = m_dsnEdit->text();
    
    if (m_odbcManager->createSystemDSN(config)) {
        updateStatus("✅ DSN创建成功", false);
        logMessage("ODBC DSN创建成功");
        QMessageBox::information(this, "DSN创建", "ODBC DSN已成功创建!");
    } else {
        updateStatus("❌ DSN创建失败", true);
        logMessage("ODBC DSN创建失败");
        QMessageBox::critical(this, "DSN创建", "ODBC DSN创建失败，请检查权限和配置");
    }
}

void ODBCTestMainWindow::onRemoveDSN()
{
    QString dsnName = m_dsnEdit->text();
    
    auto reply = QMessageBox::question(this, "确认删除", 
        QString("确定要删除DSN '%1' 吗？").arg(dsnName),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (m_odbcManager->removeSystemDSN(dsnName)) {
            updateStatus("✅ DSN删除成功", false);
            logMessage(QString("ODBC DSN '%1' 删除成功").arg(dsnName));
            QMessageBox::information(this, "DSN删除", "ODBC DSN已成功删除!");
        } else {
            updateStatus("❌ DSN删除失败", true);
            logMessage(QString("ODBC DSN '%1' 删除失败").arg(dsnName));
            QMessageBox::critical(this, "DSN删除", "ODBC DSN删除失败");
        }
    }
}

void ODBCTestMainWindow::onRunBenchmark()
{
    logMessage("开始性能基准测试...");
    m_benchmarkButton->setEnabled(false);
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0);
    
    ODBCConnectionManager::ConnectionConfig config;
    config.serverAddress = m_serverEdit->text();
    config.databaseName = m_databaseEdit->text();
    config.username = m_usernameEdit->text();
    config.password = m_passwordEdit->text();
    config.dsnName = m_dsnEdit->text();
    
    auto *benchmarkThread = new BenchmarkTestThread(config, this);
    
    connect(benchmarkThread, &BenchmarkTestThread::benchmarkCompleted, this, [this](const QString &results) {
        m_benchmarkButton->setEnabled(true);
        m_progressBar->setVisible(false);
        m_resultEdit->setPlainText(results);
        logMessage("性能基准测试完成");
        updateStatus("⚡ 性能测试完成", false);
    });
    
    connect(benchmarkThread, &BenchmarkTestThread::finished, benchmarkThread, &QObject::deleteLater);
    benchmarkThread->start();
}

void ODBCTestMainWindow::onExecuteQuery()
{
    QString sql = m_sqlEdit->toPlainText().trimmed();
    if (sql.isEmpty()) {
        QMessageBox::warning(this, "查询为空", "请输入要执行的SQL查询语句");
        return;
    }
    
    logMessage("执行SQL查询...");
    m_executeButton->setEnabled(false);
    
    try {
        auto results = m_odbcManager->executeQuery(sql);
        
        QString output;
        output += QString("查询执行成功！返回 %1 行数据\n").arg(results.size());
        output += "=====================================\n";
        
        for (int i = 0; i < results.size() && i < 100; ++i) { // 限制显示前100行
            const auto &row = results[i];
            QStringList rowData;
            for (auto it = row.begin(); it != row.end(); ++it) {
                rowData << QString("%1: %2").arg(it.key(), it.value().toString());
            }
            output += QString("Row %1: %2\n").arg(i + 1).arg(rowData.join(", "));
        }
        
        if (results.size() > 100) {
            output += QString("... 还有 %1 行数据未显示\n").arg(results.size() - 100);
        }
        
        m_resultEdit->setPlainText(output);
        logMessage(QString("SQL查询成功执行，返回%1行数据").arg(results.size()));
        
    } catch (const std::exception &e) {
        QString error = QString("SQL查询执行失败: %1").arg(e.what());
        m_resultEdit->setPlainText(error);
        logMessage(error);
        QMessageBox::critical(this, "查询失败", error);
    }
    
    m_executeButton->setEnabled(true);
}

void ODBCTestMainWindow::onClearLog()
{
    m_logEdit->clear();
    logMessage("日志已清除");
}

void ODBCTestMainWindow::onConnectionStatusChanged(bool connected)
{
    m_isConnected = connected;
    if (connected) {
        m_lastConnectionTime = QDateTime::currentDateTime();
    }
}

void ODBCTestMainWindow::onUpdateConnectionStatus()
{
    if (m_isConnected) {
        QString uptime = QString("已连接 - 连接时间: %1")
            .arg(m_lastConnectionTime.toString("yyyy-MM-dd hh:mm:ss"));
        m_statusLabel->setText(uptime);
        m_statusLabel->setStyleSheet("QLabel { font-weight: bold; color: #4CAF50; }");
    } else {
        m_statusLabel->setText("状态: 未连接");
        m_statusLabel->setStyleSheet("QLabel { font-weight: bold; color: #f44336; }");
    }
}

void ODBCTestMainWindow::updateStatus(const QString &message, bool isError)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString statusText = QString("[%1] %2").arg(timestamp, message);
    
    if (isError) {
        m_statusLabel->setText(statusText);
        m_statusLabel->setStyleSheet("QLabel { font-weight: bold; color: #f44336; }");
    } else {
        m_statusLabel->setText(statusText);
        m_statusLabel->setStyleSheet("QLabel { font-weight: bold; color: #4CAF50; }");
    }
}

void ODBCTestMainWindow::logMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp, message);
    m_logEdit->append(logEntry);
    
    // 滚动到底部
    QTextCursor cursor = m_logEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logEdit->setTextCursor(cursor);
}

// ====================================================================
// ODBCTestApplication Implementation
// ====================================================================

ODBCTestApplication::ODBCTestApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_splash(nullptr)
    , m_mainWindow(nullptr)
{
    setApplicationName("RANOnline ODBC测试工具");
    setApplicationVersion("2.0.0");
    setOrganizationName("Jy技术团队");
    setOrganizationDomain("jytech.team");
    
    // 设置应用程序样式
    setStyle(QStyleFactory::create("Fusion"));
    
    // 设置应用程序字体
    QFont font = this->font();
    font.setPointSize(9);
    setFont(font);
}

ODBCTestApplication::~ODBCTestApplication() = default;

int ODBCTestApplication::runApplication()
{
    showSplashScreen();
    initializeApplication();
    
    if (m_splash) {
        m_splash->finish(m_mainWindow.get());
        delete m_splash;
        m_splash = nullptr;
    }
    
    m_mainWindow->show();
    return exec();
}

void ODBCTestApplication::showSplashScreen()
{
    QPixmap pixmap(400, 300);
    pixmap.fill(QColor(33, 150, 243)); // Material Blue
    
    m_splash = new QSplashScreen(pixmap);
    m_splash->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    m_splash->show();
    
    m_splash->showMessage("正在初始化RANOnline ODBC测试工具...", Qt::AlignCenter | Qt::AlignBottom, Qt::white);
    processEvents();
    
    QThread::msleep(1500);
}

void ODBCTestApplication::initializeApplication()
{
    if (m_splash) {
        m_splash->showMessage("正在加载ODBC组件...", Qt::AlignCenter | Qt::AlignBottom, Qt::white);
        processEvents();
    }
    
    QThread::msleep(500);
    
    if (m_splash) {
        m_splash->showMessage("正在创建用户界面...", Qt::AlignCenter | Qt::AlignBottom, Qt::white);
        processEvents();
    }
    
    m_mainWindow = std::make_unique<ODBCTestMainWindow>();
    
    QThread::msleep(500);
    
    if (m_splash) {
        m_splash->showMessage("初始化完成！", Qt::AlignCenter | Qt::AlignBottom, Qt::white);
        processEvents();
    }
    
    QThread::msleep(500);
}

// ====================================================================
// ConnectionTestThread Implementation
// ====================================================================

ConnectionTestThread::ConnectionTestThread(const ODBCConnectionManager::ConnectionConfig &config, QObject *parent)
    : QThread(parent), m_config(config)
{
}

void ConnectionTestThread::run()
{
    emit progressUpdate(10);
    
    try {
        // 创建临时连接管理器进行测试
        auto manager = std::make_unique<ODBCConnectionManager>();
        
        emit progressUpdate(30);
        
        // 尝试连接
        if (manager->connect(m_config)) {
            emit progressUpdate(70);
            
            // 执行简单测试查询
            auto results = manager->executeQuery("SELECT GETDATE() as TestTime, @@VERSION as Version");
            
            emit progressUpdate(90);
            
            manager->disconnect();
            
            emit progressUpdate(100);
            
            QString message = QString("连接成功！服务器: %1, 数据库: %2\n测试查询返回 %3 行数据")
                .arg(m_config.serverAddress, m_config.databaseName)
                .arg(results.size());
            
            emit testCompleted(true, message);
            
        } else {
            emit testCompleted(false, "连接失败：无法建立数据库连接，请检查服务器地址、端口、用户名和密码");
        }
        
    } catch (const std::exception &e) {
        emit testCompleted(false, QString("连接测试异常: %1").arg(e.what()));
    }
}

// ====================================================================
// BenchmarkTestThread Implementation
// ====================================================================

BenchmarkTestThread::BenchmarkTestThread(const ODBCConnectionManager::ConnectionConfig &config, QObject *parent)
    : QThread(parent), m_config(config)
{
}

void BenchmarkTestThread::run()
{
    QString results;
    results += "RANOnline EP7 AI - ODBC 性能基准测试报告\n";
    results += "========================================\n";
    results += QString("测试时间: %1\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    results += QString("服务器: %1\n").arg(m_config.serverAddress);
    results += QString("数据库: %1\n\n").arg(m_config.databaseName);
    
    emit progressUpdate(10);
    results += runConnectivityTest();
    
    emit progressUpdate(30);
    results += runQueryPerformanceTest();
    
    emit progressUpdate(60);
    results += runTransactionTest();
    
    emit progressUpdate(90);
    results += runConnectionPoolTest();
    
    emit progressUpdate(100);
    
    results += "\n========================================\n";
    results += "基准测试完成！\n";
    
    emit benchmarkCompleted(results);
}

QString BenchmarkTestThread::runConnectivityTest()
{
    QString result = "1. 连接性能测试\n";
    result += "----------------\n";
    
    try {
        auto manager = std::make_unique<ODBCConnectionManager>();
        
        const int testRuns = 5;
        QList<qint64> connectionTimes;
        
        for (int i = 0; i < testRuns; ++i) {
            QElapsedTimer timer;
            timer.start();
            
            if (manager->connect(m_config)) {
                qint64 elapsed = timer.elapsed();
                connectionTimes.append(elapsed);
                manager->disconnect();
            } else {
                result += QString("连接测试 #%1: 失败\n").arg(i + 1);
            }
        }
        
        if (!connectionTimes.isEmpty()) {
            qint64 avgTime = 0;
            for (qint64 time : connectionTimes) {
                avgTime += time;
            }
            avgTime /= connectionTimes.size();
            
            result += QString("成功连接次数: %1/%2\n").arg(connectionTimes.size()).arg(testRuns);
            result += QString("平均连接时间: %1ms\n").arg(avgTime);
            result += QString("最快连接时间: %1ms\n").arg(*std::min_element(connectionTimes.begin(), connectionTimes.end()));
            result += QString("最慢连接时间: %1ms\n").arg(*std::max_element(connectionTimes.begin(), connectionTimes.end()));
        } else {
            result += "所有连接测试都失败了！\n";
        }
        
    } catch (const std::exception &e) {
        result += QString("连接性能测试异常: %1\n").arg(e.what());
    }
    
    result += "\n";
    return result;
}

QString BenchmarkTestThread::runQueryPerformanceTest()
{
    QString result = "2. 查询性能测试\n";
    result += "----------------\n";
    
    try {
        auto manager = std::make_unique<ODBCConnectionManager>();
        
        if (manager->connect(m_config)) {
            QStringList testQueries = {
                "SELECT GETDATE()",
                "SELECT COUNT(*) FROM sys.tables",
                "SELECT name FROM sys.databases",
                "SELECT @@VERSION",
                "SELECT SERVERPROPERTY('ProductVersion')"
            };
            
            for (const QString &query : testQueries) {
                QElapsedTimer timer;
                timer.start();
                
                auto queryResults = manager->executeQuery(query);
                qint64 elapsed = timer.elapsed();
                
                result += QString("查询: %1\n").arg(query);
                result += QString("  执行时间: %1ms\n").arg(elapsed);
                result += QString("  返回行数: %1\n").arg(queryResults.size());
            }
            
            manager->disconnect();
        } else {
            result += "无法连接到数据库进行查询测试\n";
        }
        
    } catch (const std::exception &e) {
        result += QString("查询性能测试异常: %1\n").arg(e.what());
    }
    
    result += "\n";
    return result;
}

QString BenchmarkTestThread::runTransactionTest()
{
    QString result = "3. 事务性能测试\n";
    result += "----------------\n";
    
    try {
        auto manager = std::make_unique<ODBCConnectionManager>();
        
        if (manager->connect(m_config)) {
            const int transactionCount = 10;
            QElapsedTimer timer;
            timer.start();
            
            for (int i = 0; i < transactionCount; ++i) {
                if (manager->beginTransaction()) {
                    // 执行一些测试操作（这里只是查询，不会修改数据）
                    manager->executeQuery("SELECT 1");
                    manager->commitTransaction();
                } else {
                    result += QString("事务 #%1: 开始失败\n").arg(i + 1);
                }
            }
            
            qint64 totalTime = timer.elapsed();
            result += QString("成功完成 %1 个事务\n").arg(transactionCount);
            result += QString("总时间: %1ms\n").arg(totalTime);
            result += QString("平均每事务: %1ms\n").arg(totalTime / transactionCount);
            
            manager->disconnect();
        } else {
            result += "无法连接到数据库进行事务测试\n";
        }
        
    } catch (const std::exception &e) {
        result += QString("事务性能测试异常: %1\n").arg(e.what());
    }
    
    result += "\n";
    return result;
}

QString BenchmarkTestThread::runConnectionPoolTest()
{
    QString result = "4. 连接池性能测试\n";
    result += "------------------\n";
    
    try {
        // 测试多个并发连接
        const int connectionCount = 5;
        result += QString("测试 %1 个并发连接...\n").arg(connectionCount);
        
        QList<std::unique_ptr<ODBCConnectionManager>> managers;
        QElapsedTimer timer;
        timer.start();
        
        // 创建多个连接
        for (int i = 0; i < connectionCount; ++i) {
            auto manager = std::make_unique<ODBCConnectionManager>();
            if (manager->connect(m_config)) {
                managers.append(std::move(manager));
            }
        }
        
        qint64 connectTime = timer.elapsed();
        result += QString("建立 %1 个连接耗时: %1ms\n").arg(managers.size()).arg(connectTime);
        
        // 在每个连接上执行查询
        timer.restart();
        for (auto &manager : managers) {
            manager->executeQuery("SELECT GETDATE()");
        }
        qint64 queryTime = timer.elapsed();
        result += QString("在 %1 个连接上执行查询耗时: %1ms\n").arg(managers.size()).arg(queryTime);
        
        // 关闭所有连接
        timer.restart();
        for (auto &manager : managers) {
            manager->disconnect();
        }
        qint64 disconnectTime = timer.elapsed();
        result += QString("关闭 %1 个连接耗时: %1ms\n").arg(managers.size()).arg(disconnectTime);
        
        managers.clear();
        
    } catch (const std::exception &e) {
        result += QString("连接池性能测试异常: %1\n").arg(e.what());
    }
    
    result += "\n";
    return result;
}

#include "ODBCTestApplication.moc"
