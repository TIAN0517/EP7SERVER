/****************************************************************************
** Meta object code from reading C++ file 'AIPlayerGenerator.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../AIPlayerGenerator.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AIPlayerGenerator.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9RANOnline2AI17AIPlayerGeneratorE_t {};
} // unnamed namespace

template <> constexpr inline auto RANOnline::AI::AIPlayerGenerator::qt_create_metaobjectdata<qt_meta_tag_ZN9RANOnline2AI17AIPlayerGeneratorE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RANOnline::AI::AIPlayerGenerator",
        "aiPlayerGenerated",
        "",
        "AIPlayerData",
        "player",
        "decisionMade",
        "aiId",
        "AIDecisionResponse",
        "decision",
        "logGenerated",
        "logEntry",
        "errorOccurred",
        "error"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'aiPlayerGenerated'
        QtMocHelpers::SignalData<void(const AIPlayerData &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'decisionMade'
        QtMocHelpers::SignalData<void(const QString &, const AIDecisionResponse &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { 0x80000000 | 7, 8 },
        }}),
        // Signal 'logGenerated'
        QtMocHelpers::SignalData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AIPlayerGenerator, qt_meta_tag_ZN9RANOnline2AI17AIPlayerGeneratorE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RANOnline::AI::AIPlayerGenerator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI17AIPlayerGeneratorE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI17AIPlayerGeneratorE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9RANOnline2AI17AIPlayerGeneratorE_t>.metaTypes,
    nullptr
} };

void RANOnline::AI::AIPlayerGenerator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AIPlayerGenerator *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->aiPlayerGenerated((*reinterpret_cast< std::add_pointer_t<AIPlayerData>>(_a[1]))); break;
        case 1: _t->decisionMade((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<AIDecisionResponse>>(_a[2]))); break;
        case 2: _t->logGenerated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AIPlayerGenerator::*)(const AIPlayerData & )>(_a, &AIPlayerGenerator::aiPlayerGenerated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIPlayerGenerator::*)(const QString & , const AIDecisionResponse & )>(_a, &AIPlayerGenerator::decisionMade, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIPlayerGenerator::*)(const QString & )>(_a, &AIPlayerGenerator::logGenerated, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AIPlayerGenerator::*)(const QString & )>(_a, &AIPlayerGenerator::errorOccurred, 3))
            return;
    }
}

const QMetaObject *RANOnline::AI::AIPlayerGenerator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RANOnline::AI::AIPlayerGenerator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9RANOnline2AI17AIPlayerGeneratorE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RANOnline::AI::AIPlayerGenerator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void RANOnline::AI::AIPlayerGenerator::aiPlayerGenerated(const AIPlayerData & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void RANOnline::AI::AIPlayerGenerator::decisionMade(const QString & _t1, const AIDecisionResponse & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void RANOnline::AI::AIPlayerGenerator::logGenerated(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void RANOnline::AI::AIPlayerGenerator::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
