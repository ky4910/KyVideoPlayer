/****************************************************************************
** Meta object code from reading C++ file 'demuxworker.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../demuxworker.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'demuxworker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.1. It"
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
struct qt_meta_tag_ZN11DemuxWorkerE_t {};
} // unnamed namespace

template <> constexpr inline auto DemuxWorker::qt_create_metaobjectdata<qt_meta_tag_ZN11DemuxWorkerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DemuxWorker",
        "finished",
        "",
        "audioPacketReady",
        "AVPacket*",
        "pkt",
        "videoPacketReady",
        "audioCodecParReady",
        "AVCodecParameters*",
        "codecpar",
        "videoCodecParReady"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'finished'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'audioPacketReady'
        QtMocHelpers::SignalData<void(AVPacket *)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 5 },
        }}),
        // Signal 'videoPacketReady'
        QtMocHelpers::SignalData<void(AVPacket *)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 5 },
        }}),
        // Signal 'audioCodecParReady'
        QtMocHelpers::SignalData<void(AVCodecParameters *)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Signal 'videoCodecParReady'
        QtMocHelpers::SignalData<void(AVCodecParameters *)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DemuxWorker, qt_meta_tag_ZN11DemuxWorkerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DemuxWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DemuxWorkerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DemuxWorkerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11DemuxWorkerE_t>.metaTypes,
    nullptr
} };

void DemuxWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DemuxWorker *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->finished(); break;
        case 1: _t->audioPacketReady((*reinterpret_cast<std::add_pointer_t<AVPacket*>>(_a[1]))); break;
        case 2: _t->videoPacketReady((*reinterpret_cast<std::add_pointer_t<AVPacket*>>(_a[1]))); break;
        case 3: _t->audioCodecParReady((*reinterpret_cast<std::add_pointer_t<AVCodecParameters*>>(_a[1]))); break;
        case 4: _t->videoCodecParReady((*reinterpret_cast<std::add_pointer_t<AVCodecParameters*>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DemuxWorker::*)()>(_a, &DemuxWorker::finished, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DemuxWorker::*)(AVPacket * )>(_a, &DemuxWorker::audioPacketReady, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DemuxWorker::*)(AVPacket * )>(_a, &DemuxWorker::videoPacketReady, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DemuxWorker::*)(AVCodecParameters * )>(_a, &DemuxWorker::audioCodecParReady, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DemuxWorker::*)(AVCodecParameters * )>(_a, &DemuxWorker::videoCodecParReady, 4))
            return;
    }
}

const QMetaObject *DemuxWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DemuxWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DemuxWorkerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DemuxWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void DemuxWorker::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DemuxWorker::audioPacketReady(AVPacket * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void DemuxWorker::videoPacketReady(AVPacket * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void DemuxWorker::audioCodecParReady(AVCodecParameters * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void DemuxWorker::videoCodecParReady(AVCodecParameters * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
