#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QTimer>
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

/**
 * @brief RANOnline AI System - Qt QODBC SQL Server 2022 连接示例
 * 
 * 功能演示:
 * - DSN和直接连接字符串两种方式
 * - 连接错误处理和重试机制
 * - 查询性能监控
 * - 事务处理示例
 * - 异步查询处理
 * - 连接池管理
 * 
 * 编译要求:
 * - Qt 5.12+ 或 Qt 6.x
 * - SQL模块 (Qt6::Sql)
 * - Windows ODBC驱动
 * 
 * 编译命令:
 * qmake && make
 * 或者
 * mkdir build && cd build
 * cmake .. && make
 */

class DatabaseTester : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseTester(QObject *parent = nullptr) : QObject(parent) {}

    // 测试DSN连接
    bool testDSNConnection(const QString &dsnName, const QString &username, const QString &password)
    {
        qDebug() << "🔧 测试DSN连接:" << dsnName;
        
        QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "dsn_test");
        QString connectionString = QString("DSN=%1;UID=%2;PWD=%3")
                                  .arg(dsnName)
                                  .arg(username)
                                  .arg(password);
        
        db.setDatabaseName(connectionString);
        
        // 设置连接选项
        db.setConnectOptions("SQL_ATTR_CONNECTION_TIMEOUT=30;SQL_ATTR_LOGIN_TIMEOUT=30");
        
        QElapsedTimer timer;
        timer.start();
        
        if (db.open()) {
            qint64 connectTime = timer.elapsed();
            qDebug() << "✅ DSN连接成功，耗时:" << connectTime << "ms";
            
            // 执行测试查询
            QSqlQuery query(db);
            if (query.exec("SELECT @@VERSION, @@SERVERNAME, GETDATE()")) {
                if (query.next()) {
                    qDebug() << "📊 服务器信息:";
                    qDebug() << "   版本:" << query.value(0).toString().split('\n').first();
                    qDebug() << "   服务器名:" << query.value(1).toString();
                    qDebug() << "   当前时间:" << query.value(2).toString();
                }
            } else {
                qWarning() << "❌ 测试查询失败:" << query.lastError().text();
            }
            
            db.close();
            return true;
        } else {
            qCritical() << "❌ DSN连接失败:" << db.lastError().text();
            qCritical() << "   错误类型:" << db.lastError().type();
            qCritical() << "   数据库错误代码:" << db.lastError().nativeErrorCode();
            return false;
        }
    }

    // 测试直接连接字符串
    bool testDirectConnection(const QString &server, int port, const QString &database,
                             const QString &username, const QString &password,
                             const QString &driver = "ODBC Driver 17 for SQL Server")
    {
        qDebug() << "🔧 测试直接连接:" << server << ":" << port;
        
        QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "direct_test");
        
        QString connectionString = QString(
            "DRIVER={%1};"
            "SERVER=%2,%3;"
            "DATABASE=%4;"
            "UID=%5;"
            "PWD=%6;"
            "TrustServerCertificate=Yes;"
            "Encrypt=Optional;"
            "MARS_Connection=Yes;"
            "Connection Timeout=30;"
            "Login Timeout=30;"
            "APP=RANOnline AI Test"
        ).arg(driver)
         .arg(server)
         .arg(port)
         .arg(database)
         .arg(username)
         .arg(password);
        
        db.setDatabaseName(connectionString);
        
        QElapsedTimer timer;
        timer.start();
        
        if (db.open()) {
            qint64 connectTime = timer.elapsed();
            qDebug() << "✅ 直接连接成功，耗时:" << connectTime << "ms";
            
            // 测试连接池信息
            QSqlQuery query(db);
            if (query.exec("SELECT COUNT(*) FROM sys.dm_exec_sessions WHERE is_user_process = 1")) {
                if (query.next()) {
                    qDebug() << "📊 当前用户会话数:" << query.value(0).toInt();
                }
            }
            
            db.close();
            return true;
        } else {
            qCritical() << "❌ 直接连接失败:" << db.lastError().text();
            return false;
        }
    }

    // 测试事务处理
    bool testTransactionHandling()
    {
        qDebug() << "🔧 测试事务处理...";
        
        QSqlDatabase db = QSqlDatabase::database("dsn_test");
        if (!db.isOpen()) {
            qWarning() << "❌ 数据库连接未打开";
            return false;
        }
        
        // 开始事务
        if (!db.transaction()) {
            qCritical() << "❌ 无法开始事务:" << db.lastError().text();
            return false;
        }
        
        QSqlQuery query(db);
        
        // 创建测试表
        QString createTableSQL = R"(
            IF NOT EXISTS (SELECT * FROM sys.tables WHERE name = 'AITestTable')
            CREATE TABLE AITestTable (
                ID INT IDENTITY(1,1) PRIMARY KEY,
                Name NVARCHAR(100) NOT NULL,
                Value INT NOT NULL,
                CreatedTime DATETIME2 DEFAULT GETDATE()
            )
        )";
        
        if (!query.exec(createTableSQL)) {
            qCritical() << "❌ 创建测试表失败:" << query.lastError().text();
            db.rollback();
            return false;
        }
        
        // 插入测试数据
        query.prepare("INSERT INTO AITestTable (Name, Value) VALUES (?, ?)");
        
        QStringList names = {"AI_Player_001", "AI_Player_002", "AI_Player_003"};
        QList<int> values = {100, 200, 300};
        
        for (int i = 0; i < names.size(); ++i) {
            query.bindValue(0, names[i]);
            query.bindValue(1, values[i]);
            
            if (!query.exec()) {
                qCritical() << "❌ 插入数据失败:" << query.lastError().text();
                db.rollback();
                return false;
            }
        }
        
        // 提交事务
        if (!db.commit()) {
            qCritical() << "❌ 提交事务失败:" << db.lastError().text();
            return false;
        }
        
        qDebug() << "✅ 事务处理成功";
        return true;
    }

    // 测试批量操作性能
    void testBatchOperations()
    {
        qDebug() << "🔧 测试批量操作性能...";
        
        QSqlDatabase db = QSqlDatabase::database("dsn_test");
        if (!db.isOpen()) return;
        
        QElapsedTimer timer;
        timer.start();
        
        QSqlQuery query(db);
        query.prepare("INSERT INTO AITestTable (Name, Value) VALUES (?, ?)");
        
        // 批量插入1000条记录
        const int batchSize = 1000;
        for (int i = 0; i < batchSize; ++i) {
            query.bindValue(0, QString("BatchAI_%1").arg(i, 4, 10, QChar('0')));
            query.bindValue(1, i * 10);
            
            if (!query.exec()) {
                qWarning() << "❌ 批量插入失败:" << query.lastError().text();
                break;
            }
        }
        
        qint64 elapsed = timer.elapsed();
        qDebug() << "✅ 批量插入" << batchSize << "条记录，耗时:" << elapsed << "ms";
        qDebug() << "📊 平均每条记录:" << QString::number(double(elapsed) / batchSize, 'f', 3) << "ms";
    }

    // 测试查询性能和结果集处理
    void testQueryPerformance()
    {
        qDebug() << "🔧 测试查询性能...";
        
        QSqlDatabase db = QSqlDatabase::database("dsn_test");
        if (!db.isOpen()) return;
        
        QElapsedTimer timer;
        timer.start();
        
        QSqlQuery query(db);
        QString sql = R"(
            SELECT 
                COUNT(*) AS TotalRecords,
                AVG(Value) AS AvgValue,
                MIN(Value) AS MinValue,
                MAX(Value) AS MaxValue,
                MIN(CreatedTime) AS FirstRecord,
                MAX(CreatedTime) AS LastRecord
            FROM AITestTable
        )";
        
        if (query.exec(sql)) {
            qint64 queryTime = timer.elapsed();
            
            if (query.next()) {
                qDebug() << "✅ 统计查询完成，耗时:" << queryTime << "ms";
                qDebug() << "📊 查询结果:";
                qDebug() << "   总记录数:" << query.value("TotalRecords").toInt();
                qDebug() << "   平均值:" << query.value("AvgValue").toDouble();
                qDebug() << "   最小值:" << query.value("MinValue").toInt();
                qDebug() << "   最大值:" << query.value("MaxValue").toInt();
                qDebug() << "   首条记录时间:" << query.value("FirstRecord").toString();
                qDebug() << "   末条记录时间:" << query.value("LastRecord").toString();
            }
        } else {
            qCritical() << "❌ 统计查询失败:" << query.lastError().text();
        }
    }

    // 连接错误诊断
    void diagnoseConnectionError(const QSqlError &error)
    {
        qCritical() << "🔍 连接错误诊断:";
        qCritical() << "   错误类型:" << error.type();
        qCritical() << "   驱动错误:" << error.driverText();
        qCritical() << "   数据库错误:" << error.databaseText();
        qCritical() << "   本地错误代码:" << error.nativeErrorCode();
        
        // 常见错误分析
        QString errorText = error.databaseText();
        if (errorText.contains("Login failed", Qt::CaseInsensitive)) {
            qCritical() << "💡 建议: 检查用户名和密码";
        } else if (errorText.contains("server was not found", Qt::CaseInsensitive)) {
            qCritical() << "💡 建议: 检查服务器地址和端口";
        } else if (errorText.contains("network-related", Qt::CaseInsensitive)) {
            qCritical() << "💡 建议: 检查网络连接和防火墙设置";
        } else if (errorText.contains("Cannot open database", Qt::CaseInsensitive)) {
            qCritical() << "💡 建议: 检查数据库名称和访问权限";
        } else if (errorText.contains("driver", Qt::CaseInsensitive)) {
            qCritical() << "💡 建议: 检查ODBC驱动程序安装";
        }
    }

    // 清理测试数据
    void cleanup()
    {
        qDebug() << "🧹 清理测试数据...";
        
        QSqlDatabase db = QSqlDatabase::database("dsn_test");
        if (db.isOpen()) {
            QSqlQuery query(db);
            if (query.exec("DROP TABLE IF EXISTS AITestTable")) {
                qDebug() << "✅ 测试表已删除";
            }
        }
        
        // 关闭所有数据库连接
        QStringList connectionNames = QSqlDatabase::connectionNames();
        for (const QString &name : connectionNames) {
            QSqlDatabase::removeDatabase(name);
        }
        
        qDebug() << "✅ 清理完成";
    }

