/****************************************************************************
** Meta object code from reading C++ file 'AIManagementWidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../AIManagementWidget.h"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AIManagementWidget.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9RANOnline2AI13AIStatsWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto RANOnline::AI::AIStatsWidget::qt_create_metaobjectdata<qt_meta_tag_ZN9RANOnline2AI13AIStatsWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RANOnline::AI::AIStatsWidget"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AIStatsWidget, qt_meta_tag_ZN9RANOnline2AI13AIStatsWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RANOnline::AI::AIStatsWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI13AIStatsWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI13AIStatsWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9RANOnline2AI13AIStatsWidgetE_t>.metaTypes,
    nullptr
} };

void RANOnline::AI::AIStatsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AIStatsWidget *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *RANOnline::AI::AIStatsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RANOnline::AI::AIStatsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI13AIStatsWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RANOnline::AI::AIStatsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN9RANOnline2AI11AILogWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto RANOnline::AI::AILogWidget::qt_create_metaobjectdata<qt_meta_tag_ZN9RANOnline2AI11AILogWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RANOnline::AI::AILogWidget",
        "onClearButtonClicked",
        "",
        "onExportButtonClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onClearButtonClicked'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExportButtonClicked'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AILogWidget, qt_meta_tag_ZN9RANOnline2AI11AILogWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RANOnline::AI::AILogWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI11AILogWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI11AILogWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9RANOnline2AI11AILogWidgetE_t>.metaTypes,
    nullptr
} };

void RANOnline::AI::AILogWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AILogWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onClearButtonClicked(); break;
        case 1: _t->onExportButtonClicked(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *RANOnline::AI::AILogWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RANOnline::AI::AILogWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI11AILogWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RANOnline::AI::AILogWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}
namespace {
struct qt_meta_tag_ZN9RANOnline2AI19AIPlayerTableWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto RANOnline::AI::AIPlayerTableWidget::qt_create_metaobjectdata<qt_meta_tag_ZN9RANOnline2AI19AIPlayerTableWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RANOnline::AI::AIPlayerTableWidget",
        "aiPlayerSelected",
        "",
        "aiId",
        "aiPlayerRemoved",
        "onCellClicked",
        "row",
        "column",
        "onRemoveButtonClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'aiPlayerSelected'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'aiPlayerRemoved'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Slot 'onCellClicked'
        QtMocHelpers::SlotData<void(int, int)>(5, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 6 }, { QMetaType::Int, 7 },
        }}),
        // Slot 'onRemoveButtonClicked'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AIPlayerTableWidget, qt_meta_tag_ZN9RANOnline2AI19AIPlayerTableWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RANOnline::AI::AIPlayerTableWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QTableWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI19AIPlayerTableWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI19AIPlayerTableWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9RANOnline2AI19AIPlayerTableWidgetE_t>.metaTypes,
    nullptr
} };

void RANOnline::AI::AIPlayerTableWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AIPlayerTableWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->aiPlayerSelected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->aiPlayerRemoved((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->onCellClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->onRemoveButtonClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AIPlayerTableWidget::*)(const QString & )>(_a, &AIPlayerTableWidget::aiPlayerSelected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIPlayerTableWidget::*)(const QString & )>(_a, &AIPlayerTableWidget::aiPlayerRemoved, 1))
            return;
    }
}

const QMetaObject *RANOnline::AI::AIPlayerTableWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RANOnline::AI::AIPlayerTableWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI19AIPlayerTableWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QTableWidget::qt_metacast(_clname);
}

int RANOnline::AI::AIPlayerTableWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void RANOnline::AI::AIPlayerTableWidget::aiPlayerSelected(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void RANOnline::AI::AIPlayerTableWidget::aiPlayerRemoved(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN9RANOnline2AI18AIManagementWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto RANOnline::AI::AIManagementWidget::qt_create_metaobjectdata<qt_meta_tag_ZN9RANOnline2AI18AIManagementWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RANOnline::AI::AIManagementWidget",
        "aiPlayersGenerated",
        "",
        "players",
        "batchDecisionCompleted",
        "results",
        "testCompleted",
        "TestReport",
        "report",
        "logGenerated",
        "logEntry",
        "generateAIRequested",
        "count",
        "testAIRequested",
        "modelType",
        "onGenerateAIClicked",
        "onClearAIClicked",
        "onAIPlayerGenerated",
        "AIPlayerData",
        "player",
        "onBatchDecisionClicked",
        "onTeamDecisionClicked",
        "onAutoTestClicked",
        "onDecisionCompleted",
        "aiId",
        "AIDecisionResponse",
        "response",
        "onBatchDecisionCompleted",
        "onTestProgressUpdated",
        "completed",
        "total",
        "onTestCompleted",
        "onOllamaSettingsChanged",
        "onAutoRepairToggled",
        "enabled",
        "onModelSelectionChanged",
        "onLogGenerated",
        "onExportAllLogsClicked",
        "onConnectGameServerClicked",
        "onConnectionStatusChanged",
        "connected"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'aiPlayersGenerated'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QJsonArray, 3 },
        }}),
        // Signal 'batchDecisionCompleted'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QJsonArray, 5 },
        }}),
        // Signal 'testCompleted'
        QtMocHelpers::SignalData<void(const TestReport &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Signal 'logGenerated'
        QtMocHelpers::SignalData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Signal 'generateAIRequested'
        QtMocHelpers::SignalData<void(int)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 12 },
        }}),
        // Signal 'testAIRequested'
        QtMocHelpers::SignalData<void(int, const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 12 }, { QMetaType::QString, 14 },
        }}),
        // Slot 'onGenerateAIClicked'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClearAIClicked'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAIPlayerGenerated'
        QtMocHelpers::SlotData<void(const AIPlayerData &)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 18, 19 },
        }}),
        // Slot 'onBatchDecisionClicked'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTeamDecisionClicked'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAutoTestClicked'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDecisionCompleted'
        QtMocHelpers::SlotData<void(const QString &, const AIDecisionResponse &)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 }, { 0x80000000 | 25, 26 },
        }}),
        // Slot 'onBatchDecisionCompleted'
        QtMocHelpers::SlotData<void(const QJsonArray &)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QJsonArray, 5 },
        }}),
        // Slot 'onTestProgressUpdated'
        QtMocHelpers::SlotData<void(int, int)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 29 }, { QMetaType::Int, 30 },
        }}),
        // Slot 'onTestCompleted'
        QtMocHelpers::SlotData<void(const TestReport &)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Slot 'onOllamaSettingsChanged'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAutoRepairToggled'
        QtMocHelpers::SlotData<void(bool)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 34 },
        }}),
        // Slot 'onModelSelectionChanged'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLogGenerated'
        QtMocHelpers::SlotData<void(const QString &)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Slot 'onExportAllLogsClicked'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectGameServerClicked'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectionStatusChanged'
        QtMocHelpers::SlotData<void(bool)>(39, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 40 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AIManagementWidget, qt_meta_tag_ZN9RANOnline2AI18AIManagementWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RANOnline::AI::AIManagementWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI18AIManagementWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI18AIManagementWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9RANOnline2AI18AIManagementWidgetE_t>.metaTypes,
    nullptr
} };

void RANOnline::AI::AIManagementWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AIManagementWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->aiPlayersGenerated((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 1: _t->batchDecisionCompleted((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 2: _t->testCompleted((*reinterpret_cast< std::add_pointer_t<TestReport>>(_a[1]))); break;
        case 3: _t->logGenerated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->generateAIRequested((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->testAIRequested((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 6: _t->onGenerateAIClicked(); break;
        case 7: _t->onClearAIClicked(); break;
        case 8: _t->onAIPlayerGenerated((*reinterpret_cast< std::add_pointer_t<AIPlayerData>>(_a[1]))); break;
        case 9: _t->onBatchDecisionClicked(); break;
        case 10: _t->onTeamDecisionClicked(); break;
        case 11: _t->onAutoTestClicked(); break;
        case 12: _t->onDecisionCompleted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<AIDecisionResponse>>(_a[2]))); break;
        case 13: _t->onBatchDecisionCompleted((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 14: _t->onTestProgressUpdated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 15: _t->onTestCompleted((*reinterpret_cast< std::add_pointer_t<TestReport>>(_a[1]))); break;
        case 16: _t->onOllamaSettingsChanged(); break;
        case 17: _t->onAutoRepairToggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 18: _t->onModelSelectionChanged(); break;
        case 19: _t->onLogGenerated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 20: _t->onExportAllLogsClicked(); break;
        case 21: _t->onConnectGameServerClicked(); break;
        case 22: _t->onConnectionStatusChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AIManagementWidget::*)(const QJsonArray & )>(_a, &AIManagementWidget::aiPlayersGenerated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIManagementWidget::*)(const QJsonArray & )>(_a, &AIManagementWidget::batchDecisionCompleted, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIManagementWidget::*)(const TestReport & )>(_a, &AIManagementWidget::testCompleted, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIManagementWidget::*)(const QString & )>(_a, &AIManagementWidget::logGenerated, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIManagementWidget::*)(int )>(_a, &AIManagementWidget::generateAIRequested, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIManagementWidget::*)(int , const QString & )>(_a, &AIManagementWidget::testAIRequested, 5))
            return;
    }
}

const QMetaObject *RANOnline::AI::AIManagementWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RANOnline::AI::AIManagementWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI18AIManagementWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RANOnline::AI::AIManagementWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void RANOnline::AI::AIManagementWidget::aiPlayersGenerated(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void RANOnline::AI::AIManagementWidget::batchDecisionCompleted(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void RANOnline::AI::AIManagementWidget::testCompleted(const TestReport & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void RANOnline::AI::AIManagementWidget::logGenerated(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void RANOnline::AI::AIManagementWidget::generateAIRequested(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void RANOnline::AI::AIManagementWidget::testAIRequested(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}
QT_WARNING_POP
