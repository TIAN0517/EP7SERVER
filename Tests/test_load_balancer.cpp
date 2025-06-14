#include <gtest/gtest.h>
#include <QApplication>
#include <QTest>
#include <QSignalSpy>
#include "LoadBalancer.h"

class LoadBalancerTest : public ::testing::Test {
protected:
    void SetUp() override {
        loadBalancer = std::make_unique<LoadBalancer>();
        
        // 创建测试AI实例
        LoadBalancer::AIInstance instance1;
        instance1.id = "AI_001";
        instance1.name = "测试AI-1";
        instance1.isActive = true;
        instance1.isHealthy = true;
        instance1.maxConnections = 100;
        instance1.weight = 1;
        
        LoadBalancer::AIInstance instance2;
        instance2.id = "AI_002";
        instance2.name = "测试AI-2";
        instance2.isActive = true;
        instance2.isHealthy = true;
        instance2.maxConnections = 150;
        instance2.weight = 2;
        
        loadBalancer->addAIInstance(instance1);
        loadBalancer->addAIInstance(instance2);
    }
    
    void TearDown() override {
        // 清理
    }
    
    std::unique_ptr<LoadBalancer> loadBalancer;
};

TEST_F(LoadBalancerTest, InstanceManagement) {
    // 测试实例管理
    auto instances = loadBalancer->getAllAIInstances();
    EXPECT_EQ(instances.size(), 2);
    
    auto healthyInstances = loadBalancer->getHealthyAIInstances();
    EXPECT_EQ(healthyInstances.size(), 2);
}

TEST_F(LoadBalancerTest, RoundRobinAlgorithm) {
    // 测试轮询算法
    loadBalancer->setAlgorithm(LoadBalancer::Algorithm::RoundRobin);
    EXPECT_EQ(loadBalancer->getCurrentAlgorithm(), LoadBalancer::Algorithm::RoundRobin);
    
    // 创建测试请求
    LoadBalancer::Request request1;
    request1.type = "TEST";
    request1.data = "Test Request 1";
    
    LoadBalancer::Request request2;
    request2.type = "TEST";
    request2.data = "Test Request 2";
    
    QString requestId1 = loadBalancer->submitRequest(request1);
    QString requestId2 = loadBalancer->submitRequest(request2);
    
    EXPECT_FALSE(requestId1.isEmpty());
    EXPECT_FALSE(requestId2.isEmpty());
    EXPECT_NE(requestId1, requestId2);
}

TEST_F(LoadBalancerTest, LeastConnectionsAlgorithm) {
    // 测试最少连接算法
    loadBalancer->setAlgorithm(LoadBalancer::Algorithm::LeastConnections);
    
    // 更新实例连接数
    loadBalancer->updateInstanceMetrics("AI_001", 50.0, 60.0, 10);
    loadBalancer->updateInstanceMetrics("AI_002", 40.0, 50.0, 5);
    
    // 提交请求，应该选择连接数较少的实例
    LoadBalancer::Request request;
    request.type = "TEST";
    request.data = "Least Connections Test";
    
    QString requestId = loadBalancer->submitRequest(request);
    EXPECT_FALSE(requestId.isEmpty());
}

TEST_F(LoadBalancerTest, HealthCheck) {
    // 测试健康检查
    QSignalSpy healthSpy(loadBalancer.get(), &LoadBalancer::instanceHealthChanged);
    
    // 更新实例健康状态
    loadBalancer->updateInstanceHealth("AI_001", false);
    
    // 验证信号发射
    EXPECT_EQ(healthSpy.count(), 1);
    
    // 验证健康实例数量
    auto healthyInstances = loadBalancer->getHealthyAIInstances();
    EXPECT_EQ(healthyInstances.size(), 1);
}

TEST_F(LoadBalancerTest, Statistics) {
    // 测试统计信息
    auto stats = loadBalancer->getStatistics();
    
    EXPECT_EQ(stats.totalInstances, 2);
    EXPECT_EQ(stats.healthyInstances, 2);
    EXPECT_GE(stats.queuedRequests, 0);
    EXPECT_GE(stats.totalRequests, 0);
}

TEST_F(LoadBalancerTest, RequestCancellation) {
    // 测试请求取消
    LoadBalancer::Request request;
    request.type = "CANCEL_TEST";
    request.data = "Test Cancellation";
    
    QString requestId = loadBalancer->submitRequest(request);
    EXPECT_FALSE(requestId.isEmpty());
    
    bool cancelled = loadBalancer->cancelRequest(requestId);
    EXPECT_TRUE(cancelled);
    
    // 再次尝试取消应该失败
    bool cancelledAgain = loadBalancer->cancelRequest(requestId);
    EXPECT_FALSE(cancelledAgain);
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
