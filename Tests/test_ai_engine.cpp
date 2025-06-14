#include <gtest/gtest.h>
#include <QApplication>
#include <QTest>
#include "AIEngine.h"

class AIEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        aiEngine = std::make_unique<AIEngine>();
    }
    
    void TearDown() override {
        if (aiEngine) {
            aiEngine->stop();
        }
    }
    
    std::unique_ptr<AIEngine> aiEngine;
};

TEST_F(AIEngineTest, Initialization) {
    // 测试AI引擎初始化
    EXPECT_NE(aiEngine.get(), nullptr);
    EXPECT_FALSE(aiEngine->isRunning());
}

TEST_F(AIEngineTest, AICreation) {
    // 测试AI创建
    QString aiId = aiEngine->createAI("测试AI", "聖門", 1, 10);
    EXPECT_FALSE(aiId.isEmpty());
    
    // 验证AI存在
    auto ai = aiEngine->getAI(aiId);
    EXPECT_FALSE(ai.isNull());
    EXPECT_EQ(ai->getName(), "测试AI");
    EXPECT_EQ(ai->getSchool(), "聖門");
}

TEST_F(AIEngineTest, BatchAICreation) {
    // 测试批量AI创建
    auto aiIds = aiEngine->createBatchAI(5, "聖門", 1, 20);
    EXPECT_EQ(aiIds.size(), 5);
    
    // 验证所有AI都被创建
    for (const auto& id : aiIds) {
        auto ai = aiEngine->getAI(id);
        EXPECT_FALSE(ai.isNull());
    }
}

TEST_F(AIEngineTest, LoadBalancing) {
    // 测试负载均衡
    auto aiIds = aiEngine->createBatchAI(10, "聖門", 1, 30);
    
    // 检查AI分布
    auto serverLoads = aiEngine->getServerLoads();
    EXPECT_EQ(serverLoads.size(), 4); // 4个服务器
    
    // 验证负载均衡
    int totalAIs = 0;
    for (auto load : serverLoads) {
        totalAIs += load;
    }
    EXPECT_EQ(totalAIs, aiIds.size());
}

TEST_F(AIEngineTest, PerformanceMetrics) {
    // 测试性能指标
    auto stats = aiEngine->getPerformanceStats();
    
    EXPECT_GE(stats["avgDecisionTime"].toDouble(), 0.0);
    EXPECT_GE(stats["commandsPerSecond"].toDouble(), 0.0);
    EXPECT_GE(stats["memoryUsageMB"].toDouble(), 0.0);
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
