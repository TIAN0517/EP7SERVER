#include <gtest/gtest.h>
#include <QApplication>
#include <QTest>
#include "Protocol.h"

class CommunicationProtocolTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前设置
    }
    
    void TearDown() override {
        // 测试后清理
    }
};

TEST_F(CommunicationProtocolTest, MessageCreation) {
    Protocol protocol;
    
    // 测试消息创建
    QByteArray testData = "Hello, RANOnline!";
    auto message = protocol.createMessage("TEST", testData);
    
    EXPECT_FALSE(message.isEmpty());
    EXPECT_GT(message.size(), testData.size()); // 包含头部信息
}

TEST_F(CommunicationProtocolTest, MessageParsing) {
    Protocol protocol;
    
    // 创建测试消息
    QByteArray testData = "Test Message";
    auto message = protocol.createMessage("PING", testData);
    
    // 解析消息
    QString messageType;
    QByteArray messageData;
    bool success = protocol.parseMessage(message, messageType, messageData);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(messageType.toStdString(), "PING");
    EXPECT_EQ(messageData, testData);
}

TEST_F(CommunicationProtocolTest, ChecksumValidation) {
    Protocol protocol;
    
    QByteArray testData = "Checksum Test";
    auto message = protocol.createMessage("CHECK", testData);
    
    // 验证校验和
    EXPECT_TRUE(protocol.validateChecksum(message));
    
    // 破坏消息
    if (!message.isEmpty()) {
        message[message.size()-1] = ~message[message.size()-1];
        EXPECT_FALSE(protocol.validateChecksum(message));
    }
}

int main(int argc, char **argv) {
    QApplication app(argc, argv); // Qt需要QApplication
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
