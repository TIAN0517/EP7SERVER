/****************************************************************************
** Meta object code from reading C++ file 'AIDecisionEngine.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../AIDecisionEngine.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AIDecisionEngine.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9RANOnline2AI16AIDecisionEngineE_t {};
} // unnamed namespace

template <> constexpr inline auto RANOnline::AI::AIDecisionEngine::qt_create_metaobjectdata<qt_meta_tag_ZN9RANOnline2AI16AIDecisionEngineE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RANOnline::AI::AIDecisionEngine",
        "decisionCompleted",
        "",
        "aiId",
        "AIDecisionResponse",
        "response",
        "batchDecisionCompleted",
        "results",
        "testProgressUpdated",
        "completed",
        "total",
        "testCompleted",
        "TestReport",
        "report",
        "logGenerated",
        "logEntry",
        "errorOccurred",
        "error",
        "autoRepairTriggered",
        "repairAction",
        "decisionMade",
        "onLLMResponseReceived",
        "onNetworkError",
        "QNetworkReply::NetworkError",
        "onTestTimerTimeout"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'decisionCompleted'
        QtMocHelpers::SignalData<void(const QString &, const AIDecisionResponse &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 4, 5 },
        }}),
        // Signal 'batchDecisionCompleted'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QJsonArray, 7 },
        }}),
        // Signal 'testProgressUpdated'
        QtMocHelpers::SignalData<void(int, int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 9 }, { QMetaType::Int, 10 },
        }}),
        // Signal 'testCompleted'
        QtMocHelpers::SignalData<void(const TestReport &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 12, 13 },
        }}),
        // Signal 'logGenerated'
        QtMocHelpers::SignalData<void(const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 17 },
        }}),
        // Signal 'autoRepairTriggered'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 19 },
        }}),
        // Signal 'decisionMade'
        QtMocHelpers::SignalData<void(const AIDecisionResponse &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 5 },
        }}),
        // Slot 'onLLMResponseReceived'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onNetworkError'
        QtMocHelpers::SlotData<void(QNetworkReply::NetworkError)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 23, 17 },
        }}),
        // Slot 'onTestTimerTimeout'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AIDecisionEngine, qt_meta_tag_ZN9RANOnline2AI16AIDecisionEngineE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RANOnline::AI::AIDecisionEngine::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI16AIDecisionEngineE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI16AIDecisionEngineE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9RANOnline2AI16AIDecisionEngineE_t>.metaTypes,
    nullptr
} };

void RANOnline::AI::AIDecisionEngine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AIDecisionEngine *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->decisionCompleted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<AIDecisionResponse>>(_a[2]))); break;
        case 1: _t->batchDecisionCompleted((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 2: _t->testProgressUpdated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->testCompleted((*reinterpret_cast< std::add_pointer_t<TestReport>>(_a[1]))); break;
        case 4: _t->logGenerated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->autoRepairTriggered((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->decisionMade((*reinterpret_cast< std::add_pointer_t<AIDecisionResponse>>(_a[1]))); break;
        case 8: _t->onLLMResponseReceived(); break;
        case 9: _t->onNetworkError((*reinterpret_cast< std::add_pointer_t<QNetworkReply::NetworkError>>(_a[1]))); break;
        case 10: _t->onTestTimerTimeout(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QNetworkReply::NetworkError >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AIDecisionEngine::*)(const QString & , const AIDecisionResponse & )>(_a, &AIDecisionEngine::decisionCompleted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIDecisionEngine::*)(const QJsonArray & )>(_a, &AIDecisionEngine::batchDecisionCompleted, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIDecisionEngine::*)(int , int )>(_a, &AIDecisionEngine::testProgressUpdated, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIDecisionEngine::*)(const TestReport & )>(_a, &AIDecisionEngine::testCompleted, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIDecisionEngine::*)(const QString & )>(_a, &AIDecisionEngine::logGenerated, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIDecisionEngine::*)(const QString & )>(_a, &AIDecisionEngine::errorOccurred, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIDecisionEngine::*)(const QString & , const QString & )>(_a, &AIDecisionEngine::autoRepairTriggered, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIDecisionEngine::*)(const AIDecisionResponse & )>(_a, &AIDecisionEngine::decisionMade, 7))
            return;
    }
}

const QMetaObject *RANOnline::AI::AIDecisionEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RANOnline::AI::AIDecisionEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI16AIDecisionEngineE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RANOnline::AI::AIDecisionEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void RANOnline::AI::AIDecisionEngine::decisionCompleted(const QString & _t1, const AIDecisionResponse & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void RANOnline::AI::AIDecisionEngine::batchDecisionCompleted(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void RANOnline::AI::AIDecisionEngine::testProgressUpdated(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void RANOnline::AI::AIDecisionEngine::testCompleted(const TestReport & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void RANOnline::AI::AIDecisionEngine::logGenerated(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void RANOnline::AI::AIDecisionEngine::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void RANOnline::AI::AIDecisionEngine::autoRepairTriggered(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void RANOnline::AI::AIDecisionEngine::decisionMade(const AIDecisionResponse & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}
QT_WARNING_POP
