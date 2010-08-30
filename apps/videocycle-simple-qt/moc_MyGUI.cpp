/****************************************************************************
** Meta object code from reading C++ file 'MyGUI.h'
**
** Created: Thu Aug 12 14:51:19 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MyGUI.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MyGUI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MyGUI[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x0a,
      24,    6,    6,    6, 0x0a,
      43,    6,    6,    6, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MyGUI[] = {
    "MyGUI\0\0BrowseVideoDir()\0BrowseOutputFile()\0"
    "Calculate()\0"
};

const QMetaObject MyGUI::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MyGUI,
      qt_meta_data_MyGUI, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MyGUI::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MyGUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MyGUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyGUI))
        return static_cast<void*>(const_cast< MyGUI*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MyGUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: BrowseVideoDir(); break;
        case 1: BrowseOutputFile(); break;
        case 2: Calculate(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
