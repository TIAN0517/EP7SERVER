/****************************************************************************
** Meta object code from reading C++ file 'OllamaLLMManager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../OllamaLLMManager.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OllamaLLMManager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN14LLMRequestTaskE_t {};
} // unnamed namespace

template <> constexpr inline auto LLMRequestTask::qt_create_metaobjectdata<qt_meta_tag_ZN14LLMRequestTaskE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "LLMRequestTask",
        "requestCompleted",
        "",
        "LLMResponse",
        "response",
        "requestProgress",
        "requestId",
        "progress",
        "streamUpdate",
        "content"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'requestCompleted'
        QtMocHelpers::SignalData<void(const LLMResponse &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'requestProgress'
        QtMocHelpers::SignalData<void(const QString &, int)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::Int, 7 },
        }}),
        // Signal 'streamUpdate'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::QString, 9 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<LLMRequestTask, qt_meta_tag_ZN14LLMRequestTaskE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject LLMRequestTask::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14LLMRequestTaskE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14LLMRequestTaskE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14LLMRequestTaskE_t>.metaTypes,
    nullptr
} };

void LLMRequestTask::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LLMRequestTask *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->requestCompleted((*reinterpret_cast< std::add_pointer_t<LLMResponse>>(_a[1]))); break;
        case 1: _t->requestProgress((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 2: _t->streamUpdate((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (LLMRequestTask::*)(const LLMResponse & )>(_a, &LLMRequestTask::requestCompleted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (LLMRequestTask::*)(const QString & , int )>(_a, &LLMRequestTask::requestProgress, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (LLMRequestTask::*)(const QString & , const QString & )>(_a, &LLMRequestTask::streamUpdate, 2))
            return;
    }
}

const QMetaObject *LLMRequestTask::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LLMRequestTask::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14LLMRequestTaskE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QRunnable"))
        return static_cast< QRunnable*>(this);
    return QObject::qt_metacast(_clname);
}

int LLMRequestTask::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void LLMRequestTask::requestCompleted(const LLMResponse & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void LLMRequestTask::requestProgress(const QString & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void LLMRequestTask::streamUpdate(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN12LoadBalancerE_t {};
} // unnamed namespace

template <> constexpr inline auto LoadBalancer::qt_create_metaobjectdata<qt_meta_tag_ZN12LoadBalancerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "LoadBalancer"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<LoadBalancer, qt_meta_tag_ZN12LoadBalancerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject LoadBalancer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12LoadBalancerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12LoadBalancerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12LoadBalancerE_t>.metaTypes,
    nullptr
} };

void LoadBalancer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LoadBalancer *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *LoadBalancer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LoadBalancer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12LoadBalancerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LoadBalancer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN16OllamaLLMManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto OllamaLLMManager::qt_create_metaobjectdata<qt_meta_tag_ZN16OllamaLLMManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "OllamaLLMManager",
        "requestCompleted",
        "",
        "LLMResponse",
        "response",
        "requestFailed",
        "requestId",
        "error",
        "streamUpdate",
        "content",
        "requestProgress",
        "progress",
        "modelsRefreshed",
        "QList<LLMModelInfo>",
        "models",
        "serverStatusChanged",
        "serverName",
        "isOnline",
        "statisticsUpdated",
        "onRequestCompleted",
        "onRequestProgress",
        "onStreamUpdate",
        "checkServerHealth",
        "cleanupCompletedRequests",
        "updateStatistics"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'requestCompleted'
        QtMocHelpers::SignalData<void(const LLMResponse &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'requestFailed'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::QString, 7 },
        }}),
        // Signal 'streamUpdate'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::QString, 9 },
        }}),
        // Signal 'requestProgress'
        QtMocHelpers::SignalData<void(const QString &, int)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::Int, 11 },
        }}),
        // Signal 'modelsRefreshed'
        QtMocHelpers::SignalData<void(const QList<LLMModelInfo> &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
        // Signal 'serverStatusChanged'
        QtMocHelpers::SignalData<void(const QString &, bool)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 16 }, { QMetaType::Bool, 17 },
        }}),
        // Signal 'statisticsUpdated'
        QtMocHelpers::SignalData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onRequestCompleted'
        QtMocHelpers::SlotData<void(const LLMResponse &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Slot 'onRequestProgress'
        QtMocHelpers::SlotData<void(const QString &, int)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::Int, 11 },
        }}),
        // Slot 'onStreamUpdate'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::QString, 9 },
        }}),
        // Slot 'checkServerHealth'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'cleanupCompletedRequests'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateStatistics'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<OllamaLLMManager, qt_meta_tag_ZN16OllamaLLMManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject OllamaLLMManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16OllamaLLMManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16OllamaLLMManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16OllamaLLMManagerE_t>.metaTypes,
    nullptr
} };

void OllamaLLMManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<OllamaLLMManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->requestCompleted((*reinterpret_cast< std::add_pointer_t<LLMResponse>>(_a[1]))); break;
        case 1: _t->requestFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->streamUpdate((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->requestProgress((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->modelsRefreshed((*reinterpret_cast< std::add_pointer_t<QList<LLMModelInfo>>>(_a[1]))); break;
        case 5: _t->serverStatusChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 6: _t->statisticsUpdated(); break;
        case 7: _t->onRequestCompleted((*reinterpret_cast< std::add_pointer_t<LLMResponse>>(_a[1]))); break;
        case 8: _t->onRequestProgress((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 9: _t->onStreamUpdate((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 10: _t->checkServerHealth(); break;
        case 11: _t->cleanupCompletedRequests(); break;
        case 12: _t->updateStatistics(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (OllamaLLMManager::*)(const LLMResponse & )>(_a, &OllamaLLMManager::requestCompleted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (OllamaLLMManager::*)(const QString & , const QString & )>(_a, &OllamaLLMManager::requestFailed, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (OllamaLLMManager::*)(const QString & , const QString & )>(_a, &OllamaLLMManager::streamUpdate, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (OllamaLLMManager::*)(const QString & , int )>(_a, &OllamaLLMManager::requestProgress, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (OllamaLLMManager::*)(const QList<LLMModelInfo> & )>(_a, &OllamaLLMManager::modelsRefreshed, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (OllamaLLMManager::*)(const QString & , bool )>(_a, &OllamaLLMManager::serverStatusChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (OllamaLLMManager::*)()>(_a, &OllamaLLMManager::statisticsUpdated, 6))
            return;
    }
}

const QMetaObject *OllamaLLMManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OllamaLLMManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16OllamaLLMManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int OllamaLLMManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void OllamaLLMManager::requestCompleted(const LLMResponse & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void OllamaLLMManager::requestFailed(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void OllamaLLMManager::streamUpdate(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void OllamaLLMManager::requestProgress(const QString & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void OllamaLLMManager::modelsRefreshed(const QList<LLMModelInfo> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void OllamaLLMManager::serverStatusChanged(const QString & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}

// SIGNAL 6
void OllamaLLMManager::statisticsUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
namespace {
struct qt_meta_tag_ZN21PromptTemplateManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto PromptTemplateManager::qt_create_metaobjectdata<qt_meta_tag_ZN21PromptTemplateManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PromptTemplateManager"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PromptTemplateManager, qt_meta_tag_ZN21PromptTemplateManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PromptTemplateManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21PromptTemplateManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21PromptTemplateManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN21PromptTemplateManagerE_t>.metaTypes,
    nullptr
} };

void PromptTemplateManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PromptTemplateManager *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *PromptTemplateManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PromptTemplateManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21PromptTemplateManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PromptTemplateManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
