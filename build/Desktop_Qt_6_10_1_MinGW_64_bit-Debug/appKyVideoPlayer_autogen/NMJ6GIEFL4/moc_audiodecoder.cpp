/****************************************************************************
** Meta object code from reading C++ file 'audiodecoder.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../Decoder/audiodecoder.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audiodecoder.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12AudioDecoderE_t {};
} // unnamed namespace

template <> constexpr inline auto AudioDecoder::qt_create_metaobjectdata<qt_meta_tag_ZN12AudioDecoderE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AudioDecoder",
        "pushPacket",
        "",
        "AVPacket*",
        "pkt",
        "onAudioCodecParReady",
        "AVCodecParameters*",
        "codecpar",
        "AVRational",
        "timeBase",
        "processQueuedPackets",
        "stopDecode",
        "getContext",
        "PlayContext*",
        "setContext",
        "context"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'pushPacket'
        QtMocHelpers::SlotData<void(AVPacket *)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Slot 'onAudioCodecParReady'
        QtMocHelpers::SlotData<void(AVCodecParameters *, AVRational)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 }, { 0x80000000 | 8, 9 },
        }}),
        // Slot 'processQueuedPackets'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'stopDecode'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'getContext'
        QtMocHelpers::SlotData<PlayContext *()>(12, 2, QMC::AccessPublic, 0x80000000 | 13),
        // Slot 'setContext'
        QtMocHelpers::SlotData<void(PlayContext *)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 13, 15 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AudioDecoder, qt_meta_tag_ZN12AudioDecoderE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AudioDecoder::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AudioDecoderE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AudioDecoderE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12AudioDecoderE_t>.metaTypes,
    nullptr
} };

void AudioDecoder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AudioDecoder *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->pushPacket((*reinterpret_cast<std::add_pointer_t<AVPacket*>>(_a[1]))); break;
        case 1: _t->onAudioCodecParReady((*reinterpret_cast<std::add_pointer_t<AVCodecParameters*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<AVRational>>(_a[2]))); break;
        case 2: _t->processQueuedPackets(); break;
        case 3: _t->stopDecode(); break;
        case 4: { PlayContext* _r = _t->getContext();
            if (_a[0]) *reinterpret_cast<PlayContext**>(_a[0]) = std::move(_r); }  break;
        case 5: _t->setContext((*reinterpret_cast<std::add_pointer_t<PlayContext*>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *AudioDecoder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AudioDecoder::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AudioDecoderE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AudioDecoder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
