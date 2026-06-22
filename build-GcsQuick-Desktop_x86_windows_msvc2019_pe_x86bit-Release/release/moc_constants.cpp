/****************************************************************************
** Meta object code from reading C++ file 'constants.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../GcsQuick/constants.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'constants.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ThemeType_t {
    QByteArrayData data[5];
    char stringdata0[42];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ThemeType_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ThemeType_t qt_meta_stringdata_ThemeType = {
    {
QT_MOC_LITERAL(0, 0, 9), // "ThemeType"
QT_MOC_LITERAL(1, 10, 11), // "QML.Element"
QT_MOC_LITERAL(2, 22, 8), // "DarkMode"
QT_MOC_LITERAL(3, 31, 5), // "Light"
QT_MOC_LITERAL(4, 37, 4) // "Dark"

    },
    "ThemeType\0QML.Element\0DarkMode\0Light\0"
    "Dark"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ThemeType[] = {

 // content:
       8,       // revision
       0,       // classname
       1,   14, // classinfo
       0,    0, // methods
       0,    0, // properties
       1,   16, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // classinfo: key, value
       1,    0,

 // enums: name, alias, flags, count, data
       2,    2, 0x0,    2,   21,

 // enum data: key, value
       3, uint(ThemeType::Light),
       4, uint(ThemeType::Dark),

       0        // eod
};

QT_INIT_METAOBJECT const QMetaObject ThemeType::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_ThemeType.data,
    qt_meta_data_ThemeType,
    nullptr,
    nullptr,
    nullptr
} };

struct qt_meta_stringdata_TreeViewType_t {
    QByteArrayData data[6];
    char stringdata0[60];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TreeViewType_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TreeViewType_t qt_meta_stringdata_TreeViewType = {
    {
QT_MOC_LITERAL(0, 0, 12), // "TreeViewType"
QT_MOC_LITERAL(1, 13, 11), // "QML.Element"
QT_MOC_LITERAL(2, 25, 13), // "SelectionMode"
QT_MOC_LITERAL(3, 39, 4), // "None"
QT_MOC_LITERAL(4, 44, 6), // "Single"
QT_MOC_LITERAL(5, 51, 8) // "Multiple"

    },
    "TreeViewType\0QML.Element\0SelectionMode\0"
    "None\0Single\0Multiple"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TreeViewType[] = {

 // content:
       8,       // revision
       0,       // classname
       1,   14, // classinfo
       0,    0, // methods
       0,    0, // properties
       1,   16, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // classinfo: key, value
       1,    0,

 // enums: name, alias, flags, count, data
       2,    2, 0x0,    3,   21,

 // enum data: key, value
       3, uint(TreeViewType::None),
       4, uint(TreeViewType::Single),
       5, uint(TreeViewType::Multiple),

       0        // eod
};

QT_INIT_METAOBJECT const QMetaObject TreeViewType::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_TreeViewType.data,
    qt_meta_data_TreeViewType,
    nullptr,
    nullptr,
    nullptr
} };

QT_WARNING_POP
QT_END_MOC_NAMESPACE