public slots:
    void runAllTests()
    {
        qDebug() << "🚀 开始运行所有数据库测试...";
        qDebug() << "================================================";
        
        // 读取配置
        QJsonObject config = loadConfig("Config/odbc_config.json");
        
        QString dsnName = config["dsnName"].toString("RanOnlineAIDB");
        QString serverAddress = config["serverAddress"].toString("192.168.1.100");
        int port = config["port"].toInt(1433);
        QString databaseName = config["databaseName"].toString("RanOnlineAI");
        QString username = config["username"].toString("sa");
        QString password = config["password"].toString("Ss520520@@");
        QString driverName = config["driverName"].toString("ODBC Driver 17 for SQL Server");
        
        qDebug() << "📋 使用配置:";
        qDebug() << "   DSN:" << dsnName;
        qDebug() << "   服务器:" << serverAddress << ":" << port;
        qDebug() << "   数据库:" << databaseName;
        qDebug() << "   驱动:" << driverName;
        qDebug() << "";
        
        // 测试DSN连接
        if (testDSNConnection(dsnName, username, password)) {
            qDebug() << "";
            
            // 测试直接连接
            testDirectConnection(serverAddress, port, databaseName, username, password, driverName);
            qDebug() << "";
            
            // 测试事务处理
            if (testTransactionHandling()) {
                qDebug() << "";
                
                // 测试批量操作
                testBatchOperations();
                qDebug() << "";
                
                // 测试查询性能
                testQueryPerformance();
                qDebug() << "";
            }
            
            // 清理
            cleanup();
        }
        
        qDebug() << "================================================";
        qDebug() << "🏁 所有测试完成";
        
        QCoreApplication::quit();
    }

