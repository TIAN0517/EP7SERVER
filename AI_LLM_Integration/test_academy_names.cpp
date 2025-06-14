/**
 * @file test_academy_names.cpp
 * @brief 測試新學院命名系統
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 1.0.0
 */

#include "AIPlayerGenerator.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "=== RAN Online 四部門AI學院命名系統測試 ===";
    qDebug() << "測試新的三學院命名規則：";
    qDebug() << "• 聖門：God主題（威嚴神聖）";
    qDebug() << "• 懸岩：水墨風格（詩意文雅）";
    qDebug() << "• 鳳凰：心情感系列（情感豐富）";
    qDebug() << "";
    
    RANOnline::AI::AIPlayerGenerator generator;
    
    // 測試每個學院每個部門的名字生成
    QStringList academies = {"聖門", "懸岩", "鳳凰"};
    QStringList departments = {"劍道", "弓箭", "格鬥", "氣功"};
    
    for (const QString &academy : academies) {
        qDebug() << QString("=== %1學院 ===").arg(academy);
        
        for (const QString &department : departments) {
            qDebug() << QString("【%1部門】").arg(department);
            
            // 生成5個測試AI
            for (int i = 0; i < 5; ++i) {
                // 生成單個AI來測試命名
                QJsonArray testPlayers = generator.generateAIPlayers(1);
                if (!testPlayers.isEmpty()) {
                    QJsonObject player = testPlayers[0].toObject();
                    
                    // 確保是我們想要的學院和部門
                    QString actualAcademy = player["academy"].toString();
                    QString actualDept = player["department"].toString();
                    QString name = player["name"].toString();
                    QString personality = player["personality"].toString();
                    QString signatureLine = player["signature_line"].toString();
                    
                    // 只顯示匹配的學院和部門
                    if (actualAcademy == academy && actualDept == department) {
                        qDebug() << QString("  %1. %2 | %3 | \"%4\"")
                                    .arg(i + 1)
                                    .arg(name)
                                    .arg(personality)
                                    .arg(signatureLine);
                        
                        // 檢查名字長度
                        if (name.length() > 6) {
                            qDebug() << QString("    ⚠️ 警告：名字超過6字符限制！長度=%1").arg(name.length());
                        }
                    } else {
                        // 如果不匹配，重新生成
                        i--;
                        continue;
                    }
                }
            }
            qDebug() << "";
        }
        qDebug() << "";
    }
    
    // 統計測試
    qDebug() << "=== 命名系統統計測試 ===";
    QJsonArray bulkTest = generator.generateAIPlayers(100);
    
    QMap<QString, int> academyCount;
    QMap<QString, int> nameLength;
    QMap<QString, QStringList> sampleNames;
    
    for (const QJsonValue &value : bulkTest) {
        QJsonObject player = value.toObject();
        QString academy = player["academy"].toString();
        QString name = player["name"].toString();
        
        academyCount[academy]++;
        nameLength[QString::number(name.length())]++;
        
        if (sampleNames[academy].size() < 3) {
            sampleNames[academy].append(name);
        }
    }
    
    qDebug() << "學院分佈：";
    for (auto it = academyCount.begin(); it != academyCount.end(); ++it) {
        qDebug() << QString("  %1: %2名AI").arg(it.key()).arg(it.value());
        qDebug() << QString("    樣本名字: %1").arg(sampleNames[it.key()].join(", "));
    }
    
    qDebug() << "";
    qDebug() << "名字長度分佈：";
    for (auto it = nameLength.begin(); it != nameLength.end(); ++it) {
        qDebug() << QString("  %1字符: %2個名字").arg(it.key()).arg(it.value());
    }
    
    // 檢查是否有超過6字符的名字
    bool hasOverlength = false;
    for (const QJsonValue &value : bulkTest) {
        QJsonObject player = value.toObject();
        QString name = player["name"].toString();
        if (name.length() > 6) {
            qDebug() << QString("⚠️ 發現超長名字: %1 (%2字符)").arg(name).arg(name.length());
            hasOverlength = true;
        }
    }
    
    if (!hasOverlength) {
        qDebug() << "✅ 所有名字都符合6字符限制！";
    }
    
    qDebug() << "";
    qDebug() << "=== 測試完成 ===";
    
    return 0;
}
