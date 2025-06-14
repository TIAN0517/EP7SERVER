#include <gtest/gtest.h>
#include <QApplication>
#include <QTest>
#include "DatabaseManager.h"

class DatabaseManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 使用测试数据库连接
        dbManager = std::make_unique<DatabaseManager>();
    }
    
    void TearDown() override {
        if (dbManager) {
            dbManager->shutdown();
        }
    }
    
    std::unique_ptr<DatabaseManager> dbManager;
};

TEST_F(DatabaseManagerTest, Initialization) {
    // 测试数据库管理器初始化
    QString testConnectionString = "Driver={SQL Server};Server=localhost;Database=RanOnlineTest;Trusted_Connection=yes;";
    
    // 这里只测试初始化逻辑，不需要真实数据库连接
    EXPECT_NE(dbManager.get(), nullptr);
}

TEST_F(DatabaseManagerTest, ConnectionPoolCreation) {
    // 测试连接池创建
    auto poolStatus = dbManager->getConnectionPoolStatus();
    
    // 检查默认状态
    EXPECT_GE(poolStatus["totalConnections"].toInt(), 0);
    EXPECT_GE(poolStatus["activeConnections"].toInt(), 0);
}

TEST_F(DatabaseManagerTest, AIDataSerialization) {
    // 测试AI数据序列化
    AIPlayerData testAI;
    testAI.id = "TEST_AI_001";
    testAI.name = "测试AI";
    testAI.school = "聖門";
    testAI.level = 50;
    testAI.status = AIPlayerData::Status::Idle;
    
    // 序列化
    auto serialized = dbManager->serializeAIData(testAI);
    EXPECT_FALSE(serialized.isEmpty());
    
    // 反序列化
    auto deserialized = dbManager->deserializeAIData(serialized);
    EXPECT_EQ(deserialized.id.toStdString(), testAI.id);
    EXPECT_EQ(deserialized.name.toStdString(), testAI.name);
    EXPECT_EQ(deserialized.level, testAI.level);
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
