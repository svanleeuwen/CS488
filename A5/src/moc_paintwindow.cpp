/****************************************************************************
** Meta object code from reading C++ file 'paintwindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "paintwindow.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'paintwindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PaintWindow_t {
    QByteArrayData data[16];
    char stringdata[180];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_PaintWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_PaintWindow_t qt_meta_stringdata_PaintWindow = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 7),
QT_MOC_LITERAL(2, 20, 0),
QT_MOC_LITERAL(3, 21, 5),
QT_MOC_LITERAL(4, 27, 11),
QT_MOC_LITERAL(5, 39, 4),
QT_MOC_LITERAL(6, 44, 12),
QT_MOC_LITERAL(7, 57, 14),
QT_MOC_LITERAL(8, 72, 12),
QT_MOC_LITERAL(9, 85, 13),
QT_MOC_LITERAL(10, 99, 13),
QT_MOC_LITERAL(11, 113, 15),
QT_MOC_LITERAL(12, 129, 14),
QT_MOC_LITERAL(13, 144, 10),
QT_MOC_LITERAL(14, 155, 11),
QT_MOC_LITERAL(15, 167, 11)
    },
    "PaintWindow\0newGame\0\0pause\0printStatus\0"
    "save\0setSlowSpeed\0setMediumSpeed\0"
    "setFastSpeed\0setOneSamples\0setTwoSamples\0"
    "setThreeSamples\0setFourSamples\0"
    "setNoAccel\0setBihAccel\0setAllAccel\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PaintWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x08,
       3,    0,   85,    2, 0x08,
       4,    0,   86,    2, 0x08,
       5,    0,   87,    2, 0x08,
       6,    0,   88,    2, 0x08,
       7,    0,   89,    2, 0x08,
       8,    0,   90,    2, 0x08,
       9,    0,   91,    2, 0x08,
      10,    0,   92,    2, 0x08,
      11,    0,   93,    2, 0x08,
      12,    0,   94,    2, 0x08,
      13,    0,   95,    2, 0x08,
      14,    0,   96,    2, 0x08,
      15,    0,   97,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PaintWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PaintWindow *_t = static_cast<PaintWindow *>(_o);
        switch (_id) {
        case 0: _t->newGame(); break;
        case 1: _t->pause(); break;
        case 2: _t->printStatus(); break;
        case 3: _t->save(); break;
        case 4: _t->setSlowSpeed(); break;
        case 5: _t->setMediumSpeed(); break;
        case 6: _t->setFastSpeed(); break;
        case 7: _t->setOneSamples(); break;
        case 8: _t->setTwoSamples(); break;
        case 9: _t->setThreeSamples(); break;
        case 10: _t->setFourSamples(); break;
        case 11: _t->setNoAccel(); break;
        case 12: _t->setBihAccel(); break;
        case 13: _t->setAllAccel(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject PaintWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_PaintWindow.data,
      qt_meta_data_PaintWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *PaintWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PaintWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PaintWindow.stringdata))
        return static_cast<void*>(const_cast< PaintWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int PaintWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
