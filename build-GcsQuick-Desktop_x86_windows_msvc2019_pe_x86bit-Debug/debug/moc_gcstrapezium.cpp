/****************************************************************************
** Meta object code from reading C++ file 'gcstrapezium.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../GcsQuick/gcstrapezium.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gcstrapezium.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GcsTrapezium_t {
    QByteArrayData data[9];
    char stringdata0[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GcsTrapezium_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GcsTrapezium_t qt_meta_stringdata_GcsTrapezium = {
    {
QT_MOC_LITERAL(0, 0, 12), // "GcsTrapezium"
QT_MOC_LITERAL(1, 13, 11), // "QML.Element"
QT_MOC_LITERAL(2, 25, 12), // "colorChanged"
QT_MOC_LITERAL(3, 38, 0), // ""
QT_MOC_LITERAL(4, 39, 15), // "penColorChanged"
QT_MOC_LITERAL(5, 55, 19), // "specailColorChanged"
QT_MOC_LITERAL(6, 75, 5), // "color"
QT_MOC_LITERAL(7, 81, 8), // "penColor"
QT_MOC_LITERAL(8, 90, 12) // "specailColor"

    },
    "GcsTrapezium\0QML.Element\0colorChanged\0"
    "\0penColorChanged\0specailColorChanged\0"
    "color\0penColor\0specailColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GcsTrapezium[] = {

 // content:
       8,       // revision
       0,       // classname
       1,   14, // classinfo
       3,   16, // methods
       3,   34, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // classinfo: key, value
       1,    0,

 // signals: name, argc, parameters, tag, flags
       2,    0,   31,    3, 0x06 /* Public */,
       4,    0,   32,    3, 0x06 /* Public */,
       5,    0,   33,    3, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       6, QMetaType::QColor, 0x00495003,
       7, QMetaType::QColor, 0x00495003,
       8, QMetaType::QColor, 0x00495003,

 // properties: notify_signal_id
       0,
       1,
       2,

       0        // eod
};

void GcsTrapezium::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GcsTrapezium *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->colorChanged(); break;
        case 1: _t->penColorChanged(); break;
        case 2: _t->specailColorChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GcsTrapezium::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GcsTrapezium::colorChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GcsTrapezium::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GcsTrapezium::penColorChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (GcsTrapezium::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GcsTrapezium::specailColorChanged)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<GcsTrapezium *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = _t->_color; break;
        case 1: *reinterpret_cast< QColor*>(_v) = _t->_penColor; break;
        case 2: *reinterpret_cast< QColor*>(_v) = _t->_specailColor; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<GcsTrapezium *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->_color != *reinterpret_cast< QColor*>(_v)) {
                _t->_color = *reinterpret_cast< QColor*>(_v);
                Q_EMIT _t->colorChanged();
            }
            break;
        case 1:
            if (_t->_penColor != *reinterpret_cast< QColor*>(_v)) {
                _t->_penColor = *reinterpret_cast< QColor*>(_v);
                Q_EMIT _t->penColorChanged();
            }
            break;
        case 2:
            if (_t->_specailColor != *reinterpret_cast< QColor*>(_v)) {
                _t->_specailColor = *reinterpret_cast< QColor*>(_v);
                Q_EMIT _t->specailColorChanged();
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject GcsTrapezium::staticMetaObject = { {
    QMetaObject::SuperData::link<QQuickPaintedItem::staticMetaObject>(),
    qt_meta_stringdata_GcsTrapezium.data,
    qt_meta_data_GcsTrapezium,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *GcsTrapezium::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GcsTrapezium::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GcsTrapezium.stringdata0))
        return static_cast<void*>(this);
    return QQuickPaintedItem::qt_metacast(_clname);
}

int GcsTrapezium::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QQuickPaintedItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void GcsTrapezium::colorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void GcsTrapezium::penColorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void GcsTrapezium::specailColorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
