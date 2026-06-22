/****************************************************************************
** Meta object code from reading C++ file 'textstyle.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../GcsQuick/textstyle.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'textstyle.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TextStyle_t {
    QByteArrayData data[15];
    char stringdata0[155];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TextStyle_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TextStyle_t qt_meta_stringdata_TextStyle = {
    {
QT_MOC_LITERAL(0, 0, 9), // "TextStyle"
QT_MOC_LITERAL(1, 10, 11), // "QML.Element"
QT_MOC_LITERAL(2, 22, 13), // "QML.Singleton"
QT_MOC_LITERAL(3, 36, 4), // "true"
QT_MOC_LITERAL(4, 41, 15), // "fontSizeChanged"
QT_MOC_LITERAL(5, 57, 0), // ""
QT_MOC_LITERAL(6, 58, 14), // "CaptionChanged"
QT_MOC_LITERAL(7, 73, 11), // "BodyChanged"
QT_MOC_LITERAL(8, 85, 16), // "BodySmallChanged"
QT_MOC_LITERAL(9, 102, 13), // "familyChanged"
QT_MOC_LITERAL(10, 116, 8), // "fontSize"
QT_MOC_LITERAL(11, 125, 7), // "Caption"
QT_MOC_LITERAL(12, 133, 4), // "Body"
QT_MOC_LITERAL(13, 138, 9), // "BodySmall"
QT_MOC_LITERAL(14, 148, 6) // "family"

    },
    "TextStyle\0QML.Element\0QML.Singleton\0"
    "true\0fontSizeChanged\0\0CaptionChanged\0"
    "BodyChanged\0BodySmallChanged\0familyChanged\0"
    "fontSize\0Caption\0Body\0BodySmall\0family"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TextStyle[] = {

 // content:
       8,       // revision
       0,       // classname
       2,   14, // classinfo
       5,   18, // methods
       5,   48, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // classinfo: key, value
       1,    0,
       2,    3,

 // signals: name, argc, parameters, tag, flags
       4,    0,   43,    5, 0x06 /* Public */,
       6,    0,   44,    5, 0x06 /* Public */,
       7,    0,   45,    5, 0x06 /* Public */,
       8,    0,   46,    5, 0x06 /* Public */,
       9,    0,   47,    5, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      10, QMetaType::Int, 0x00495003,
      11, QMetaType::Int, 0x00495003,
      12, QMetaType::Int, 0x00495003,
      13, QMetaType::Int, 0x00495003,
      14, QMetaType::QString, 0x00495003,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,
       4,

       0        // eod
};

void TextStyle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TextStyle *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->fontSizeChanged(); break;
        case 1: _t->CaptionChanged(); break;
        case 2: _t->BodyChanged(); break;
        case 3: _t->BodySmallChanged(); break;
        case 4: _t->familyChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TextStyle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextStyle::fontSizeChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TextStyle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextStyle::CaptionChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TextStyle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextStyle::BodyChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TextStyle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextStyle::BodySmallChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (TextStyle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextStyle::familyChanged)) {
                *result = 4;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<TextStyle *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->_fontSize; break;
        case 1: *reinterpret_cast< int*>(_v) = _t->_Caption; break;
        case 2: *reinterpret_cast< int*>(_v) = _t->_Body; break;
        case 3: *reinterpret_cast< int*>(_v) = _t->_BodySmall; break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->_family; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<TextStyle *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->_fontSize != *reinterpret_cast< int*>(_v)) {
                _t->_fontSize = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->fontSizeChanged();
            }
            break;
        case 1:
            if (_t->_Caption != *reinterpret_cast< int*>(_v)) {
                _t->_Caption = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->CaptionChanged();
            }
            break;
        case 2:
            if (_t->_Body != *reinterpret_cast< int*>(_v)) {
                _t->_Body = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->BodyChanged();
            }
            break;
        case 3:
            if (_t->_BodySmall != *reinterpret_cast< int*>(_v)) {
                _t->_BodySmall = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->BodySmallChanged();
            }
            break;
        case 4:
            if (_t->_family != *reinterpret_cast< QString*>(_v)) {
                _t->_family = *reinterpret_cast< QString*>(_v);
                Q_EMIT _t->familyChanged();
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject TextStyle::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_TextStyle.data,
    qt_meta_data_TextStyle,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TextStyle::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TextStyle::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TextStyle.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TextStyle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void TextStyle::fontSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void TextStyle::CaptionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void TextStyle::BodyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void TextStyle::BodySmallChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void TextStyle::familyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
