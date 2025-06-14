/****************************************************************************
** Meta object code from reading C++ file 'LLMMainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../LLMMainWindow.h"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LLMMainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN13LLMMainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto LLMMainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN13LLMMainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "LLMMainWindow",
        "onSubmitRequest",
        "",
        "onSubmitBatchRequests",
        "onCancelAllRequests",
        "onRefreshModels",
        "onExportLogs",
        "onImportTemplates",
        "onExportTemplates",
        "onAddServer",
        "onRemoveServer",
        "onTestConnection",
        "onThemeChanged",
        "onShowSettings",
        "onShowAbout",
        "onToggleFullscreen",
        "onMinimizeToTray",
        "onLLMRequestCompleted",
        "LLMResponse",
        "response",
        "onLLMRequestFailed",
        "requestId",
        "error",
        "onLLMStreamUpdate",
        "content",
        "onLLMRequestProgress",
        "progress",
        "onLLMModelsRefreshed",
        "QList<LLMModelInfo>",
        "models",
        "onLLMServerStatusChanged",
        "serverName",
        "isOnline",
        "onLLMStatisticsUpdated",
        "onGameEventReceived",
        "eventType",
        "data",
        "onPlayerMessageReceived",
        "playerId",
        "message",
        "onSkillGenerationRequest",
        "academy",
        "level",
        "onGameConnectionStatusChanged",
        "connected",
        "onTrayIconActivated",
        "QSystemTrayIcon::ActivationReason",
        "reason",
        "onAIManagementTabActivated",
        "onAIPlayersGenerated",
        "players",
        "onAIBatchDecisionCompleted",
        "results",
        "onAITestCompleted",
        "RANOnline::AI::TestReport",
        "report",
        "onAILogGenerated",
        "logEntry",
        "onAutoRepairTriggered",
        "aiId",
        "repairAction",
        "onAIPlayerGenerated",
        "RANOnline::AI::AIPlayerData",
        "player",
        "onAIErrorOccurred",
        "onAIDecisionMade",
        "RANOnline::AI::AIDecisionResponse",
        "onAIDecisionError",
        "onTestAIRequested",
        "count",
        "modelType"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onSubmitRequest'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSubmitBatchRequests'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCancelAllRequests'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRefreshModels'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExportLogs'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onImportTemplates'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExportTemplates'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddServer'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRemoveServer'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTestConnection'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onThemeChanged'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onShowSettings'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onShowAbout'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToggleFullscreen'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMinimizeToTray'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLLMRequestCompleted'
        QtMocHelpers::SlotData<void(const LLMResponse &)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 18, 19 },
        }}),
        // Slot 'onLLMRequestFailed'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 21 }, { QMetaType::QString, 22 },
        }}),
        // Slot 'onLLMStreamUpdate'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 21 }, { QMetaType::QString, 24 },
        }}),
        // Slot 'onLLMRequestProgress'
        QtMocHelpers::SlotData<void(const QString &, int)>(25, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 21 }, { QMetaType::Int, 26 },
        }}),
        // Slot 'onLLMModelsRefreshed'
        QtMocHelpers::SlotData<void(const QList<LLMModelInfo> &)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 28, 29 },
        }}),
        // Slot 'onLLMServerStatusChanged'
        QtMocHelpers::SlotData<void(const QString &, bool)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 31 }, { QMetaType::Bool, 32 },
        }}),
        // Slot 'onLLMStatisticsUpdated'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGameEventReceived'
        QtMocHelpers::SlotData<void(const QString &, const QJsonObject &)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 35 }, { QMetaType::QJsonObject, 36 },
        }}),
        // Slot 'onPlayerMessageReceived'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(37, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 38 }, { QMetaType::QString, 39 },
        }}),
        // Slot 'onSkillGenerationRequest'
        QtMocHelpers::SlotData<void(const QString &, int)>(40, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 41 }, { QMetaType::Int, 42 },
        }}),
        // Slot 'onGameConnectionStatusChanged'
        QtMocHelpers::SlotData<void(bool)>(43, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 44 },
        }}),
        // Slot 'onTrayIconActivated'
        QtMocHelpers::SlotData<void(QSystemTrayIcon::ActivationReason)>(45, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 46, 47 },
        }}),
        // Slot 'onAIManagementTabActivated'
        QtMocHelpers::SlotData<void()>(48, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAIPlayersGenerated'
        QtMocHelpers::SlotData<void(const QJsonArray &)>(49, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QJsonArray, 50 },
        }}),
        // Slot 'onAIBatchDecisionCompleted'
        QtMocHelpers::SlotData<void(const QJsonArray &)>(51, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QJsonArray, 52 },
        }}),
        // Slot 'onAITestCompleted'
        QtMocHelpers::SlotData<void(const RANOnline::AI::TestReport &)>(53, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 54, 55 },
        }}),
        // Slot 'onAILogGenerated'
        QtMocHelpers::SlotData<void(const QString &)>(56, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 57 },
        }}),
        // Slot 'onAutoRepairTriggered'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(58, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 59 }, { QMetaType::QString, 60 },
        }}),
        // Slot 'onAIPlayerGenerated'
        QtMocHelpers::SlotData<void(const RANOnline::AI::AIPlayerData &)>(61, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 62, 63 },
        }}),
        // Slot 'onAIErrorOccurred'
        QtMocHelpers::SlotData<void(const QString &)>(64, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 22 },
        }}),
        // Slot 'onAIDecisionMade'
        QtMocHelpers::SlotData<void(const RANOnline::AI::AIDecisionResponse &)>(65, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 66, 19 },
        }}),
        // Slot 'onAIDecisionError'
        QtMocHelpers::SlotData<void(const QString &)>(67, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 22 },
        }}),
        // Slot 'onTestAIRequested'
        QtMocHelpers::SlotData<void(int, const QString &)>(68, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 69 }, { QMetaType::QString, 70 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<LLMMainWindow, qt_meta_tag_ZN13LLMMainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject LLMMainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<FramelessWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13LLMMainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13LLMMainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13LLMMainWindowE_t>.metaTypes,
    nullptr
} };

void LLMMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LLMMainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onSubmitRequest(); break;
        case 1: _t->onSubmitBatchRequests(); break;
        case 2: _t->onCancelAllRequests(); break;
        case 3: _t->onRefreshModels(); break;
        case 4: _t->onExportLogs(); break;
        case 5: _t->onImportTemplates(); break;
        case 6: _t->onExportTemplates(); break;
        case 7: _t->onAddServer(); break;
        case 8: _t->onRemoveServer(); break;
        case 9: _t->onTestConnection(); break;
        case 10: _t->onThemeChanged(); break;
        case 11: _t->onShowSettings(); break;
        case 12: _t->onShowAbout(); break;
        case 13: _t->onToggleFullscreen(); break;
        case 14: _t->onMinimizeToTray(); break;
        case 15: _t->onLLMRequestCompleted((*reinterpret_cast< std::add_pointer_t<LLMResponse>>(_a[1]))); break;
        case 16: _t->onLLMRequestFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 17: _t->onLLMStreamUpdate((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 18: _t->onLLMRequestProgress((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 19: _t->onLLMModelsRefreshed((*reinterpret_cast< std::add_pointer_t<QList<LLMModelInfo>>>(_a[1]))); break;
        case 20: _t->onLLMServerStatusChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 21: _t->onLLMStatisticsUpdated(); break;
        case 22: _t->onGameEventReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[2]))); break;
        case 23: _t->onPlayerMessageReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 24: _t->onSkillGenerationRequest((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 25: _t->onGameConnectionStatusChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 26: _t->onTrayIconActivated((*reinterpret_cast< std::add_pointer_t<QSystemTrayIcon::ActivationReason>>(_a[1]))); break;
        case 27: _t->onAIManagementTabActivated(); break;
        case 28: _t->onAIPlayersGenerated((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 29: _t->onAIBatchDecisionCompleted((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 30: _t->onAITestCompleted((*reinterpret_cast< std::add_pointer_t<RANOnline::AI::TestReport>>(_a[1]))); break;
        case 31: _t->onAILogGenerated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 32: _t->onAutoRepairTriggered((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 33: _t->onAIPlayerGenerated((*reinterpret_cast< std::add_pointer_t<RANOnline::AI::AIPlayerData>>(_a[1]))); break;
        case 34: _t->onAIErrorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 35: _t->onAIDecisionMade((*reinterpret_cast< std::add_pointer_t<RANOnline::AI::AIDecisionResponse>>(_a[1]))); break;
        case 36: _t->onAIDecisionError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 37: _t->onTestAIRequested((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *LLMMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LLMMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13LLMMainWindowE_t>.strings))
        return static_cast<void*>(this);
    return FramelessWindow::qt_metacast(_clname);
}

int LLMMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = FramelessWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 38)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 38;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 38)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 38;
    }
    return _id;
}
QT_WARNING_POP
