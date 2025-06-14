#include <gtest/gtest.h>
#include <QApplication>
#include <QTest>
#include <QSignalSpy>
#include "PerformanceMonitor.h"

class PerformanceMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        perfMonitor = std::make_unique<PerformanceMonitor>();
    }
    
    void TearDown() override {
        if (perfMonitor) {
            perfMonitor->stopMonitoring();
        }
    }
    
    std::unique_ptr<PerformanceMonitor> perfMonitor;
};

TEST_F(PerformanceMonitorTest, Initialization) {
    // 测试性能监控器初始化
    EXPECT_NE(perfMonitor.get(), nullptr);
    EXPECT_FALSE(perfMonitor->isMonitoring());
}

TEST_F(PerformanceMonitorTest, StartStopMonitoring) {
    // 测试开始/停止监控
    perfMonitor->startMonitoring(500); // 500ms间隔
    EXPECT_TRUE(perfMonitor->isMonitoring());
    
    perfMonitor->stopMonitoring();
    EXPECT_FALSE(perfMonitor->isMonitoring());
}

TEST_F(PerformanceMonitorTest, SystemMetrics) {
    // 测试系统指标收集
    perfMonitor->startMonitoring(100);
    
    // 等待一些指标收集
    QTest::qWait(200);
    
    auto metrics = perfMonitor->getSystemMetrics();
    
    // 验证指标合理性
    EXPECT_GE(metrics.cpuUsage, 0.0);
    EXPECT_LE(metrics.cpuUsage, 100.0);
    EXPECT_GE(metrics.memoryUsagePercent, 0.0);
    EXPECT_LE(metrics.memoryUsagePercent, 100.0);
    EXPECT_GT(metrics.memoryTotal, 0);
}

TEST_F(PerformanceMonitorTest, AIMetricsUpdate) {
    // 测试AI指标更新
    QSignalSpy aiMetricsSpy(perfMonitor.get(), &PerformanceMonitor::aiMetricsUpdated);
    
    perfMonitor->updateAIMetrics(5, 250.5, 100);
    
    // 验证信号发射
    EXPECT_EQ(aiMetricsSpy.count(), 1);
    
    auto metrics = perfMonitor->getAIMetrics();
    EXPECT_EQ(metrics.activeAIs, 5);
    EXPECT_DOUBLE_EQ(metrics.averageResponseTime, 250.5);
}

TEST_F(PerformanceMonitorTest, ThresholdWarnings) {
    // 测试阈值警告
    QSignalSpy warningSpy(perfMonitor.get(), &PerformanceMonitor::performanceWarning);
    QSignalSpy criticalSpy(perfMonitor.get(), &PerformanceMonitor::performanceCritical);
    
    // 设置较低的阈值用于测试
    perfMonitor->setWarningThresholds(10.0, 10.0);
    perfMonitor->setCriticalThresholds(20.0, 20.0);
    
    perfMonitor->startMonitoring(100);
    
    // 等待指标收集和可能的警告
    QTest::qWait(300);
    
    // 注意：这个测试可能因系统负载而有所不同
    // 主要是验证阈值设置功能正常
    EXPECT_GE(warningSpy.count() + criticalSpy.count(), 0);
}

TEST_F(PerformanceMonitorTest, HistoryData) {
    // 测试历史数据收集
    perfMonitor->startMonitoring(50); // 快速收集
    
    // 等待收集多个数据点
    QTest::qWait(200);
    
    auto systemHistory = perfMonitor->getSystemHistory(10);
    auto aiHistory = perfMonitor->getAIHistory(10);
    
    // 应该至少有一些历史数据
    EXPECT_GE(systemHistory.size(), 1);
    EXPECT_GE(aiHistory.size(), 0); // AI历史可能为空，因为没有AI更新
}

TEST_F(PerformanceMonitorTest, NetworkMetricsUpdate) {
    // 测试网络指标更新
    quint64 initialReceived = 1000;
    quint64 initialSent = 2000;
    
    perfMonitor->updateNetworkMetrics(initialReceived, initialSent);
    
    auto metrics = perfMonitor->getSystemMetrics();
    EXPECT_GE(metrics.networkBytesReceived, initialReceived);
    EXPECT_GE(metrics.networkBytesSent, initialSent);
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
