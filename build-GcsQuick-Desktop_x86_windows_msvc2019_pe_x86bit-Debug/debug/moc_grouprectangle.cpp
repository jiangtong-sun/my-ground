/****************************************************************************
** Meta object code from reading C++ file 'grouprectangle.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../GcsQuick/grouprectangle.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'grouprectangle.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GroupRectangle_t {
    QByteArrayData data[19];
    char stringdata0[250];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GroupRectangle_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GroupRectangle_t qt_meta_stringdata_GroupRectangle = {
    {
QT_MOC_LITERAL(0, 0, 14), // "GroupRectangle"
QT_MOC_LITERAL(1, 15, 11), // "QML.Element"
QT_MOC_LITERAL(2, 27, 16), // "textColorChanged"
QT_MOC_LITERAL(3, 44, 0), // ""
QT_MOC_LITERAL(4, 45, 15), // "penColorChanged"
QT_MOC_LITERAL(5, 61, 19), // "leftRightOffChanged"
QT_MOC_LITERAL(6, 81, 17), // "leftMarginChanged"
QT_MOC_LITERAL(7, 99, 18), // "rightMarginChanged"
QT_MOC_LITERAL(8, 118, 19), // "bottomMarginChanged"
QT_MOC_LITERAL(9, 138, 16), // "topMarginChanged"
QT_MOC_LITERAL(10, 155, 11), // "textChanged"
QT_MOC_LITERAL(11, 167, 9), // "textColor"
QT_MOC_LITERAL(12, 177, 8), // "penColor"
QT_MOC_LITERAL(13, 186, 12), // "leftRightOff"
QT_MOC_LITERAL(14, 199, 10), // "leftMargin"
QT_MOC_LITERAL(15, 210, 11), // "rightMargin"
QT_MOC_LITERAL(16, 222, 12), // "bottomMargin"
QT_MOC_LITERAL(17, 235, 9), // "topMargin"
QT_MOC_LITERAL(18, 245, 4) // "text"

    },
    "GroupRectangle\0QML.Element\0textColorChanged\0"
    "\0penColorChanged\0leftRightOffChanged\0"
    "leftMarginChanged\0rightMarginChanged\0"
    "bottomMarginChanged\0topMarginChanged\0"
    "textChanged\0textColor\0penColor\0"
    "leftRightOff\0leftMargin\0rightMargin\0"
    "bottomMargin\0topMargin\0text"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GroupRectangle[] = {

 // content:
       8,       // revision
       0,       // classname
       1,   14, // classinfo
       8,   16, // methods
       8,   64, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // classinfo: key, value
       1,    0,

 // signals: name, argc, parameters, tag, flags
       2,    0,   56,    3, 0x06 /* Public */,
       4,    0,   57,    3, 0x06 /* Public */,
       5,    0,   58,    3, 0x06 /* Public */,
       6,    0,   59,    3, 0x06 /* Public */,
       7,    0,   60,    3, 0x06 /* Public */,
       8,    0,   61,    3, 0x06 /* Public */,
       9,    0,   62,    3, 0x06 /* Public */,
      10,    0,   63,    3, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      11, QMetaType::QColor, 0x00495003,
      12, QMetaType::QColor, 0x00495003,
      13, QMetaType::Int, 0x00495003,
      14, QMetaType::Int, 0x00495003,
      15, QMetaType::Int, 0x00495003,
      16, QMetaType::Int, 0x00495003,
      17, QMetaType::Int, 0x00495003,
      18, QMetaType::QString, 0x00495003,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,
       4,
       5,
       6,
       7,

       0        // eod
};

void GroupRectangle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GroupRectangle *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->textColorChanged(); break;
        case 1: _t->penColorChanged(); break;
        case 2: _t->leftRightOffChanged(); break;
        case 3: _t->leftMarginChanged(); break;
        case 4: _t->rightMarginChanged(); break;
        case 5: _t->bottomMarginChanged(); break;
        case 6: _t->topMarginChanged(); break;
        case 7: _t->textChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GroupRectangle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GroupRectangle::textColorChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GroupRectangle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GroupRectangle::penColorChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (GroupRectangle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GroupRectangle::leftRightOffChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (GroupRectangle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GroupRectangle::leftMarginChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (GroupRectangle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GroupRectangle::rightMarginChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (GroupRectangle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GroupRectangle::bottomMarginChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (GroupRectangle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GroupRectangle::topMarginChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (GroupRectangle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GroupRectangle::textChanged)) {
                *result = 7;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<GroupRectangle *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = _t->_textColor; break;
        case 1: *reinterpret_cast< QColor*>(_v) = _t->_penColor; break;
        case 2: *reinterpret_cast< int*>(_v) = _t->_leftRightOff; break;
        case 3: *reinterpret_cast< int*>(_v) = _t->_leftMargin; break;
        case 4: *reinterpret_cast< int*>(_v) = _t->_rightMargin; break;
        case 5: *reinterpret_cast< int*>(_v) = _t->_bottomMargin; break;
        case 6: *reinterpret_cast< int*>(_v) = _t->_topMargin; break;
        case 7: *reinterpret_cast< QString*>(_v) = _t->_text; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<GroupRectangle *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->_textColor != *reinterpret_cast< QColor*>(_v)) {
                _t->_textColor = *reinterpret_cast< QColor*>(_v);
                Q_EMIT _t->textColorChanged();
            }
            break;
        case 1:
            if (_t->_penColor != *reinterpret_cast< QColor*>(_v)) {
                _t->_penColor = *reinterpret_cast< QColor*>(_v);
                Q_EMIT _t->penColorChanged();
            }
            break;
        case 2:
            if (_t->_leftRightOff != *reinterpret_cast< int*>(_v)) {
                _t->_leftRightOff = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->leftRightOffChanged();
            }
            break;
        case 3:
            if (_t->_leftMargin != *reinterpret_cast< int*>(_v)) {
                _t->_leftMargin = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->leftMarginChanged();
            }
            break;
        case 4:
            if (_t->_rightMargin != *reinterpret_cast< int*>(_v)) {
                _t->_rightMargin = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->rightMarginChanged();
            }
            break;
        case 5:
            if (_t->_bottomMargin != *reinterpret_cast< int*>(_v)) {
                _t->_bottomMargin = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->bottomMarginChanged();
            }
            break;
        case 6:
            if (_t->_topMargin != *reinterpret_cast< int*>(_v)) {
                _t->_topMargin = *reinterpret_cast< int*>(_v);
                Q_EMIT _t->topMarginChanged();
            }
            break;
        case 7:
            if (_t->_text != *reinterpret_cast< QString*>(_v)) {
                _t->_text = *reinterpret_cast< QString*>(_v);
                Q_EMIT _t->textChanged();
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject GroupRectangle::staticMetaObject = { {
    QMetaObject::SuperData::link<QQuickPaintedItem::staticMetaObject>(),
    qt_meta_stringdata_GroupRectangle.data,
    qt_meta_data_GroupRectangle,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *GroupRectangle::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GroupRectangle::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GroupRectangle.stringdata0))
        return static_cast<void*>(this);
    return QQuickPaintedItem::qt_metacast(_clname);
}

int GroupRectangle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QQuickPaintedItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 8;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void GroupRectangle::textColorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void GroupRectangle::penColorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void GroupRectangle::leftRightOffChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void GroupRectangle::leftMarginChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void GroupRectangle::rightMarginChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void GroupRectangle::bottomMarginChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void GroupRectangle::topMarginChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void GroupRectangle::textChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
