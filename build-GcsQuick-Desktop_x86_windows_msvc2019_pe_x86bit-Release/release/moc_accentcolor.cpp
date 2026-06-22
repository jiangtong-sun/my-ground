/****************************************************************************
** Meta object code from reading C++ file 'accentcolor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../GcsQuick/accentcolor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'accentcolor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AccentColor_t {
    QByteArrayData data[13];
    char stringdata0[126];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AccentColor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AccentColor_t qt_meta_stringdata_AccentColor = {
    {
QT_MOC_LITERAL(0, 0, 11), // "AccentColor"
QT_MOC_LITERAL(1, 12, 11), // "QML.Element"
QT_MOC_LITERAL(2, 24, 14), // "darkestChanged"
QT_MOC_LITERAL(3, 39, 0), // ""
QT_MOC_LITERAL(4, 40, 13), // "darkerChanged"
QT_MOC_LITERAL(5, 54, 11), // "darkChanged"
QT_MOC_LITERAL(6, 66, 13), // "normalChanged"
QT_MOC_LITERAL(7, 80, 12), // "lightChanged"
QT_MOC_LITERAL(8, 93, 7), // "darkest"
QT_MOC_LITERAL(9, 101, 6), // "darker"
QT_MOC_LITERAL(10, 108, 4), // "dark"
QT_MOC_LITERAL(11, 113, 6), // "normal"
QT_MOC_LITERAL(12, 120, 5) // "light"

    },
    "AccentColor\0QML.Element\0darkestChanged\0"
    "\0darkerChanged\0darkChanged\0normalChanged\0"
    "lightChanged\0darkest\0darker\0dark\0"
    "normal\0light"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AccentColor[] = {

 // content:
       8,       // revision
       0,       // classname
       1,   14, // classinfo
       5,   16, // methods
       5,   46, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // classinfo: key, value
       1,    0,

 // signals: name, argc, parameters, tag, flags
       2,    0,   41,    3, 0x06 /* Public */,
       4,    0,   42,    3, 0x06 /* Public */,
       5,    0,   43,    3, 0x06 /* Public */,
       6,    0,   44,    3, 0x06 /* Public */,
       7,    0,   45,    3, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       8, QMetaType::QColor, 0x00495003,
       9, QMetaType::QColor, 0x00495003,
      10, QMetaType::QColor, 0x00495003,
      11, QMetaType::QColor, 0x00495003,
      12, QMetaType::QColor, 0x00495003,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,
       4,

       0        // eod
};

void AccentColor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AccentColor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->darkestChanged(); break;
        case 1: _t->darkerChanged(); break;
        case 2: _t->darkChanged(); break;
        case 3: _t->normalChanged(); break;
        case 4: _t->lightChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AccentColor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AccentColor::darkestChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AccentColor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AccentColor::darkerChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (AccentColor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AccentColor::darkChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (AccentColor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AccentColor::normalChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (AccentColor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AccentColor::lightChanged)) {
                *result = 4;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<AccentColor *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = _t->_darkest; break;
        case 1: *reinterpret_cast< QColor*>(_v) = _t->_darker; break;
        case 2: *reinterpret_cast< QColor*>(_v) = _t->_dark; break;
        case 3: *reinterpret_cast< QColor*>(_v) = _t->_normal; break;
        case 4: *reinterpret_cast< QColor*>(_v) = _t->_light; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<AccentColor *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->_darkest != *reinterpret_cast< QColor*>(_v)) {
                _t->_darkest = *reinterpret_cast< QColor*>(_v);
                Q_EMIT _t->darkestChanged();
            }
            break;
        case 1:
            if (_t->_darker != *reinterpret_cast< QColor*>(_v)) {
                _t->_darker = *reinterpret_cast< QColor*>(_v);
                Q_EMIT _t->darkerChanged();
            }
            break;
        case 2:
            if (_t->_dark != *reinterpret_cast< QColor*>(_v)) {
                _t->_dark = *reinterpret_cast< QColor*>(_v);
                Q_EMIT _t->darkChanged();
            }
            break;
        case 3:
            if (_t->_normal != *reinterpret_cast< QColor*>(_v)) {
                _t->_normal = *reinterpret_cast< QColor*>(_v);
                Q_EMIT _t->normalChanged();
            }
            break;
        case 4:
            if (_t->_light != *reinterpret_cast< QColor*>(_v)) {
                _t->_light = *reinterpret_cast< QColor*>(_v);
                Q_EMIT _t->lightChanged();
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject AccentColor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_AccentColor.data,
    qt_meta_data_AccentColor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AccentColor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AccentColor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AccentColor.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AccentColor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void AccentColor::darkestChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AccentColor::darkerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AccentColor::darkChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void AccentColor::normalChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void AccentColor::lightChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