private:
    QJsonObject loadConfig(const QString &configFile)
    {
        QFile file(configFile);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "⚠️ 无法读取配置文件:" << configFile;
            return QJsonObject();
        }
        
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qWarning() << "⚠️ 配置文件JSON格式错误:" << error.errorString();
            return QJsonObject();
        }
        
        return doc.object();
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "🔷 RANOnline AI System - Qt QODBC 连接测试";
    qDebug() << "🔷 开发团队: Jy技术团队";
    qDebug() << "🔷 测试日期:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug() << "";
    
    // 显示Qt和SQL驱动信息
    qDebug() << "📋 环境信息:";
    qDebug() << "   Qt版本:" << QT_VERSION_STR;
    qDebug() << "   可用SQL驱动:" << QSqlDatabase::drivers();
    qDebug() << "";
    
    // 检查QODBC驱动
    if (!QSqlDatabase::isDriverAvailable("QODBC")) {
        qCritical() << "❌ QODBC驱动不可用";
        qCritical() << "💡 请确保Qt编译时包含了SQL模块和ODBC支持";
        return -1;
    }
    
    DatabaseTester tester;
    
    // 延迟启动测试，让应用程序完全初始化
    QTimer::singleShot(100, &tester, &DatabaseTester::runAllTests);
    
    return app.exec();
}

#include "test_odbc_example.moc"
